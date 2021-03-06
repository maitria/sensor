#include <SparkFun_RHT03.h>
#include <Wire.h>
#include <SPI.h>
#include <RF24.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "sense8.h"

#define SENSOR_DATA_PIN A8
#define OLED_RESET -1
#define OLED_I2C_ADDRESS 0x3C

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

RHT03 sensor;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
RF24 radio(5,4);

void setupRadio()
{
    radio.begin();
    radio.setPALevel(RF24_PA_LOW);
    uint8_t pipe_name[] = "ANode";
    radio.openWritingPipe(pipe_name);
}

void setupDisplay()
{
    display.begin(SSD1306_SWITCHCAPVCC, OLED_I2C_ADDRESS);
    display.setTextSize(2);
    display.setTextColor(SSD1306_WHITE);
    display.cp437(true);
    display.clearDisplay();
}

void setup()
{
    setupRadio();
    Serial.begin(9600);
    sensor.begin(SENSOR_DATA_PIN);
    setupDisplay();
}

int tick = 0;

void console_sensor_error_log(String message)
{
    Serial.println(message);
}

void console_sensor_data_log(SensorData& sensorData, bool radio_error)
{
    Serial.println("Tick: " + String(tick));
    Serial.println(sensorData.displayTemperature());
    Serial.println(sensorData.displayHumidity());
    if (radio_error)
        Serial.println("RADIO ERROR");
}

void broadcast(SensorData& sensorData)
{
    radio.write(&sensorData, sizeof(sensorData));
}

void show(SensorData& sensorData, int tick, bool radio_error)
{
    display.clearDisplay();
    display.setCursor(0,0); 
    display.println(sensorData.location);
    display.println();
    display.println(sensorData.displayTemperature());
    display.println(sensorData.displayHumidity());
    if (radio_error)
        display.println("RADIO ERR");

    display.display();
}

void loop()
{
    bool radio_error = false;
    if (!radio.isPVariant())
        radio_error = true;

    tick ++;
    int updateResult = sensor.update();
    bool succeeded = (updateResult == 1);
    if (succeeded)
    {
        SensorData sensorData;
        strcpy(sensorData.location, "Basement");
        sensorData.humidity = sensor.humidity();
        sensorData.temperature = sensor.tempC();

        show(sensorData, tick, radio_error);
        broadcast(sensorData); 
        console_sensor_data_log(sensorData, radio_error);
    }
    else
    {
        console_sensor_error_log("Sensor failed at tick: "+String(tick)); 
    }
    
    delay(RHT_READ_INTERVAL_MS);
}
