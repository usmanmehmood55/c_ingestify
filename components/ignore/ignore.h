/**
 * @file      ignore.h
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      2024-07-24
 * @copyright 2024, your company / association / school
 */

#ifndef IGNORE_H_
#define IGNORE_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Structure to hold the ignore list
 */
typedef struct
{
    char **entries; /**< Array of strings representing ignore patterns */
    size_t count;   /**< Number of entries in the ignore list */
} ignore_list_t;

ignore_list_t *ignore_read_list(const char *ignore_file);

/**
 * @brief Checks if a file or directory should be ignored based on the ignore list.
 * 
 * @param[in] ignore_list Pointer to the ignore list structure.
 * @param[in] name        Name of the file or directory.
 * 
 * @return true If the file or directory should be ignored.
 * @return false If the file or directory should not be ignored.
 */
bool ignore_is_match(const ignore_list_t *ignore_list, const char *name);

/**
 * @brief Frees the memory allocated for the ignore list.
 * 
 * @param ignore_list Pointer to the ignore list structure.
 */
void ignore_free_list(ignore_list_t *ignore_list);

#endif // IGNORE_H_
