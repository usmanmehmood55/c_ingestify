/**
 * @file      ingestify.c
 * @author    Your Name (your-email@example.com)
 * @brief     your file's description
 * @version   0.1
 * @date      2024-07-24
 * @copyright 2024, your company / association / school
 */

#include "ingestify.h"
#include "common.h"
#include "ignore.h"

#include <stdio.h>
#include <dirent.h>
#include <sys/stat.h>

/**
 * @brief Maintains the size of data written to output file.
 */
static off_t data_written = 0;

/**
 * @brief Calculates the total size of a directory recursively.
 * 
 * @param[in] dir_path Path to the directory.
 * 
 * @return off_t The total size of the directory.
 */
off_t ingestify_calculate_directory_size(const char *dir_path)
{
    off_t total_size = 0;
    DIR *dir = opendir(dir_path);
    if (IS_NULL(dir))
    {
        fprintf(stderr, "Could not open directory: %s\n", dir_path);
        return -1;
    }

    struct dirent *entry;
    while (EXISTS((entry = readdir(dir))))
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);

        struct stat path_stat;
        if (stat(full_path, &path_stat) != 0)
        {
            fprintf(stderr, "Could not retrieve status for: %s\n", full_path);
            continue;
        }

        if (S_ISDIR(path_stat.st_mode))
        {
            off_t dir_size = ingestify_calculate_directory_size(full_path);
            if (dir_size == -1)
            {
                closedir(dir);
                return -1;
            }
            total_size += dir_size;
        }
        else if (S_ISREG(path_stat.st_mode))
        {
            total_size += path_stat.st_size;
        }
    }

    closedir(dir);
    return total_size;
}

/**
 * @brief Recursively traverses a directory and writes the contents to an output file.
 * 
 * @param[in]      dir_path         Path to the directory.
 * @param[in]      ignore_list      Pointer to the ignore list structure.
 * @param[in, out] output_file      Pointer to the output file.
 * @param[in]      output_file_path Path to the output file.
 * @param[in]      max_output_size  Maximum allowed size for the output file.
 */
void ingestify_traverse_and_write(const char *dir_path, const ignore_list_t *ignore_list, FILE *output_file, const char *output_file_path, const off_t max_output_size)
{
    DIR *dir = opendir(dir_path);
    if (IS_NULL(dir))
    {
        fprintf(stderr, "Could not open directory: %s\n", dir_path);
        return;
    }

    struct dirent *entry;
    while (EXISTS((entry = readdir(dir))))
    {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_path, entry->d_name);
        if (strcmp(sanitize_path(full_path), output_file_path) == 0)
        {
            fprintf(stdout, "Ignoring: \"%s\"\n", full_path);
            continue;
        }

        if (ignore_is_match(ignore_list, sanitize_path(full_path)))
        {
            fprintf(stdout, "Ignoring: \"%s\"\n", full_path);
            continue;
        }

        struct stat path_stat;
        if (stat(full_path, &path_stat) != 0)
        {
            fprintf(stderr, "Could not retrieve status for: %s\n", full_path);
            continue;
        }

        if (S_ISDIR(path_stat.st_mode))
        {
            ingestify_traverse_and_write(full_path, ignore_list, output_file, output_file_path, max_output_size);
        }
        else if (S_ISREG(path_stat.st_mode))
        {
            FILE *input_file = fopen(full_path, "r");
            if (EXISTS(input_file))
            {
                fprintf(stdout, "Writing:  \"%s\"\n", full_path);
                fprintf(output_file, "\nFILE \"%s\" =============================================================:\n", full_path);
                char buffer[BUFSIZ];
                size_t n;
                while ((n = fread(buffer, 1, sizeof(buffer), input_file)) > 0)
                {
                    data_written += ftell(output_file);
                    if (data_written > max_output_size)
                    {
                        fprintf(stderr, "Output file size exceeded the limit. Aborting.\n");
                        fclose(input_file);
                        closedir(dir);
                        return;
                    }

                    fwrite(buffer, 1, n, output_file);
                }
                fputs("\n", output_file);
                fclose(input_file);
            }
            else
            {
                fprintf(stderr, "Could not open file: %s\n", full_path);
            }
        }
    }

    closedir(dir);
}

// end of file ingestify.c
