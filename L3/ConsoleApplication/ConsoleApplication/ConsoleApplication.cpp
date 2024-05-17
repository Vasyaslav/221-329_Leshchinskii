// ConsoleApplication.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include <string.h>

#define buffer_length 14

const char* data[] = {
    "aaaaa bbbbb 1",
    "ccccc ddddd 2",
    "eeeee fffff 3",
    "ggggg hhhhh 4",
    "iiiii jjjjj 5",
    "kkkkk lllll 6"
};

int main()
{
    for (;;) {
        int bid = -1;
        printf_s("Input data ID (begin with 0): ");
        int readData = scanf_s("%d", &bid);
        if (readData != 1)
            return -1;
        char buffer[buffer_length] = { 0 };
        if (sizeof(data) / sizeof(char*) <= bid)
            strcpy_s(buffer, buffer_length, "\0");
        else
            strcpy_s(buffer, buffer_length, data[bid]);
        printf_s("Retrieved string: %s\n", buffer);
    }
    printf_s("Конец");
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
