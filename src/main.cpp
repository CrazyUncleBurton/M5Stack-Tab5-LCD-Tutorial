/*

  M5Stack Tab5 LCD Tutorial

  by Bryan A. "CrazyUncleBurton" Thompson

  Last Updated 2/17/2026

*/

#include <M5Unified.h>
#include "M5UnitENV.h"

M5GFX& display = M5.Display;
M5Canvas canvas(&display);
SHT3X sht3x;
QMP6988 qmp;

// Variables
float shtTempF = 0.0f;
float shtHumidity = 0.0f;
float qmpTempF = 0.0f;
float qmpPressureInHg = 0.0f;
float qmpAltitudeFt = 0.0f;
bool shtPresent = false;
bool qmpPresent = false;
const int i2cSdaPin = 53;
const int i2cSclPin = 54;
const int textSize = 4;
const char degreeSymbol[] = { static_cast<char>(0xC2), static_cast<char>(0xB0), '\0' };

void drawDashboard()
{
  canvas.fillScreen(TFT_BLACK);
  canvas.setTextSize(textSize);

  int lineHeight = canvas.fontHeight() + 14;
  int x = 175;
  int y = 100;


  if ((shtPresent)&&(qmpPresent)) {

    canvas.setCursor(x, y);
    canvas.printf("  Temp 1: %.2f%sF", shtTempF, degreeSymbol);
    y += lineHeight;

    canvas.setCursor(x, y);
    canvas.printf("  Temp 2: %.2f%sF", qmpTempF, degreeSymbol);
    y += lineHeight;

    canvas.setCursor(x, y);
    canvas.printf("Humidity: %.2f%% RH", shtHumidity);
    y += lineHeight;

    canvas.setCursor(x, y);
    canvas.printf("Pressure: %.2f inHg", qmpPressureInHg);
    y += lineHeight;

    canvas.setCursor(x, y);
    canvas.printf("Altitude: %.2f ft", qmpAltitudeFt);

  } else {
    canvas.setCursor(x, y);
    canvas.printf("ENV III not found");
  }

  canvas.pushSprite(0, 0);
}

void setup()
{
  auto cfg = M5.config();
  M5.begin(cfg);

  Wire1.begin(i2cSdaPin, i2cSclPin);

  display.setRotation(1);
  canvas.setColorDepth(16);
  canvas.createSprite(display.width(), display.height());
  canvas.setTextColor(TFT_WHITE, TFT_BLACK);
  canvas.setFont(&fonts::efontJA_24_b);
  canvas.setTextSize(textSize);
  canvas.setTextWrap(false, false);

  canvas.fillScreen(TFT_BLACK);
  canvas.drawCenterString("M5Stack Tab5", display.width() / 2, display.height() / 2 - 150);
  canvas.drawCenterString("Starting...", display.width() / 2 + 20, display.height() / 2 + 50);
  canvas.pushSprite(0, 0);
  delay(1000);

  qmpPresent = qmp.begin(&Wire1, QMP6988_SLAVE_ADDRESS_L, i2cSdaPin, i2cSclPin, 100000U);
  shtPresent = sht3x.begin(&Wire1, SHT3X_I2C_ADDR, i2cSdaPin, i2cSclPin, 100000U);

  drawDashboard();
}

void loop()
{
  static uint32_t lastFrameMs = 0;
  const uint32_t frameIntervalMs = 1000;

  if (millis() - lastFrameMs < frameIntervalMs) {
    return;
  }
  lastFrameMs = millis();

  if (shtPresent && sht3x.update()) {
    shtTempF = (sht3x.cTemp * 1.8f) + 32.0f;
    shtHumidity = sht3x.humidity;
  }

  if (qmpPresent && qmp.update()) {
    qmpTempF = (qmp.cTemp * 1.8f) + 32.0f;
    qmpPressureInHg = qmp.pressure * 0.0002953f;
    qmpAltitudeFt = qmp.altitude * 3.28084f;
  }

  drawDashboard();
}
