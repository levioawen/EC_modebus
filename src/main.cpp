#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
#include <DFRobot_PH.h>
#include <microDS18B20.h>
#include <Address_map.h>
//#include <EEPROM.h>

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


HardwareSerial Serial2(PA3, PA2);  // сериал порт для модбас, который использует RS485.cpp.
MicroDS18B20<TEMPERATURE_PIN> sensor;
DFRobot_EC ec;
DFRobot_PH ph;
float voltage_adc(uint8_t pin);
float temperature_1wire();
void calibration_ec(uint8_t pin);
void calibration_ph(uint8_t pin);
uint16_t ecValue;
uint16_t phValue;
int calibration_cmd_enterec, calibration_cmd_calec, calibration_cmd_exitec;
uint8_t calibration_run= false;
uint8_t calibration_mode_enter=false;
uint8_t calibration_mode_calc=false;
uint8_t calibration_mode_exit=false;
void setup() {



    ec.begin(); //раскоментить для считывания из епром коэффицентов
    ph.begin();
    // start the Modbus RTU server, with (slave) id 1
    if (!ModbusRTUServer.begin(1, 9600)) {
    }

    // configure a single coil at address 0x00
    ModbusRTUServer.configureInputRegisters(0x0000, 7);
    ModbusRTUServer.configureCoils(0x0000,6);

}

void loop() {


    int packetReceived = ModbusRTUServer.poll();

    if(packetReceived) {
        calibration_cmd_enterec=ModbusRTUServer.coilRead(enterec_addres);
        calibration_cmd_calec=ModbusRTUServer.coilRead(calec_addres);
        calibration_cmd_exitec=ModbusRTUServer.coilRead(exitec_addres);
        ModbusRTUServer.coilWrite(run_addres,calibration_run);
        ModbusRTUServer.coilWrite(successful_address,ec.calib_succesfull);
        ModbusRTUServer.coilWrite(error_address,ec.errorflag);
        ModbusRTUServer.inputRegisterWrite(EC_address,ecValue);  // перевод из мили в микро сименсы на куб см
        ModbusRTUServer.inputRegisterWrite(temp_address,(uint16_t)(temperature_1wire()*100));
        ModbusRTUServer.inputRegisterWrite(kvalueLow_address,(uint16_t)(ec.kvalueLow*100)); // перевод из 1.23 в 123 вид
        ModbusRTUServer.inputRegisterWrite(kvalueHigh_address,(uint16_t)(ec.kvalueHigh*100));
        ModbusRTUServer.inputRegisterWrite(PH_address,phValue);  // перевод из мили в микро сименсы на куб см
        ModbusRTUServer.inputRegisterWrite(kvalueNeutral_address,(uint16_t)(ph.neutralVoltage*100)); // перевод из 1.23 в 123 вид
        ModbusRTUServer.inputRegisterWrite(kvalueAcid_address,(uint16_t)(ph.acidVoltage*100));

    }

    calibration_ec(EC_PIN);
    //calibration_ph();


    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U && !calibration_run)  //time interval: 1s
    {
        timepoint = millis();
        ecValue = (uint16_t)(ec.readEC(voltage_adc(EC_PIN),temperature_1wire())*1000);  // convert voltage to EC with temperature compensation
        phValue = (uint16_t)(ph.readPH(voltage_adc(PH_PIN),temperature_1wire())*1000);
    }
}

void calibration_ec(uint8_t pin)
{
    if(!calibration_cmd_enterec && !calibration_cmd_calec && !calibration_cmd_exitec)
    {


        ec.errorflag=FALSE;
        calibration_mode_enter= FALSE;
        calibration_mode_calc=FALSE;
        calibration_mode_exit=FALSE;
        calibration_run=FALSE;
        ec.calib_succesfull=FALSE;
    }

    if(calibration_cmd_enterec && !calibration_mode_enter)
    {

        calibration_run=TRUE;
        calibration_mode_enter= TRUE;
        ec.calibration_int(voltage_adc(pin),temperature_1wire(),1);
    }
    if(calibration_cmd_calec && calibration_mode_enter && !calibration_mode_calc)
    {
        calibration_mode_calc=TRUE;
        ec.calibration_int(voltage_adc(pin),temperature_1wire(),2);
    }
    if(calibration_cmd_exitec && calibration_mode_enter && calibration_mode_calc && !calibration_mode_exit )
    {
        calibration_mode_exit=TRUE;
        ec.calibration_int(voltage_adc(pin),temperature_1wire(),3);
        calibration_run=FALSE;
    }
}
void calibration_ph()
{


}


float voltage_adc(uint8_t pin)
{
    float voltage;
    voltage = float(analogRead(pin)/1024.0*VREF);
    return voltage;
}


float temperature_1wire()
{
    float temperature;
    sensor.requestTemp();
    temperature=sensor.getTemp();
    return temperature;
}
