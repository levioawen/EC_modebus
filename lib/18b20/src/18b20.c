#include "18b20.h"
#include "_string.h"
#include "flash.h"
#include "at_service.h"

#define PORT  GPIOB   //указать порт, к которому подключены датчики
#define PIN 9
#define SKIP_ROM_ADR 0xCC // пропуск адресации
#define SEARCH_ROM 0xF0 // поиск устройств
#define CONVERT_TEMP 0x44 // запускает преобразование температуры
#define READ_DATA_COMAND 0xBE //команда чтения содержимого памяти


uint8_t temp_sensor_i;


void one_wire_search() {
    ow_reset_search();
    flash_earse(ADD_OW_SECTOR);
    char addr[9];
    clearArray(addr, 9);
    int i = 0;
    while (ow_search(addr)) {
        save_string_limit(OW1_ADD + 10 * i, addr, 8);
        i++;
        HAL_Delay(200);
        if (i>9) return;
    }
    write_flash32(OW_NUM_ADD, i);
    ow_reset_search();

//    char ch[3] = {0,};
//    char msg[30] = {0,};
//    concat(msg, "Serched ds18b20: ", msg);
//    itoa(i,ch,3);
//    concat(msg, ch, msg);
//    add_que_info_msg(msg);

}


/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);

static void MX_GPIO_Init(void);

static void MX_TIM3_Init(void);

static void delay_us(uint16_t us);

//*********************************************************************************************
//function  импульс сброса                                                                   //
//argument  маска порта                                                                      //
//return    0 - устройство обнаружен, 1 - не обнаружено, 2 - к.з. на линии                   //
//*********************************************************************************************
uint8_t ds_reset_pulse(uint16_t PinMask) {
    uint16_t result;

    if ((PORT->IDR & PinMask) == 0) return 2;         //проверить линию на отсутствие замыкания
    PORT->ODR &= ~PinMask;                          //потянуть шину к земле
    //TIMER->CNT = 0;
    // while(TIMER->CNT<480){};                        //ждать 480 микросекунд
    //HAL_Delay(1);
    delay_us(480);
    PORT->ODR |= PinMask;                          //отпустить шину
    //while(TIMER->CNT<550){};                        //ждать 70 микросекунд
    delay_us(70);
    result = PORT->IDR & PinMask;                 //прочитать шину
    // while(TIMER->CNT<960){};                        //дождаться окончания инициализации
    delay_us(960);
    //HAL_Delay(1);
    if (result) return 1;                            //датчик не обнаружен
    return 0;                                       //датчик обнаружен
}


//*********************************************************************************************
//function  передача бита                                                                    //
//argument  значение передаваемого бита,маска порта                                          //
//return    none                                                                             //
//*********************************************************************************************
void ds_write_bit(uint8_t bit, uint16_t PinMask) {
    //TIMER->CNT = 0;
    PORT->ODR &= ~PinMask;                          //потянуть шину к земле
    //while(TIMER->CNT<2){};                          //ждать 1 микросекунду
    delay_us(1);
    if (bit) PORT->ODR |= PinMask;                  //если передаем 1, то отпускаем шину
    // while(TIMER->CNT<60){};                         //задержка 60 микросекунд
    delay_us(60);
    PORT->ODR |= PinMask;                          //отпускаем шину
}

//*********************************************************************************************
//function  чтение бита                                                                      //
//argument  маска порта                                                                      //
//return    прочитанный бит                                                                  //
//*********************************************************************************************
uint16_t ds_read_bit(uint16_t PinMask) {
    uint16_t result;

    //TIMER->CNT = 0;
    PORT->ODR &= ~PinMask;                          //потянуть шину к земле
    //while(TIMER->CNT<2){};
    delay_us(1);
    PORT->ODR |= PinMask;                          //отпускаем шину
    //while(TIMER->CNT<15){};                         //задержка 15 микросекунд
    delay_us(1);
    result = PORT->IDR & PinMask;              //прочитать шину
    //while(TIMER->CNT<60){};                         //оставшееся время
    delay_us(20);
    return result;                                  //возвратить результат
}

//*********************************************************************************************
//function  запись байта                                                                     //
//argument  передаваемый байт,маска порта                                                    //
//return    none                                                                             //
//*********************************************************************************************
void ds_write_byte(uint8_t byte, uint16_t PinMask) {
    uint8_t i;
    for (i = 0; i < 8; i++) ds_write_bit(byte & (1 << i), PinMask);
}

//*********************************************************************************************
//function  чтение байта                                                                     //
//argument  маска порта                                                                      //
//return    прочитанный байт                                                                 //
//*********************************************************************************************
uint8_t ds_read_byte(uint16_t PinMask) {
    uint8_t i, result = 0;
    for (i = 0; i < 8; i++)
        if (ds_read_bit(PinMask)) result |= 1 << i;
    return result;
}

void send_query(uint8_t rom[8]) {
    ds_reset_pulse(1 << PIN);          //послать импульс сброса
    select_(rom);
    ds_write_byte(CONVERT_TEMP, 1 << PIN);
}


uint8_t crc_8( uint8_t *addr, uint8_t len)
{
    uint8_t crc=0;

    for (uint8_t i=0; i<len;i++)
    {
        uint8_t inbyte = addr[i];
        for (uint8_t j=0;j<8;j++)
        {
            uint8_t mix = (crc ^ inbyte) & 0x01;
            crc >>= 1;
            if (mix)
                crc ^= 0x8C;

            inbyte >>= 1;
        }
    }
    return crc;
}

int get_data(uint8_t rom[8]) {
    uint8_t ds_buff[9];
    ds_reset_pulse(1 << PIN);          //послать импульс сброса
    select_(rom);
    ds_write_byte(READ_DATA_COMAND, 1 << PIN);      //команда, заставляющая датчик выдать 9 байт своей памяти
    for (int i = 0; i < 9; i++)           //прочитать 9 байт в массив
        ds_buff[i] = ds_read_byte(1 << PIN);

    int temp;
    //CRC control
    volatile uint8_t crc = crc_8(ds_buff, 8);
    if (crc!=(uint8_t)ds_buff[8]) return -4000;

    //read temperature
    temp = ds_buff[1];
    temp = temp << 8;
    temp |= ds_buff[0];
    temp = temp * 10;
    temp = temp >> 4;//отбрасываем дробную чать
    if (temp > 10000){//если температура <0
        temp = 40960 - temp;
        temp = temp * -1;
        return temp;
    } else

        return temp;
}

int read_temp(uint8_t rom[8]) {
    send_query(rom);//запустить преобразование
    HAL_Delay(750);
    return get_data(rom);//дать время для измерения
}

void query_temp_saved_sensors() {
    int num_sensors = read_flash32(OW_NUM_ADD);
    if (num_sensors>9) num_sensors = 9;
    for (int i = 0; i < num_sensors; i++) {
        char addr[8];
        clearArray(addr, 8);
        read_flash_string_limt(OW1_ADD + i * 10, addr, 8);
        send_query(addr);
    }
}


#define PC_BUFF_SIZE 3

typedef struct ring_buffer {
    int buffer[PC_BUFF_SIZE];
    int counter_buffer;
    long buff_sum;
} ring_buffer;

int calc_ring_buff(ring_buffer *ring_buffer, int value) {
    ring_buffer->buff_sum += value;
    ring_buffer->buff_sum -= ring_buffer->buffer[ring_buffer->counter_buffer];
    ring_buffer->buffer[ring_buffer->counter_buffer] = value;
    ring_buffer->counter_buffer++;
    if (ring_buffer->counter_buffer == PC_BUFF_SIZE) ring_buffer->counter_buffer = 0;
    return ring_buffer->buff_sum / PC_BUFF_SIZE;
}

static ring_buffer temp_buffer[10];

int get_temp_saved_sensors(int *sensor_temp){
    int num_sensors = read_flash32(OW_NUM_ADD);
    if (num_sensors > 9) return 0;
        for (int i = 0; i < 15; i++){
            sensor_temp[i] = -4000;
        }
    for (int i = 0; i < num_sensors; i++) {
        uint8_t addr[8];
        clearArray(addr, 8);
        read_flash_string_limt(OW1_ADD + i * 10, addr, 8);
        int sensor_data = get_data(addr);
        static uint8_t count_error = 0;
        if (sensor_data != -4000) {
            sensor_temp[i] = calc_ring_buff(&temp_buffer[i], sensor_data);

        }else{
            count_error++;
            if (count_error > 30){
                sensor_temp[i] = sensor_data;
            }
        }
    }
    return num_sensors;
}

int read_temp_saved_sensors(int *sensor_temp) {
    query_temp_saved_sensors();
    HAL_Delay(750);
    return get_temp_saved_sensors(sensor_temp);
}

int read_temp_skip_rom(void) {
    uint8_t ds_buff[9];
    ds_reset_pulse(1 << PIN);          //послать импульс сброса                                       /
    ds_write_byte(SKIP_ROM_ADR, 1 << PIN);//разрешить доступ к датчику не используя адрес
    ds_write_byte(CONVERT_TEMP, 1 << PIN);      //запустить преобразование
    HAL_Delay(850);               //дать время для измерения
    ds_reset_pulse(1 << PIN);          //послать импульс сброса
    ds_write_byte(SKIP_ROM_ADR, 1 << PIN);//разрешить доступ к датчику не используя адрес
    ds_write_byte(READ_DATA_COMAND, 1 << PIN);      //команда, заставляющая датчик выдать 9 байт своей памяти
    for (int i = 0; i < 9; i++)           //прочитать 9 байт в массив
        ds_buff[i] = ds_read_byte(1 << PIN);

    uint8_t crc = crc_8(ds_buff, 8);
    if (crc!=(uint8_t)ds_buff[8]) return -4000;
    int temp;

    temp = ds_buff[1];
    temp = temp << 8;
    temp |= ds_buff[0];
    temp = temp * 10;
    temp = temp >> 4;//отбрасываем дробную чать
    if (temp > 10000)//если температура <0
    {
        temp = 40960 - temp;
        temp = temp * -1;
        return temp;
    } else

        return temp;
}


uint8_t address[8];
int searchJunction;
uint8_t searchExhausted;

void ow_reset_search() {
    uint8_t i;
    searchJunction = -1;
    searchExhausted = 0;
    for (i = 7;; i--) {
        address[i] = 0;
        if (i == 0) break;
    }
}

void select_(uint8_t *rom) {
    ds_reset_pulse(1 << PIN);          //послать импульс сброса
    ds_write_byte(0x55, 1 << PIN);
    for (int i = 0; i < 8; i++) ds_write_byte(rom[i], 1 << PIN);
}

void ow_ini() {
    HAL_Delay(100);
    ds_reset_pulse(1 << PIN);
    ds_read_bit(1 << PIN);
    ds_read_bit(1 << PIN);
    ds_reset_pulse(1 << PIN);
    HAL_Delay(100);
}

uint8_t ow_search(uint8_t *new_addr) {
    uint8_t i;
    int lastJunction = -1;
    uint8_t done = 1;
    if (searchExhausted) return 0;
    ds_reset_pulse(1 << PIN);//послать импульс сброса
    ds_write_byte(SEARCH_ROM, 1 << PIN);
    for (i = 0; i < 64; i++) {
        uint8_t a = 0;
        if (ds_read_bit(1 << PIN)) a = 1;
        uint8_t nota = 0;
        if (ds_read_bit(1 << PIN)) nota = 1;
        uint8_t ibyte = i / 8;
        uint8_t ibit = 1 << (i & 7);
        if (a && nota) return 0;
        if (!a && !nota) {
            if (i == searchJunction) {   // this is our time to decide differently, we went zero last time, go one.
                a = 1;
                searchJunction = lastJunction;
            } else if (i < searchJunction) {   // take whatever we took last time, look in address
                if (address[ibyte] & ibit) a = 1;
                else {                            // Only 0s count as pending junctions, we've already exhasuted the 0 side of 1s
                    a = 0;
                    done = 0;
                    lastJunction = i;
                }
            } else {                            // we are blazing new tree, take the 0
                a = 0;
                searchJunction = i;
                done = 0;
            }
            lastJunction = i;
        }

        if (a) address[ibyte] |= ibit;
        else address[ibyte] &= ~ibit;

        ds_write_bit(a, 1 << PIN);
    }
    if (done) searchExhausted = 1;
    for (i = 0; i < 8; i++) new_addr[i] = address[i];
    return 1;
}

void delay_us(uint16_t us) {
    volatile uint16_t _us = us * 7; // этот параметр под вопросом
    while (_us != 0) {
        _us--;
    }
}

void ow_read_addr() {
    int num_sensors;
    char last_byte_add[10];

    for (int i = 0; i < 10; i++) //проверяем 10 доступных адресов
    {
        char addr[9];
        clearArray(addr, 9);
        read_flash_string_limt(OW1_ADD + i * 10, addr, 8);
        last_byte_add[i] = addr[7];
        num_sensors = read_flash32(OW_NUM_ADD);
        char buff[50];
        clearArray(buff, 50);
        char intbuff[10];
        intToStr(i, intbuff);
        concat(buff, "{\\\"addr ", buff);
        concat(buff, intbuff, buff);
        concat(buff, "\\\":[", buff);
        for (int i = 0; i < 8; i++) {

            char intbuff[10];
            intToStr(addr[i], intbuff);
            concat(buff, intbuff, buff);
            if (i!=7)concat(buff, ",", buff);
        }
        concat(buff, "]}", buff);
        add_msg_queue(buff);
        print_str(buff);
        print_str("\r\n");
    }
}

