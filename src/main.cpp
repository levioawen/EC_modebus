#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
#include <DFRobot_PH.h>
#include <microDS18B20.h>
#include <Address_map.h>
#include <EEPROM.h>
#include <stm32f1xx_ll_utils.h>

#define VREF 3300          //stm32 3.3v
//#define VREF 5000  //arduino 5v
#define RESET_MB_PIN PA4
#define EC_PIN PA0
#define PH_PIN PA1
#define TEMPERATURE_PIN 2 // эта библиотека не воспринимает stm style пины, вычислить по таблице ниже
/*
Arduino pin  2 = B7
Arduino pin  3 = B6
Arduino pin  4 = B5
Arduino pin  5 = B4
Arduino pin  6 = B3
Arduino pin  7 = A15
Arduino pin  8 = A12
Arduino pin  9 = A11
Arduino pin 10 = A10
Arduino pin 11 = A9
Arduino pin 12 = A8
Arduino pin 13 = B15
Arduino pin 14 = B14
Arduino pin 15 = B13
Arduino pin 16 = null
Arduino pin 17 = C13, onboard led, input only.
Arduino pin 18 = C14
Arduino pin 19 = C15
Arduino pin 20 = A0
Arduino pin 21 = A1
Arduino pin 22 = A2
Arduino pin 23 = A3
Arduino pin 24 = A4
Arduino pin 25 = A5
Arduino pin 26 = A6
Arduino pin 27 = A7
Arduino pin 28 = B0
Arduino pin 29 = B1
Arduino pin 30 = B10
Arduino pin 31 = B11

 */

HardwareSerial Serial2(PA3, PA2);  // сериал порт для модбас, который использует RS485.cpp.

//HardwareSerial Serial2(PA10, PA9);  // сериал порт для модбас, который использует RS485.cpp.
MicroDS18B20<TEMPERATURE_PIN> sensor;
DFRobot_EC ec;
DFRobot_PH ph;
float voltage_adc(uint8_t pin);
float temperature(uint8_t option);
void modbus_broadcast();
uint8_t temperature_option;
uint16_t temperature_mb_input;
uint16_t ecValue;
uint16_t phValue;
uint16_t modbus_id;

uint32_t modbus_baud_rate;
uint8_t ec_calib_high_start;
uint8_t ec_calib_low_start;
uint8_t ec_calib_high_end;
uint8_t ec_calib_low_end;
uint8_t ec_calib_error;

uint8_t ph_calib_normal_start;
uint8_t ph_calib_acid_start;
uint8_t ph_calib_normal_end;
uint8_t ph_calib_acid_end;
uint8_t ph_calib_error;
uint8_t write_flash_start;
float  ec_k_valueHigh;
float ec_k_valueLow;
float ph_k_valueNeutral;
float ph_k_valueAcid;

void setup() {
    pinMode(RESET_MB_PIN, INPUT_PULLUP);
    if(!digitalRead(RESET_MB_PIN)){
        modbus_id=10;
        modbus_baud_rate=9600;
        for(int i=0;i<0x14;i++){
            EEPROM.put(i,0xFF);
        }
        EEPROM.put(modbus_id_eeprom,modbus_id);
        EEPROM.put(modbus_baud_rate_eeprom,modbus_baud_rate);
    }


    EEPROM.get(modbus_id_eeprom,modbus_id);
    if(modbus_id>255 || modbus_id==0){
        modbus_id=10;
        EEPROM.put(modbus_id_eeprom,modbus_id);
    }
    EEPROM.get(modbus_baud_rate_eeprom,modbus_baud_rate);
    if(!(modbus_baud_rate==4800 || modbus_baud_rate==9600 || modbus_baud_rate==19200 || modbus_baud_rate==38400 || modbus_baud_rate==57600 || modbus_baud_rate==115200)){
        modbus_baud_rate=9600;
        EEPROM.put(modbus_baud_rate_eeprom,modbus_baud_rate);
    }

    ph.begin();
    ec.begin();
    if (!ModbusRTUServer.begin(uint16_t (modbus_id), modbus_baud_rate)) {
    }

    // configure a single coil at address 0x00
    ModbusRTUServer.configureInputRegisters(0x0000, 13);
    ModbusRTUServer.configureCoils(0x0000,30);
    ModbusRTUServer.configureHoldingRegisters(0x0000, 4);

}

void loop() {


   modbus_broadcast();



    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U)  //time interval: 1s
    {
        timepoint = millis();
        ecValue = (uint16_t)(ec.readEC(voltage_adc(EC_PIN),temperature(temperature_option))*1000);  // convert voltage to EC with temperature compensation
        phValue = (uint16_t)(ph.readPH(voltage_adc(PH_PIN),temperature(temperature_option))*1000);
    }

}

void modbus_broadcast()
{
    int packetReceived = ModbusRTUServer.poll();

    if(packetReceived) {
        uint32_t new_modbus_id;
        uint32_t new_modbus_modbus_baud_rate;

        new_modbus_id=ModbusRTUServer.holdingRegisterRead(modbus_id_address);
        if(new_modbus_id !=0){
            EEPROM.put(modbus_id_eeprom,uint16_t(new_modbus_id));
        }

        new_modbus_modbus_baud_rate=ModbusRTUServer.holdingRegisterRead(modbus_baud_rate_address)*100;
        if(new_modbus_modbus_baud_rate !=0){
            EEPROM.put(modbus_baud_rate_eeprom,new_modbus_modbus_baud_rate);
        }
        if(ModbusRTUServer.coilRead(reboot_address)){
            NVIC_SystemReset();
        }
        if(ModbusRTUServer.coilRead(temperature_option_address)){
            temperature_option=1;
            temperature_mb_input=ModbusRTUServer.holdingRegisterRead(temperature_in_out_address);
        }
        if(!ModbusRTUServer.coilRead(temperature_option_address)){
            temperature_option=0;
            ModbusRTUServer.holdingRegisterWrite(temperature_in_out_address,(uint16_t)(temperature(temperature_option)*100));
        }

        if(ModbusRTUServer.coilRead(ec_calib_high_start_address)){
            ec.ecCalibration_high(voltage_adc(EC_PIN), temperature(temperature_option));
            ModbusRTUServer.coilWrite(ec_calib_high_end_address,ec.calib_end_high);
            ModbusRTUServer.coilWrite(ec_calib_error_address,ec.error_flag);
            ModbusRTUServer.coilWrite(ec_calib_high_start_address,0);
        }
        if(ModbusRTUServer.coilRead(ec_calib_low_start_address)){
            ec.ecCalibration_low(voltage_adc(EC_PIN), temperature(temperature_option));
            ModbusRTUServer.coilWrite(ec_calib_low_end_address,ec.calib_end_low);
            ModbusRTUServer.coilWrite(ec_calib_error_address,ec.error_flag);
            ModbusRTUServer.coilWrite(ec_calib_low_start_address,0);
        }
        if(ModbusRTUServer.coilRead(ph_calib_neutral_start_address)){
            ph.phCalibration_neutral(voltage_adc(PH_PIN), temperature(temperature_option));
            ModbusRTUServer.coilWrite(ph_calib_neutral_end_address,ph.calib_end_neutral);
            ModbusRTUServer.coilWrite(ph_calib_error_address,ph.error_flag);
            ModbusRTUServer.coilWrite(ph_calib_neutral_start_address,0);
        }
        if(ModbusRTUServer.coilRead(ph_calib_acid_start_address)){
            ph.phCalibration_acid(voltage_adc(PH_PIN), temperature(temperature_option));
            ModbusRTUServer.coilWrite(ph_calib_acid_end_address,ph.calib_end_acid);
            ModbusRTUServer.coilWrite(ph_calib_error_address,ph.error_flag);
            ModbusRTUServer.coilWrite(ph_calib_acid_start_address,0);
        }




        ModbusRTUServer.inputRegisterWrite(ec_address,ecValue);  // перевод из мили в микро сименсы на куб см
        ModbusRTUServer.inputRegisterWrite(ec_k_valueLow_address,(uint16_t)(ec._kvalueLow*100)); // перевод из 1.23 в 123 вид
        ModbusRTUServer.inputRegisterWrite(ec_k_valueHigh_address,(uint16_t)(ec._kvalueHigh*100));
        ModbusRTUServer.inputRegisterWrite(ph_address,phValue);  // перевод из мили в микро сименсы на куб см
        ModbusRTUServer.inputRegisterWrite(ph_k_valueNeutral_address,(uint16_t)(ph._neutralVoltage)); // перевод из 1.23 в 123 вид
        ModbusRTUServer.inputRegisterWrite(ph_k_valueAcid_address,(uint16_t)(ph._acidVoltage));



    }
}



float voltage_adc(uint8_t pin)
{
    float voltage;
    voltage = float(analogRead(pin)/1024.0*VREF);
    return voltage;
}


float temperature(uint8_t option) {
    float temperature;
    if (option == 0) {

        sensor.requestTemp();
        temperature = sensor.getTemp();
    }
    if(option == 1) {
        temperature=float(temperature_mb_input)/100;


    }
    return temperature;
}
