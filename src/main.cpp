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
boolean calibration_mod= false;
boolean calibration_mode_enter=false;
boolean calibration_mode_calc=false;
boolean calibration_mode_exit=false;


void setup() {
    Serial.begin(9600);
    Serial.println("Modbus RTU Server");

    // start the Modbus RTU server, with (slave) id 1
    if (!ModbusRTUServer.begin(1, 9600)) {
        Serial.println("Failed to start Modbus RTU Server!");

    }

    // configure a single coil at address 0x00
    ModbusRTUServer.configureInputRegisters(0x0000, 2);
    ModbusRTUServer.configureCoils(0x0000,3);
    //ModbusRTUServer.configureHoldingRegisters(0x0000,20);
}
void loop() {


    int packetReceived = ModbusRTUServer.poll();
    if(packetReceived) {
        calibration_cmd_enterec=ModbusRTUServer.coilRead(enterec_addres);
        calibration_cmd_calec=ModbusRTUServer.coilRead(calec_addres);
        calibration_cmd_exitec=ModbusRTUServer.coilRead(exitec_addres);
        ModbusRTUServer.inputRegisterWrite(EC_address,ecValue);
        ModbusRTUServer.inputRegisterWrite(temp_address,(uint16_t)(temperature*100));

    }
    if(!calibration_cmd_enterec && !calibration_cmd_calec && !calibration_cmd_exitec)
    {
        calibration_mode_enter= false;
        calibration_mode_calc=false;
        calibration_mode_exit=false;
    }

    if(calibration_cmd_enterec && !calibration_mode_enter)
        {
        calibration_mod=true;
        calibration_mode_enter= true;
        voltage = analogRead(EC_PIN)/1024.0*5000;
        sensor.requestTemp();
        temperature=sensor.getTemp();
        ec.calibration_int(voltage,temperature,1);
        calibration_cmd_enterec=0;
    }
    if(calibration_cmd_calec && calibration_mode_enter && !calibration_mode_calc)
    {
        calibration_mode_calc=true;
        voltage = analogRead(EC_PIN)/1024.0*5000;
        sensor.requestTemp();
        temperature=sensor.getTemp();
        ec.calibration_int(voltage,temperature,2);
    }
    if(calibration_cmd_exitec && calibration_mode_enter && calibration_mode_calc && !calibration_mode_exit )
    {
        calibration_mode_exit=true;
        voltage = analogRead(EC_PIN)/1024.0*5000;
        sensor.requestTemp();
        temperature=sensor.getTemp();
        ec.calibration_int(voltage,temperature,3);
        calibration_mod=false;
    }

    static unsigned long timepoint = millis();
    if(millis()-timepoint>1000U && !calibration_mod)  //time interval: 1s
    {
        timepoint = millis();
        voltage = analogRead(EC_PIN)/1024.0*5000;   // read the voltage
        ecValue =  (uint16_t)(ec.readEC(voltage,temperature)*1000);  // convert voltage to EC with temperature compensation
        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  EC:");
        Serial.print(ecValue);

        Serial.println("us/cm");
    }
}





