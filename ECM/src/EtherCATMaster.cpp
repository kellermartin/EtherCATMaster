#include "EtherCATMaster/EtherCATMaster.h"
#include "ethercat.h"

#include <iostream>
#include <thread>
#include <bitset>

EtherCATMaster::EtherCATMaster()
{
    _configuredSlaves = 0;
}
bool EtherCATMaster::initialize(const std::string ifname){
    // init ethernet adapter
    if (ec_init(ifname.c_str()))
    {
        std::cout << "Succesfully initialized EtherCAT Master on " << ifname << std::endl;
        return true;
    }
    else
    {
        std::cout << "ERROR - Failed to initialize EtherCAT Master on " << ifname << std::endl;
        return false;
    }
    
}

void EtherCATMaster::shutdown()
{
    // todo cleanup here
    ec_close();
}

int EtherCATMaster::autoconfig_slaves()
{
    std::cout << "Configuring slaves on bus ..." << std::endl;
    auto workcounter = 0;
    if (ec_config_init(FALSE) > 0)
    {
        std::cout << "Found and configured " << ec_slavecount <<" slaves on bus ..." << std::endl;
        return ec_slavecount;
    }         
    else
    {
        std::cout << "No slaves found on bus ..." << std::endl;
        return 0; 
    }
       

}

unsigned int EtherCATMaster::getnumberOfSlaves()
{
    return _configuredSlaves;
}

void EtherCATMaster::startMaster()
{
    if (autoconfig_slaves() > 0)
    {
        // Map slaves
        std::cout << "Trying to read SDO 0x6041 "<< std::endl;
        int value = 0;
        int size = 4;
        uint16 index = 0x6041;
        uint8 subIDX = 0;


        ec_SDOread(4,index,subIDX,FALSE,&size,&value,EC_TIMEOUTRXM);
        std::cout << "Read SDO 0x6041 with return value "<< std::hex << value << std::endl;
        std::bitset<16> statusWordBitset(value);
        std::cout << "Bitset dump "<< statusWordBitset.to_string() << std::endl;
        
        char io[128];
        auto usedIOMemory = ec_config_map(&io);
        std::cout << "Used Memory for IO Map " << std::dec << usedIOMemory << " bytes " << std::endl;
        
        
        // configure DC
        std::cout << "Configuring distributed clock " << std::endl;
        ec_configdc();
        std::cout << "Distributed clock configured " << std::endl;

        // check all slaves are in safe op state - this is done via slave 0 which is owned by the master
        ec_statecheck(0,EC_STATE_SAFE_OP,EC_TIMEOUTSTATE);
        
        
        //check configured topology against actual slaves on bus

        // dump network topology
        // make a function to do this

        std::cout << "Send processdata " << std::endl;
        // if topology is OK - request operational state for all slaves
        /* send one valid process data to make outputs in slaves happy*/
        ec_slave[0].state = EC_STATE_OPERATIONAL;
        ec_writestate(0);
        
        //TODO change this 
        while (ec_slave[0].state != EC_STATE_OPERATIONAL)
        {
            ec_send_processdata();
            auto wkc = ec_receive_processdata(EC_TIMEOUTRET);
            std::cout << "WKC " << wkc <<  std::endl;
            ec_statecheck(0, EC_STATE_OPERATIONAL, 50000);
        }

        std::cout << "All slaves in OPERATIONAL " << std::endl;     
        dumpBusTopology();

       
        bool doRun = true;
        while(doRun)
        {
           
            ec_send_processdata();
            auto wkc = ec_receive_processdata(EC_TIMEOUTRET);

            //std::cout << "Input IO Map " << std::to_string(io[1]) << std::endl;
            //std::cout << "Output IO Map " << std::to_string(io[0]) << std::endl;

            // check all slaves are operational
            if ( wkc < ec_slavecount )
            {
                std::cout << "ERROR - Expected WKC is less than number of configured slaves - stopping master thread" << std::endl;
                std::cout << "ERROR - WKC is " << wkc << std::endl;
                std::cout << "ERROR - Configured slaves are " << ec_slavecount << std::endl;
                //doRun = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
        //ec_close();
    }
    else{
        std::cout << " ERROR - failed to auto-configure slaves" << std::endl;
    }
   
}

void EtherCATMaster::dumpBusTopology()
{
    std::cout << "Starting topology dump " << std::endl;
    for(int i=1;i<= ec_slavecount; i++){
        //ec_send_processdata();
        //auto wkc = ec_receive_processdata(EC_TIMEOUTRET);
        std::string name =  ec_slave[i].name;
        auto configuredAddress = ec_slave[i].configadr;
        
        auto outputbits = ec_slave[i].Obits;
        auto inputbits = ec_slave[i].Ibits;
        auto hasDC = ec_slave[i].hasdc;

        std::cout << "Slave name: " << name << 
        " Address := " << std::hex <<configuredAddress << 
        " outputbits := " << outputbits << 
        " inputbits := " << inputbits << 
        " HasDC := " << hasDC << std::endl;
        std::this_thread::sleep_for(std::chrono::microseconds(5000));  
    }
    
    
}

 bool EtherCATMaster::checkConfiguration(const EtherCATConfig& cfg)
 {
    for(int i=1;i<= ec_slavecount; i++){
        
    }
    return false;
 }

 void EtherCATMaster::set_output_int16 (uint16_t slave_no, uint8_t module_index, int16_t value)
{
   uint8_t *data_ptr;
   data_ptr = ec_slave[slave_no].outputs;
   /* Move pointer to correct module index*/
   data_ptr += module_index * 2;
   /* Read value byte by byte since all targets can't handle misaligned
    * addresses
    */
   *data_ptr++ = (value >> 0) & 0xFF;
   *data_ptr++ = (value >> 8) & 0xFF;
}

void EtherCATMaster::get_input_int16(uint16_t slave_nb, uint8_t module_index, int16_t *value)
 {
    uint8_t *data_ptr;
    
    data_ptr = ec_slave[slave_nb].inputs;
   
  	/* Move pointer to correct module index*/
  	data_ptr += module_index * 2;
    /* Read value byte by byte since all targets can't handle misaligned
        addresses */
    *value |= ((*data_ptr++) & 0xFF);
    *value |= ((*data_ptr) << 8) & 0xff00;
 }

int EtherCATMaster::writeSDO(uint16 Slave, uint16 Index, uint8 SubIndex, boolean CA, int psize, void *p)
{
    return ec_SDOwrite(Slave,Index,SubIndex,CA,psize,p,EC_TIMEOUTRXM);
}
int EtherCATMaster::readSDO(uint16 slave, uint16 index, uint8 subindex, boolean CA, int *psize, void *p)
{
    return ec_SDOread(slave,index,subindex,CA,psize,p,EC_TIMEOUTRXM);
}

void EtherCATMaster::linkTXPDOMaster(int slave, uint8_t* p)
{
    std::cout << "Obytes "<< ec_slave[slave].Obytes << std::endl;
    p = ec_slave[slave].outputs;
    uint16_t word1 = *(uint16_t*) ec_slave[slave].outputs;
    std::cout << "word1 "<< word1<< std::endl;

}
void EtherCATMaster::linkRXPDOMaster(int slave, uint8_t* p)
{
    std::cout << "Ibytes "<< ec_slave[slave].Ibytes << std::endl;
    p = ec_slave[slave].inputs;
    uint16_t word1 = *(uint16_t*) ec_slave[slave].inputs;
    std::cout << "word1 "<< word1<< std::endl;
}

uint8_t* EtherCATMaster::getSlaveRxPDOAddress(int slave){
    return ec_slave[slave].outputs;
}

uint8_t* EtherCATMaster::getSlaveTxPDOAddress(int slave){
    return ec_slave[slave].inputs;
}