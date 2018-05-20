#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unistd.h>

using namespace std;

int main(int argc, char* argv[]) {
    string line;

    line = "zeroes.txt";

    if (line == "")
    {
        printf("This program is meant to be run only as gzip -cd sparse-file.gz | ./myprogram newsparsefile");
        return 1;
    }

    if (argc != 2)
    {
        printf("This program is meant to be run only as gzip -cd sparse-file.gz | ./myprogram newsparsefile");
        return 1;
    }

    FILE *input_file_pointer;
    input_file_pointer = fopen(line.c_str(), "r"); // Открываем разреженный файл

    if (input_file_pointer == NULL)
    {
        printf("Input file is not correct.");
        return 1;
    }

    FILE *output_file_pointer;
    output_file_pointer = fopen(argv[1], "w"); // Открываем файл куда будем записывать результат

    if (output_file_pointer == NULL)
    {
        printf("Output file is not correct.");
        return 1;
    }

    int buffer[1024]; // fwrite принимает buffer в качестве аргумента, поэтому записывать в файл будет проще так как предлагает функция, то есть кусками.
    size_t buffer_current_index = 0; // Текущий индекс в буфере
    int zero_count = 0; // Количество нулевых символов в последовательности, его и передаем в lseek
    bool is_zeros_flag = false; // Флаг - индикатор того, идём ли мы по последовательности нулей или нет

    while (!feof(input_file_pointer)) {
        int current_char = fgetc(input_file_pointer);

        if (current_char == 0) {
            if (is_zeros_flag) // Очередной ноль просто съедаем
                zero_count++;
            else {
                if (buffer_current_index != 0) { // Первый ноль - индикатор того, что пора записать буфер в файл
                    fwrite(buffer, 1, buffer_current_index, output_file_pointer);
                    buffer_current_index = 0;
                }
                is_zeros_flag = true;
                zero_count = 1;
            }
        } else {
            if (is_zeros_flag) { // Нули кончились
                is_zeros_flag = false;
                zero_count = 0;
                lseek(fileno(output_file_pointer), zero_count, SEEK_CUR);
            }

            buffer[buffer_current_index] = current_char;
            buffer_current_index++;
            if (buffer_current_index == 1024) { // Буфер заполнился
                fwrite(buffer, 1, buffer_current_index, output_file_pointer);
                buffer_current_index = 0;
            }
        }
    }

    // Обработка "хвостов". Если мы закончили читать на нулях, то их надо передать в lseek.
    // Если мы закончили на ненулях, то их надо записать в выходной файл.
    if (is_zeros_flag) {
        lseek(fileno(output_file_pointer), zero_count, SEEK_CUR);
    }
    else if (buffer_current_index > 0) {
        fwrite(buffer, 1, buffer_current_index - 1, output_file_pointer);
    }

    return 0;
}