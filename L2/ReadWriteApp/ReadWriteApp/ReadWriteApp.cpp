// ReadWriteApp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//
#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <stdio.h>
#include <string>

int main()
{
    char file_content[166];
    FILE* file;
    file = fopen("hello.crypt_lab2", "r");
    std::cout << "Current file text:" << std::endl;
    if (file != NULL)
    {
        fread(&file_content, sizeof(char[166]), 1, file);
        std::cout << file_content << std::endl;
        fclose(file);
    }
    file = fopen("hello.crypt_lab2", "w");  // открываем файл для записи     
    std::cout << "File will contain part of lorem ipsum text" << std::endl;
    char lorem_ipsum[166] = "Sed ut perspiciatis, unde omnis iste natus error sit voluptatem accusantium doloremque laudantium, totam rem aperiam eaque ipsa, quae ab illo inventore veritatis et.";
    if (file != NULL)
    {
        fwrite(lorem_ipsum, sizeof (lorem_ipsum), 1, file);
        fclose(file);
    }
    std::cout << "File has been written. You can close this window" << std::endl;
    std::string skipper;
    std::cin >> skipper;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"

// Советы по началу работы 
//   1. В окне обозревателя решений можно добавлять файлы и управлять ими.
//   2. В окне Team Explorer можно подключиться к системе управления версиями.
//   3. В окне "Выходные данные" можно просматривать выходные данные сборки и другие сообщения.
//   4. В окне "Список ошибок" можно просматривать ошибки.
//   5. Последовательно выберите пункты меню "Проект" > "Добавить новый элемент", чтобы создать файлы кода, или "Проект" > "Добавить существующий элемент", чтобы добавить в проект существующие файлы кода.
//   6. Чтобы снова открыть этот проект позже, выберите пункты меню "Файл" > "Открыть" > "Проект" и выберите SLN-файл.
