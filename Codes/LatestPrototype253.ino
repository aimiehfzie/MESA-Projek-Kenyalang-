#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME680.h>

#define BME_I2C_ADDR 0x76  // SDO = GND

Adafruit_BME680 bme(BME_I2C_ADDR);

void setup() {
  Serial.begin(115200);

  // Initialize I2C on chosen pins
  Wire.begin(20, 19);  // SDA = 20, SCL = 19

  if (!bme.begin()) {
    Serial.println("❌ BME688 not detected!");
    while (1);
  }
  Serial.println("✅ BME688 detected via I2C!");

  // Sensor configuration
  bme.setTemperatureOversampling(BME680_OS_8X);
  bme.setHumidityOversampling(BME680_OS_2X);
  bme.setPressureOversampling(BME680_OS_4X);
  bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
  bme.setGasHeater(320, 150);
}

void loop() {
  if (bme.performReading()) {
    Serial.print("Temp: "); Serial.print(bme.temperature); Serial.println(" °C");
    Serial.print("Humidity: "); Serial.print(bme.humidity); Serial.println(" %");
    Serial.print("Pressure: "); Serial.print(bme.pressure / 100.0); Serial.println(" hPa");
    Serial.print("Gas: "); Serial.print(bme.gas_resistance / 1000.0); Serial.println(" KOhm");
  } else {
    Serial.println("❌ Reading failed");
  }
  delay(1000);
}