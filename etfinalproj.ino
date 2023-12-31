/*
*******************************************************************************
* Copyright (c) 2022 by M5Stack
*                  Equipped with M5StickC Plus sample source code
*                          配套  M5StickC Plus 示例源代码
* Visit for more information: https://docs.m5stack.com/en/app/scales_kit
* 获取更多资料请访问: https://docs.m5stack.com/zh_CN/app/scales_kit
*
* Describe: SCALES KIT WEIGHT UNIT EXAMPLE.
* Date: 2022/02/23
*******************************************************************************
  Connect WEIGHT UNIT to port A (G33/32), calibration instructions: press button
A to remove the tare weight when there is no load, press button B, switch the
standard weight value left and right and put down the corresponding weight,
confirm for calibration. 将WEIGHT UNIT连接至端口A (G33/32),，
校准说明: 无负重情况下按下按键A去处皮重，
按下按键B开始校准，A/B切换标准重量值并放下对应重量砝码，长按按键B
comfirm进行校准。 Libraries:
  - [HX711](https://github.com/bogde/HX711)

*/

#include <M5GFX.h>
#include <M5StickCPlus.h>
#include "HX711.h"

M5GFX display;
M5Canvas canvas(&display);

// HX711 related pin Settings.  HX711 相关引脚设置
#define LOADCELL_DOUT_PIN 33
#define LOADCELL_SCK_PIN  32

HX711 scale;
float base = 0;

void setup() {
    M5.begin();  // Init M5Stack.  初始化M5Stack
    Serial.begin(9600);
    display.begin();
    display.setRotation(3);
    canvas.setColorDepth(1);  // mono color
    canvas.createSprite(display.width(), display.height());
    canvas.setTextDatum(MC_DATUM);
    canvas.setPaletteColor(1, GREEN);
    canvas.drawString("Calibration sensor....", 80, 40);
    canvas.pushSprite(0, 0);
    scale.begin(LOADCELL_DOUT_PIN, LOADCELL_SCK_PIN);
    // The scale value is the adc value corresponding to 1g
    scale.set_scale(27.61f);  // set scale
    scale.tare();             // auto set offset
}

char info[100];

void loop() {
    canvas.fillSprite(BLACK);
    canvas.setTextSize(1);
    canvas.drawString("<-Tare", 30, 20);
    canvas.drawString("Calibration", 80, 120);
    float weight = scale.get_units(10) / 1000.0;
    float flagbase = 0;
    canvas.setTextSize(3);
    //visually showing the weight on the screen 
    //if the weight is above 10 g then print the weight 
    if (weight > 0) {
        sprintf(info, "%.2f", weight);
        canvas.drawString(String(info) + "kg", 80, 60);
        Serial.printf("%.2f", weight);
        Serial.print('\n');
        Serial.print("Base: " + String(base));
        Serial.print('\n');
        float refillevel = base * 1/3;
        Serial.print("Refill Level: " + String(refillevel));
        Serial.print('\n');

        delay(2000);
        if (weight > 0.01) {
          if (refillevel < weight) {
            canvas.setPaletteColor(1, BLUE);
            Serial.print("AW NAH HONEY YOU'RE GOOD");
            Serial.print('\n');
          } else {
            canvas.setPaletteColor(1, RED);
            Serial.print("YEEHAW REFILL");
            Serial.print('\n');
          }
        }
    //if it is below 10g or negative show as 0
    } else {
        canvas.drawString("0.00kg", 80, 60);
    }

    //using the weight to now store the lowering weight compared to the base


    M5.update();
    if (M5.BtnA.wasPressed()) {
        scale.tare();
        Serial.print("entered A");
        Serial.print('\n');
        canvas.drawString("0g Cal", 80, 120);
        delay(5000);
        base = scale.get_units(10) / 1000.0;
        Serial.print("Base: " + String(base));
        Serial.print('\n');
    }
        
    
    if (M5.BtnB.wasPressed()) {
        long kg = 5;
        while (1) {
            M5.update();
            canvas.fillSprite(BLACK);
            canvas.setTextSize(2);
            canvas.drawString("Cal:" + String(kg) + "kg", 80, 60);
            canvas.setTextSize(1);
            canvas.drawString("<-change", 30, 20);
            canvas.drawString("long press comfirm", 80, 120);
            canvas.pushSprite(0, 0);
            if (M5.BtnB.pressedFor(2000)) {
                break;
            }
            if (M5.BtnA.wasReleased()) {
                kg--;
            }
            if (M5.BtnB.wasReleased()) {
                kg++;
            }
            delay(10);
        }
        long kg_adc = scale.read_average(20);
        kg_adc      = kg_adc - scale.get_offset();
        scale.set_scale(kg_adc / (kg * 1000.0));
    }
    canvas.pushSprite(0, 0);
}