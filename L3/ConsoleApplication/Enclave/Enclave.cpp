#include "Enclave_t.h"

#include "sgx_trts.h"
#include "string.h"


const char* data[] = {
    "aaaaa bbbbb 1",
    "ccccc ddddd 2",
    "eeeee fffff 3",
    "ggggg hhhhh 4",
    "iiiii jjjjj 5",
    "kkkkk lllll 6"
};

void getD(
    char* buffer,
    size_t buffer_length,
    int id
)
{
    if (sizeof(data) / sizeof(char*) <= id)
        for (int i = 0; i < buffer_length; i++)
            buffer[i] = '\0';
    else
        for (int i = 0; i < buffer_length; i++)
            buffer[i] = data[id][i];
    return;
}