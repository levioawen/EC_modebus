#include <Arduino.h>
#include <ArduinoModbus.h>
#include <ArduinoRS485.h>
#include <DFRobot_EC.h>
#include <microDS18B20.h>
MicroDS18B20<A3> sensor;



void setup() {
    Serial.begin(9600);
}
void loop() {
    // запрос температуры
    sensor.requestTemp();

    // вместо delay используй таймер на millis(), пример async_read
    delay(1000);

    // проверяем успешность чтения и выводим
    if (sensor.readTemp()) Serial.println(sensor.getTemp());
    else Serial.println("error");
}