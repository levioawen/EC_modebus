
//#include "flash.h"
#include <Arduino.h>

/*Строк 10 по 100 символов каждая*/



extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart1;

/*
bool save_string(uint32_t addr, char *str) {
    int l = length(str);
    l = l / 4;
    l++;

    uint32_t word[l];

    for (uint8_t i = 0; i < l; i++) {
        word[i] = *str;
        word[i] = word[i] << 8;
        str++;
        word[i] = word[i] | *str;
        word[i] = word[i] << 8;
        str++;
        word[i] = word[i] | *str;
        word[i] = word[i] << 8;
        str++;
        word[i] = word[i] | *str;
        str++;

        if (write_flash32(addr, word[i]) == false)return false;
        addr = addr + 4;
    }
    return true;
}

bool save_string_limit(uint32_t addr, char *str, int limit) {

    int l = limit / 4;
    //l++;

    uint32_t word[l];

    for (uint8_t i = 0; i < l; i++) {
        word[i] = *str;
        word[i] = word[i] << 8;
        str++;
        word[i] = word[i] | *str;
        word[i] = word[i] << 8;
        str++;
        word[i] = word[i] | *str;
        word[i] = word[i] << 8;
        str++;
        word[i] = word[i] | *str;
        str++;

        if (write_flash32(addr, word[i]) == false)return false;
        addr = addr + 4;
    }
    return true;
}


void read_flash_string(uint32_t addr, char *str) {
    //char ch[5];
    uint32_t word[13];
    for (uint8_t i = 0; i < 12; i++) {
        word[i] = read_flash32(addr);

        *str = word[i] >> 24;
        str++;
        *str = word[i] >> 16;
        str++;
        *str = word[i] >> 8;
        str++;
        *str = word[i];
        if (*str == 0x00) break;
        str++;
        addr += 4;
    }
}


void read_flash_string_limt(uint32_t addr, char *str, int limit) {
    //char ch[5];
    uint32_t word[25];
    for (uint8_t i = 0; i < limit / 4; i++) {
        word[i] = read_flash32(addr);

        *str = word[i] >> 24;
        str++;
        *str = word[i] >> 16;
        str++;
        *str = word[i] >> 8;
        str++;
        *str = word[i];
        if (*str == 0x00) break;
        str++;
        addr += 4;
    }
}


void add_flash_string(uint32_t addr, char *str) {
    int index = length(str);
    str += index;
    read_flash_string(addr, str);
}
*/
uint32_t read_flash32(uint32_t Data_adr) {
    return *(uint32_t *) Data_adr;
}


bool write_flash32(uint32_t data_adr, uint32_t data) {
    HAL_FLASH_Unlock();
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, data_adr, data) == HAL_OK) {
        HAL_FLASH_Lock();
        return true;
    } else {
        HAL_FLASH_Lock();
        return false;
    }

}

bool flash_earse(uint32_t addr) {

    FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PageError = 0;


    EraseInitStruct.TypeErase = TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = addr;
    EraseInitStruct.NbPages = 1;
    HAL_FLASH_Unlock();

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) == HAL_OK) {
        HAL_FLASH_Lock();
        return true;
    } else {
        HAL_FLASH_Lock();
        return false;
    }
}

/*bool write_flash64(uint32_t data_adr, uint64_t data)
{  	 
	//HAL_FLASH_Unlock();
	data_adr=ADD_PRE_PRE_LAST_SECTOR; //определяем в предпоследний сектор
	
	  if (HAL_FLASH_Program(TYPEPROGRAM_WORD,data_adr,data) == HAL_OK)
         {           
					 HAL_FLASH_Lock();
					 return true;
         }
         else
         {
            HAL_FLASH_Lock();
            return false;
         }

}*/


