/**
 * @file      common.h
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      2024-07-24
 * @copyright 2024, your company / association / school
 */

#ifndef COMMON_H_
#define COMMON_H_

#define MAX_PATH 1024

#define IS_NULL(ptr)     (ptr == NULL)
#define IS_NOT_NULL(ptr) (ptr != NULL)
#define EXISTS(ptr)      IS_NOT_NULL(ptr)

/**
 * @brief Retrieves the file extension from a filename.
 * 
 * @param[in] filename The name of the file.
 * 
 * @return const char* The file extension.
 */
const char *get_filename_ext(const char *filename);

/**
 * @brief Sanitizes a file path by removing leading "./" and trailing "/".
 * 
 * @param[in,out] path The path to sanitize.
 * 
 * @return char* The sanitized path.
 */
char *sanitize_path(char *path);

#endif // COMMON_H_
