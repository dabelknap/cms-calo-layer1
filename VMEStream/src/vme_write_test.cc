#include <iostream>

#include <stdint.h>
#include <iomanip>

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>

#include "VMEController.h"
#include "VMEStreamAddress.h"

#define DATAWIDTH 2

using std::cout;
using std::endl;
using std::hex;

int test()
{
    VMEController *vme = VMEController::getVMEController();

    //uint32_t value = 0xCAFEBABE;
    uint32_t* value = (uint32_t*) malloc(2*sizeof(uint32_t));
    value[0] = 0xDEADBEEF;
    value[1] = 0xD00DBABE;

    uint32_t* buf = (uint32_t*) malloc(2*sizeof(uint32_t));

    uint32_t* rx_size = (uint32_t*) calloc(1, sizeof(uint32_t));
    uint32_t* tx_size = (uint32_t*) calloc(1, sizeof(uint32_t));

    rx_size[0] = 0xD00D;
    tx_size[0] = 0xDAAD;

    //return vme->write(0x0000, 2, &value);
    for (int i = 0; i < 40; ++i) {

        cout << vme->block_write(0x0, DATAWIDTH, value, 2*sizeof(uint32_t)) << endl;

        cout << vme->read(0x2000, DATAWIDTH, tx_size) << endl;
        cout << vme->read(0x2002, DATAWIDTH, rx_size) << endl;

        cout << vme->block_read(0x1000, DATAWIDTH, buf, 2*sizeof(uint32_t)) << endl;
        cout << hex << buf[0] << endl;

        cout << vme->write(0x2000, DATAWIDTH, tx_size) << endl;
        cout << vme->write(0x2002, DATAWIDTH, rx_size) << endl;
    }

    return 0;
}


int main()
{
    cout << test() << endl;

    return 0;
}
