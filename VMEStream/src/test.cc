#include <iostream>

#include <stdint.h>
#include <iomanip>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#include "VMEController.h"

#define DATAWIDTH 2

uint32_t ZERO = 0;

using std::cout;
using std::endl;
using std::hex;

int test()
{
    VMEController *vme = VMEController::getVMEController();

    cout << vme->write(0x2000, DATAWIDTH, &ZERO) << endl;
    cout << vme->write(0x2004, DATAWIDTH, &ZERO) << endl;

    uint32_t* tx_size = (uint32_t*) calloc(1, sizeof(uint32_t));
    uint32_t* rx_size = (uint32_t*) calloc(1, sizeof(uint32_t));
    
    int counter = 1;
    for (int i = 0; i < 10; ++i) {
        printf("-- Loop %d --\n", counter++);

        cout << "Reading" << endl;
        cout << vme->read(0x2000, DATAWIDTH, tx_size) << endl;
        cout << vme->read(0x2004, DATAWIDTH, rx_size) << endl;

        printf("tx_size: %d\n", *(tx_size));
        printf("rx_size: %d\n", *(rx_size));

        cout << "Writing" << endl;
        cout << vme->write(0x2000, DATAWIDTH, tx_size) << endl;
        cout << vme->write(0x2004, DATAWIDTH, rx_size) << endl;

        printf("tx_size: %d\n", *(tx_size));
        printf("rx_size: %d\n", *(rx_size));
        
        cout << endl;
    }

    return 0;
}


int main()
{
    test();

    return 0;
}
