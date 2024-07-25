/**
 * @file      ignore.c
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      2024-07-24
 * @copyright 2024, your company / association / school
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

/**
 * @brief Checks if a file or directory should be ignored based on the ignore list.
 * 
 * @param[in] ignore_list Pointer to the ignore list structure.
 * @param[in] name        Name of the file or directory.
 * 
 * @return true If the file or directory should be ignored.
 * @return false If the file or directory should not be ignored.
 */
bool ignore_is_match(const ignore_list_t *ignore_list, const char *name)
{
    if (EXISTS(ignore_list))
    {
        for (size_t i = 0; i < ignore_list->count; i++)
        {
            char *ignore_item     = sanitize_path(ignore_list->entries[i]);
            char *match_substring = strstr(name, ignore_item);
            char *wildcard        = strstr(ignore_item, "*");
            char *double_wildcard = strstr(ignore_item, "**");

            // Wildcard was found in the entry
            if (EXISTS(wildcard))
            {
                int exact_match = strcmp(get_filename_ext(name), get_filename_ext(ignore_item));
                if (exact_match == 0)
                    return true;
            }

            // Wildcard was found in the entry
            if (EXISTS(double_wildcard))
            {
                char *after_wildcard_and_slash = &double_wildcard[3];
                char *double_wildcard_name = strstr(name, after_wildcard_and_slash);
                if (EXISTS(double_wildcard_name))
                {
                    //                                       "test/some_file.c"    "test"
                    char *possible_ignore_subfolder = strstr(double_wildcard_name, after_wildcard_and_slash);
                    if (EXISTS(possible_ignore_subfolder))
                    {
                        int len_possible_ignore_subfolder = strlen(after_wildcard_and_slash);
                        int exact_match = strncmp(possible_ignore_subfolder, after_wildcard_and_slash, len_possible_ignore_subfolder);
                        if (exact_match == 0)
                            return true;
                    }
                }
                int exact_match = strcmp(get_filename_ext(double_wildcard_name), get_filename_ext(ignore_item));
                if (exact_match == 0)
                    return true;
            }

            // Part of the path matches with the ignore entry
            if (EXISTS(match_substring))
            {
                //                                       "test/some_file.c"    "test"
                char *possible_ignore_subfolder = strstr(name, ignore_item);
                if (EXISTS(possible_ignore_subfolder))
                {
                    int len_possible_ignore_subfolder = strlen(name);
                    int exact_match = strncmp(possible_ignore_subfolder, name, len_possible_ignore_subfolder);
                    if (exact_match == 0)
                        return true;
                }

                int exact_match = strcmp(ignore_item, name);
                if (exact_match == 0)
                    return true;
            }
        }
    }

    return false;
}

// end of file ignore.c
