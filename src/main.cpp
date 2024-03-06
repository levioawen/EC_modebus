#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
#include <microDS18B20.h>
//#include <EEPROM.h>
#include <Address_map.h>


#define EC_PIN A1
MicroDS18B20<A3> sensor;
float voltage,temperature = 25;
uint16_t ecValue;
DFRobot_EC ec;
int calibration_cmd_enterec, calibration_cmd_calec, calibration_cmd_exitec;
boolean calibration_run= false;
boolean calibration_mode_enter=false;
boolean calibration_mode_calc=false;
boolean calibration_mode_exit=false;

void setup() {
    ec.begin();
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
        ModbusRTUServer.inputRegisterWrite(EC_address,ecValue);
        ModbusRTUServer.inputRegisterWrite(temp_address,(uint16_t)(temperature*100));
        ModbusRTUServer.inputRegisterWrite(kvalueLow_address,(uint16_t)ec.kvalueLow*100);
        ModbusRTUServer.inputRegisterWrite(kvalueHigh_address,(uint16_t)ec.kvalueHigh*100);

    }
    Serial.print(calibration_cmd_enterec);
    Serial.print(calibration_cmd_calec);
    Serial.println(calibration_cmd_exitec);

    if(!calibration_cmd_enterec && !calibration_cmd_calec && !calibration_cmd_exitec)
    {


        ec.errorflag=FALSE;
        calibration_mode_enter= FALSE;
        calibration_mode_calc=FALSE;
        calibration_mode_exit=FALSE;
        calibration_run=FALSE;
        ec.calib_succesfull=FALSE;
    }
    if(calibration_run)digitalWrite(13,HIGH);
    if(!calibration_run)digitalWrite(13,LOW);
    if(calibration_cmd_enterec && !calibration_mode_enter)
        {

        calibration_run=TRUE;
        calibration_mode_enter= TRUE;
        voltage = analogRead(EC_PIN)/1024.0*5000;
        sensor.requestTemp();
        temperature=sensor.getTemp();
        ec.calibration_int(voltage,25,1);
    }
    if(calibration_cmd_calec && calibration_mode_enter && !calibration_mode_calc)
    {
        calibration_mode_calc=TRUE;
        voltage = analogRead(EC_PIN)/1024.0*5000;
        sensor.requestTemp();
        temperature=sensor.getTemp();
        ec.calibration_int(voltage,25,2);
    }
    if(calibration_cmd_exitec && calibration_mode_enter && calibration_mode_calc && !calibration_mode_exit )
    {
        calibration_mode_exit=TRUE;
        voltage = analogRead(EC_PIN)/1024.0*5000;
        sensor.requestTemp();
        temperature=sensor.getTemp();
        ec.calibration_int(voltage,25,3);
        calibration_run=FALSE;
    }

    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U && !calibration_run)  //time interval: 1s
    {
        timepoint = millis();
        voltage = analogRead(EC_PIN)/1024.0*5000;   // read the voltage
        ecValue =  (uint16_t)(ec.readEC(voltage,temperature)*1000);  // convert voltage to EC with temperature compensation

    }
}





