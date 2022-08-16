#ifndef LENZEI550DEFINES_H
#define LENZEI550DEFINES_H
#include <cstdint>

// PDO's are seen from slave side
typedef struct PACKED {
    uint16_t statusWord; // 0x6041
    uint16_t actualVelocity; // 0x6044
    uint16_t errorCode; // 0x603F 
} Lenze_I550_TXPDO;

typedef struct PACKED {
    uint16_t controlWord; // 0x6040
    uint16_t setVelocity; // 0x6042
} Lenze_I550_RXPDO;
#endif
