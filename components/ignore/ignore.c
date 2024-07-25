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

    char line[MAX_PATH];
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

// A slash needs to be added to ignore item so that it appears as a subfolder, and not
// a substring of a larger filename.
// "test" -> "test/some_file.c" -> true
// "test" -> "test_some"        -> false
// so "test" has to become "test/"
int match_as_directory(const char *ignore_item, const char *path)
{
    int exact_match = -1;

    int len_ignore_item = strnlen(ignore_item, PATH_MAX);
    char *ignore_item_slash = malloc(len_ignore_item + 2); // < ------- ALLOC HERE -------------------
    {                                                                                               //
        snprintf(ignore_item_slash, len_ignore_item + 2, "%s/", ignore_item);                       //
        //                                       "test/some_file.c"    "test/"                      //
        char *possible_ignore_subfolder = strstr(path,                 ignore_item_slash);          //
        if (EXISTS(possible_ignore_subfolder))                                                      //
        {                                                                                           //
            int len_possible_ignore_subfolder = strlen(path);                                       //
            exact_match = strncmp(possible_ignore_subfolder, path, len_possible_ignore_subfolder);  //
        }                                                                                           //
    }                                                                                               //
    free(ignore_item_slash); // <-------------------------------------- FREE HERE --------------------

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
    if (EXISTS(ignore_list))
    {
        int exact_match;

        for (size_t i = 0; i < ignore_list->count; i++)
        {
            char *ignore_item           = sanitize_path(ignore_list->entries[i]);
            char *substring_match       = strstr(path, ignore_item);
            char *wildcard_match        = strstr(ignore_item, "*");
            char *double_wildcard_match = strstr(ignore_item, "**");

            // Wildcard was found in the entry
            if (EXISTS(wildcard_match))
            {
                exact_match = strncmp(get_filename_ext(path), get_filename_ext(ignore_item), MAX_PATH);
                if (exact_match == 0)
                    return true;
            }

            // Double wildcard was found in the entry
            if (EXISTS(double_wildcard_match))
            {
                char *after_wildcard_and_slash = &double_wildcard_match[3]; // "**/some_path" -> "some_path"
                char *double_wildcard_path_match = strstr(path, after_wildcard_and_slash);
                if (EXISTS(double_wildcard_path_match))
                {
                    exact_match = match_as_directory(after_wildcard_and_slash, double_wildcard_path_match);
                    if (exact_match == 0)
                        return true;
                }
            }

            // Part of the path matches with the ignore entry
            if (EXISTS(substring_match))
            {
                // A directory in the path has matched
                exact_match = match_as_directory(ignore_item, path);
                if (exact_match == 0)
                    return true;

                // The full path has matched
                exact_match = strncmp(ignore_item, path, PATH_MAX);
                if (exact_match == 0)
                    return true;
            }
        }
    }

    return false;
}

// end of file ignore.c
