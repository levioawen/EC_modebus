Датчик Gravity EC meter   и  Gravity PH meter
Ардуино про мини 16мгц 5в (328p)  и stm32f103c6t6
РС485 модбас  
Таблица адресов модбас
```
//Input register
//Input register

#define ec_address 0x0000
#define ec_k_valueLow_address 0x0001
#define ec_k_valueHigh_address 0x0002
#define ph_address 0x0010
#define ph_k_valueNeutral_address 0x0011
#define ph_k_valueAcid_address 0x0012

//Coils
#define ec_calib_high_start_address 0x0000
#define ec_calib_low_start_address 0x0001
#define ec_calib_high_successful_address 0x0002
#define ec_calib_low_successful_address 0x0003
#define ec_calib_error_address 0x0009

#define ph_calib_normal_start_address 0x0010
#define ph_calib_acid_start_address 0x0011
#define ph_calib_normal_successful_address 0x0011
#define ph_calib_acid_successful_address 0x0012
#define ph_calib_error_address 0x0019

#define calib_run_address 0x0020
#define temperature_option_address 0x0021
```
1. Для калибровки установить 1 в enterec_addres
2. Опустить измерительный щуп в раствор 1.413us/cm для калибровки на низкыом уровне или 12.88ms/cm для калибровки на высоком уровне(для лучшей точности следует провести 2 калибровки для двух растворов)
3. Установить 1 в calec_addres
4. Установить 1 в exitec_addres
5. Если получен флаг successful_address то калибровка удалась.  
   Установите флаги enterec_addres, calec_addres, exitec_addres на 0. И перейдите к следущей калибровке или к измерениям.  
   При получении флага error_address сбросить флаги enterec_addres, calec_addres, exitec_addres на 0 и повторить с попытку с пункта №1


Измерения проводятся вне калибровки(когда флаги enterec_addres, calec_addres, exitec_addres=0)
В папке workspace_modbus_pull сохраннёное рабоче пространство win программы modbus poll. File-Openworkspace + настроить ком порт


