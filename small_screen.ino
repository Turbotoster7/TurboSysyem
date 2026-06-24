#include "small_screen.hpp"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("Error while loading small screen module"));
    for (;;);
  }

  display.clearDisplay();
  boot::display_radar();
  layout::drawTime(0, 0);
  display.display();
}

void loop() {
}
