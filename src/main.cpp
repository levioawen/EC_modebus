#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
#include <microDS18B20.h>
#include <EEPROM.h>
#include <Address_map.h>


#define EC_PIN A1
MicroDS18B20<A3> sensor;
float voltage,temperature = 25;
uint16_t ecValue;
DFRobot_EC ec;



void setup() {
    Serial.begin(9600);
    Serial.println("Modbus RTU Server");

    // start the Modbus RTU server, with (slave) id 1
    if (!ModbusRTUServer.begin(1, 9600)) {
        Serial.println("Failed to start Modbus RTU Server!");

    }

    // configure a single coil at address 0x00
    ModbusRTUServer.configureInputRegisters(0x0000, 10);
    ModbusRTUServer.configureHoldingRegisters(0x0000,20);
}
void loop() {


    int packetReceived = ModbusRTUServer.poll();
    if(packetReceived) {

        ModbusRTUServer.inputRegisterWrite(EC_address,ecValue);
        ModbusRTUServer.holdingRegisterWrite(t_address,(uint16_t)temperature);

    }

    static unsigned long timepoint = millis();

    if(millis()-timepoint>1000U)  //time interval: 1s
    {
        timepoint = millis();
        voltage = analogRead(EC_PIN)/1024.0*5000;   // read the voltage
        sensor.requestTemp();
        if (sensor.readTemp())
        {
            temperature=sensor.getTemp();
        }
        else Serial.println("error");         // read your temperature sensor to execute temperature compensation
        ecValue =  (uint16_t)(ec.readEC(voltage,temperature)*1000);  // convert voltage to EC with temperature compensation

        Serial.print("temperature:");
        Serial.print(temperature,1);
        Serial.print("^C  EC:");
        Serial.print(ecValue);

        Serial.println("us/cm");
    }



}






