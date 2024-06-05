#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <coz.h>
#include <omp.h>


int find_sequence_in_file(const char *filename, const char *sequence)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        perror("Could not open file");
        return 0;
    }

    // Determine the file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory to hold the entire file content
    char *file_content = (char *)malloc(file_size + 1);
    if (!file_content)
    {
        perror("Could not allocate memory for file content");
        fclose(file);
        return 0;
    }

    // Read the entire file into memory
    fread(file_content, 1, file_size, file);
    file_content[file_size] = '\0';
    fclose(file);

    size_t sequence_length = strlen(sequence);
    int found = 0;

    COZ_BEGIN("search_sequence"); // Start profiling the sequence search

    #pragma omp parallel shared(found)
    {
        #pragma omp for
        for (long i = 0; i <= file_size - sequence_length; i++)
        {
            if (found)
            {
                continue; // Skip further processing if sequence is already found
            }

            int match = 1;
            for (size_t j = 0; j < sequence_length; j++)
            {
                if (file_content[i + j] != sequence[j])
                {
                    match = 0;
                    break;
                }
            }

            if (match)
            {
                #pragma omp atomic write
                found = 1;
            }
        }
    }

    COZ_END("search_sequence"); // End profiling the sequence search

    free(file_content);
    return found;
}




int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <filename> <sequence>\n", argv[0]);
        return EXIT_FAILURE;
    }

    COZ_PROGRESS("main_start"); // Mark the start of the main function

    const char *filename = argv[1];
    const char *sequence = argv[2];

    COZ_PROGRESS("iteration_start"); // Mark the start of each iteration
    int found = find_sequence_in_file(filename, sequence);
    COZ_PROGRESS("iteration_end"); // Mark the end of each iteration

    if (found)
    {
        printf("Sequence found\n");
    }
    else
    {
        printf("Sequence not found\n");
    }

    COZ_PROGRESS("main_end"); // Mark the end of the main function

    return EXIT_SUCCESS;
}
