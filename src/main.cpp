#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
#include <microDS18B20.h>
#include <Address_map.h>

#define EC_PIN A1
HardwareSerial Serial2(PA3, PA2);



MicroDS18B20<01> sensor;
float voltage_adc_ec();
float temperature_1wire();
uint16_t ecValue;
DFRobot_EC ec;
int calibration_cmd_enterec, calibration_cmd_calec, calibration_cmd_exitec;
uint8_t calibration_run= false;
uint8_t calibration_mode_enter=false;
uint8_t calibration_mode_calc=false;
uint8_t calibration_mode_exit=false;

void setup() {
   // ec.begin(); //раскоментить для считывания из епром коэффицентов

    // start the Modbus RTU server, with (slave) id 1
    if (!ModbusRTUServer.begin(1, 9600)) {
        Serial.println("Failed to start Modbus RTU Server!");

    }

    // configure a single coil at address 0x00
    ModbusRTUServer.configureInputRegisters(0x0000, 5);
    ModbusRTUServer.configureCoils(0x0000,6);
    //ModbusRTUServer.configureHoldingRegisters(0x0000,20);
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
        ModbusRTUServer.inputRegisterWrite(temp_address,(uint16_t)(temperature_1wire()));
        ModbusRTUServer.inputRegisterWrite(kvalueLow_address,(uint16_t)ec.kvalueLow*100); // перевод из 1.23 в 123 вид
        ModbusRTUServer.inputRegisterWrite(kvalueHigh_address,(uint16_t)ec.kvalueHigh*100);

    }


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
        ec.calibration_int(voltage_adc_ec(),temperature_1wire(),1);
    }
    if(calibration_cmd_calec && calibration_mode_enter && !calibration_mode_calc)
    {
        calibration_mode_calc=TRUE;
        ec.calibration_int(voltage_adc_ec(),temperature_1wire(),2);
    }
    if(calibration_cmd_exitec && calibration_mode_enter && calibration_mode_calc && !calibration_mode_exit )
    {
        calibration_mode_exit=TRUE;
        ec.calibration_int(voltage_adc_ec(),temperature_1wire(),3);
        calibration_run=FALSE;
    }

    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U && !calibration_run)  //time interval: 1s
    {
        timepoint = millis();
        ecValue =  (uint16_t)(ec.readEC(voltage_adc_ec(),temperature_1wire())*1000);  // convert voltage to EC with temperature compensation

    }
}


float voltage_adc_ec()
{
    float voltage;
    voltage = float(analogRead(EC_PIN)/1024.0*5000);   // read the voltage
    return voltage;
}

float temperature_1wire()
{
    float temperature;
    sensor.requestTemp();
    temperature=sensor.getTemp();
    return temperature;
}

