/** \file
 * \brief Example code for Simple Open EtherCAT master
 *
 * Usage : simple_test [ifname1]
 * ifname is NIC interface, f.e. eth0
 *
 * This is a minimal test.
 *
 * (c)Arthur Ketels 2010 - 2011
 */

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <thread>
#include <iostream>
#include <memory>
#include <bitset>
#include "ethercat.h"
#include "EtherCATMaster/EtherCATMaster.h"
#include "EtherCATMaster/CIA402Defines.h"
#include "EtherCATMaster/LenzeI550Defines.h"

auto ecMaster = std::shared_ptr<EtherCATMaster>();
auto globalstatusword = 0;
void toggleOutput()
{
    bool toggleFlag;
    int16_t inputValue = 0;
    auto pos = 0;
    std::this_thread::sleep_for(std::chrono::seconds(2));
    while(true)
    {
        if(ec_statecheck(0,EC_STATE_OPERATIONAL,EC_TIMEOUTRET)){
           
            if(toggleFlag){
                std::cout << "Setting output HIGH" << std::endl;
                std::bitset<4> el2004;
                el2004.set(pos,true);
                ecMaster->set_output_int16(3,0,el2004.to_ulong());
                
                if(pos == 3){
                    pos = 0;
                }
                else
                {
                    pos++;
                }

                toggleFlag = false;    
            }
            else{
                std::cout << "Setting output LOW" << std::endl;

                ecMaster->set_output_int16(3,0,0);
                toggleFlag = true;;   
            }
            ecMaster->get_input_int16(2,0,&inputValue);
            
            std::bitset<4> el1004(inputValue);
            std::cout << "Input Value := " << inputValue <<  std::endl;
        }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}
CIA402_StatusState getCIAStatusState(uint16_t statusWord){
    std::bitset<16> statusBitset(statusWord);

    if(!statusBitset[0] && !statusBitset[1] && !statusBitset[2] && !statusBitset[3] && !statusBitset[6])
        return NOT_READY_TO_SWITCH_ON;
    else if(!statusBitset[0] && !statusBitset[1] && !statusBitset[2] && !statusBitset[3] && statusBitset[6])
        return SWITCH_ON_DISABLED;
    else if(statusBitset[0] && !statusBitset[1] && !statusBitset[2] && !statusBitset[3] && statusBitset[5] && !statusBitset[6])
        return READY_TO_SWITCH_ON;
    else if(statusBitset[0] && statusBitset[1] && !statusBitset[2] && !statusBitset[3] && statusBitset[5] && !statusBitset[6])
        return SWITCHED_ON;
    else if(statusBitset[0] && statusBitset[1] && statusBitset[2] && !statusBitset[3] && statusBitset[5] && !statusBitset[6])
        return OPERATION_ENABLED;
    else if(statusBitset[0] && statusBitset[1] && statusBitset[2] && !statusBitset[3] && !statusBitset[5] && !statusBitset[6])
        return QUICK_STOP_ACTIVE;
    else if(statusBitset[0] && statusBitset[1] && statusBitset[2] && statusBitset[3] && !statusBitset[6])
        return FAULT_REACTION_ACTIVE;
    else if(!statusBitset[0] && !statusBitset[1] && !statusBitset[2] && statusBitset[3] && !statusBitset[6])
        return FAULT;    
}

void setupDrive()
{

}
void handleDrive()
{
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Lenze_I550_RXPDO rxPDO;
    Lenze_I550_TXPDO txPDO;

    std::cout << "Size of rxPDO = " << sizeof(rxPDO) << std::endl;
    std::cout << "Size of txPDO = " << sizeof(txPDO) << std::endl;

    auto pSlaveTxPDO = ecMaster->getSlaveTxPDOAddress(4);
    txPDO = *(Lenze_I550_TXPDO*)pSlaveTxPDO;
    std::cout << " **Status word txPDO = " << txPDO.statusWord << std::endl;

    auto pSlaveRxPDO = ecMaster->getSlaveRxPDOAddress(4);
    rxPDO = *(Lenze_I550_RXPDO*)pSlaveRxPDO;
    std::cout << " **Control word rxPDO = " << rxPDO.controlWord << std::endl;
    

    while(true)
    {
        if(ec_statecheck(0,EC_STATE_OPERATIONAL,EC_TIMEOUTRET))
        {
            /********** DON'T CHANGE ***********************/
            int16_t statusword = 0;
            ecMaster->get_input_int16(4,0,&statusword);
            std::cout << "Status word = " << statusword << std::endl;
            
            auto statusState = getCIAStatusState(txPDO.statusWord);

            int16_t controlWord = 0;
            switch(statusState)
            {
                case (CIA402_StatusState::NOT_READY_TO_SWITCH_ON): {
                    /* Now the FSM should automatically go to Switch_on_disabled*/
                    std::cout << "Not_ready_to_switch_on" << std::endl;
                    break;
                }
                case (CIA402_StatusState::SWITCH_ON_DISABLED): {
                    /* Automatic transition (2)*/
                    std::cout << "Switch_on_disabled" << std::endl;
                    //rxPDO.controlWord = 0;
                    //rxPDO.controlWord |= (1 << control_enable_voltage) | (1 << control_quick_stop);

                    controlWord |= (1 << control_enable_voltage) | (1 << control_quick_stop);
                    controlWord = 6;
                    ecMaster->set_output_int16(4,0,controlWord);
                    break;
                }
                case (CIA402_StatusState::READY_TO_SWITCH_ON): {
                    /* Switch on command for transition (3) */
                    std::cout << "Ready_to_switch_on" << std::endl;

                    controlWord |= 1 << control_switch_on;
                    controlWord = 7;
                    ecMaster->set_output_int16(4,0,controlWord);
                    //rxPDO.controlWord |= 1 << control_switch_on;
                    break;
                }
                case (CIA402_StatusState::SWITCHED_ON): {
                    /* Enable operation command for transition (4) */
                    std::cout << "Switch_on" << std::endl;
                    //rxPDO.controlWord |= 1 << control_enable_operation;

                    controlWord |= 1 << control_enable_operation;
                    controlWord = 15;
                    ecMaster->set_output_int16(4,0,controlWord);
                    break;
                }
                case (CIA402_StatusState::OPERATION_ENABLED): {
                    std::cout << "Operation_enabled" << std::endl;
                    /* Setting modes of operation
                            * Value Description
                            -128...-2 Reserved
                            -1 No mode
                            0 Reserved
                            1 Profile position mode
                            2 Velocity (not supported)
                            3 Profiled velocity mode
                            4 Torque profiled mode
                            5 Reserved
                            6 Homing mode
                            7 Interpolated position mode
                            8 Cyclic Synchronous position
                            ...127 Reserved*/
                            
                    //uint16_t mode = 8; /* Setting Cyclic Synchronous position */
                    //int mode_size = sizeof(mode);
                    //SDO_result = ec_SDOwrite(SLAVE_NB, MODES_OF_OPERATION_INDEX, 0,
                    //                       0, mode_size, &mode, EC_TIMEOUTRXM);
                    controlWord = 15;
                    ecMaster->set_output_int16(4,0,controlWord);
                    uint16_t vel = 1000;
                    ecMaster->set_output_int16(4,1,vel);


                    int16_t actVel = 0;
                    ecMaster->get_input_int16(4,1,&actVel);
                    std::cout << "Status word = " << statusword << std::endl;
                    std::cout << "Set velocity = " << std::dec << vel << std::endl;
                    std::cout << "Actual velocity = " << std::dec  << actVel << std::endl;
                    break;
                }
                case (CIA402_StatusState::QUICK_STOP_ACTIVE): {
                    break;
                }
                case (CIA402_StatusState::FAULT_REACTION_ACTIVE): {
                    break;
                }
                case (CIA402_StatusState::FAULT): {
                    /* Returning to Switch on Disabled */
                    //rxPDO.controlWord = (1 << control_fault_reset);
 
                    controlWord  |=  (1 << control_fault_reset);
                    ecMaster->set_output_int16(4,0,controlWord);
                    break;
                }
                default:{
                    std::cout << "Unkown status" << std::endl;
                    std::bitset<16> statuswordbitsetunmasked(statusword);
                    std::cout << "Status bits received unmasked (bin)   = " << std::hex << statuswordbitsetunmasked << std::endl;
                    // std::bitset<16> statuswordbitset(statusword);
                    // std::cout << "Status bits received (bin)            = " << statuswordbitset << std::endl;
                    //rxPDO.controlWord = (1 << control_fault_reset);
                   
                   //controlWord  |=  (1 << control_fault_reset);
                    //ecMaster->set_output_int16(4,0,controlWord);
                    break;
                }
            
            }           
            std::cout << "Control word = " << controlWord << std::endl;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

int main(int argc, char *argv[])
{
    std::cout << "SOEM (Simple Open EtherCAT Master)" << std::endl;

    //auto ecMaster = std::shared_ptr<EtherCATMaster>();

    auto ifName = "enp1s0";
    ecMaster->initialize(ifName);
    std::thread drivehandler(handleDrive);
    //std::thread ioHandler(toggleOutput);
    //ecMaster->startMaster();
    std::thread masterThread(&EtherCATMaster::startMaster,ecMaster);
    


    while(true){
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    
    return (0);
}
