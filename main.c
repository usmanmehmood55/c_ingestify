/**
 * @file      main.c
 * @author    Usman Mehmood (usmanmehmood55@gmail.com)
 * @brief     Implementation of Ingestify in C.
 * @version   0.1
 * @date      2024-07-22
 * @copyright 2024 Usman Mehmood
 */

#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "ignore.h"
#include "ingestify.h"

/**
 * @brief Main function of the program.
 * 
 * @param argc Argument count.
 * @param argv Argument vector.
 * 
 * @return int Exit status.
 */
int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <directory> <output_file> [ignore_file]\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *directory        = sanitize_path(argv[1]);
    const char *output_file_path = sanitize_path(argv[2]);
    const char *ignore_file_path = (argc > 3) ? sanitize_path(argv[3]) : NULL;

    ignore_list_t *ignore_list = (ignore_file_path) ? ignore_read_list(ignore_file_path) : NULL;

    FILE *output_file = fopen(output_file_path, "w");
    if (IS_NULL(output_file))
    {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    off_t input_directory_size = ingestify_calculate_directory_size(directory);
    if (input_directory_size == -1)
    {
        fclose(output_file);
        return EXIT_FAILURE;
    }
    off_t max_output_size = 2 * input_directory_size;

    ingestify_traverse_and_write(directory, ignore_list, output_file, output_file_path, max_output_size);

    fclose(output_file);

    ignore_free_list(ignore_list);

    return EXIT_SUCCESS;
}
