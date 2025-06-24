;#include <FastLED.h>

// 定义LED灯带的数量
#define NUM_LEDS 16
// 定义数据引脚
#define DATA_PIN 3

// 创建一个CRGB类型的数组来存储LED的颜色
CRGB leds[NUM_LEDS];

// 定义亮度变量，范围是0 - 255
uint8_t brightness = 30;

void setup() {
  // 初始化FastLED库
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  // 设置初始亮度
  FastLED.setBrightness(brightness);
}

void loop() {
  // 将所有LED设置为白色
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = CRGB::Red;
    // leds[i] = CRGB::Green;
  }
  // 显示LED的颜色
  FastLED.show();

  // // 增加亮度
  // brightness++;
  // if (brightness > 255) {
  //   brightness = 0;
  // }
  // 设置新的亮度
  FastLED.setBrightness(brightness);

  // 延迟一段时间，以便观察亮度变化
  delay(50);
}    