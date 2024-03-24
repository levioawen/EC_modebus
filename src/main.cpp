#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
//#include <DFRobot_PH.h>
#include <microDS18B20.h>
#include <Address_map.h>
#include <EEPROM.h>

#define VREF 3300          //stm32 3.3v
//#define VREF 5000  //arduino 5v

#define EC_PIN PA0
#define PH_PIN PA1
#define TEMPERATURE_PIN 31 // эта библиотека не воспринимает stm style пины, вычислить по таблице ниже
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

#define modbus_id_eeprom 0x12
#define modbus_baud_rate_eeprom 0x14
//HardwareSerial Serial2(PA3, PA2);  // сериал порт для модбас, который использует RS485.cpp.
MicroDS18B20<TEMPERATURE_PIN> sensor;
DFRobot_EC ec;
//DFRobot_PH ph;
float voltage_adc(uint8_t pin);
float temperature(uint8_t option);
void modbus_broadcast();
uint8_t temperature_option;
uint16_t temperature_mb_input;
uint16_t ecValue;
uint16_t phValue;
uint16_t modbus_id;
uint16_t modbus_baud_rate;
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

void setup() {

    EEPROM.get(modbus_id_eeprom,modbus_id);
    if(modbus_id==0 || modbus_id==0xFFFF) modbus_id=1;
    EEPROM.get(modbus_baud_rate_eeprom,modbus_baud_rate);
    if(modbus_baud_rate==0 || modbus_baud_rate==0xFFFF) modbus_baud_rate=96;


    ec.begin(); //раскоментить для считывания из еeпром коэффицентов
    //ph.begin();
    // start the Modbus RTU server, with (slave) id 1
    if (!ModbusRTUServer.begin(modbus_id, modbus_baud_rate*100)) {
    }

    // configure a single coil at address 0x00
    ModbusRTUServer.configureInputRegisters(0x0000, 10);
    ModbusRTUServer.configureCoils(0x0000,10);
    ModbusRTUServer.configureHoldingRegisters(0x0000, 3);

}

void loop() {


    modbus_broadcast();



    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U)  //time interval: 1s
    {
        timepoint = millis();
        ecValue = (uint16_t)(ec.readEC(voltage_adc(EC_PIN),temperature(temperature_option))*1000);  // convert voltage to EC with temperature compensation
        //phValue = (uint16_t)(ph.readPH(voltage_adc(PH_PIN),temperature(temperature_option))*1000);
    }
}

void modbus_broadcast()
{
    int packetReceived = ModbusRTUServer.poll();

    if(packetReceived) {

        if(ModbusRTUServer.holdingRegisterRead(modbus_id_address) != modbus_id){
            modbus_id=ModbusRTUServer.holdingRegisterRead(modbus_id_address);
            EEPROM.put(modbus_id_eeprom,modbus_id);
        }
        if(ModbusRTUServer.holdingRegisterRead(modbus_baud_rate_address) != modbus_baud_rate) {
            modbus_baud_rate = ModbusRTUServer.holdingRegisterRead(modbus_baud_rate_address);
            EEPROM.put(modbus_baud_rate_eeprom,modbus_baud_rate);
        }
        temperature_option=ModbusRTUServer.coilRead(temperature_option_address);
        if(temperature_option){
            temperature_mb_input=ModbusRTUServer.holdingRegisterRead(temperature_in_out_address);
        }

        if(!temperature_mb_input){
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
        /*if(ModbusRTUServer.coilRead(ph_calib_neutral_start_address)){
            ec.ecCalibration_high(voltage_adc(PH_PIN), temperature(temperature_option));
            ModbusRTUServer.coilWrite(ph_calib_neutral_end_address,ph.calib_end_neutral);
            ModbusRTUServer.coilWrite(ph_calib_error_address,ph.error_flag);
            ModbusRTUServer.coilWrite(ph_calib_neutral_start_address,0);
        }
        if(ModbusRTUServer.coilRead(ph_calib_acid_start_address)){
            ec.ecCalibration_low(voltage_adc(PH_PIN), temperature(temperature_option));
            ModbusRTUServer.coilWrite(ph_calib_acid_end_address,ph.calib_end_acid);
            ModbusRTUServer.coilWrite(ph_calib_error_address,ph.error_flag);
            ModbusRTUServer.coilWrite(ph_calib_acid_start_address,0);
        }
         */



        ModbusRTUServer.inputRegisterWrite(ec_address,ecValue);  // перевод из мили в микро сименсы на куб см
        ModbusRTUServer.inputRegisterWrite(ec_k_valueLow_address,(uint16_t)(ec.kvalueLow*100)); // перевод из 1.23 в 123 вид
        ModbusRTUServer.inputRegisterWrite(ec_k_valueHigh_address,(uint16_t)(ec.kvalueHigh*100));
        ModbusRTUServer.inputRegisterWrite(ph_address,phValue);  // перевод из мили в микро сименсы на куб см
        //ModbusRTUServer.inputRegisterWrite(ph_k_valueNeutral_address,(uint16_t)(ph.neutralVoltage*100)); // перевод из 1.23 в 123 вид
        //ModbusRTUServer.inputRegisterWrite(ph_k_valueAcid_address,(uint16_t)(ph.acidVoltage*100));



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
        temperature=temperature_mb_input;


    }
    return temperature;
}
