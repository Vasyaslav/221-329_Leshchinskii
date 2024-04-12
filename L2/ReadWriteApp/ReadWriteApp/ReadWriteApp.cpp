// ReadWriteApp.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <fstream>
#include <string>

int main()
{
    std::string file_content;
    std::ifstream in("hello.crypt_lab2");
    std::cout << "Current file text:" << std::endl;
    if (in.is_open())
    {
        while (std::getline(in, file_content))
        {
            std::cout << file_content << std::endl;
        }
    }
    in.close();
    std::ofstream file_writing;          // поток для записи
    file_writing.open("hello.crypt_lab2");      // открываем файл для записи
    std::cout << "File will contain following text: Hello World!" << std::endl;
    if (file_writing.is_open())
    {
        file_writing << "Hello World!" << std::endl;
    }
    file_writing.close();
    std::cout << "File has been written. You can close this window" << std::endl;
    std::cin >> file_content;
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
