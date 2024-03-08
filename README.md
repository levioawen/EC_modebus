Датчик Gravity EC meter  
Ардуино про мини 16мгц 5в (328p)  
РС485 модбас  
Таблица адресов модбас  
```
//Input register
#define EC_address 0x0000 // значение с датчика в мкСм/см3(uS/sm3)
#define temp_address 0x0001 //температура в Градусах цельсия*100
#define kvalueLow_address 0x0002 //Коэффицент для низкого уровня *100
#define kvalueHigh_address 0x0003 //Коэффицент для высокого уровня *100

//Coils входы
#define enterec_addres 0x0000 //вход в калибровку
#define calec_addres 0x0001 //калибровка
#define exitec_addres 0x0002 // выход и запись в еепром результата калибровки
//Coils выходы
#define run_addres 0x0003    // флаг в процесса калибровки
#define successful_address 0x0004  // успешная калибровка
#define error_address 0x0005     // ошибка при калибровке
```
1. Для калибровки установить 1 в enterec_addres  
2. Опустить измерительный щуп в раствор 1.413us/cm для калибровки на низкыом уровне или 12.88ms/cm для калибровки на высоком уровне(для лучшей точности следует провести 2 калибровки для двух растворов)
3. Установить 1 в calec_addres  
4. Установить 1 в exitec_addres  
5. Если получен флаг successful_address то калибровка удалась.  
Установите флаги enterec_addres, calec_addres, exitec_addres на 0. И перейдите к следущей калибровке или к измерениям.  
При получении флага error_address сбросить флаги enterec_addres, calec_addres, exitec_addres на 0 и повторить с попытку с пункта №1


Измерения проводятся вне калибровки(когда флаги enterec_addres, calec_addres, exitec_addres=0)
