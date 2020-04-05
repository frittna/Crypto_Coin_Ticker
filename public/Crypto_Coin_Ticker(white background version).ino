// Crypto Currency Ticker with 24 candlesticks chart for M5Stack  (White Background Version !) - receiving WiFi data from Binance API/Websocket_v3
//
// it shows 24 candles, min/max prize and volume as line, the date and time are from time.nist.gov timeserver.
// original code was for SPI TFT display ILI9341 and NodeMCU Board from: https://github.com/olbed/bitcoin-ticker from 18.dec.2019
//
// this version is MODIFIED by frittna to use on M5-Stack with ArduinoIDE - created 5.Apr.2020 17:12 CET - Version 1.0.0
// added the use of free fonts, changed format for small currencies, added the use of SPIFFS*) for jpg+png pics, settings will remain stored after a reset
// buttonA: switches through 8 (as many you want) preconfigured pairs e.g: BTC to USDT etc. which are available on Binance.com
// buttonB: changes the LCD-Brightness in 4 levels
// buttonC: 9 changeable Timeframes from 1 Minute to 1 Month
// press buttonC, then, within 2 sec press buttonA to switch down or buttonB to switch up through the timeframes
// available timeframes are 1m, 3m, 5m, 15m, 1h, 4h, 1d, 1w, 1M
// hold ButtonC at Startup: will start with alternative SSID/WiFi-password instead (e.g your mobile phone's hotspot)
// the new infoPanel shows: WiFi-strength, batterylevel and indicates charging (can have delay up to 30s), a colored "busy" light, sleeptimer-active light, changings in %
// SleepTimer: when holding ButtonB longer than 2 seconds it will start a 20 minutes timer to powerOFF the device
// if WiFi is failing more than 2 minutes it reduces the reconnect interval and brightness level, after 10 minutes -> shutdown device
// prepared for the use of a Neopixel RGB-LED bar (i use the built-in one in the Battery-Bottom Module for M5Stack/Fire with rgb 10 LEDs)
// Menu Loader compatible, if SD-Updater (menu.bin) is installed in your SD-card hold buttonA while booting up to start MenuLoader to load your apps
// the impoovements are based quick and dirty solutions - no complains please ;) - changings welcome :)
// known bugs: buttonC sometimes has the bug that it reacts like if it was pressed twice..
//
// INSTALLATION INSTRUCTIONS:
// - download Arduino IDE from their homepage https://www.arduino.cc/en/Main/Software
// - like instructed in the M5-Stack mini-manual be sure to add the additional boards manager url at Arduino preferencies:
//   file -> preferencies: https://dl.espressif.com/dl/package_esp32_index.json  -  then restart Arduino
// - install the M5-Stack board in Arduino: Tools -> Board -> Boards Manager -> search for esp32 and install it
//   afterwards select the right board at the tools menu called M5-Stack-Core-ESP32, then select your actual COM port (restart Arduino 
//   with USB connected to your M5-Stack if no COM-port is shown, also be shure to try the USB connector the other way round if you can't get it done)
// - open new sketch, save it to create a sketch folder and paste all of this code into the code window
// - install all included librarys in your Arduino: Sketch -> Include Library -> Manage Libraries -> search for the correct ones (look very carefully)
//
// - for the esp32fs tool you have to search with google or use the github link bewlow, i have also put all needed files into a folder called public on my github site.
// - to install the esp32fs tool correctly you have to copy the folder called ESP32FS inside the ESP32FS-1.0.zip archive into your Arduino's sketchbook folder
//   so first create a tools folder if there is no and paste the ESP32FS folder in it (it should look like C:\Users\yourName\Documents\Arduino\tools\ESP32FS\tool\esp32fs.jar )
//   (for the standalone verion of Arduino put the esp32fs tool into your current arduino program folder like C:\arduino-1.8.12\tools\ESP32FS\tool\esp32fs.jar )
// - if esp32fs is loaded correctly you can see after a restart of Arduino a tool called ESP32 Sketch Data Uploader in you tools menu 
// - you have to download all my png picture files from my data folder on github and put it into your sketch subfolder called data. (open your sketch folder quickly with CTRL+K) 
//   click verify, afterwards you can click ESP32 Sketch Data Uploader from the tools menu to flash the data into the M5Stack embedded memory
//   
//   If you don't want to use the SPIFFS filesystem at all you can change "drawPngFile(SPIFFS," to "drawPngFile(SD," in my code and put the pics to your root 
//   of your SDcard. All this effort is only because i don't want to use a sd-card at all for my graphics data.
// 
// - finally modify the code at WiFi host/password, timezone and maybe your favorite currency pair or other individual things in the code 
// --> compile and upload to M5Stack - enjoy
//
// ##BEGIN##
//
//       name:                                     from:                                               search library manager for:
// --------------------------------------+------------------------------------------+------ + --------------------------------------------------------------|
#include "Free_Fonts.h"       // Library | Arduino                                  |       |                                                               |
#include <WiFi.h>             // Library | Arduino                                  |       |                                                               |
#include <Preferences.h>      // Library | Arduino                                  |       |                                                               |
#include <M5Stack.h>          // Library | Arduino Librarymanager M5Stack           | 0.2.9 | "M5Stack"                                                     |
#include <Timezone.h>         // Library | Arduino Librarymanager Jack Christensen  | 1.2.4 | "Timezone"                                                    |
#include <time.h>             // Library | Arduino Librarymanager Michael Margolis  | 1.1.6 | "timekeeping"                                                 |
#include <WebSocketsClient.h> // Library | Arduino Librarymanager Markus Sattler    | 2.1.4 | "Websockets"                                                  |
#include <ArduinoJson.h>      // Library | Arduino Librarymanager Benoit Blanchon   | 6.15  | "ArduinoJson"                                                 |
#include "M5StackUpdater.h"   // Library | Arduino Librarymanager SD-Menu Loader    | 0.5.1 | "M5Stack SD"                  |
#include <Adafruit_NeoPixel.h>// Library | Arduino Librarymanager Adafruit NeoPixel | 1.3.5 | "Neopixel"                                                    |
#include "FS.h"               // Library | github: esp32fs for SPIFFS filesystem    | 1.0   | https://github.com/me-no-dev/arduino-esp32fs-plugin           |
// --------------------------------------+------------------------------------------+-------+----------------------------------------------------------------
//
// Wi-Fi connection settings:
const char* ssid     = "**"; // regular wi-fi host
const char* password = "**"; // regular wi-fi password
const char* ssid2     = "**"; // alternative wi-fi host (when ButtonC is held at startup)
const char* password2 = "**"; // alternative wi-fi password (when ButtonC is held at startup)
//
// Time Zone: modify for your local timezone here
//TimeChangeRule summer = {"AEDT", First, Sun, Oct, 2, 660};        // Australia Eastern Time Zone (Sydney, Melbourne)
//TimeChangeRule standard = {"AEST", First, Sun, Apr, 3, 600};
//TimeChangeRule summer = {"BST", Last, Sun, Mar, 1, 60};           // United Kingdom (London, Belfast)
//TimeChangeRule standard = {"GMT", Last, Sun, Oct, 2, 0};
//TimeChangeRule summer = {"EEST", Last, Sun, Mar, 3, 180};         // Eastern European Time (Bulgaria, Greece, Romania, Ukraine, Egypt)
//TimeChangeRule standard = {"EET ", Last, Sun, Oct, 4, 120};
//TimeChangeRule summer = {"CEST", Last, Sun, Mar, 2, 120};         // Central European Time Zone (Frankfurt, Paris)
//TimeChangeRule standard = {"CET ", Last, Sun, Oct, 3, 60};
TimeChangeRule summer = {"EDT", Second, Sun, Mar, 2, -240};     // US Eastern Time Zone (New York, Detroit)
TimeChangeRule standard = {"EST", First, Sun, Nov, 2, -300};
//TimeChangeRule summer = {"CDT", Second, dowSunday, Mar, 2, -300}; // US Central Time Zone (Chicago, Houston)
//TimeChangeRule standard = {"CST", First, dowSunday, Nov, 2, -360};
//TimeChangeRule summer = {"MDT", Second, dowSunday, Mar, 2, -360}; // US Mountain Time Zone (Denver, Salt Lake City)
//TimeChangeRule standard = {"MST", First, dowSunday, Nov, 2, -420};
//TimeChangeRule summer = {"PDT", Second, dowSunday, Mar, 2, -420}  // US Pacific Time Zone (Las Vegas, Los Angeles);
//TimeChangeRule standard = {"PST", First, dowSunday, Nov, 2, -480};
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
byte pairs = 8;       // default: 8, total numbers of currency pairs, max 255
String pair_name[]    = {"BITCOIN",    "Etherium",    "BNB",        "RIPPLE",      "LiteCoin",      "Tron",      "WAVES",        "Elrond"         }; //name for TopPanel (6-8 letters)
String pair_STRING[]  = {"BTCUSDT",    "ETHUSDT",     "BNBUSDT",    "XRPUSDT",     "LTCUSDT",       "TRXUSDT",   "WAVESUSDT",    "ERDUSDT"        }; //name for url in BIG LETTERS
String pair_string[]  = {"btcusdt",    "ethusdt",     "bnbusdt",    "xrpusdt",     "ltcusdt",       "trxusdt",   "wavesusdt",    "erdusdt"        }; //name for url in small letters
uint32_t pair_color[] = {TFT_ORANGE,   TFT_DARKCYAN,  TFT_OLIVE,    TFT_LIGHTGREY, TFT_PURPLE,      TFT_BLUE,    TFT_RED,        TFT_MAGENTA      }; //color in uint32_t format
// predefined color names:
// TFT_BLACK, TFT_NAVY, TFT_DARKGREEN, TFT_DARKCYAN, TFT_MAROON, TFT_PURPLE, TFT_OLIVE, TFT_LIGHTGREY, TFT_DARKGREY,
// TFT_BLUE, TFT_GREEN, TFT_CYAN, TFT_RED, TFT_MAGENTA, TFT_YELLOW, TFT_WHITE, TFT_ORANGE, TFT_GREENYELLOW, TFT_PINK
// Or you can define your own colors in RGB values: with #define my_col M5.Lcd.color565(80,50,125) /* uint16_t color565(uint8_t r, uint8_t g, uint8_t b), */
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// day and month names for topPanel:
const char* weekDay[] = {"", "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
const char* monthName[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};
//const char* weekDay[] = {"", "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};                                             // translated to german
//const char* monthName[] = {"", "Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"}; //translated to german
// using Binance REST API for drawing charts and Binance Web Sockets API to update candles and price.
//  REST API DOCS: https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md
const char* restApiHost = "api.binance.com";
const byte candlesLimit = 24;
const byte timeframes = 9;
const char* candlesTimeframes[timeframes] = {"1m", "3m", "5m", "15m", "1h", "4h", "1d", "1w", "1M"};
int current_Currency;
byte current_Timeframe;
const uint32_t volColor = TFT_LIGHTGREY;
//  WS API DOCS: https://github.com/binance-exchange/binance-official-api-docs/blob/master/web-socket-streams.md
const char* wsApiHost = "stream.binance.com";
const int wsApiPort = 9443;
// Layout: The space between the info and the bottom panel is for candlechart => 240px minus top+info+bottom
const byte topPanel = 22;
const byte infoPanel = 12;
const byte bottomPanel = 36;
WiFiClient client;
WebSocketsClient webSocket;
StaticJsonDocument<8750> jsonDoc;
Timezone myTZ(summer, standard);
TimeChangeRule *tcr;
Preferences preferences; // if you want to clear all stored data in memory of you device run this "preferences.clear();"
// LED-Pixel bar(see end of file for further details)
#define PIN       15 // use the built-in LED bar in the M5-Stack/Fire Battery-Bottom-Module (10xLED on Pin15)
#define NUMPIXELS 10 // 10 LEDs
Adafruit_NeoPixel LEDbar(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
unsigned long sleeptime = 20 * 60;        // SLEEPTIMER: 20m in seconds - hold ButtonB 2sek for sleeptimer to start or stop
unsigned long currentMs;
byte Brightness_level;
int Brightness_value;
unsigned long sleeptimer_counter;
bool sleeptimer_bool;
unsigned int lastPrintTime;
float lastPrice = -1;
float lastLow = -1;
float lastHigh = -1;
bool candleIsNew;
float openTime;
float last_correct_openTime;
float w = 320.0 / candlesLimit;
float center;
int lastTimeframe = -1;
unsigned int wsFails;
byte empty_candles;
bool current_Currency_changed;
bool current_bright_changed;
bool current_timeframe_changed;
bool timeframe_really_changed;
unsigned long currency_btn_timeout_counter = 4294966295; // = max
unsigned long bright_btn_timeout_counter = 4294966295;
unsigned long timeframe_btn_timeout_counter1 = 4294966295;
unsigned long timeframe_btn_timeout_counter2 = 4294966295;
String strname_api;
String strname_ws;
String strname_currency;
uint32_t strname_color;
int err_count;
long multi = 1;
byte multi_level;
bool ws_error;
bool wifi_error;
typedef struct {
  float l; // Low
  float h; // High
  float o; // Open
  float c; // Close
  float v; // Volume
} Candle;
Candle candles[candlesLimit];
float lastCandleOpenTime;
float ph; // Price High
float pl; // Price Low
float vh; // Volume High
float vl; // Volume Low




void setup() {

  M5.begin();
  Wire.begin();
  M5.lcd.setBrightness(0);

  // load last settings from memory on startup
  preferences.begin("settings", false);
  unsigned int last_stored_Currency = preferences.getUInt("currency", 1);
  unsigned int last_stored_Timeframe = preferences.getUInt("timeframe", 4);
  current_Currency = last_stored_Currency;
  current_Timeframe = last_stored_Timeframe;
  strname_api = pair_STRING[current_Currency - 1];
  strname_ws = pair_string[current_Currency - 1];
  strname_currency = pair_name[current_Currency - 1];
  strname_color = pair_color[current_Currency - 1];

  // spiffs filesystem for pictures or maybe sound data
  if (!SPIFFS.begin(true)) {
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // setting alternative WiFi2-settings
  if (digitalRead(BUTTON_C_PIN) == 0) {
    ssid = ssid2;
    password = password2;
  }

  // for optional M5 Stack SD-Menu-Loader
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  M5.update();

  // startup with splashscreen on LCD Display and start up RGB LCD-pixel-bar (Neopixel)
  M5.Lcd.drawPngFile(SPIFFS, "/m5_logo_dark.png", 0, 0);
  for (int i = 0; i < 22 ; i++) {
    Brightness_value += 1;
    M5.lcd.setBrightness(Brightness_value);
    delay(45);                                            // end of fading-in Lcd brightness
  }
  M5.Lcd.fillScreen(TFT_WHITE);
  LEDbar.begin();
  LEDbar.setBrightness(35);                               // Set LED BRIGHTNESS to about 1/5 (max = 255)
  LEDbar.clear();
  colorWipe(LEDbar.Color(  0, 255,   0), 30); // Green    // fill LEDbar in various colors...(delay in ms)
  LEDbar.clear();
  rainbow_effect(3);                                      // Rainbow cycle along the LEDbar..(delay in ms)
  LEDbar.clear();
  LEDbar.show();                                          // update LED status
  M5.Lcd.setTextDatum(MC_DATUM);
  M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_WHITE);
  M5.Lcd.setFreeFont(FSS12);
  M5.Lcd.setTextWrap(false);
  unsigned int last_stored_Brightness_value = preferences.getUInt("bright", 22);
  unsigned int last_stored_Brightness_level = preferences.getUInt("briglv", 2);
  Brightness_value = last_stored_Brightness_value;
  Brightness_level = last_stored_Brightness_level;
  M5.lcd.setBrightness(Brightness_value);                // use last stored brightness value from memory
  yield();

  // Setting Power:
  if (!M5.Power.canControl()) M5.Lcd.printf("IP5306 is not i2c version\n");
  M5.setWakeupButton(BUTTON_B_PIN);                      // because powerOFF is used in sleep timer, no wakeup Button is listened to
  M5.Power.setPowerBoostKeepOn(false);

  // Connecting to WiFi:
  M5.Lcd.print("\n\nConnecting to ");
  M5.Lcd.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_RED);  //busy light red
    M5.Lcd.setTextWrap(true);
    M5.Lcd.print(".");
    err_count ++;
    if (err_count > 288) {                                // if 10 minutes no wifi -> power off device !
      M5.Lcd.drawPngFile(SPIFFS, "/m5_logo_dark.png", 0 , 0);
      delay(1000);
      M5.Lcd.fillScreen(TFT_WHITE);
      delay(1000);
      M5.Power.powerOFF();                                // shut down
    }
    else if (err_count > 240) {                           // if 2 minutes no wifi error persists reduce
      LEDbar.clear();
      colorWipe(LEDbar.Color(255, 255, 0), 30);           // flash yellow twice with LED bar every 10s
      LEDbar.show();                                      // update LED status
      LEDbar.clear();
      LEDbar.show();                                      // update LED status
      delay(100);
      colorWipe(LEDbar.Color(255, 255, 0), 30);
      LEDbar.show();                                      // update LED status
      LEDbar.clear();
      LEDbar.show();                                      // update LED status
      delay(10000);                                       // the reconnect interval from 0.5s to 10s
      M5.lcd.setBrightness(1);                            // and lower the LCD brighness to lowest value (1)
    } else {
      delay(500);
    }
  }
  showWifiStrength();
  err_count = 0;
  M5.lcd.setBrightness(Brightness_value);
  M5.Lcd.println("\nWiFi connected");
  M5.Lcd.setTextWrap(false);
  
  // Settings time:
  M5.Lcd.println("\nWaiting for current time");
  M5.Lcd.println("\ndrawing price and candles");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (true) {
    time_t now = time(nullptr);
    if (now && year(now) > 2017) break;
  }
  delay(400);
  M5.Lcd.fillScreen(TFT_WHITE);
  yield();
  showWifiStrength();
  printTime();

  // load battery meter and draw all candles
  showBatteryLevel();
  while (!requestRestApi()) {}
  drawCandles();
 
  // Connecting to WS:
  webSocket.beginSSL(wsApiHost, wsApiPort, getWsApiUrl());
  webSocket.onEvent(webSocketEvent);
  webSocket.setReconnectInterval(1000);
}




// ***************************************BEGIN LOOP**************************************************
void loop()
{
  currentMs = millis();

  // check for buttons
  buttonActions();
  M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_WHITE);  //reset busy light
  if (currentMs - lastPrintTime >= 35000 && second(time(nullptr)) < 25) {  // draw new date and candles regularly
    lastPrintTime = currentMs;
    printTime();
    multi_level = 0;
    webSocket.disconnect();
    webSocket.beginSSL(wsApiHost, wsApiPort, getWsApiUrl());
    while (!requestRestApi()) {}
    drawCandles();
  }
  webSocket.loop();
  M5.update();                                                             // update button checks
} // **************************************END LOOP ********************************************************




void printTime() {
  time_t now = myTZ.toLocal(time(nullptr), &tcr);
  M5.Lcd.fillRect(0, 0, 320, topPanel, TFT_WHITE); M5.Lcd.setFreeFont(FSSB12);
  M5.Lcd.setCursor(1, 19); M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(strname_color);
  M5.Lcd.print(strname_currency);
  M5.Lcd.setFreeFont(FMB12); M5.Lcd.setCursor(111, 15); M5.Lcd.setTextColor(TFT_DARKGREY);
  M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay[weekday(now)], day(now), monthName[month(now)], hour(now), minute(now));
}




String getRestApiUrl() {
  return "/api/v3/klines?symbol=" + strname_api + "&interval=" + String(candlesTimeframes[current_Timeframe]) +
         "&limit=" + String(candlesLimit);
}



String getWsApiUrl() {
  return "/ws/" + strname_ws + "@kline_" + String(candlesTimeframes[current_Timeframe]);
}




void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_DARKGREY);  //busy light darkgrey
  showWifiStrength();
  switch (type) {
    case WStype_DISCONNECTED:
      wsFails++;
      if (wsFails > 3) error("WS disconnected!");
      ws_error = true;
      break;
    case WStype_CONNECTED:
      break;
    case WStype_TEXT:
      wsFails = 0;
      DeserializationError err = deserializeJson(jsonDoc, payload);
      if (err) {
        error(err.c_str());
        ws_error = true;
        break;
      }
      openTime = jsonDoc["k"]["t"];
      openTime = openTime / 1000;                                              // opentime is sent in milliseconds
      if (openTime == 0) {
        error("empty WS object");
        break;
      }
      candleIsNew = openTime > lastCandleOpenTime;
      if (candleIsNew) {
        lastCandleOpenTime = openTime;
        for (int i = 1; i < candlesLimit; i++) {
          candles[i - 1] = candles[i];
        }
      }
      candles[candlesLimit - 1].o = jsonDoc["k"]["o"];
      candles[candlesLimit - 1].h = jsonDoc["k"]["h"];
      candles[candlesLimit - 1].l = jsonDoc["k"]["l"];
      candles[candlesLimit - 1].c = jsonDoc["k"]["c"];
      candles[candlesLimit - 1].v = jsonDoc["k"]["v"];

      // If we get new low/high we need to redraw all candles, otherwise just last one:
      if (candleIsNew ||
          candles[candlesLimit - 1].l < pl || candles[candlesLimit - 1].h > ph ||
          candles[candlesLimit - 1].v < vl || candles[candlesLimit - 1].v > vh)
      {
        drawCandles();
      } else {
        drawPrice();
        drawCandle(candlesLimit - 1);
      }
      break;
  }
}



bool requestRestApi() {

  WiFiClientSecure client;
  if (!client.connect(restApiHost, 443)) {
    error("\n\nWiFi connection lost!");
    wifi_error = true;
    return false;
  }
  client.print("GET " + getRestApiUrl() + " HTTP/1.1\r\n" +
               "Host: " + restApiHost + "\r\n" +
               "Accept: application/json\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  while (client.connected()) {
    M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_YELLOW);  //busy light yellow
    String line = client.readStringUntil('\r');
    line.trim();
    if (line.startsWith("[") && line.endsWith("]")) {
      DeserializationError err = deserializeJson(jsonDoc, line);
      if (err) {
        error(err.c_str());
        ws_error = true;
        return false;
      } else if (jsonDoc.as<JsonArray>().size() == 0) {
        error("Empty JSON array");
        ws_error = true;
        return false;
      }

      // Data format: [[TS, OPEN, HIGH, LOW, CLOSE, VOL, ...], ...]
      JsonArray _candles = jsonDoc.as<JsonArray>() ;
      for (int i = 0; i < candlesLimit; i++) {
        candles[i].o = _candles[i][1];
        candles[i].h = _candles[i][2];
        candles[i].l = _candles[i][3];
        candles[i].c = _candles[i][4];
        candles[i].v = _candles[i][5];
      }

      // if pair is not listed long enought to draw all requested candles take only the last valid values to not get confused with zeros
      empty_candles = 0;
      for (int i = 0; i < candlesLimit; i++) {
        if (candles[i].c == 0) {
          empty_candles++;
          candles[i].o = candles[i - 1].o;
          candles[i].h = candles[i - 1].h;
          candles[i].l = candles[i - 1].l;
          candles[i].c = candles[i - 1].c;
          candles[i].v = candles[i - 1].v;
          openTime = last_correct_openTime;
        } else {
          last_correct_openTime = openTime;
        }
      }
      lastCandleOpenTime = _candles[candlesLimit - 1 - empty_candles][0];
      lastCandleOpenTime = lastCandleOpenTime / 1000;                 // because time is sent in milliseconds !
      return true;
    }
  }
  ws_error = true;
  error("no JSON in response");
}




// format price:
void formatPrice() {
  for (int i = 0; i < candlesLimit; i++) {
    if (multi_level == 0) {
      if (candles[i].l < 0.001) {
        multi = 100000000;         // if old price is under 0.001$ use exponent format X.XXe-XX
        multi_level = 3;
      }
    }
    if (multi_level < 3) {
      if (candles[i].l < 1) {
        multi = 100000;            // multiply old price if price is under 1$ for higher resolution
        multi_level = 2;
      }
    }
    if (multi_level < 2) {
      if (candles[i].l < 1000) {
        multi = 100;               // multiply old price if price is under 1000$ for higher resolution
        multi_level = 1;
      }
    }
    if (multi_level < 1) {
      if (candles[i].l > 1000) {
        multi = 1;                 // if old price is over 1000$? change nothing
        multi_level = 0;
      }
    }
  }
}




void drawCandles() {

  M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_MAGENTA);  //busy light magenta
  // Find highs and lows
  ph = candles[0 + empty_candles].h;
  pl = candles[0 + empty_candles].l;
  vh = candles[0 + empty_candles].v;
  vl = candles[0 + empty_candles].v;
  for (int i = 0; i < candlesLimit; i++) {
    if (candles[i].h > ph) ph = candles[i].h;
    if (candles[i].l < pl) pl = candles[i].l;
    if (candles[i].v > vh) vh = candles[i].v;
    if (candles[i].v < vl) vl = candles[i].v;
  }

  // format Price for candle drawing if over 1000, under 1000, under 1, or under 0.001
  formatPrice();

  // Draw bottom panel with price, high and low:
  drawPrice();

  // Draw empty candle:
  for (int i = 0; i < empty_candles; i++) {
    drawEmptyCandle(i);
  }

  // Draw candles:
  for (int i = 0; i < candlesLimit - empty_candles; i++) {
    drawCandle(i);
  }
  M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_DARKGREY); // reset magenta busy light;
}

// Remap dollars data to pixels
int getY(float val, float minVal, float maxVal) {
  // function copied from of 'map' math function  (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min
  return round((val - minVal) * (topPanel + infoPanel + 2 - 162 - bottomPanel) / (maxVal - minVal) + 235 - bottomPanel);
}




void drawEmptyCandle(int i) {
  center = w / 2.0;
  center += ((i) * w);
  M5.Lcd.fillRect((center - w) + 5, topPanel + infoPanel, ceil(w), 240 - (topPanel + infoPanel + bottomPanel), TFT_WHITE);
}




// Data format: [[TS, OPEN, HIGH, LOW, CLOSE, VOL, ...]]
void drawCandle(int i) {
  int oy = getY(candles[i].o * multi, pl * multi, ph * multi);
  int hy = getY(candles[i].h * multi, pl * multi, ph * multi);
  int ly = getY(candles[i].l * multi, pl * multi, ph * multi);
  int cy = getY(candles[i].c * multi, pl * multi, ph * multi);
  int vy = getY(candles[i].v, vl , vh);
  int prevVY = vy;
  if (i != 0) {
    prevVY = getY(candles[i - 1].v, vl, vh);
  }
  center = w / 2.0;
  center += ((i + empty_candles) * w);
  uint32_t color = cy < oy ? TFT_GREEN : TFT_RED;

  // Background:
  M5.Lcd.fillRect((center - w) + 5, topPanel + infoPanel, ceil(w), 240 - (topPanel + infoPanel + bottomPanel), TFT_WHITE);

  // Volume line:
  M5.Lcd.drawLine((center - w) + 5, prevVY, center - 5, vy, volColor);
  M5.Lcd.drawLine(center - 4, vy, center + 4, vy, volColor);
  if (i == candlesLimit - 1) {
    M5.Lcd.fillRect(center + 5, topPanel + infoPanel, w / 2, 240 - (topPanel + infoPanel + bottomPanel), TFT_WHITE);
    M5.Lcd.drawLine(center + 5, vy, 320, vy, volColor);
  }

  // Head and tail:
  M5.Lcd.drawLine(center, hy, center, ly, color);

  // Candle body:
  int bodyHeight = abs(cy - oy);
  if (bodyHeight < 1) bodyHeight = 1;             // show at least 1px, if candle body was not formed yet
  M5.Lcd.fillRect(center - 3, min(oy, cy), 7, bodyHeight, color);

  // show actual price as small line in the last candle
  if (i + empty_candles == candlesLimit - 1) M5.Lcd.drawLine(center - 4, cy, center + 4, cy, TFT_WHITE);

  // calculate PrizeChanings for the infoPanel
  PriceChangings();
}




// show remaining battery power (0-100) and draw the matching battery status picture
void  showBatteryLevel() {
  uint8_t battery = M5.Power.getBatteryLevel();
  M5.Lcd.fillRect(34, topPanel, 60, infoPanel, TFT_WHITE);
  if (M5.Power.isCharging())M5.Lcd.drawPngFile(SPIFFS, "/batt_gre.png", 34, topPanel); // show a green battery icon
  else M5.Lcd.drawPngFile(SPIFFS, "/batt_gry.png", 34, topPanel);                      // show a grey battery icon
  if (battery == -1)     M5.Lcd.drawPngFile(SPIFFS, "/batt_nc.png", 34, topPanel);     // 60 x 60 px
  else if (battery < 12) M5.Lcd.drawPngFile(SPIFFS, "/batt0.png",  34, topPanel);
  else if (battery < 32) M5.Lcd.drawPngFile(SPIFFS, "/batt25.png", 34, topPanel);
  else if (battery < 64) M5.Lcd.drawPngFile(SPIFFS, "/batt50.png", 34, topPanel);
  else if (battery < 78) M5.Lcd.drawPngFile(SPIFFS, "/batt75.png", 34, topPanel);
  else if (battery > 90) M5.Lcd.drawPngFile(SPIFFS, "/batt100.png", 34, topPanel);
}




void drawPrice() {
  float price = (candles[candlesLimit - 1].c);
  float oy_var2 (candles[candlesLimit - 1].o);
  if (lastPrice != price) {
    M5.Lcd.fillRect(0, 240 - bottomPanel, 202, bottomPanel, TFT_WHITE); M5.Lcd.setTextSize(1);
    M5.Lcd.setFreeFont(FSSB24); M5.Lcd.setCursor(0, 273 - bottomPanel);
    M5.Lcd.setTextColor(price > oy_var2 ? TFT_GREEN : TFT_RED);
    lastPrice = price;
    if (multi_level == 1) {
      M5.Lcd.printf("%.4f", price);
    }
    else if (multi_level == 2) {
      M5.Lcd.printf("%.6f", price);
    }
    else if (multi_level == 3) {
      M5.Lcd.printf("%.2e", price);
    } else {
      M5.Lcd.printf("%.2f", price);
    }
  }
  if (ph != lastHigh) {
    M5.Lcd.fillRect(202, 240 - bottomPanel, 99, bottomPanel / 2, TFT_WHITE); M5.Lcd.setTextColor(TFT_GREEN);
    M5.Lcd.setFreeFont(FSSB9); M5.Lcd.setCursor(202, 254 - bottomPanel); M5.Lcd.setTextSize(1);
    lastHigh = ph;
    if (multi == 100) {
      M5.Lcd.printf("H %.4f", ph);
    }
    else if (multi == 100000) {
      M5.Lcd.printf("H %.6f", ph);
    }
    else if (multi == 100000000) {
      M5.Lcd.printf("H %.2e", ph);
    } else {
      M5.Lcd.printf("H %.2f", ph);
    }
  }
  if (pl != lastLow) {
    M5.Lcd.fillRect(202, 240 - bottomPanel / 2, 99, bottomPanel / 2, TFT_WHITE); M5.Lcd.setTextColor(TFT_RED);
    M5.Lcd.setFreeFont(FSSB9); M5.Lcd.setCursor(202, 256 - floor(bottomPanel / 2)); M5.Lcd.setTextSize(1);
    lastLow = pl;
    if (multi == 100) {
      M5.Lcd.printf("L %.4f", pl);
    }
    else if (multi == 100000) {
      M5.Lcd.printf("L %.6f", pl);
    }
    else if (multi == 100000000) {
      M5.Lcd.printf("L %.2e", pl);
    } else {
      M5.Lcd.printf("L %.2f", pl);
    }
  }
  if (lastTimeframe != current_Timeframe) {
    lastTimeframe = current_Timeframe;
    String timeframe = candlesTimeframes[current_Timeframe];
    M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_WHITE); M5.Lcd.setFreeFont(FSSB9);
    M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_BLUE); M5.Lcd.setCursor(301, 254 - bottomPanel);
    M5.Lcd.print(timeframe[0]);
    M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
    M5.Lcd.print(timeframe[1]);
  }
}




// price changings for info panel: changings relative to the (mostly 24th) past candles close price in percent:
void PriceChangings() {
  if (!timeframe_really_changed) {
    M5.Lcd.fillRect(108, topPanel, 225, infoPanel, TFT_WHITE);
    M5.Lcd.setFreeFont(FM9); M5.Lcd.setCursor(108, topPanel + infoPanel - 2); M5.Lcd.setTextSize(1);
    if (current_Timeframe == 0) { //1m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 21].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c); //+%.1f shows prefix even if positive with 1 digit after comma
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 21].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":20m");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 21].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":20m");
      }
    }
    else if (current_Timeframe == 1) { //3m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 21].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 21].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":60m");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 21].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":60m");
      }
    }
    else if (current_Timeframe == 2) { //5m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":2h");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":2h");
      }
    }
    else if (current_Timeframe == 3) { //15m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":6h");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":6h");
      }
    }
    else if (current_Timeframe == 4) { //1h
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":24h");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":24h");
      }
    }
    else if (current_Timeframe == 5) { //4h
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":4d");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":4d");
      }
    }
    else if (current_Timeframe == 6) { //1d
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 22].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 22].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 22].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":3w");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 22].c / (candles[candlesLimit - 22].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":3w");
      }
    }
    else if (current_Timeframe == 7) { //1w
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":5M");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":5M");
      }
    }
    else if (current_Timeframe == 8) { //1M
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(TFT_GREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f ", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":2y");
      } else {
        M5.Lcd.setTextColor(TFT_RED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f ", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_BLACK);
        M5.Lcd.print(":2y");
      }
    }
  }
}




// show Wifi-RSSI level (signal strength)
void showWifiStrength() {
  int WifiRSSI = WiFi.RSSI();
//  M5.Lcd.fillRect(2, topPanel + infoPanel + 8, 69, 15, TFT_BLACK);
//  M5.Lcd.setCursor(2, topPanel + infoPanel + 20); M5.Lcd.setFreeFont(FM9); M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_BLACK);
//  M5.Lcd.print(String(WifiRSSI) + "dBm");
  if (WifiRSSI > -50 & ! WifiRSSI == 0 ) M5.Lcd.fillRoundRect(26, topPanel, 5, 12, 1, TFT_BLACK);
  else
    M5.Lcd.fillRoundRect(26, topPanel, 5, 12, 1, TFT_LIGHTGREY);
  if (WifiRSSI > -70 & ! WifiRSSI == 0) M5.Lcd.fillRoundRect(18, topPanel + 2, 5, 10, 1, TFT_BLACK);
  else M5.Lcd.fillRoundRect(18, topPanel + 2, 5, 10, 1, TFT_LIGHTGREY);
  if (WifiRSSI > -80 & ! WifiRSSI == 0) M5.Lcd.fillRoundRect(10, topPanel + 4, 5, 8, 1, TFT_BLACK);
  else M5.Lcd.fillRoundRect(10, topPanel + 4, 5, 8, 1, TFT_LIGHTGREY);
  if (WifiRSSI > -90 & ! WifiRSSI == 0)
  M5.Lcd.fillRoundRect(2, topPanel + 6, 5, 6, 1, TFT_BLACK);
  else M5.Lcd.fillRoundRect(2, topPanel + 6, 5, 6, 1, TFT_RED);
}




// visual error outputs
void error(String text) {
  drawCandles();
  M5.Lcd.setTextWrap(true); M5.Lcd.setTextColor(TFT_WHITE); M5.Lcd.setFreeFont(FSSB12); M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(3, topPanel + infoPanel + 73);
  if (ws_error) {
    M5.Lcd.drawPngFile(SPIFFS, "/ws_error_small.png", 100, topPanel + infoPanel + 5); // show a 50x50px png
    M5.Lcd.print(text);
    ws_error = false;
    M5.Lcd.setTextWrap(false);
  }
  if (wifi_error) {
    M5.Lcd.print(text);
    M5.Lcd.drawPngFile(SPIFFS, "/wifi_error_small.png", 155, topPanel + infoPanel + 5); // show a 50x50px png
    wifi_error = false;
    M5.Lcd.print("\nconnecting to ");
    M5.Lcd.print(ssid);
    WiFi.begin(ssid, password);
    M5.Lcd.setTextWrap(false);
    while (WiFi.status() != WL_CONNECTED) {         // after a wifi error occured try to reconnect
      M5.Lcd.print(".");
      err_count ++;
      if (err_count > 288) {                        // if 10 minutes no wifi -> power off device !
        M5.Lcd.drawPngFile(SPIFFS, "/m5_logo_dark.png", 0 , 0);
        delay(1000);
        M5.Lcd.fillScreen(TFT_WHITE);
        delay(1000);
        M5.Power.powerOFF();                        // shut down
      }
      else if (err_count > 240) {                   // if 2 minutes constant wifi error persists
        LEDbar.clear();
        colorWipe(LEDbar.Color(255, 255, 0), 30);   // flash yellow twice with LED bar every 10s
        LEDbar.show();                              // update LED status
        LEDbar.clear();
        LEDbar.show();                              // update LED status
        delay(100);
        colorWipe(LEDbar.Color(255, 255, 0), 30);
        LEDbar.show();                              // update LED status
        LEDbar.clear();
        LEDbar.show();                              // update LED status
        delay(10000);                               // the reconnect interval from 0.5s to 10s
        M5.lcd.setBrightness(1);                    // and lower the LCD brighness to lowest value (1)
      } else {
        delay(500);
      }
    }
    err_count = 0;
    M5.lcd.setBrightness(Brightness_value);
  }
  // Reset last data to make it redraw after error screen
  delay(2000);
  lastPrice = lastLow = lastHigh = lastTimeframe = -1;
  drawCandles();
}




// BUTTON ACTIONS
void buttonActions() {
  // change current currency button A
  if (M5.BtnA.wasPressed() && !current_timeframe_changed) {
    current_Currency_changed = true;
    currency_btn_timeout_counter = currentMs + 2000;
    ++current_Currency;
    if (current_Currency > pairs) current_Currency = 1;
    strname_color = pair_color[current_Currency - 1];
    strname_currency = pair_name[current_Currency - 1];
    M5.Lcd.fillRect(0, 0, 106, topPanel, TFT_WHITE); M5.Lcd.setCursor(1, 19);
    M5.Lcd.setTextColor(strname_color); M5.Lcd.setFreeFont(FSSB12);; M5.Lcd.setTextSize(1);
    M5.Lcd.print(strname_currency);
    drawCandles();
    M5.Lcd.drawPngFile(SPIFFS, "/currency.png", (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); // center for 100x100px png
  }

  // brighness button B
  if (M5.BtnB.wasPressed() && !current_Currency_changed && !current_timeframe_changed) {
    current_bright_changed = true;
    bright_btn_timeout_counter = currentMs + 750;
    Brightness_level++;
    if (Brightness_level > 4) Brightness_level = 1;                                       // center for 75x75px Png
    drawCandles();
    if (Brightness_level == 1) M5.Lcd.drawPngFile(SPIFFS, "/brightness_low.png",    (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    if (Brightness_level == 2) M5.Lcd.drawPngFile(SPIFFS, "/brightness_medlow.png", (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    if (Brightness_level == 3) M5.Lcd.drawPngFile(SPIFFS, "/brightness_med.png",    (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    if (Brightness_level == 4) M5.Lcd.drawPngFile(SPIFFS, "/brightness_high.png",   (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    switch (Brightness_level) {
      case 1:
        Brightness_value = 1;                                   // low LCD brightness
        break;
      case 2:
        Brightness_value = 22;                                  // medium low LCD brightness
        break;
      case 3:
        Brightness_value = 64;                                  // medium LCD brightness
        break;
      case 4:
        Brightness_value = 150;                                 // high LCD brightness
        break;
    }
    M5.lcd.setBrightness(Brightness_value);
  }

  //  timeframe change buttons: after C was pressed press A or B for - and +
  if (M5.BtnC.wasPressed() && !current_timeframe_changed && !current_Currency_changed && !current_bright_changed) {
    delay(105);  //attempt to debounce buttonC
    current_timeframe_changed = true;
    timeframe_btn_timeout_counter1 = currentMs + 2000;
    String timeframe = candlesTimeframes[current_Timeframe];
    M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_WHITE); M5.Lcd.setFreeFont(FSSB9);
    M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
    M5.Lcd.print(timeframe[0]);
    M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
    M5.Lcd.print(timeframe[1]);
    drawCandles();
    M5.Lcd.drawPngFile(SPIFFS, "/timeframe.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2));     // center for 100x100px Png
  }
  if (M5.BtnB.wasPressed() && current_timeframe_changed && !current_Currency_changed && !current_bright_changed) {   // set a higher timeframe
    if (current_Timeframe <= 7 && current_Timeframe >= 0) {
      timeframe_really_changed = true;
      timeframe_btn_timeout_counter1 = currentMs + 2000;
      timeframe_btn_timeout_counter2 = currentMs + 2000;
      current_Timeframe++;
      String timeframe = candlesTimeframes[current_Timeframe];
      M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_WHITE); M5.Lcd.setFreeFont(FSSB9);
      M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
      M5.Lcd.print(timeframe[0]);
      M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
      M5.Lcd.print(timeframe[1]);
      drawCandles();
      switch (current_Timeframe) {
        case 0: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1_m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;    // center for 100x100px
        case 1: M5.Lcd.drawPngFile(SPIFFS, "/timeframe3m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 2: M5.Lcd.drawPngFile(SPIFFS, "/timeframe5m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 3: M5.Lcd.drawPngFile(SPIFFS, "/timeframe15m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 4: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 5: M5.Lcd.drawPngFile(SPIFFS, "/timeframe4H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 6: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1D.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 7: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1W.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 8: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1M.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
      }
    }
  }
  if (M5.BtnA.wasPressed() && current_timeframe_changed && !current_Currency_changed && !current_bright_changed) {   // set a lower timeframe
    if (current_Timeframe <= 8 && current_Timeframe >= 1) {
      timeframe_really_changed = true;
      timeframe_btn_timeout_counter1 = currentMs + 2000;
      timeframe_btn_timeout_counter2 = currentMs + 2000;
      current_Timeframe--;
      String timeframe = candlesTimeframes[current_Timeframe];
      M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_WHITE); M5.Lcd.setFreeFont(FSSB9);
      M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
      M5.Lcd.print(timeframe[0]);
      M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
      M5.Lcd.print(timeframe[1]);
      drawCandles();
      switch (current_Timeframe) {
        case 0: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1_m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;    // center for 100x100px
        case 1: M5.Lcd.drawPngFile(SPIFFS, "/timeframe3m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 2: M5.Lcd.drawPngFile(SPIFFS, "/timeframe5m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 3: M5.Lcd.drawPngFile(SPIFFS, "/timeframe15m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 4: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 5: M5.Lcd.drawPngFile(SPIFFS, "/timeframe4H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 6: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1D.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 7: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1W.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 8: M5.Lcd.drawPngFile(SPIFFS, "/timeframe1M.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
      }
    }
  }
  // do after a delay of a button press..
  if (current_Currency_changed && currency_btn_timeout_counter < currentMs) {
    current_Currency_changed = false;
    currency_btn_timeout_counter = 4294966295;
    strname_api = pair_STRING[current_Currency - 1];
    strname_ws = pair_string[current_Currency - 1];
    lastPrintTime = currentMs;
    printTime();
    multi_level = 0;
    webSocket.disconnect();
    webSocket.beginSSL(wsApiHost, wsApiPort, getWsApiUrl());
    while (!requestRestApi()) {}
    preferences.putUInt("currency", current_Currency);                         // store setting to memory
    showBatteryLevel();
    drawCandles();
  }
  if (current_bright_changed && bright_btn_timeout_counter < currentMs) {      // clear png icon with drawCandles()
    current_bright_changed = false;
    preferences.putUInt("bright", Brightness_value);                           // store setting to memory
    preferences.putUInt("briglv", Brightness_level);                           // store setting to memory
    bright_btn_timeout_counter = 4294966295;
    drawCandles();
  }
  if (current_timeframe_changed && timeframe_really_changed && timeframe_btn_timeout_counter1 < currentMs) {
    current_timeframe_changed = false;
    timeframe_really_changed = false;
    timeframe_btn_timeout_counter1 = 4294966295;
    multi_level = 0;
    webSocket.disconnect();
    webSocket.beginSSL(wsApiHost, wsApiPort, getWsApiUrl());
    preferences.putUInt("timeframe", current_Timeframe);                         // store setting to memory
    while (!requestRestApi()) {}
    showBatteryLevel();
    drawCandles();
  }
  if (current_timeframe_changed && !timeframe_really_changed && timeframe_btn_timeout_counter1 < currentMs) {
    current_timeframe_changed = false;
    timeframe_btn_timeout_counter1 = 4294966295;
    drawCandles();
  }
  if (current_timeframe_changed && !timeframe_really_changed && timeframe_btn_timeout_counter2 < currentMs) {
    current_timeframe_changed = false;
    timeframe_btn_timeout_counter2 = 4294966295;
    drawCandles();
  }

  // Button for Sleep Timer zzz zzZ..ZZ.Z.ZZ..Zzz zzz
  if (M5.BtnB.pressedFor(2000)) {
    if (!sleeptimer_bool) {      // enables sleeptimer if buttonB is long pressed
      sleeptimer_bool = true;
      sleeptimer_counter = now();
      Brightness_level--;        // because brighness and sleeptimer are on same ButtonA correct brightness one level back
      if (Brightness_level < 1) Brightness_level = 4;
      switch (Brightness_level) {
        case 1:
          Brightness_value = 1;                                   // lowest LCD brightness
          break;
        case 2:
          Brightness_value = 22;                                  // medium low LCD brightness
          break;
        case 3:
          Brightness_value = 64;                                  // medium high LCD brightness
          break;
        case 4:
          Brightness_value = 150;                                 // highest LCD brightness
          break;
      }
      M5.lcd.setBrightness(Brightness_value);
    preferences.putUInt("bright", Brightness_value);                           // store setting to memory
    preferences.putUInt("briglv", Brightness_level);                           // store setting to memory
      M5.Lcd.drawCircle(101, topPanel + (infoPanel / 2), 5, TFT_BLUE); // set blue status light when sleeptimer was activated
      drawCandles();
      M5.Lcd.drawPngFile(SPIFFS, "/sleep20m.png", (320 / 2) - (100 / 2), (240 / 2) - ((100 / 2))); // sleep.png is 100x100px
      LEDbar.clear();
      colorWipe(LEDbar.Color(0, 0, 255), 35); // Blue             // fill LEDbar in various colors...(delay in ms)
      LEDbar.clear();
      LEDbar.show();                                              // update LED status
      delay(1300);
      drawCandles();
    }
    else if (sleeptimer_bool) {                                   // disables sleeptimer if it is already enabled
      sleeptimer_bool = false;
      sleeptimer_counter = 0;
      Brightness_level--;        // because brighness and sleeptimer are on same ButtonA correct brightness one level back
      if (Brightness_level < 1) Brightness_level = 4;
      switch (Brightness_level) {
        case 1:
          Brightness_value = 1;                                   // low LCD brightness
          break;
        case 2:
          Brightness_value = 22;                                  // medium low LCD brightness
          break;
        case 3:
          Brightness_value = 64;                                  // medium LCD brightness
          break;
        case 4:
          Brightness_value = 150;                                 // high LCD brightness
          break;
      }
      M5.lcd.setBrightness(Brightness_value);
      preferences.putUInt("bright", Brightness_value);            // store setting to memory
      preferences.putUInt("briglv", Brightness_level);            // store setting to memory
      M5.Lcd.drawCircle(101, topPanel + (infoPanel / 2), 5, TFT_WHITE); // reset blue status light when sleeptimer was deactivated
      drawCandles();
      M5.Lcd.drawPngFile(SPIFFS, "/cancel_sleep20m.png", (320 / 2) - (130 / 2), (240 / 2) - ((130 / 2))); //sleep.png is 130x130px
      LEDbar.clear();
      colorWipe(LEDbar.Color(255, 255, 255), 35); // White        // fill LEDbar in various colors...(delay in ms)
      LEDbar.clear();
      LEDbar.show();                                              // update LED status
      delay(1300);
      drawCandles();
    }
  }
  if (sleeptimer_bool == true && (now() > (sleeptimer_counter + sleeptime))) {
    sleeptimer_bool = false;
    M5.Lcd.drawCircle(101, topPanel + (infoPanel / 2), 5, TFT_WHITE); // reset status light when sleeptimer was finished
    M5.Lcd.drawPngFile(SPIFFS, "/m5_logo_dark.png", 0 , 0);
    delay(1000);
    for (int i = Brightness_value; i > 0 ; i--) {                  // dimm LCD slowly before shutdown
      Brightness_value -= 1;
      M5.lcd.setBrightness(Brightness_value);
      delay(50);
    }
    M5.Lcd.fillScreen(TFT_WHITE);
    delay(1000);
    M5.Power.powerOFF();
  }
}



// LED functions - for details see: https://github.com/adafruit/Adafruit_NeoPixel
// Fill LEDbar pixels one after another with a color. LEDbar is NOT cleared
// first; anything there will be covered pixel by pixel. Pass in color
// (as a single 'packed' 32-bit value, which you can get by calling
// LEDbar.Color(red, green, blue) as shown in the loop() function above),
// and a delay time (in milliseconds) between pixels.

// colorWipe
void colorWipe(uint32_t color, int wait) {
  for (int i = 0; i < LEDbar.numPixels(); i++) {                  // For each pixel in LEDbar...
    LEDbar.setPixelColor(i, color);                               // Set pixel's color (in RAM)
    LEDbar.show();                                                // Update LEDbar to match
    delay(wait);                                                  // Pause for a moment
  }
}

// Rainbow cycle along whole LEDbar. Pass delay time (in ms) between frames.
void rainbow_effect(int wait) {
  // Hue of first pixel runs 1* complete loops through the color wheel.  *modified from 5
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 1 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < LEDbar.numPixels(); i++) { // For each pixel in LEDbar...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the LEDbar
      // (LEDbar.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / LEDbar.numPixels());
      // LEDbar.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through LEDbar.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      LEDbar.setPixelColor(i, LEDbar.gamma32(LEDbar.ColorHSV(pixelHue)));
    }
    LEDbar.show();                                                // Update LEDbar with new contents
    delay(wait);                                                  // Pause for a moment
  }
}
// ##END##
