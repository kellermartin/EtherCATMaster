#ifndef CIA402DEFINES_H
#define CIA402DEFINES_H
#include <bitset>
#define STATUS_WORD_MASK(x)          (x &= 0x6F)
#define MODES_OF_OPERATION_INDEX	(0x6060)

// enum CIA402_CONTROLBIT{
//    SWITCH_ON = 0,
//    ENABLE_VOLTAGE,
//    QUICK_STOP,
//    ENABLE_OPERATION,
//    OMS_4,
//    OMS_5,
//    OMS_6,
//    FAULT_RESET,
//    HALT,
//    OMS_9,
//    RESERVED,
//    MANUFACTURER_SPECIFIC 
// };

// enum CIA402_STATUSBIT{
//     READY_TO_SWITCH_ON = 0,
//     SWITCHED_ON,
//     OPERATION_ENABLE,
//     FAULT,
//     VOLTAGE_ENABLED,
//     QUICK_STOP,
//     SWITCH_ON_DISABLED,
//     WARNING,
//     MANUFACTURER_SPECIFIC,
//     REMOTE,
//     TARGET_REACHED,
//     INTERNAL_LIMIT_ACTIVE,
//     OMS_12,
//     OMS_13,
//     MS_14,
//     MS_15
// };

typedef enum{
       control_switch_on = 0,
       control_enable_voltage = 1,
       control_quick_stop = 2,
       control_enable_operation = 3,
       control_fault_reset = 7,
       control_4 = 4,
       control_5 = 5,
       control_6 = 6,
       control_8 = 8
   }control_bit_t;

  
typedef enum{
       /*xxxx xxxx x0xx 0000*/ NOT_READY_TO_SWITCH_ON, 
       /*xxxx xxxx x1xx 0000*/ SWITCH_ON_DISABLED,
       /*xxxx xxxx x01x 0001*/ READY_TO_SWITCH_ON,
       /*xxxx xxxx x01x 0011*/ SWITCHED_ON,
       /*xxxx xxxx x01x 0111*/ OPERATION_ENABLED,
       /*xxxx xxxx x00x 0111*/ QUICK_STOP_ACTIVE,
       /*xxxx xxxx x0xx 1111*/ FAULT_REACTION_ACTIVE,
       /*xxxx xxxx x0xx 1000*/ FAULT
} CIA402_StatusState;
#endif