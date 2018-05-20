#include <iostream>
#include <cstdio>
#include <malloc.h>
#include <cstdlib>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iterator>

size_t parse_file(char *filename, int unsorted_numbers[4096], size_t index);

size_t read_all_numbers_from_file(FILE *file, int *unsorted_numbers, size_t index);

int main(int argc, char **argv) {
    if (argc < 3) {
        printf("You must specify at least one file from which numbers must be taken,"
               "and one more file to which numbers will be sorted to\n");
        return 1;
    }

    int unsorted_numbers[4096];
    size_t index = 0;

    for (int current_file_index = 1; current_file_index < argc - 1; ++current_file_index) {
        index = parse_file(argv[current_file_index], unsorted_numbers, index);
    }

    std::vector<int> sort_int_vector(unsorted_numbers, unsorted_numbers + index);
    int i;

    try {
        // сортировка по умолчанию, по возрастанию
        sort(sort_int_vector.begin(), sort_int_vector.end());
    } catch (...){
        printf("Sort function exited with unexpected error. Program will now shut down.");
        std::exit(1);
    }

    // вывод в последний переданный файл
    std::ofstream output_file(argv[argc - 1]);

    if (!output_file)
    {
        printf("File %s does not exist.", argv[argc - 1]);
        std::exit(1);
    }

    std::ostream_iterator<int> output_iterator(output_file, "\n");
    std::copy(sort_int_vector.begin(), sort_int_vector.end(), output_iterator);
}

size_t parse_file(char *filename, int unsorted_numbers[4096], size_t index) {
    FILE* file = fopen(filename, "r");

    if (file == NULL) {
        printf("Unable to open file %s\n", filename);
        return index;
    }

    index = read_all_numbers_from_file(file, unsorted_numbers, index);
    fclose(file);

    return index;
}

size_t read_all_numbers_from_file(FILE *file, int *unsorted_numbers, size_t index) {
    char *line = (char *) malloc(1024);
    int line_index = 0;
    int current_char;
    bool digit_flag = false;
    for (int i = 0; (current_char = getc(file)) != EOF; i++) {
        if (isdigit(current_char)) {
            if (digit_flag) {
                if (line_index >= 1024) {
                    printf("Numbers in files are too big for this program to function");
                    fclose(file);
                    std::exit(1);
                }
            } else {
                digit_flag = true;
            }
            line[line_index] = (char) current_char;
            line_index++;
        } else {
            digit_flag = false;
            if (line_index != 0) {
                if (index == 4095) {
                    printf("Too many numbers to sort");
                    std::exit(1);
                }
                unsorted_numbers[index] = atoi(line);
                index++;
                free(line);
                line_index = 0;
            }
        }
    }

    return index;
}
