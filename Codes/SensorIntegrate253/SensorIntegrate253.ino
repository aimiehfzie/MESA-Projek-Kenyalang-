/*!
 * Combined BMP280 (I2C) + ADXL343 (SPI) for ESP32-S3
 * BMP280  → Temperature, Pressure, Altitude
 * ADXL343 → Acceleration X/Y/Z
 */

#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include "DFRobot_BMP280.h"

// ─── I2C pins (BMP280) ────────────────────────────────
#define I2C_SDA  8
#define I2C_SCL  9

// ─── SPI pins (ADXL343) ───────────────────────────────
#define ADXL343_SCK  13
#define ADXL343_MISO 12
#define ADXL343_MOSI 11
#define ADXL343_CS   10

// ─── Sensor objects ───────────────────────────────────
typedef DFRobot_BMP280_IIC BMP;
BMP bmp(&Wire, BMP::eSdoLow);   // ADDR to GND → 0x76

Adafruit_ADXL343 accel = Adafruit_ADXL343(
  ADXL343_SCK, ADXL343_MISO, ADXL343_MOSI, ADXL343_CS, 12345
);

#define SEA_LEVEL_PRESSURE 1015.0f

// ─── BMP280 helpers ───────────────────────────────────
void printBmpStatus(BMP::eStatus_t eStatus) {
  switch (eStatus) {
    case BMP::eStatusOK:                   Serial.println("OK"); break;
    case BMP::eStatusErr:                  Serial.println("Unknown error"); break;
    case BMP::eStatusErrDeviceNotDetected: Serial.println("Device not detected"); break;
    case BMP::eStatusErrParameter:         Serial.println("Parameter error"); break;
    default: Serial.println("Unknown status"); break;
  }
}

// ─── ADXL343 helpers ──────────────────────────────────
void displayDataRate() {
  Serial.print("ADXL343 Data Rate: ");
  switch (accel.getDataRate()) {
    case ADXL343_DATARATE_3200_HZ: Serial.println("3200 Hz"); break;
    case ADXL343_DATARATE_1600_HZ: Serial.println("1600 Hz"); break;
    case ADXL343_DATARATE_800_HZ:  Serial.println("800 Hz");  break;
    case ADXL343_DATARATE_400_HZ:  Serial.println("400 Hz");  break;
    case ADXL343_DATARATE_200_HZ:  Serial.println("200 Hz");  break;
    case ADXL343_DATARATE_100_HZ:  Serial.println("100 Hz");  break;
    case ADXL343_DATARATE_50_HZ:   Serial.println("50 Hz");   break;
    case ADXL343_DATARATE_25_HZ:   Serial.println("25 Hz");   break;
    default:                        Serial.println("Other");   break;
  }
}

void displayRange() {
  Serial.print("ADXL343 Range: +/- ");
  switch (accel.getRange()) {
    case ADXL343_RANGE_16_G: Serial.println("16 g"); break;
    case ADXL343_RANGE_8_G:  Serial.println("8 g");  break;
    case ADXL343_RANGE_4_G:  Serial.println("4 g");  break;
    case ADXL343_RANGE_2_G:  Serial.println("2 g");  break;
    default:                  Serial.println("?? g"); break;
  }
}

// ─── Setup ────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(1000);  // Let Serial stabilize on ESP32-S3
  Serial.println("=== BMP280 (I2C) + ADXL343 (SPI) on ESP32-S3 ===\n");

  // Init I2C for BMP280
  Wire.begin(I2C_SDA, I2C_SCL);
  bmp.reset();
  Serial.print("Initializing BMP280... ");
  while (bmp.begin() != BMP::eStatusOK) {
    Serial.println("BMP280 init failed:");
    printBmpStatus(bmp.lastOperateStatus);
    delay(2000);
  }
  Serial.println("BMP280 OK");

  // Init SPI for ADXL343
  Serial.print("Initializing ADXL343... ");
  if (!accel.begin()) {
    Serial.println("ADXL343 not detected! Check wiring.");
    while (1);
  }
  Serial.println("ADXL343 OK");

  accel.setRange(ADXL343_RANGE_16_G);
  accel.printSensorDetails();
  displayDataRate();
  displayRange();

  Serial.println("\nStarting measurements...\n");
}

// ─── Loop ─────────────────────────────────────────────
void loop() {
  // --- BMP280 readings ---
  float    temp  = bmp.getTemperature();
  uint32_t press = bmp.getPressure();
  float    alti  = bmp.calAltitude(SEA_LEVEL_PRESSURE, press);

  // --- ADXL343 reading ---
  sensors_event_t event;
  accel.getEvent(&event);

  // --- Print all ---
  Serial.println("┌─────────────────────────────────┐");
  Serial.println("│         BMP280 (I2C)            │");
  Serial.println("├─────────────────────────────────┤");
  Serial.print  ("│ Temperature : "); Serial.print(temp);         Serial.println(" °C");
  Serial.print  ("│ Pressure    : "); Serial.print(press / 100.0f); Serial.println(" hPa");
  Serial.print  ("│ Altitude    : "); Serial.print(alti);          Serial.println(" m");
  Serial.println("├─────────────────────────────────┤");
  Serial.println("│         ADXL343 (SPI)           │");
  Serial.println("├─────────────────────────────────┤");
  Serial.print  ("│ Accel X     : "); Serial.print(event.acceleration.x); Serial.println(" m/s²");
  Serial.print  ("│ Accel Y     : "); Serial.print(event.acceleration.y); Serial.println(" m/s²");
  Serial.print  ("│ Accel Z     : "); Serial.print(event.acceleration.z); Serial.println(" m/s²");
  Serial.println("└─────────────────────────────────┘");
  Serial.println();

  delay(1000);
}