#ifndef FLASH_H
#define FLASH_H
#include <Arduino.h>
#include "flash.c"
//#include "_string.h"
#include "stm32f1xx_hal.h"
#include "uart.h"

#define ADD_TCP_PORT INT1_ADD
#define ADD_UPPER_TIMELIMIT INT2_ADD
#define ADD_LOWER_TIMELIMIT INT3_ADD
#define ADD_MSG_TIME_INTERVAL INT4_ADD
#define ADD_MSG_LIGHT_CONTROL INT5_ADD
#define ADD_INTERVAL_LIGHT_ON INT6_ADD
#define ADD_INTERVAL_LIGHT_OFF INT7_ADD
#define ADD_WATER_PUMP_TIMER INT8_ADD
#define ADD_PUMP_INTERVAL_A INT9_ADD
#define ADD_PUMP_INTERVAL_B INT10_ADD
#define ADD_PUMP_INTERVAL_C INT11_ADD
#define ADD_COOLER_TEMP_ON INT12_ADD
#define ADD_UPPER_COOLER_TEMP_OFF INT13_ADD
#define ADD_TIMEZONE INT14_ADD
#define ADD_WD_A_ACTIVATE INT15_ADD
#define ADD_WD_A_TIMEOUT INT16_ADD
#define ADD_WD_A_HIGH_POWER INT17_ADD
#define ADD_WD_A_LOW_POWER INT18_ADD
#define ADD_TEMP_SENSOR_MASK_TANK INT19_ADD
#define ADD_TEMP_SENSOR_MASK_OUT INT20_ADD
#define ADD_TEMP_SENSOR_MASK_INPUT INT21_ADD


#define ADD_PRE_PRE_LAST_SECTOR 0x0801F400  //- адрес предпредпоследнего сектора
#define ADD_PRE_LAST_SECTOR 0x0801F800 // -адрес предпоследнего сектора
#define ADD_LAST_SECTOR 0x0801FC00 // - адрес последнего сектора

#define ADD_USER_STRING_SECTOR 0x0801F000
#define ADD_STRING_SECTOR   ADD_PRE_PRE_LAST_SECTOR 
#define ADD_INT_SECTOR   ADD_PRE_LAST_SECTOR 
#define ADD_OW_SECTOR   ADD_LAST_SECTOR 

#define STR1U_ADD 0x0801F000
#define STR2U_ADD 0x0801F000+50
#define STR3U_ADD 0x0801F000+100
#define STR4U_ADD 0x0801F000+150
#define STR5U_ADD 0x0801F000+200
#define STR6U_ADD 0x0801F000+250
#define STR7U_ADD 0x0801F000+300
#define STR8U_ADD 0x0801F000+350

#define OW1_ADD 0x0801FC00
#define OW2_ADD 0x0801FC00+10
#define OW3_ADD 0x0801FC00+20
#define OW4_ADD 0x0801FC00+30
#define OW5_ADD 0x0801FC00+40
#define OW6_ADD 0x0801FC00+50
#define OW7_ADD 0x0801FC00+60
#define OW8_ADD 0x0801FC00+70
#define OW9_ADD 0x0801FC00+80
#define OW10_ADD 0x0801FC00+90
#define OW_NUM_ADD 0x0801FC00+100

#define STR1_ADD 0x0801F400
#define STR2_ADD 0x0801F400+50
#define STR3_ADD 0x0801F400+100
#define STR4_ADD 0x0801F400+150
#define STR5_ADD 0x0801F400+200
#define STR6_ADD 0x0801F400+250
#define STR7_ADD 0x0801F400+300
#define STR8_ADD 0x0801F400+350

#define INT1_ADD 0x0801F800
#define INT2_ADD 0x0801F800+4
#define INT3_ADD 0x0801F800+8
#define INT4_ADD 0x0801F800+12
#define INT5_ADD 0x0801F800+16
#define INT6_ADD 0x0801F800+20
#define INT7_ADD 0x0801F800+24
#define INT8_ADD 0x0801F800+28
#define INT9_ADD 0x0801F800+32
#define INT10_ADD 0x0801F800+36
#define INT11_ADD 0x0801F800+40
#define INT12_ADD 0x0801F800+44
#define INT13_ADD 0x0801F800+48
#define INT14_ADD 0x0801F800+52
#define INT15_ADD 0x0801F800+56
#define INT16_ADD 0x0801F800+60
#define INT17_ADD 0x0801F800+64
#define INT18_ADD 0x0801F800+68
#define INT19_ADD 0x0801F800+72
#define INT20_ADD 0x0801F800+76
#define INT21_ADD 0x0801F800+80


uint32_t read_flash32(uint32_t Data_adr);
bool write_flash64(uint32_t data_adr, uint64_t data);
bool write_flash32(uint32_t data_adr, uint32_t data);
bool flash_earse(uint32_t addr);
//bool save_string(uint32_t addr, char *str);
//void read_flash_string(uint32_t addr, char * str);
//void add_flash_string(uint32_t addr, char * str);
//void read_flash_string_limt(uint32_t addr, char * str, int limit);
//bool save_string_limit(uint32_t addr, char *str, int limit);
#endif
