#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <stdint.h>

HANDLE com_init(char *port)
{
    HANDLE hSerial;
    char port_path[16];
    sprintf(port_path, "\\\\.\\%s", port);
    hSerial = CreateFile(port_path, GENERIC_READ | GENERIC_WRITE, 0, NULL,
                         OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DCB dcbSerialParams = {0};

    if (hSerial == INVALID_HANDLE_VALUE)
    {
        fprintf(stderr, "Error opening serial port\n");
        exit(-1);
    }

    // Set device parameters (38400 baud, 1 start bit,
    // 1 stop bit, no parity)
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
    if (GetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error getting device state\n");
        CloseHandle(hSerial);
        exit(-2);
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.StopBits = ONESTOPBIT;
    dcbSerialParams.Parity = NOPARITY;
    if (SetCommState(hSerial, &dcbSerialParams) == 0)
    {
        fprintf(stderr, "Error setting device parameters\n");
        CloseHandle(hSerial);
        exit(-3);
    }

    return hSerial;
}

int com_write(HANDLE hSerial, uint8_t *data, uint8_t len)
{
    if (!WriteFile(hSerial, data, len, NULL, NULL))
    {
        fprintf(stderr, "Error sending data\n");
        CloseHandle(hSerial);
        exit(-4);
    }
    return 0;
}

int com_read(HANDLE hSerial, uint8_t *data, uint8_t len)
{
    if (!ReadFile(hSerial, data, len, NULL, NULL))
    {
        fprintf(stderr, "Error reading data\n");
        CloseHandle(hSerial);
        exit(-5);
    }
    return 0;
}

int com_close(HANDLE hSerial)
{
    CloseHandle(hSerial);
    return 0;
}

uint8_t arg_translate(char *arg)
{
    if (arg[0] == '0')
    {
        return 0x00;
    }
    else
    {
        return 0xFF;
    }
}

int main(int argc, char *argv[])
{
    uint8_t data_r[20], data_t[20]={};
    HANDLE virtual_com = com_init(argv[1]);

    com_read(virtual_com, data_r, 12);
    data_t[9] = arg_translate(argv[2]);
    com_write(virtual_com, data_t, 13);

    com_read(virtual_com, data_r, 13);
    data_t[9] = arg_translate(argv[3]);
    com_write(virtual_com, data_t, 13);

    com_read(virtual_com, data_r, 17);
    data_t[9] = arg_translate(argv[4]);
    com_write(virtual_com, data_t, 13);

    return 0;
}
