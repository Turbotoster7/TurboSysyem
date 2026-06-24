#include <WiFi.h>
#include <time.h>
#include "TurboSystem.hpp"
#include "small_screen.hpp"


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
StateManager deviceState;

#define BUTTON_FUNCTION 23
#define BUTTTON_NEXT 33
#define BUTTON_PREV 25

int lastState_NEXT = HIGH;
int lastState_PREV = HIGH;
int lastState_FUNC = HIGH;

int currentStateNEXT;
int currentStatePREV;
int currentStateFUNC;

bool IS_ALARM_SET = false;
int alarmHour = 0;
int alarmMinute = 0;

const char* ssid = "Wokwi-GUEST";
const char* password = "";

const char* NTP_SERVER = "pool.ntp.org";
const char* TZ_WARSAW = "CET-1CEST,M3.5.0,M10.5.0/3";
constexpr unsigned long DAY_MS = 24UL * 60UL * 60UL * 1000UL;

unsigned long lastSync = 0;
unsigned long lastWifiAttempt = 0;
bool timeSynced = false;
int lastShownMinute = -1;

bool wifiConnected()
{
    return WiFi.status() == WL_CONNECTED;
}

void connectWiFi(unsigned long timeoutMs = 8000)
{
    if (wifiConnected()) return;

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < timeoutMs) {
        delay(250);
        Serial.print(".");
    }

    Serial.println();
    if (wifiConnected()) {
        Serial.print("WiFi polaczone, IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("WiFi: brak polaczenia (timeout)");
    }
}

void syncTime()
{
    if (!wifiConnected()) return;

    setenv("TZ", TZ_WARSAW, 1);
    tzset();
    configTime(0, 0, NTP_SERVER);

    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 10000)) {
        timeSynced = true;
        lastSync = millis();
        Serial.printf("Czas zsynchronizowany: %02d:%02d\n",
                      timeinfo.tm_hour, timeinfo.tm_min);
    } else {
        Serial.println("NTP: synchronizacja nieudana");
    }
}

// Big screen
void drawMainScreen()
{
    //
}

void blankMainScreen()
{
    //
}

void drawSleepScreen()
{
    display.clearDisplay();
    struct tm timeinfo;
    if (getLocalTime(&timeinfo, 100)) {
        layout::drawTime(timeinfo.tm_hour, timeinfo.tm_min);
    } else {
        layout::drawTime(0, 0);
    }

    if (wifiConnected()) {
        layout::drawWifiIcon();
    }

    if (IS_ALARM_SET) {
        layout::drawAlarmRow(alarmHour, alarmMinute);
    }

    display.display();
}

void redrawScreen()
{
    switch (deviceState.getMode())
    {
        case DeviceMode::MAIN:
            display.clearDisplay();
            display.display();
            drawMainScreen();
            break;

        case DeviceMode::SLEEP:
            blankMainScreen();
            // boot::display_radar();
            drawSleepScreen();
            break;

        case DeviceMode::DUAL:
            drawMainScreen();
            drawSleepScreen();
            break;
    }
}

void setup() {
    Serial.begin(115200);

    pinMode(BUTTON_FUNCTION, INPUT_PULLUP);
    pinMode(BUTTTON_NEXT, INPUT_PULLUP);
    pinMode(BUTTON_PREV, INPUT_PULLUP);

    Wire.begin(21, 22);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("OLED init failed"));
        for (;;);
    }

    display.clearDisplay();
    boot::display_radar();
    display.display();

    connectWiFi();
    syncTime();
}

void loop() {
    currentStateNEXT = digitalRead(BUTTTON_NEXT);
    currentStatePREV = digitalRead(BUTTON_PREV);
    currentStateFUNC = digitalRead(BUTTON_FUNCTION);

    if (lastState_NEXT == HIGH && currentStateNEXT == LOW) {
        Serial.println("Przycisk NEXT wcisniety");
    }

    if (lastState_PREV == HIGH && currentStatePREV == LOW) {
        Serial.println("Przycisk PREV wcisniety");
    }

    if (lastState_FUNC == HIGH && currentStateFUNC == LOW) {
        switch (deviceState.getMode()) {
            case DeviceMode::MAIN:  deviceState.set_mode(DeviceMode::SLEEP); break;
            case DeviceMode::SLEEP: deviceState.set_mode(DeviceMode::DUAL);  break;
            case DeviceMode::DUAL:  deviceState.set_mode(DeviceMode::MAIN);  break;
        }
        lastShownMinute = -1;
        redrawScreen();
    }
    if (!wifiConnected()) {
        if (millis() - lastWifiAttempt >= 30000) {
            lastWifiAttempt = millis();
            connectWiFi(5000);
        }
    } else if (!timeSynced || millis() - lastSync >= DAY_MS) {
        syncTime();
    }

    if (deviceState.getMode() == DeviceMode::SLEEP ||
        deviceState.getMode() == DeviceMode::DUAL) {
        struct tm timeinfo;
        if (getLocalTime(&timeinfo, 0) && timeinfo.tm_min != lastShownMinute) {
            lastShownMinute = timeinfo.tm_min;
            drawSleepScreen();
        }
    }

    lastState_NEXT = currentStateNEXT;
    lastState_PREV = currentStatePREV;
    lastState_FUNC = currentStateFUNC;

    delay(10);
}
