#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_BME680.h>
#include <Adafruit_BMP280.h>

// OLED Configuration
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32
#define OLED_ADDR 0x3C
#define SDA_PIN 8
#define SCL_PIN 9

// Initialize Objects
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
Adafruit_BME680 bme;
Adafruit_BMP280 bmp;
Adafruit_ADXL343 accel = Adafruit_ADXL343(12345);

void setup() {
  Serial.begin(115200);

  // Start I2C with your specific ESP32-S3 pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // 1. Start OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 10);
  display.println("Initializing...");
  display.display();

  // 2. Start Accelerometer
  if (!accel.begin()) { Serial.println("ADXL343 fail"); }

  // 3. Start BME680 (Address 0x77)
  if (!bme.begin(0x77)) { Serial.println("BME680 fail"); }

  // 4. Start BMP280 (Address 0x76)
  if (!bmp.begin(0x76)) { Serial.println("BMP280 fail"); }

  // Quick Sensor Config
  bme.setGasHeater(320, 150);
  accel.setRange(ADXL343_RANGE_4_G);

  delay(1000);
}

void loop() {
  // --- Read Data ---
  sensors_event_t aEvent;
  accel.getEvent(&aEvent);
  bool bmeSuccess = bme.performReading();

  // --- Update Serial Monitor ---
  Serial.print("Temp: "); Serial.println(bme.temperature);

  // --- Update OLED Display ---
  display.clearDisplay();
  display.setCursor(0, 0);
  
  // Line 1: Temperature & Pressure
  display.print("T:"); display.print(bme.temperature, 1);
  display.print("C P:"); display.print(bmp.readPressure() / 100, 0);
  display.println("hPa");

  // Line 2: Accelerometer
  display.print("X:"); display.print(aEvent.acceleration.x, 1);
  display.print(" Y:"); display.println(aEvent.acceleration.y, 1);

  // Line 3: Gas/Air Quality
  display.print("Gas: "); display.print(bme.gas_resistance / 1000.0, 1);
  display.println(" KOhms");

  display.display(); // Push to screen
  
  delay(1000); // Refresh every second
}
