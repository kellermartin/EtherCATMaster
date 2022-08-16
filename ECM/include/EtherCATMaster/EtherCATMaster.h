#ifndef ETHERCATMASTER_H
#define ETHERCATMASTER_H
#include "IOMapping.h"
#include <string>
#include <array>
#include <inttypes.h>
#include <cstdint>

class EtherCATConfig;

class EtherCATMaster{
public:
    EtherCATMaster();
    bool initialize(const std::string ifname);
    void shutdown();
    unsigned int getnumberOfSlaves();

    int writeSDO(uint16_t Slave, uint16_t Index, uint8_t SubIndex,
                uint8_t CA, int psize, void *p);

    int readSDO(uint16_t slave, uint16_t index, uint8_t subindex,
               uint8_t CA, int *psize, void *p);

    void linkRXPDOMaster(int slave, uint8_t* p);
    void linkTXPDOMaster(int slave, uint8_t* p);

    uint8_t* getSlaveRxPDOAddress(int slave);
    uint8_t* getSlaveTxPDOAddress(int slave);

    bool checkConfiguration(const EtherCATConfig& cfg);
    void dumpBusTopology();
    void createIOMap();
    void startMaster();

    void set_output_int16(uint16_t slave_no, uint8_t module_index, int16_t value);
    void get_input_int16(uint16_t slave_nb, uint8_t module_index, int16_t *value);
private:
    int autoconfig_slaves();
    void run();
    int _configuredSlaves;
    char _ioMap[128]; 
};
#endif /* ETHERCATMASTER_H */