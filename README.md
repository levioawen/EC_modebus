# Адаптер для датчиков для подключения к сети модбас RS485

- [Датчик электропроводности Gravity: Analog Electrical Conductivity Sensor /Meter V2](https://www.dfrobot.com/product-1123.html)
- [Датчик PH(кисло\щелочной) Gravity: Analog Industrial pH Sensor / Meter Pro Kit V2](https://www.dfrobot.com/product-2069.html)

Прошивка написана с возможностью подключить оба датчика к одному контроллеру одновременно, но может использоваться и с одним из них, не требует при этом модификации.  
Прошивка написанна с использоованием Arduino фрэймворка и може запускаться на разный микроконтроллерах, испытанные:  
- Arduino Uno  
- Arduino pro micro 5v  
- Stm32BluePill (stm32f103c8t6  64kb flash) 

## Регистры модбас (/src/Address_map.h)


### Input register
Эти регистры доступны только для чтения
| Имя   |      Адрес hex(dec)      |  Множитель | Пояснение |
|----------|:-------------:|------:|------:|
|ec_address             |0x0000(0)| 1 | Значение с датчика EC в uS/cm
|ec_k_valueLow_address| 0x0001(1) | 100| Коэффициент для низкого раствора
|ec_k_valueHigh_address| 0x0002(2) | 100| Коэффициент для насыщеного раствора
|ph_address| 0x000A(10)|  100| Значение с датчика PH раствора (0-14)
|ph_k_valueNeutral_address| 0x000B(11)  |1000| Коэффициент для нейтрального раствора
|ph_k_valueAcid_address| 0x000C(12)|1000|Коэффициент для кислого раствора

### Holding register
Эти регистры доступны  для чтения и записи
| Имя   |      Адрес hex(dec)      |  Множитель | Пояснение |
|----------|:-------------:|------:|------:|
|temperature_in_out_address|0x0000(0)| 100 | Значение температуры, при  temperature_option_address = 1 значения можно записывать. При  temperature_option_address = 0 значение можно считывать
|modbus_id_address| 0x0001(1) | 1| Установить новый адрес устройства(нельзя использовать 0 и значения выше 247 согласно спецификации)
|modbus_baud_rate_address| 0x0002(2) | 100| Установить скорость(допустимы значения 4800, 9600, 19200, 38400, 57600, 115200)

###  Coils
Эти регистры доступны  для чтения и записи
| Имя   |      Адрес hex(dec)      |  Множитель | Пояснение |
|----------|:-------------:|------:|------:|
|ec_calib_high_start_address|0x0000(0)| 1 | Значение с датчика EC в uS/cm
|ec_calib_low_start_address| 0x0001(1) | 1| Коэффициент для низкого раствора
|ec_calib_high_successful_address| 0x0002(2) | 1| Коэффициент для насыщеного раствора
|ec_calib_low_successful_address| 0x0003(3)|  1| Значение с датчика PH раствора (0-14)
|ph_calib_normal_start_address|0x0000(0)| 1 | Значение с датчика EC в uS/cm
|ph_calib_acid_start_address| 0x0001(1) | 1| Коэффициент для низкого раствора
|ph_calib_normal_successful_address| 0x0002(2) | 1| Коэффициент для насыщеного раствора
|ph_calib_acid_successful_address| 0x000A(10)|  1| Значение с датчика PH раствора (0-14)
|ph_calib_error_address| 0x000B(11)  |1| Коэффициент для нейтрального раствора

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


