/**
 * @file      ignore.c
 * @author    Usman Mehmood (usmanmehmood55@gmail.com)
 * @brief     This is a git-like ignore implementation. I want to make it
 *            as close to gitignore as possible. 
 * @version   0.1
 * @date      2024-07-24
 * @copyright Usman Mehmood 2024
 */

#include "ignore.h"
#include "common.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static inline int match_exact_path(const char *pattern, const char *path)
{
    return strncmp(pattern, path, __PATH_MAX);
}

static inline int unsafe_char_to_digit(const char _char)
{
    return (48 <= _char && _char <= 57) ? ((int)_char - 48) : -1;
}

/**
 * @brief Converts string representing a number into an int.
 * 
 * @param[in]      str         String to convert.
 * @param[in, out] ptr_int_out Buffer to put converted number in.
 * 
 * @return 0 on success.
 */
static int str_to_int(char *str, int *ptr_int_out)
{
    int int_out = 0;
    char *_str_max = str + __PATH_MAX;
    for (char *rp = str; *rp && (rp != _str_max); rp++)
    {
        int char_digit = unsafe_char_to_digit(*rp);
        if (char_digit >= 0)
        {
            int_out *= 10;
            int_out += char_digit;
        }
        else
        {
            return 1; // error
        }
    }
    *ptr_int_out = int_out;
    return 0;
}

/**
 * @brief Frees the memory allocated for the ignore list.
 * 
 * @param ignore_list Pointer to the ignore list structure.
 */
void ignore_free_list(ignore_list_t *ignore_list)
{
    if (ignore_list)
    {
        for (size_t i = 0; i < ignore_list->count; i++)
        {
            free(ignore_list->entries[i]);
        }
        free(ignore_list->entries);
        free(ignore_list);
    }
}

/**
 * @brief Reads the ignore list from a file.
 * 
 * @param[in] ignore_file Path to the ignore file.
 * 
 * @return ignore_list_t* Pointer to the ignore list structure.
 */
ignore_list_t *ignore_read_list(const char *ignore_file)
{
    FILE *file = fopen(ignore_file, "r");
    if (IS_NULL(file))
    {
        perror("Error opening ignore file");
        return NULL;
    }

    ignore_list_t *ignore_list = malloc(sizeof(ignore_list_t));
    if (IS_NULL(ignore_list))
    {
        perror("Memory allocation failed");
        fclose(file);
        return NULL;
    }
    ignore_list->count = 0;
    ignore_list->entries = NULL;

    char line[__PATH_MAX];
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0'; // Remove the newline character
        ignore_list->entries = realloc(ignore_list->entries, (ignore_list->count + 1) * sizeof(char *));
        if (IS_NULL(ignore_list->entries))
        {
            perror("Memory allocation failed");
            fclose(file);
            return NULL;
        }
        ignore_list->entries[ignore_list->count] = strdup(line);
        if (IS_NULL(ignore_list->entries[ignore_list->count]))
        {
            perror("Memory allocation failed");
            fclose(file);
            return NULL;
        }
        ignore_list->count++;
    }

    fclose(file);
    return ignore_list;
}

/**
 * @brief A slash needs to be added to ignore item so that it appears as a subfolder,
 * and not a substring of a larger filename.
 * 
 * - "test" -> "test/some_file.c" -> true
 * 
 * - "test" -> "test_some"        -> false
 * 
 * So "test" has to become "test/"
 * 
 * @param[in] ignore_item Ignore list entry to check against
 * @param[in] path        Path to the file or directory.
 * 
 * @return 0 if match, some other value otherwise.
 */
static int match_directory(const char *ignore_item, const char *path)
{
    int exact_match = -1;
    int len_ignore_item = strnlen(ignore_item, __PATH_MAX);

    char *ignore_item_slash = malloc(len_ignore_item + 2);
    {
        snprintf(ignore_item_slash, len_ignore_item + 2, "%s/", ignore_item);
        char *possible_ignore_subfolder = strstr(path, ignore_item_slash);
        if (EXISTS(possible_ignore_subfolder))
        {
            int len_possible_ignore_subfolder = strnlen(path, __PATH_MAX);
            exact_match = strncmp(possible_ignore_subfolder, path, len_possible_ignore_subfolder);
        }
    }
    free(ignore_item_slash);

    return exact_match;
}

/**
 * @brief Checks if a file or directory should be ignored based on the ignore list.
 *
 * @param[in] ignore_list Pointer to the ignore list structure.
 * @param[in] path        Path to the file or directory.
 *
 * @return true If the file or directory should be ignored.
 * @return false If the file or directory should not be ignored.
 */
bool ignore_is_match(const ignore_list_t *ignore_list, const char *path)
{
    if (IS_NULL(ignore_list) || IS_NULL(path))
    {
        return false;
    }

    bool is_match = false;
    int exact_match;

    for (size_t entry = 0; entry < ignore_list->count; entry++)
    {
        char *ignore_item = sanitize_path(ignore_list->entries[entry]);
        if (IS_NULL(ignore_item)) // TODO: add length check here for __PATH_MAX?
            continue;

        char *path_has_pattern = strstr(path, ignore_item);
        char *pattern_has_wc   = strstr(ignore_item, "*");  // wc = wildcard
        char *pattern_has_dwc  = strstr(ignore_item, "**"); // dwc = double wildcard
        char *pattern_has_ques = strstr(ignore_item, "?");
        char *pattern_has_neg  = (ignore_item[0] == '!') ? ignore_item : NULL;
        char *pattern_has_brk_s = strchr(ignore_item, '[');
        char *pattern_has_brk_e = strchr(ignore_item, ']');

        // Question mark was found in the entry
        // example:- pattern: [ "log", "!log/important.txt" ]
        // ignores: "log/some_log.txt", doesn't ignore: "log/important.txt"
        if (EXISTS(pattern_has_neg) && EXISTS(pattern_has_neg + 1)) // checking for *p and *p+1 guarantees length < 1
        {
            pattern_has_neg++; // Skip the "!"
            char *path_subdir_has_pattern = strstr(path, pattern_has_neg);
            if (path_subdir_has_pattern)
            {
                exact_match = match_exact_path(pattern_has_neg, path_subdir_has_pattern);
                if (exact_match == 0)
                {
                    is_match = false;
                    break;
                }
            }
        }

        // Double wildcard was found in the entry
        // example:- pattern: "**/logs", path: "repo/logs/file.txt"
        if (EXISTS(pattern_has_dwc))
        {
            int pattern_len = strnlen(pattern_has_dwc, __PATH_MAX);
            if (pattern_len < 4)
            {
                is_match = false;
                continue;
            }

            char *pattern_after_dwc_slash = &pattern_has_dwc[3]; // "**/some_path" -> "some_path"
            char *path_subdir_has_pattern = strstr(path, pattern_after_dwc_slash);
            if (EXISTS(path_subdir_has_pattern))
            {
                exact_match = match_exact_path(path_subdir_has_pattern, pattern_after_dwc_slash);
                if (exact_match == 0)
                {
                    is_match = true;
                    continue;
                }

                exact_match = match_directory(pattern_after_dwc_slash, path_subdir_has_pattern);
                if (exact_match == 0)
                {
                    is_match = true;
                    continue;
                }
            }
            else
                return false;
        }

        // Wildcard was found in the entry
        // example:- pattern: "*.exe", path: "file.exe"
        if (EXISTS(pattern_has_wc))
        {
            exact_match = match_exact_path(get_filename_ext(ignore_item), get_filename_ext(path));
            if (exact_match == 0)
            {
                is_match = true;
                continue;
            }
        }

        // Question mark was found in the entry
        // example:- path: "file_1.exe", pattern: "file_?.exe"
        if (EXISTS(pattern_has_ques))
        {
            int ques_position = pattern_has_ques - ignore_item;
            char path_ques[__PATH_MAX] = { 0 };
            memcpy(path_ques, path, strnlen(path, __PATH_MAX));
            path_ques[ques_position] = '?';

            // A directory in the path has matched
            exact_match = match_directory(ignore_item, path_ques);
            if (exact_match == 0)
            {
                is_match = true;
                continue;
            }

            // The full path has matched
            exact_match = match_exact_path(ignore_item, path_ques);
            if (exact_match == 0)
            {
                is_match = true;
                continue;
            }
        }

        // Part of the path matches with the ignore entry
        // example:- pattern: "some_dir", path: "some_dir/file.txt"
        if (EXISTS(path_has_pattern))
        {
            // A directory in the path has matched
            exact_match = match_directory(ignore_item, path);
            if (exact_match == 0)
            {
                is_match = true;
                continue;
            }

            // The full path has matched
            exact_match = match_exact_path(ignore_item, path);
            if (exact_match == 0)
            {
                is_match = true;
                continue;
            }
        }
    }

    return is_match;
}

// end of file ignore.c
