// ConsoleApplication.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <stdio.h>
#include <tchar.h>
#include "sgx_urts.h"
#include "sgx_tseal.h"
#include "Enclave_u.h"
#define ENCLAVE_FILE _T("Enclave.signed.dll")

#define BUFFER_LENGTH 14

const char* data[] = {
    "aaaaa bbbbb 1",
    "ccccc ddddd 2",
    "eeeee fffff 3",
    "ggggg hhhhh 4",
    "iiiii jjjjj 5",
    "kkkkk lllll 6"
};

void getData(
    sgx_enclave_id_t eid,
    char* buffer,
    const size_t buffer_length,
    int id
)
{
    if (sizeof(data) / sizeof(char*) <= id)
        strcpy_s(buffer, buffer_length, "\0");
    else
        strcpy_s(buffer, buffer_length, data[id]);
    return;
}

int main()
{
    sgx_enclave_id_t eid;
    sgx_status_t ret = SGX_SUCCESS;
    sgx_launch_token_t token = { 0 };
    int updated = 0;

    ret = sgx_create_enclave(ENCLAVE_FILE, SGX_DEBUG_FLAG, &token, &updated, &eid, NULL);
    if (ret != SGX_SUCCESS) {
        printf("App: error %#x, failed to create enclave.\n", ret);
        return -1;
    }

    for (;;) {
        int did = -1;
        printf_s("Input data ID (begin with 0): ");
        int readData = scanf_s("%d", &did);
        if (readData != 1)
            return -1;
        char buffer[BUFFER_LENGTH] = { 0 };
        getData(eid, buffer, BUFFER_LENGTH, did);
        printf_s("Retrieved string: %s\n", buffer);
    }

    if (SGX_SUCCESS != sgx_destroy_enclave(eid))
        return -1;

    printf_s("Конец");
    return 0;
}

// Запуск программы: CTRL+F5 или меню "Отладка" > "Запуск без отладки"
// Отладка программы: F5 или меню "Отладка" > "Запустить отладку"
