/**
 * @file      ingestify.h
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      2024-07-24
 * @copyright 2024, your company / association / school
 */

#ifndef INGESTIFY_H_
#define INGESTIFY_H_

#include <stdio.h>
#include "ignore.h"

/**
 * @brief Calculates the total size of a directory recursively.
 * 
 * @param[in] dir_path Path to the directory.
 * 
 * @return off_t The total size of the directory.
 */
off_t ingestify_calculate_directory_size(const char *dir_path);

/**
 * @brief Recursively traverses a directory and writes the contents to an output file.
 * 
 * @param[in]      dir_path         Path to the directory.
 * @param[in]      ignore_list      Pointer to the ignore list structure.
 * @param[in, out] output_file      Pointer to the output file.
 * @param[in]      output_file_path Path to the output file.
 * @param[in]      max_output_size  Maximum allowed size for the output file.
 */
void ingestify_traverse_and_write(const char *dir_path, const ignore_list_t *ignore_list, FILE *output_file, const char *output_file_path, const off_t max_output_size);

#endif // INGESTIFY_H_
