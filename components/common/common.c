/**
 * @file      common.c
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      2024-07-24
 * @copyright 2024, your company / association / school
 */

#include "common.h"
#include <string.h>

/**
 * @brief Retrieves the file extension from a filename.
 * 
 * @param[in] filename The name of the file.
 * 
 * @return const char* The file extension.
 */
const char *get_filename_ext(const char *filename)
{
    const char *dot = strrchr(filename, '.');
    if (IS_NULL(dot)) // this was `if (!dot || dot == filename)`
        return "";
    return dot + 1;
}

/**
 * @brief Sanitizes a file path by removing leading "./" and trailing "/".
 * 
 * @param[in,out] path The path to sanitize.
 * 
 * @return char* The sanitized path.
 */
char *sanitize_path(char *path)
{
    if (IS_NULL(path)) return NULL;
    // fprintf(stdout, "pre: \"%s\"\n", path);
    if (strncmp(path, "./", 2U) == 0) path += 2;            // Remove leading dot and slash "./", it is basically 'skipping' them from the string
    int path_len = strnlen(path, MAX_PATH);                 // Remove trailing slash "/", it NULL terminates the string at the slash
    if (path[path_len - 1] == '/') path[path_len - 1] = 0;  // This may lead to segfaults if the string is read-only
    // fprintf(stdout, "pst: \"%s\"\n\n", path);
    return path;
}

// end of file common.c
