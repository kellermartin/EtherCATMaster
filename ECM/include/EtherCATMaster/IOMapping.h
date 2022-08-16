#ifndef IOMAPPING_H
#define IOMAPPING_H
#include <cstdint>

const int EK1100_1 = 1;
const int EL1004_1 = 2;
const int EL2008_1 = 3;

const int NUMBER_OF_SLAVES = 3;

typedef struct
{
   uint8_t    input1;
   uint8_t    input2;
   uint8_t    input3;
   uint8_t    input4;
} EL1004_t;


typedef struct
{
   uint8_t    output1;
   uint8_t    output2;
   uint8_t    output3;
   uint8_t    output4;
   uint8_t    output5;
   uint8_t    output6;
   uint8_t    output7;
   uint8_t    output8;
} EL2008_t;
#endif /* IOMAPPING_H */