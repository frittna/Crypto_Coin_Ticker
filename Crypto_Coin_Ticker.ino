// CRYPTO CURRENCY PRICE TICKER with 24 candlesticks chart for M5Stack
// ##SD-CARD Version with configuation file##
// receiving WiFi data from Binance API/Websocket_v3 - by frittna (https://github.com/frittna/Crypto_Coin_Ticker)

// This will show 24 candles, the min/max price and the volume as line, date and time are from time.nist.gov timeserver.
// For M5-Stack MCU , coded in ArduinoIDE 1.8.13 - last modified Apr.18.2021 12:10 CET - Version 1.0.4

//------------------------------------------------------------------------------------------------------------------------------

// #Using the App:
// ###############
// This version needs a SD-Card whith the your WiFi credentials, favourite coinpairs, timezone and language(eng/ger/esp) in a textfile - see installation
// ButtonA: switches through your favourite Coinpair (as many you want) e.g: BTC/USDT etc. which are available on Binance.com
// ButtonB: changes the LCD-Brightness in 4 levels
// ButtonC: 9 changeable Timeframes from 1 Minute to 1 Month
// turn OFF the device pressing the red button once OR by holding ButtonC for over 1 second if USB is connected
// Press buttonC, then, within 2 sec press buttonA to switch down, or buttonB to switch up through the timeframes: 1min->15mins->1hour->..
// available timeframes are 1minute, 3m, 5m, 15m, 1h, 4h, 1d, 1w, 1Month
// if you hold ButtonC at Startup: it will start with alternative SSID/WiFi-password instead (e.g your mobile phone's hotspot)


// #Further description:
// #####################
// The top infoPanel shows the WiFi-strength, batterylevel, colored indicators for "busy", SleepTimer, price moving and if charging from usb (can have delay up to 30s)
// right now: english, german, spanish Language (day and month names)
// SleepTimer: when holding ButtonB longer than 1,5 seconds it will start a user defined timer to powerOFF the device
// If WiFi is failing more than 2 minutes it reduces the reconnect interval and brightness level, after 10 minutes -> shutdown device
// Menu Loader compatible, if SD-Updater (menu.bin) is installed in your SD-card hold buttonA while booting up to start MenuLoader to load your apps
// It is prepared for the use of a Neopixel RGB-LED bar (i use the built-in one in the Battery-Bottom Module for M5Stack/Fire with rgb 10 LEDs)
// Some settings like current timeframe, brightness level, active coinpair and the last wifi credentials will remain stored in internal memory after a reset.
// If no SD-Card is installed iw will load 4 default coinpairs, us-tinezone and englsish language. It will try to load the last set wifi-name/pw from flash memory.

// INSTALLATION INSTRUCTIONS
// #########################	
// - find a way to transfer the BIN file of this APP into your M5 Device. A very easy way is to load "M5Burner_ccticker.zip" from https://github.com/frittna/Crypto_Coin_Ticker
//// - ! IMPORTANT ! - you will need "ccticker - zipfile.zip" from there too.
//   To run the App correctly you need to have the "data" folder from my zip file in a folder called "ccticker" on the root dir of SD-Card.
//   Then you must have the config file "ccticker.cfg" inside this "ccticker" folder.
//   Modify the settings file "ccticker.cfg" with your personal wifi ssid/password, timezone, favorite currency pair - use a simple text editor !
//   So on the sd-card you should have a folder like G:\ccticker\data\ with all graphic files and the configuration file like this: G:\ccticker\ccticker.cfg
//   When you're done insert into the M5Stack and boot it up.

// (*INSTALLATION INSTRUCTIONS from skratch) ->> only for those who compile by their own in Arduino IDE <<--
//   note: you can adjust basically all main settings in the conig file from sd-card. But if you want to build 
//         it on your own or if you want to modify something special or even extend this cool gadget go for it!
// - download Arduino IDE from their homepage https://www.arduino.cc/en/Main/Software
// - like instructed in the M5-Stack mini-manual be sure to add the additional boards manager url at Arduino preferencies:
//   file -> preferencies: https://dl.espressif.com/dl/package_esp32_index.json  -  then restart Arduino
// - install the M5-Stack board in Arduino: Tools -> Board -> Boards Manager -> search for esp32 and install ver.1.0.4
//   ---> !! use ESP32 Board Manager Version 1.0.4 since higher versions are reported to fail !! <<---
//   afterwards select the right board at the tools menu called M5-Stack-Core-ESP32, then select your actual COM port (restart Arduino
//   with USB connected to your M5-Stack if no COM-port is shown, also be shure to try the USB connector the other way round if you can't get it done)
// - open new sketch, save it to create a sketch folder and paste all of this code into the code window
// - install all included librarys in your Arduino: Sketch -> Include Library -> Manage Libraries -> search for the correct ones (look very carefully)
// - compile and see the instructions above for adjusting config file and where to put the /data/ folder with all the png images on SD



// The core for the candlestick view and binance api was from: https://github.com/olbed/bitcoin-ticker on SPI TFT display ILI9341 and NodeMCU Board, from 2019

// Known bugs to fix: buttonC debouncing, battery symbol could be more precise, untested format if price will pass the 100k ;)
//                    maybe some inefficient code since this is my first public release

//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------
//   NOT USED in this version - just skip that -
//   this is only for the users who want to hardcode everything use NO SD-Card at all and want to compile the program in Arduino-IDE, SPIFFS memory for graphics data
//
// - for the esp32fs tool (for uploading the SPIFFS files) you have to search with google or use the github link https://github.com/me-no-dev/arduino-esp32fs-plugin, 
//   i have also put all needed files into a folder called public on my github site.
// - to install the esp32fs tool correctly you have to copy the folder called ESP32FS inside the ESP32FS-1.0.zip archive into your Arduino's sketchbook folder
//   so first create a tools folder if there is no and paste the ESP32FS folder in it (it should look like C:\Users\yourName\Documents\Arduino\tools\ESP32FS\tool\esp32fs.jar )
//   (for the standalone verion of Arduino put the esp32fs tool into your current arduino program folder like C:\arduino-1.8.13\tools\ESP32FS\tool\esp32fs.jar )
// - if esp32fs is loaded correctly you can see after a restart of Arduino a tool called ESP32 Sketch Data Uploader in you tools menu
// - you have to download all my png picture files from my data folder on github and put it into your sketch subfolder called data. (open your sketch folder quickly with CTRL+K)
//   click verify, afterwards you can click ESP32 Sketch Data Uploader from the tools menu to flash the data into the M5Stack embedded memory
//   If you don't want to use the SPIFFS filesystem at all you can change "drawPngFile(SPIFFS, " to "drawPngFile(SD, " in my code and put the pics to your root
//   of your SDcard. All this effort is only because i don't want to use a sd-card at all for my graphics data and i want to make the SD-Card optional only (for SD-Menu-Loader e.g.)
//   <<<<<<
//------------------------------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------------------------------




// ##BEGIN##


// ---->> for this SD-Card Version nothing has to be edited here - use the ccticker.cfg textfile on the SD-Card in "ccticker" folder <<----

// Wi-Fi connection settings:
String ssid      = "";  // change here to "myName" or whatever you have as Wifi SSID name (127 characters max.)
String password  = "";  // enter your password like "myPassword"

//optional:
String ssid2     = "";  // alternative wi-fi network to connect when ButtonC is held at startup
String password2 = "";  //

//       name:                                     from:                             version                search library manager exactly for:
// ---------------------------------------+------------------------------------------+------ + --------------------------------------------------------------|
#include "Free_Fonts.h"       // Library  | Arduino                                  |       |                                                               |
#include <SDConfig.h>         // Library  | Arduino Librarymanager Claus Mancini     | 1.1.0 | "SDConfig"                                                    |
#include <WiFi.h>             // Board-pkg| -> Problems on start? look below         |1.0.4**|                                                               |
#include <Preferences.h>      // Library  | Arduino                                  |       |                                                               |
#include <M5Stack.h>          // Library  | Arduino Librarymanager M5Stack           | 0.3.1 | "M5Stack"                                                     |
#include <Timezone.h>         // Library  | Arduino Librarymanager Jack Christensen  | 1.2.4 | "Timezone"                                                    |
#include <time.h>             // Library  | Arduino Librarymanager Michael Margolis  | 1.6   | "timekeeping"                                                 |
#include <WebSocketsClient.h> // Library  | Arduino Librarymanager Markus Sattler    | 2.3.0 | "Websockets"                                                  |
#include <ArduinoJson.h>      // Library  | Arduino Librarymanager Benoit Blanchon   | 6.17.3| "ArduinoJson"                                                 |
#include "M5StackUpdater.h"   // Library  | Arduino Librarymanager SD-Menu Loader    | 0.5.2!| "M5Stack SD"  i use 0.5.2 , not new 1.0.2 because of problems |
#include <Adafruit_NeoPixel.h>// Library  | Arduino Librarymanager Adafruit NeoPixel | 1.7.0 | "Adafriut Neopixel"                                           |
// ---------------------------------------+------------------------------------------+-------+----------------------------------------------------------------
//#include "FS.h" //esp32 filesystem for spiffs (not used anymore)

//  **if you have problems resulting a reset or similair after powering up you schould check
//    all versions again if they are really identical to the ones from this instruction above.
//    ---->>  especially for the ESP32 Board Manager use Version 1.0.4 since higher versions are reported to fail !!

int pinSelectSD = 4; // SD shield Chip Select pin. (4 for M5Stack)
boolean readConfiguration();
char configFile[] = "/ccticker/ccticker.cfg"; // filename for configuration file in directory yourSD:\ccticker\ - if no file/card is found 4 pairs will be loaded by default
char* welcome = "Hey You! ;)";
int myTimeZone = 4;   //use 4 if nothing else instructed = US Eastern Time Zone EST (New York, Detroit)
int myLanguage = 0;   //use 0 if nothing else instructed = english
int pairs = 4;        //total numbers of coin pairs to load, default: 4, max. 36 (or up to 256 if you rewrite the sd card read-in recognition)
String pair_string[36];
String pair_STRING[36]        = {"BTCUSDT",    "ETHUSDT",       "DOTUSDT",     "LINKUSDT"};   //default values if no external config file exists
String pair_name[36]          = {" Bitcoin",   "   ETH",        "   DOT",      "  LINK"  };
char* str_pair_color[36]      = {"0xFD20",     "0x03EF",        "0xC618",      "0x001F"  };
uint16_t rgb565_pair_color[36]= { 0xFD20,       0x03EF,          0xC618,        0x001F   };
// Notes for color names: use hex in rgb565 format like: 0x07FF
// or you can define your own colors in RGB values with #define my_col M5.Lcd.color565(80,50,125) /* uint16_t color565(0-255, 0-255, 0-255) for RGB, */
#define DARKERGREEN M5.Lcd.color565(146,217,0)  /* uint16_t color565(uint8_t r, uint8_t g, uint8_t b), */
#define MYRED M5.Lcd.color565(255,0,28) /* uint16_t color565(uint8_t r, uint8_t g, uint8_t b), */
const char*   weekDay_MyLang[8];
const char* monthName_MyLang[13];
const char*   weekDay_Language0[] = {"", "Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};                                           // english language
const char* monthName_Language0[] = {"", "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"}; // english language
const char*   weekDay_Language1[] = {"", "So", "Mo", "Di", "Mi", "Do", "Fr", "Sa"};                                           // translated to german
const char* monthName_Language1[] = {"", "Jan", "Feb", "Mar", "Apr", "Mai", "Jun", "Jul", "Aug", "Sep", "Okt", "Nov", "Dez"}; // translated to german
const char*   weekDay_Language2[] = {"", "do", "lu", "ma", "mi", "ju", "vi", "sá"};                                           // translated to spanish
const char* monthName_Language2[] = {"", "ene", "feb", "mar", "abr", "may", "jun", "jul", "ago", "sep", "oct", "nov", "dic"}; // translated to spanish
// REST API DOCS: https://github.com/binance-exchange/binance-official-api-docs/blob/master/rest-api.md
const char* restApiHost = "api.binance.com";
const byte candlesLimit = 24;
const byte timeframes = 9;
const char* candlesTimeframes[timeframes] = {"1m", "3m", "5m", "15m", "1h", "4h", "1d", "1w", "1M"};
int current_Currency;
byte current_Timeframe;
const uint32_t volColor = 0x22222a;
// WS API DOCS: https://github.com/binance-exchange/binance-official-api-docs/blob/master/web-socket-streams.md
const char* wsApiHost = "stream.binance.com";
const int wsApiPort = 9443;
// Layout: The space between the info and the bottom panel is for candlechart => 240px minus top+info+bottom
const byte topPanel = 22;
const byte infoPanel = 12;
const byte bottomPanel = 36;
Preferences preferences; //the last network credentials are saved, also the last coinpair, timeframe and brightness setting
WiFiClient client;
WebSocketsClient webSocket;
StaticJsonDocument<8750> jsonDoc;
TimeChangeRule summer_aedt =   {"AEDT", First, Sun, Oct, 2, 660};        // 0: Australia Eastern Time Zone (Sydney, Melbourne)
TimeChangeRule standard_aest = {"AEST", First, Sun, Apr, 3, 600};
TimeChangeRule summer_bst =    {"BST", Last, Sun, Mar, 1, 60};           // 1: United Kingdom (London, Belfast)
TimeChangeRule standard_gmt =  {"GMT", Last, Sun, Oct, 2, 0};
TimeChangeRule summer_eest =   {"EEST", Last, Sun, Mar, 3, 180};         // 2: Eastern European Time (Bulgaria, Greece, Romania, Ukraine, Egypt)
TimeChangeRule standard_eet =  {"EET ", Last, Sun, Oct, 4, 120};
TimeChangeRule summer_cest =   {"CEST", Last, Sun, Mar, 2, 120};         // 3: Central European Time Zone (Frankfurt, Paris)
TimeChangeRule standard_cet =  {"CET ", Last, Sun, Oct, 3, 60};
TimeChangeRule summer_edt =    {"EDT", Second, Sun, Mar, 2, -240};       // 4: US Eastern Time Zone (New York, Detroit)
TimeChangeRule standard_est =  {"EST", First, Sun, Nov, 2, -300};
TimeChangeRule summer_cdt =    {"CDT", Second, dowSunday, Mar, 2, -300}; // 5: US Central Time Zone (Chicago, Houston)
TimeChangeRule standard_cst =  {"CST", First, dowSunday, Nov, 2, -360};
TimeChangeRule summer_mdt =    {"MDT", Second, dowSunday, Mar, 2, -360}; // 6: US Mountain Time Zone (Denver, Salt Lake City)
TimeChangeRule standard_mst =  {"MST", First, dowSunday, Nov, 2, -420};
TimeChangeRule summer_pdt =    {"PDT", Second, dowSunday, Mar, 2, -420}; // 7: US Pacific Time Zone (Las Vegas, Los Angeles);
TimeChangeRule standard_pst =  {"PST", First, dowSunday, Nov, 2, -480};
Timezone myTZ0(summer_aedt, standard_aest);    // myTZ0: 0 Australia Eastern Time Zone (Sydney, Melbourne)
Timezone myTZ1(summer_bst, standard_gmt);      // myTZ1: 1 United Kingdom (London, Belfast)
Timezone myTZ2(summer_eest, standard_eet);     // myTZ2: 2 Eastern European Time (Bulgaria, Greece, Romania, Ukraine, Egypt)
Timezone myTZ3(summer_cest, standard_cet);     // myTZ3: 3 Central European Time Zone (Frankfurt, Paris)
Timezone myTZ4(summer_edt, standard_est);      // myTZ4: 4 US Eastern Time Zone (New York, Detroit)
Timezone myTZ5(summer_cdt, standard_cst);      // myTZ5: 5 US Central Time Zone (Chicago, Houston)
Timezone myTZ6(summer_mdt, standard_mst);      // myTZ6: 6 US Mountain Time Zone (Denver, Salt Lake City)
Timezone myTZ7(summer_pdt, standard_pst);      // myTZ7: 7 US Pacific Time Zone (Las Vegas, Los Angeles);
// LED-Pixel bar(see bottom of this file for further details)
#define PIN       15 // use the built-in LED bar in the M5-Stack/Fire Battery-Bottom-Module (10xLED on Pin15)
#define NUMPIXELS 10 // 10 LEDs
Adafruit_NeoPixel LEDbar(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
int mySleeptime = 45;
unsigned long sleeptime = mySleeptime * 60;        // SLEEPTIMER: in minutes - hold ButtonB long for sleeptimer to start or stop
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
bool alt_hotspot = false;
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

//-----------------------------------------------------------------------------------------------------------------------------

void setup() {
  M5.begin();
  Wire.begin();
  M5.lcd.setBrightness(0);
  Serial.begin(115200);
  pinMode(pinSelectSD, OUTPUT);

  // Setup the SD card
  delay(200);
  Serial.println();
  Serial.println  ("POWER ON DEVICE");
  Serial.println();
  Serial.println  ("accessing SD-Card..");
  if (!SD.begin(pinSelectSD)) {
    Serial.println("SD-Card access failed !");
    Serial.println("is card inserted ?");
    Serial.println("supported: FAT&FAT32 only");
  }
  Serial.println  ("successfully..");

  //  // spiffs filesystem for pictures or maybe sound data (not used anymore in SD-Card Version)
  //  if (!SPIFFS.begin(true)) {
  //    Serial.println("SPIFFS Mount Failed");
  //    return;
  //  }

  // setting alternative WiFi2-settings
  if (digitalRead(BUTTON_C_PIN) == 0) {
    alt_hotspot = true;
  }

  // for optional M5 Stack SD-Menu-Loader
  if (digitalRead(BUTTON_A_PIN) == 0) {
    Serial.println("Will Load menu binary");
    updateFromFS(SD);
    ESP.restart();
  }
  M5.update();

  // startup with splashscreen on LCD Display and start up RGB LCD-pixel-bar (Neopixel)
  M5.Lcd.drawPngFile(SD, "/ccticker/data/m5_logo_dark.png", 0, 0);
  for (int i = 0; i < 22 ; i++) {
    Brightness_value += 1;
    M5.lcd.setBrightness(Brightness_value);
    delay(45);                                            // end of fading-in Lcd brightness
  }
  M5.Lcd.fillScreen(TFT_BLACK);
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

  // Setting Power:   see for details: https://github.com/m5stack/m5-docs/blob/master/docs/en/api/power.md
  if (!M5.Power.canControl()) M5.Lcd.printf("IP5306 is not i2c version\n");
  M5.Power.setPowerBtnEn(true);           //allow red power button
  M5.Power.setPowerBoostSet(true);        //one press on red turns on/off device
  M5.Power.setPowerVin(false);            //no reset when usb calbe is plugged in

  // routine to read configuration from SD card file
  int maxLineLength = 127; //Length of the longest line expected in the config file
  SDConfig cfg;
  // Open the configuration file.
  if (!cfg.begin(configFile, maxLineLength)) {
    Serial.print("Failed to open configuration file: ");
    Serial.println(configFile);
  }
  while (cfg.readNextSetting()) { //at the moment are up to 36 pairs possible
    // Put a nameIs() block here for each setting you have.
    if (cfg.nameIs("welcome")) {  // welcome string (char *)
      welcome = cfg.copyValue(); Serial.print("Read welcome: "); Serial.println(welcome);
    } else if (cfg.nameIs("ssid")) {
      ssid = cfg.copyValue(); Serial.print("Read ssid: "); Serial.println(ssid);
    } else if (cfg.nameIs("password")) {
      password = cfg.copyValue(); Serial.print("Read password: "); Serial.println(password);
    } else if (cfg.nameIs("ssid2")) {
      ssid2 = cfg.copyValue(); Serial.print("Read ssid2: "); Serial.println(ssid2);
    } else if (cfg.nameIs("password2")) {
      password2 = cfg.copyValue(); Serial.print("Read password2: "); Serial.println(password2);
    } else if (cfg.nameIs("pairs")) {
      pairs = cfg.getIntValue(); Serial.print("Read pairs: "); Serial.println(pairs);
    }

    else if (cfg.nameIs("pair_name1")) {    // from here i did it one by one as don't get it to work with a for loop
      pair_name[0] = cfg.copyValue(); Serial.print("Read pair_name1: "); Serial.println(pair_name[0]);
    } else if (cfg.nameIs("pair_name2")) {
      pair_name[1] = cfg.copyValue(); Serial.print("Read pair_name2: "); Serial.println(pair_name[1]);
    } else if (cfg.nameIs("pair_name3")) {
      pair_name[2] = cfg.copyValue(); Serial.print("Read pair_name3: "); Serial.println(pair_name[2]);
    } else if (cfg.nameIs("pair_name4")) {
      pair_name[3] = cfg.copyValue(); Serial.print("Read pair_name4: "); Serial.println(pair_name[3]);
    } else if (cfg.nameIs("pair_name5")) {
      pair_name[4] = cfg.copyValue(); Serial.print("Read pair_name5: "); Serial.println(pair_name[4]);
    } else if (cfg.nameIs("pair_name6")) {
      pair_name[5] = cfg.copyValue(); Serial.print("Read pair_name6: "); Serial.println(pair_name[5]);
    } else if (cfg.nameIs("pair_name7")) {
      pair_name[6] = cfg.copyValue(); Serial.print("Read pair_name7: "); Serial.println(pair_name[6]);
    } else if (cfg.nameIs("pair_name8")) {
      pair_name[7] = cfg.copyValue(); Serial.print("Read pair_name8: "); Serial.println(pair_name[7]);
    } else if (cfg.nameIs("pair_name9")) {
      pair_name[8] = cfg.copyValue(); Serial.print("Read pair_name9: "); Serial.println(pair_name[8]);
    } else if (cfg.nameIs("pair_name10")) {
      pair_name[9] = cfg.copyValue(); Serial.print("Read pair_name10: "); Serial.println(pair_name[9]);
    } else if (cfg.nameIs("pair_name11")) {
      pair_name[10] = cfg.copyValue(); Serial.print("Read pair_name11: "); Serial.println(pair_name[10]);
    } else if (cfg.nameIs("pair_name12")) {
      pair_name[11] = cfg.copyValue(); Serial.print("Read pair_name12: "); Serial.println(pair_name[11]);
    } else if (cfg.nameIs("pair_name13")) {
      pair_name[12] = cfg.copyValue(); Serial.print("Read pair_name13: "); Serial.println(pair_name[12]);
    } else if (cfg.nameIs("pair_name14")) {
      pair_name[13] = cfg.copyValue(); Serial.print("Read pair_name14: "); Serial.println(pair_name[13]);
    } else if (cfg.nameIs("pair_name15")) {
      pair_name[14] = cfg.copyValue(); Serial.print("Read pair_name15: "); Serial.println(pair_name[14]);
    } else if (cfg.nameIs("pair_name16")) {
      pair_name[15] = cfg.copyValue(); Serial.print("Read pair_name16: "); Serial.println(pair_name[15]);
    } else if (cfg.nameIs("pair_name17")) {
      pair_name[16] = cfg.copyValue(); Serial.print("Read pair_name17: "); Serial.println(pair_name[16]);
    } else if (cfg.nameIs("pair_name18")) {
      pair_name[17] = cfg.copyValue(); Serial.print("Read pair_name18: "); Serial.println(pair_name[17]);
    } else if (cfg.nameIs("pair_name19")) {
      pair_name[18] = cfg.copyValue(); Serial.print("Read pair_name19: "); Serial.println(pair_name[18]);
    } else if (cfg.nameIs("pair_name20")) {
      pair_name[19] = cfg.copyValue(); Serial.print("Read pair_name20: "); Serial.println(pair_name[19]);
    } else if (cfg.nameIs("pair_name21")) {
      pair_name[20] = cfg.copyValue(); Serial.print("Read pair_name21: "); Serial.println(pair_name[20]);
    } else if (cfg.nameIs("pair_name22")) {
      pair_name[21] = cfg.copyValue(); Serial.print("Read pair_name22: "); Serial.println(pair_name[21]);
    } else if (cfg.nameIs("pair_name23")) {
      pair_name[22] = cfg.copyValue(); Serial.print("Read pair_name23: "); Serial.println(pair_name[22]);
    } else if (cfg.nameIs("pair_name24")) {
      pair_name[23] = cfg.copyValue(); Serial.print("Read pair_name24: "); Serial.println(pair_name[23]);
    } else if (cfg.nameIs("pair_name25")) {
      pair_name[24] = cfg.copyValue(); Serial.print("Read pair_name25: "); Serial.println(pair_name[24]);
    } else if (cfg.nameIs("pair_name26")) {
      pair_name[25] = cfg.copyValue(); Serial.print("Read pair_name26: "); Serial.println(pair_name[25]);
    } else if (cfg.nameIs("pair_name27")) {
      pair_name[26] = cfg.copyValue(); Serial.print("Read pair_name27: "); Serial.println(pair_name[26]);
    } else if (cfg.nameIs("pair_name28")) {
      pair_name[27] = cfg.copyValue(); Serial.print("Read pair_name28: "); Serial.println(pair_name[27]);
    } else if (cfg.nameIs("pair_name29")) {
      pair_name[28] = cfg.copyValue(); Serial.print("Read pair_name29: "); Serial.println(pair_name[28]);
    } else if (cfg.nameIs("pair_name30")) {
      pair_name[29] = cfg.copyValue(); Serial.print("Read pair_name30: "); Serial.println(pair_name[29]);
    } else if (cfg.nameIs("pair_name31")) {
      pair_name[30] = cfg.copyValue(); Serial.print("Read pair_name31: "); Serial.println(pair_name[30]);
    } else if (cfg.nameIs("pair_name32")) {
      pair_name[31] = cfg.copyValue(); Serial.print("Read pair_name32: "); Serial.println(pair_name[31]);
    } else if (cfg.nameIs("pair_name33")) {
      pair_name[32] = cfg.copyValue(); Serial.print("Read pair_name33: "); Serial.println(pair_name[32]);
    } else if (cfg.nameIs("pair_name34")) {
      pair_name[33] = cfg.copyValue(); Serial.print("Read pair_name34: "); Serial.println(pair_name[33]);
    } else if (cfg.nameIs("pair_name35")) {
      pair_name[34] = cfg.copyValue(); Serial.print("Read pair_name35: "); Serial.println(pair_name[34]);
    } else if (cfg.nameIs("pair_name36")) {
      pair_name[35] = cfg.copyValue(); Serial.print("Read pair_name36: "); Serial.println(pair_name[35]);
    } else if (cfg.nameIs("pair_STRING1")) {
      pair_STRING[0] = cfg.copyValue(); Serial.print("Read pair_STRING1: "); Serial.println(pair_STRING[0]);
    } else if (cfg.nameIs("pair_STRING2")) {
      pair_STRING[1] = cfg.copyValue(); Serial.print("Read pair_STRING2: "); Serial.println(pair_STRING[1]);
    } else if (cfg.nameIs("pair_STRING3")) {
      pair_STRING[2] = cfg.copyValue(); Serial.print("Read pair_STRING3: "); Serial.println(pair_STRING[2]);
    } else if (cfg.nameIs("pair_STRING4")) {
      pair_STRING[3] = cfg.copyValue(); Serial.print("Read pair_STRING4: "); Serial.println(pair_STRING[3]);
    } else if (cfg.nameIs("pair_STRING5")) {
      pair_STRING[4] = cfg.copyValue(); Serial.print("Read pair_STRING5: "); Serial.println(pair_STRING[4]);
    } else if (cfg.nameIs("pair_STRING6")) {
      pair_STRING[5] = cfg.copyValue(); Serial.print("Read pair_STRING6: "); Serial.println(pair_STRING[5]);
    } else if (cfg.nameIs("pair_STRING7")) {
      pair_STRING[6] = cfg.copyValue(); Serial.print("Read pair_STRING7: "); Serial.println(pair_STRING[6]);
    } else if (cfg.nameIs("pair_STRING8")) {
      pair_STRING[7] = cfg.copyValue(); Serial.print("Read pair_STRING8: "); Serial.println(pair_STRING[7]);
    } else if (cfg.nameIs("pair_STRING9")) {
      pair_STRING[8] = cfg.copyValue(); Serial.print("Read pair_STRING9: "); Serial.println(pair_STRING[8]);
    } else if (cfg.nameIs("pair_STRING10")) {
      pair_STRING[9] = cfg.copyValue(); Serial.print("Read pair_STRING10: "); Serial.println(pair_STRING[9]);
    } else if (cfg.nameIs("pair_STRING11")) {
      pair_STRING[10] = cfg.copyValue(); Serial.print("Read pair_STRING11: "); Serial.println(pair_STRING[10]);
    } else if (cfg.nameIs("pair_STRING12")) {
      pair_STRING[11] = cfg.copyValue(); Serial.print("Read pair_STRING12: "); Serial.println(pair_STRING[11]);
    } else if (cfg.nameIs("pair_STRING13")) {
      pair_STRING[12] = cfg.copyValue(); Serial.print("Read pair_STRING13: "); Serial.println(pair_STRING[12]);
    } else if (cfg.nameIs("pair_STRING14")) {
      pair_STRING[13] = cfg.copyValue(); Serial.print("Read pair_STRING14: "); Serial.println(pair_STRING[13]);
    } else if (cfg.nameIs("pair_STRING15")) {
      pair_STRING[14] = cfg.copyValue(); Serial.print("Read pair_STRING15: "); Serial.println(pair_STRING[14]);
    } else if (cfg.nameIs("pair_STRING16")) {
      pair_STRING[15] = cfg.copyValue(); Serial.print("Read pair_STRING16: "); Serial.println(pair_STRING[15]);
    } else if (cfg.nameIs("pair_STRING17")) {
      pair_STRING[16] = cfg.copyValue(); Serial.print("Read pair_STRING17: "); Serial.println(pair_STRING[16]);
    } else if (cfg.nameIs("pair_STRING18")) {
      pair_STRING[17] = cfg.copyValue(); Serial.print("Read pair_STRING18: "); Serial.println(pair_STRING[17]);
    } else if (cfg.nameIs("pair_STRING19")) {
      pair_STRING[18] = cfg.copyValue(); Serial.print("Read pair_STRING19: "); Serial.println(pair_STRING[18]);
    } else if (cfg.nameIs("pair_STRING20")) {
      pair_STRING[19] = cfg.copyValue(); Serial.print("Read pair_STRING20: "); Serial.println(pair_STRING[19]);
    } else if (cfg.nameIs("pair_STRING21")) {
      pair_STRING[20] = cfg.copyValue(); Serial.print("Read pair_STRING21: "); Serial.println(pair_STRING[20]);
    } else if (cfg.nameIs("pair_STRING22")) {
      pair_STRING[21] = cfg.copyValue(); Serial.print("Read pair_STRING22: "); Serial.println(pair_STRING[21]);
    } else if (cfg.nameIs("pair_STRING23")) {
      pair_STRING[22] = cfg.copyValue(); Serial.print("Read pair_STRING23: "); Serial.println(pair_STRING[22]);
    } else if (cfg.nameIs("pair_STRING24")) {
      pair_STRING[23] = cfg.copyValue(); Serial.print("Read pair_STRING24: "); Serial.println(pair_STRING[23]);
    } else if (cfg.nameIs("pair_STRING25")) {
      pair_STRING[24] = cfg.copyValue(); Serial.print("Read pair_STRING25: "); Serial.println(pair_STRING[24]);
    } else if (cfg.nameIs("pair_STRING26")) {
      pair_STRING[25] = cfg.copyValue(); Serial.print("Read pair_STRING26: "); Serial.println(pair_STRING[25]);
    } else if (cfg.nameIs("pair_STRING27")) {
      pair_STRING[26] = cfg.copyValue(); Serial.print("Read pair_STRING27: "); Serial.println(pair_STRING[26]);
    } else if (cfg.nameIs("pair_STRING28")) {
      pair_STRING[27] = cfg.copyValue(); Serial.print("Read pair_STRING28: "); Serial.println(pair_STRING[27]);
    } else if (cfg.nameIs("pair_STRING29")) {
      pair_STRING[28] = cfg.copyValue(); Serial.print("Read pair_STRING29: "); Serial.println(pair_STRING[28]);
    } else if (cfg.nameIs("pair_STRING30")) {
      pair_STRING[29] = cfg.copyValue(); Serial.print("Read pair_STRING30: "); Serial.println(pair_STRING[29]);
    } else if (cfg.nameIs("pair_STRING31")) {
      pair_STRING[30] = cfg.copyValue(); Serial.print("Read pair_STRING31: "); Serial.println(pair_STRING[30]);
    } else if (cfg.nameIs("pair_STRING32")) {
      pair_STRING[31] = cfg.copyValue(); Serial.print("Read pair_STRING32: "); Serial.println(pair_STRING[31]);
    } else if (cfg.nameIs("pair_STRING33")) {
      pair_STRING[32] = cfg.copyValue(); Serial.print("Read pair_STRING33: "); Serial.println(pair_STRING[32]);
    } else if (cfg.nameIs("pair_STRING34")) {
      pair_STRING[33] = cfg.copyValue(); Serial.print("Read pair_STRING34: "); Serial.println(pair_STRING[33]);
    } else if (cfg.nameIs("pair_STRING35")) {
      pair_STRING[34] = cfg.copyValue(); Serial.print("Read pair_STRING35: "); Serial.println(pair_STRING[34]);
    } else if (cfg.nameIs("pair_STRING36")) {
      pair_STRING[35] = cfg.copyValue(); Serial.print("Read pair_STRING36: "); Serial.println(pair_STRING[35]);
    } else if (cfg.nameIs("pair_color1")) {
      str_pair_color[0] = cfg.copyValue(); Serial.print("Read pair_color1: "); Serial.println(str_pair_color[0]);
    } else if (cfg.nameIs("pair_color2")) {
      str_pair_color[1] = cfg.copyValue(); Serial.print("Read pair_color2: "); Serial.println(str_pair_color[1]);
    } else if (cfg.nameIs("pair_color3")) {
      str_pair_color[2] = cfg.copyValue(); Serial.print("Read pair_color3: "); Serial.println(str_pair_color[2]);
    } else if (cfg.nameIs("pair_color4")) {
      str_pair_color[3] = cfg.copyValue(); Serial.print("Read pair_color4: "); Serial.println(str_pair_color[3]);
    } else if (cfg.nameIs("pair_color5")) {
      str_pair_color[4] = cfg.copyValue(); Serial.print("Read pair_color5: "); Serial.println(str_pair_color[4]);
    } else if (cfg.nameIs("pair_color6")) {
      str_pair_color[5] = cfg.copyValue(); Serial.print("Read pair_color6: "); Serial.println(str_pair_color[5]);
    } else if (cfg.nameIs("pair_color7")) {
      str_pair_color[6] = cfg.copyValue(); Serial.print("Read pair_color7: "); Serial.println(str_pair_color[6]);
    } else if (cfg.nameIs("pair_color8")) {
      str_pair_color[7] = cfg.copyValue(); Serial.print("Read pair_color8: "); Serial.println(str_pair_color[7]);
    } else if (cfg.nameIs("pair_color9")) {
      str_pair_color[8] = cfg.copyValue(); Serial.print("Read pair_color9: "); Serial.println(str_pair_color[8]);
    } else if (cfg.nameIs("pair_color10")) {
      str_pair_color[9] = cfg.copyValue(); Serial.print("Read pair_color10: "); Serial.println(str_pair_color[9]);
    } else if (cfg.nameIs("pair_color11")) {
      str_pair_color[10] = cfg.copyValue(); Serial.print("Read pair_color11: "); Serial.println(str_pair_color[10]);
    } else if (cfg.nameIs("pair_color12")) {
      str_pair_color[11] = cfg.copyValue(); Serial.print("Read pair_color12: "); Serial.println(str_pair_color[11]);
    } else if (cfg.nameIs("pair_color13")) {
      str_pair_color[12] = cfg.copyValue(); Serial.print("Read pair_color13: "); Serial.println(str_pair_color[12]);
    } else if (cfg.nameIs("pair_color14")) {
      str_pair_color[13] = cfg.copyValue(); Serial.print("Read pair_color14: "); Serial.println(str_pair_color[13]);
    } else if (cfg.nameIs("pair_color15")) {
      str_pair_color[14] = cfg.copyValue(); Serial.print("Read pair_color15: "); Serial.println(str_pair_color[14]);
    } else if (cfg.nameIs("pair_color16")) {
      str_pair_color[15] = cfg.copyValue(); Serial.print("Read pair_color16: "); Serial.println(str_pair_color[15]);
    } else if (cfg.nameIs("pair_color17")) {
      str_pair_color[16] = cfg.copyValue(); Serial.print("Read pair_color17: "); Serial.println(str_pair_color[16]);
    } else if (cfg.nameIs("pair_color18")) {
      str_pair_color[17] = cfg.copyValue(); Serial.print("Read pair_color18: "); Serial.println(str_pair_color[17]);
    } else if (cfg.nameIs("pair_color19")) {
      str_pair_color[18] = cfg.copyValue(); Serial.print("Read pair_color19: "); Serial.println(str_pair_color[18]);
    } else if (cfg.nameIs("pair_color20")) {
      str_pair_color[19] = cfg.copyValue(); Serial.print("Read pair_color20: "); Serial.println(str_pair_color[19]);
    } else if (cfg.nameIs("pair_color21")) {
      str_pair_color[20] = cfg.copyValue(); Serial.print("Read pair_color21: "); Serial.println(str_pair_color[20]);
    } else if (cfg.nameIs("pair_color22")) {
      str_pair_color[21] = cfg.copyValue(); Serial.print("Read pair_color22: "); Serial.println(str_pair_color[21]);
    } else if (cfg.nameIs("pair_color23")) {
      str_pair_color[22] = cfg.copyValue(); Serial.print("Read pair_color23: "); Serial.println(str_pair_color[22]);
    } else if (cfg.nameIs("pair_color24")) {
      str_pair_color[23] = cfg.copyValue(); Serial.print("Read pair_color24: "); Serial.println(str_pair_color[23]);
    } else if (cfg.nameIs("pair_color25")) {
      str_pair_color[24] = cfg.copyValue(); Serial.print("Read pair_color25: "); Serial.println(str_pair_color[24]);
    } else if (cfg.nameIs("pair_color26")) {
      str_pair_color[25] = cfg.copyValue(); Serial.print("Read pair_color26: "); Serial.println(str_pair_color[25]);
    } else if (cfg.nameIs("pair_color27")) {
      str_pair_color[26] = cfg.copyValue(); Serial.print("Read pair_color27: "); Serial.println(str_pair_color[26]);
    } else if (cfg.nameIs("pair_color28")) {
      str_pair_color[27] = cfg.copyValue(); Serial.print("Read pair_color28: "); Serial.println(str_pair_color[27]);
    } else if (cfg.nameIs("pair_color29")) {
      str_pair_color[28] = cfg.copyValue(); Serial.print("Read pair_color29: "); Serial.println(str_pair_color[28]);
    } else if (cfg.nameIs("pair_color30")) {
      str_pair_color[29] = cfg.copyValue(); Serial.print("Read pair_color30: "); Serial.println(str_pair_color[29]);
    } else if (cfg.nameIs("pair_color31")) {
      str_pair_color[30] = cfg.copyValue(); Serial.print("Read pair_color31: "); Serial.println(str_pair_color[30]);
    } else if (cfg.nameIs("pair_color32")) {
      str_pair_color[31] = cfg.copyValue(); Serial.print("Read pair_color32: "); Serial.println(str_pair_color[31]);
    } else if (cfg.nameIs("pair_color33")) {
      str_pair_color[32] = cfg.copyValue(); Serial.print("Read pair_color33: "); Serial.println(str_pair_color[32]);
    } else if (cfg.nameIs("pair_color34")) {
      str_pair_color[33] = cfg.copyValue(); Serial.print("Read pair_color34: "); Serial.println(str_pair_color[33]);
    } else if (cfg.nameIs("pair_color35")) {
      str_pair_color[34] = cfg.copyValue(); Serial.print("Read pair_color35: "); Serial.println(str_pair_color[34]);
    } else if (cfg.nameIs("pair_color36")) {
      str_pair_color[35] = cfg.copyValue(); Serial.print("Read pair_color36: "); Serial.println(str_pair_color[35]);
    } else if (cfg.nameIs("myTimeZone")) {
      myTimeZone = cfg.getIntValue(); Serial.print("Read myTimeZone: "); Serial.println(myTimeZone);
    } else if (cfg.nameIs("myLanguage")) {
      myLanguage = cfg.getIntValue(); Serial.print("Read myLanguage: "); Serial.println(myLanguage);
    } else if (cfg.nameIs("mySleeptime")) {
      mySleeptime = cfg.getIntValue(); Serial.print("Read mySleeptime: "); Serial.println(mySleeptime);
    } else {
      // report unrecognized names.
      Serial.print("Unknown name in config: "); Serial.println(cfg.getName());
    }
  }
  cfg.end();// clean up

  // Setting Language
  int arrsize_mn = sizeof(monthName_MyLang) / sizeof(int);
  int arrsize_wd = sizeof(weekDay_MyLang) / sizeof(int);
  if (myLanguage == 1) { // select weekday and month names from different languages (selected by e.g.: "myLanguage=1" in Config file)
    for (int i = 0; i < arrsize_mn; i++) {
      monthName_MyLang[i] = monthName_Language1[i];
    }
    for (int i = 0; i < arrsize_wd; i++) {
      weekDay_MyLang[i] = weekDay_Language1[i];
    }
  }
  else if (myLanguage == 2) {
    for (int i = 0; i < arrsize_mn; i++) {
      monthName_MyLang[i] = monthName_Language2[i];
    }
    for (int i = 0; i < arrsize_wd; i++) {
      weekDay_MyLang[i] = weekDay_Language2[i];
    }
  }
  //  else if (myLanguage == 3) {              // ---> append here if you want to extend more languages for day and month names (Mo Tu We .. Jan Feb ..)
  //    for (int i = 0; i < arrsize_mn; i++) { //      3 would represent the "MyLanguage=value" from the config file (0=english, 1=german, 2=spanish, 3=up to you)
  //      monthName_MyLang[i] = monthName_Language3[i];
  //    }
  //    for (int i = 0; i < arrsize_wd; i++) {
  //      weekDay_MyLang[i] = weekDay_Language3[i];
  //    }
  //  }
  else {
    for (int i = 0; i < arrsize_mn; i++) {
      monthName_MyLang[i] = monthName_Language0[i];
    }
    for (int i = 0; i < arrsize_wd; i++) {
      weekDay_MyLang[i] = weekDay_Language0[i];
    }
  }


  // make a copy of the pair names in LowerCase
  for (int i = 0; i < pairs; i++) {
    pair_string[i] = pair_STRING[i];
    pair_string[i].toLowerCase();
    //Serial.print("LowerCase pair_string"); Serial.print(i + 1); Serial.print(": "); Serial.println(pair_string[i]);
  }
  // change pair_colors from sd-cfg file from string to uint16_t
  for (int i = 0; i < pairs; i++) {
    //Serial.print("String color code"); Serial.print(i + 1); Serial.print(": "); Serial.println(str_pair_color[i]);
    rgb565_pair_color[i] = strtol(str_pair_color[i], NULL , 0);
    //Serial.print("RGB565 color code"); Serial.print(i + 1); Serial.print(": 0x"); Serial.println(rgb565_pair_color[i], HEX);
  }

  // load last settings from memory on startup
  preferences.begin("settings", false);
  //preferences.clear(); //if you want to clear all stored settings from internal memory uncomment this line afterwards "preferences.clear();"
  unsigned int last_stored_Currency = preferences.getUInt("currency", 1);
  unsigned int last_stored_Timeframe = preferences.getUInt("timeframe", 4);
  if (last_stored_Currency > pairs) {  //prevents loading a coinpair from flash that is higher than the default max pairs value when no SD config file is loaded
    last_stored_Currency = 1;
  }
  current_Currency = last_stored_Currency;
  current_Timeframe = last_stored_Timeframe;
  strname_api = pair_STRING[current_Currency - 1];
  strname_ws = pair_string[current_Currency - 1];
  strname_currency = pair_name[current_Currency - 1];
  strname_color = rgb565_pair_color[current_Currency - 1];
  if (!SD.begin(pinSelectSD)) { // if no SD-Card inserted, try the last from internal memory
    ssid = preferences.getString("ssid", "");
    ssid2 = preferences.getString("ssid2", "");
    password = preferences.getString("password", "");
    password2 = preferences.getString("password2", "");
  }
  else {
    preferences.putString("ssid", ssid);
    preferences.putString("ssid2", ssid2);
    preferences.putString("password", password);
    preferences.putString("password2", password2);
  }
  //  preferences.end();    not used !

  //  for debug/testing only:
  //  Serial.println(preferences.getString("ssid", ssid));
  //  Serial.println(preferences.putString("ssid2", ssid2));
  //  Serial.println(preferences.putString("password", password));
  //  Serial.println(preferences.putString("password2", password2));
  //  Serial.print("alt_hotspot: "); Serial.println(alt_hotspot);
  //  Serial.print("weekDay_MyLang[1]: "); Serial.print(weekDay_MyLang[1]); Serial.println();
  //  Serial.print("weekDay_MyLang[2]: "); Serial.print(weekDay_MyLang[2]); Serial.println();
  //  Serial.print("weekDay_MyLang[3]: "); Serial.print(weekDay_MyLang[3]); Serial.println();
  //  Serial.print("weekDay_MyLang[4]: "); Serial.print(weekDay_MyLang[4]); Serial.println();
  //  Serial.print("weekDay_MyLang[5]: "); Serial.print(weekDay_MyLang[5]); Serial.println();
  //  Serial.print("weekDay_MyLang[6]: "); Serial.print(weekDay_MyLang[6]); Serial.println();
  //  Serial.print("weekDay_MyLang[7]: "); Serial.print(weekDay_MyLang[7]); Serial.println();


  // Connecting to WiFi:
  M5.Lcd.print("\n\nConnecting to ");
  if (alt_hotspot == false) {
    M5.Lcd.println(ssid);
    WiFi.begin(ssid.c_str(), password.c_str());
  }
  else {
    M5.Lcd.println(ssid2);
    WiFi.begin(ssid2.c_str(), password2.c_str());
  }
  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_RED);  //busy light red
    M5.Lcd.setTextWrap(true);
    M5.Lcd.print(".");
    err_count ++;
    // Power Off Button (ButtonC long press) - needed because if on usb power there is no option to turn off the unit except by powerOFF command
    if (M5.BtnC.pressedFor(1333)) {
      M5.Lcd.drawPngFile(SD, "/ccticker/data/m5_logo_dark.png", 0 , 0);
      delay(500);
      for (int i = Brightness_value; i > 0 ; i--) {                  // dimm LCD slowly before shutdown
        Brightness_value -= 1;
        M5.lcd.setBrightness(Brightness_value);
        delay(50);
      }
      M5.Lcd.fillScreen(TFT_BLACK);
      delay(500);
      M5.Power.powerOFF();
    }
    if (err_count > 288) {                                // if 10 minutes no wifi -> power off device !
      M5.Lcd.drawPngFile(SD, "/ccticker/data/m5_logo_dark.png", 0 , 0);
      delay(1000);
      M5.Lcd.fillScreen(TFT_BLACK);
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

  // startup promt to lcd:
  M5.Lcd.println("getting time");
  M5.Lcd.println("drawing chart");
  if (!SD.begin(pinSelectSD)) {
    M5.Lcd.println();
    M5.Lcd.println("SD-card NOT used");
    delay(750);
  }
  else {
    M5.Lcd.println();
    M5.Lcd.println("..reading SD-card");
  }

  // Setting time:
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  while (true) {
    time_t now = time(nullptr);
    if (now && year(now) > 2017) break;
  }
  delay(400);
  M5.Lcd.fillScreen(TFT_BLACK);
  yield();
  showWifiStrength();
  printTime();    //routine to print the time on lcd screen,  adjusted to your local timzone

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
  M5.Lcd.fillCircle(101, topPanel + (infoPanel / 2), 4, TFT_BLACK);  //reset busy light
  if (currentMs - lastPrintTime >= 35000 && second(time(nullptr)) < 25) {  // draw new date and candles regularly
    lastPrintTime = currentMs;
    printTime();
    multi_level = 0;
    webSocket.disconnect();
    webSocket.beginSSL(wsApiHost, wsApiPort, getWsApiUrl());
    while (!requestRestApi()) {}
    drawCandles();
    showBatteryLevel();

  }
  webSocket.loop();
  M5.update();                                                             // update button checks
} // **************************************END LOOP ********************************************************



void printTime() {
  M5.Lcd.fillRect(0, 0, 320, topPanel, TFT_BLACK); M5.Lcd.setFreeFont(FSSB12);
  M5.Lcd.setCursor(1, 19); M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(strname_color);
  M5.Lcd.print(strname_currency);
  M5.Lcd.setFreeFont(FMB12); M5.Lcd.setCursor(111, 15); M5.Lcd.setTextColor(TFT_LIGHTGREY);
  TimeChangeRule *tcr;
  if (myTimeZone == 0) {
    time_t now = myTZ0.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else if (myTimeZone == 1) {
    time_t now = myTZ1.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else if (myTimeZone == 2) {
    time_t now = myTZ2.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  }  else if (myTimeZone == 3) {
    time_t now = myTZ3.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else if (myTimeZone == 4) {
    time_t now = myTZ4.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else if (myTimeZone == 5) {
    time_t now = myTZ5.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else if (myTimeZone == 6) {
    time_t now = myTZ6.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else if (myTimeZone == 7) {
    time_t now = myTZ7.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  } else { // default: MyTZ4 english
    time_t now = myTZ4.toLocal(time(nullptr), &tcr);
    M5.Lcd.printf("%s %2d.%s %02d:%02d", weekDay_MyLang[weekday(now)], day(now), monthName_MyLang[month(now)], hour(now), minute(now));
  }
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
  M5.Lcd.fillRect((center - w) + 5, topPanel + infoPanel, ceil(w), 240 - (topPanel + infoPanel + bottomPanel), TFT_BLACK);
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
  uint32_t color = cy < oy ? DARKERGREEN : MYRED;

  // Background:
  M5.Lcd.fillRect((center - w) + 5, topPanel + infoPanel, ceil(w), 240 - (topPanel + infoPanel + bottomPanel), TFT_BLACK);

  // Volume line:
  M5.Lcd.drawLine((center - w) + 5, prevVY, center - 5, vy, volColor);
  M5.Lcd.drawLine(center - 4, vy, center + 4, vy, volColor);
  if (i == candlesLimit - 1) {
    M5.Lcd.fillRect(center + 5, topPanel + infoPanel, w / 2, 240 - (topPanel + infoPanel + bottomPanel), TFT_BLACK);
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
  M5.Lcd.fillRect(34, topPanel, 60, infoPanel, TFT_BLACK);
  if (M5.Power.isCharging())M5.Lcd.drawPngFile(SD, "/ccticker/data/batt_gre.png", 34, topPanel); // show a green battery icon
  else M5.Lcd.drawPngFile(SD, "/ccticker/data/batt_gry.png", 34, topPanel);                      // show a grey battery icon
  if (battery == -1)     M5.Lcd.drawPngFile(SD, "/ccticker/data/batt_nc.png", 34, topPanel);     // 60 x 60 px
  else if (battery < 12) M5.Lcd.drawPngFile(SD, "/ccticker/data/batt0.png",  34, topPanel);
  else if (battery < 32) M5.Lcd.drawPngFile(SD, "/ccticker/data/batt25.png", 34, topPanel);
  else if (battery < 64) M5.Lcd.drawPngFile(SD, "/ccticker/data/batt50.png", 34, topPanel);
  else if (battery < 78) M5.Lcd.drawPngFile(SD, "/ccticker/data/batt75.png", 34, topPanel);
  else if (battery > 90) M5.Lcd.drawPngFile(SD, "/ccticker/data/batt100.png", 34, topPanel);
}



void drawPrice() {
  float price = (candles[candlesLimit - 1].c);
  float oy_var2 (candles[candlesLimit - 1].o);
  if (lastPrice != price) {
    M5.Lcd.fillRect(0, 240 - bottomPanel, 202, bottomPanel, TFT_BLACK); M5.Lcd.setTextSize(1);
    M5.Lcd.setFreeFont(FSSB24); M5.Lcd.setCursor(0, 273 - bottomPanel);
    M5.Lcd.setTextColor(price > oy_var2 ? DARKERGREEN : MYRED);
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
    M5.Lcd.fillRect(202, 240 - bottomPanel, 99, bottomPanel / 2, TFT_BLACK); M5.Lcd.setTextColor(DARKERGREEN);
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
    M5.Lcd.fillRect(202, 240 - bottomPanel / 2, 99, bottomPanel / 2, TFT_BLACK); M5.Lcd.setTextColor(MYRED);
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
    M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_BLACK); M5.Lcd.setFreeFont(FSSB9);
    M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
    M5.Lcd.print(timeframe[0]);
    M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
    M5.Lcd.print(timeframe[1]);
  }
}



// price changings for info panel: changings relative to the 24th past candles close price in percent (with exceptions):
void PriceChangings() {
  if (!timeframe_really_changed) {
    M5.Lcd.fillRect(108, topPanel, 225, infoPanel, TFT_BLACK);
    M5.Lcd.setFreeFont(FM9); M5.Lcd.setCursor(108, topPanel + infoPanel - 2); M5.Lcd.setTextSize(1);
    if (current_Timeframe == 0) { //1m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 21].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c); //+%.1f shows prefix even if positive with 1 digit after comma
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 21].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":20m");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 21].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":20m");
      }
    }
    else if (current_Timeframe == 1) { //3m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 21].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 21].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 21].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":60m");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 21].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 21].c / (candles[candlesLimit - 21].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":60m");
      }
    }
    else if (current_Timeframe == 2) { //5m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":2h");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":2h");
      }
    }
    else if (current_Timeframe == 3) { //15m
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":6h");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":6h");
      }
    }
    else if (current_Timeframe == 4) { //1h
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":24h");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":24h");
      }
    }
    else if (current_Timeframe == 5) { //4h
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":4d");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":4d");
      }
    }
    else if (current_Timeframe == 6) { //1d
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 22].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 22].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 22].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 22].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":3w");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 22].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 22].c / (candles[candlesLimit - 22].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":3w");
      }
    }
    else if (current_Timeframe == 7) { //1w
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":5M");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":5M");
      }
    }
    else if (current_Timeframe == 8) { //1M
      if (candles[candlesLimit - 1].c > candles[candlesLimit - 24].c) {
        M5.Lcd.setTextColor(DARKERGREEN);
        if (multi_level == 1) {
          M5.Lcd.printf("+%.3f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("+%.5f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("+%.1e", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        } else {
          M5.Lcd.printf("+%4.1f", candles[candlesLimit - 1].c - candles[candlesLimit - 24].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 1].c - candles[candlesLimit - 24].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":2y");
      } else {
        M5.Lcd.setTextColor(MYRED);
        if (multi_level == 1) {
          M5.Lcd.printf("-%.3f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 2) {
          M5.Lcd.printf("-%.5f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else if (multi_level == 3) {
          M5.Lcd.printf("-%.1e", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        } else {
          M5.Lcd.printf("-%4.1f", candles[candlesLimit - 24].c - candles[candlesLimit - 1].c);
        }
        M5.Lcd.printf("(%4.1f", (1 / (candles[candlesLimit - 24].c / (candles[candlesLimit - 24].c - candles[candlesLimit - 1].c)) * 100));
        M5.Lcd.print("%)");
        M5.Lcd.setTextColor(TFT_WHITE);
        M5.Lcd.print(":2y");
      }
    }
  }
}




// show Wifi-RSSI level (signal strength)
void showWifiStrength() {
  int WifiRSSI = WiFi.RSSI();
  //  M5.Lcd.fillRect(2, topPanel + infoPanel + 8, 69, 15, TFT_BLACK);
  //  M5.Lcd.setCursor(2, topPanel + infoPanel + 20); M5.Lcd.setFreeFont(FM9); M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_WHITE);
  //  M5.Lcd.print(String(WifiRSSI) + "dBm");
  if (WifiRSSI > -50 & ! WifiRSSI == 0 ) M5.Lcd.fillRoundRect(26, topPanel, 5, 12, 1, TFT_WHITE);
  else
    M5.Lcd.fillRoundRect(26, topPanel, 5, 12, 1, TFT_DARKGREY);
  if (WifiRSSI > -70 & ! WifiRSSI == 0) M5.Lcd.fillRoundRect(18, topPanel + 2, 5, 10, 1, TFT_WHITE);
  else M5.Lcd.fillRoundRect(18, topPanel + 2, 5, 10, 1, TFT_DARKGREY);
  if (WifiRSSI > -80 & ! WifiRSSI == 0) M5.Lcd.fillRoundRect(10, topPanel + 4, 5, 8, 1, TFT_WHITE);
  else M5.Lcd.fillRoundRect(10, topPanel + 4, 5, 8, 1, TFT_DARKGREY);
  if (WifiRSSI > -90 & ! WifiRSSI == 0)
    M5.Lcd.fillRoundRect(2, topPanel + 6, 5, 6, 1, TFT_WHITE);
  else M5.Lcd.fillRoundRect(2, topPanel + 6, 5, 6, 1, TFT_RED);
}



// visual error outputs
void error(String text) {
  drawCandles();
  M5.Lcd.setTextWrap(true); M5.Lcd.setTextColor(TFT_WHITE); M5.Lcd.setFreeFont(FSSB12); M5.Lcd.setTextSize(1);
  M5.Lcd.setCursor(3, topPanel + infoPanel + 73);
  if (ws_error) {
    M5.Lcd.drawPngFile(SD, "/ccticker/data/ws_error_small.png", 100, topPanel + infoPanel + 5); // show a 50x50px png
    M5.Lcd.print(text);
    ws_error = false;
    M5.Lcd.setTextWrap(false);
  }
  if (wifi_error) {
    M5.Lcd.print(text);
    M5.Lcd.drawPngFile(SD, "/ccticker/data/wifi_error_small.png", 155, topPanel + infoPanel + 5); // show a 50x50px png
    wifi_error = false;
    M5.Lcd.print("\nconnecting to ");
    if (alt_hotspot == false) {
      M5.Lcd.print(ssid);
      WiFi.begin(ssid.c_str(), password.c_str());
    }
    else {
      M5.Lcd.print(ssid);
      WiFi.begin(ssid2.c_str(), password2.c_str());
    }
    M5.Lcd.setTextWrap(false);
    while (WiFi.status() != WL_CONNECTED) {         // after a wifi error occured try to reconnect
      M5.Lcd.print(".");
      err_count ++;
      if (err_count > 288) {                        // if 10 minutes no wifi -> power off device !
        M5.Lcd.drawPngFile(SD, "/ccticker/data/m5_logo_dark.png", 0 , 0);
        delay(1000);
        M5.Lcd.fillScreen(TFT_BLACK);
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
  // Power Off Button (ButtonC long press) - needed because if on usb power there is no option to turn off the unit except by powerOFF command
  if (M5.BtnC.pressedFor(1333)) {
    M5.Lcd.drawPngFile(SD, "/ccticker/data/m5_logo_dark.png", 0 , 0);
    delay(500);
    for (int i = Brightness_value; i > 0 ; i--) {                  // dimm LCD slowly before shutdown
      Brightness_value -= 1;
      M5.lcd.setBrightness(Brightness_value);
      delay(50);
    }
    M5.Lcd.fillScreen(TFT_BLACK);
    delay(500);
    M5.Power.powerOFF();
  }

  // change current currency button A
  if (M5.BtnA.wasPressed() && !current_timeframe_changed) {
    current_Currency_changed = true;
    currency_btn_timeout_counter = currentMs + 2000;
    ++current_Currency;
    if (current_Currency > pairs) current_Currency = 1;
    strname_color = rgb565_pair_color[current_Currency - 1];
    strname_currency = pair_name[current_Currency - 1];
    M5.Lcd.fillRect(0, 0, 106, topPanel, TFT_BLACK); M5.Lcd.setCursor(1, 19);
    M5.Lcd.setTextColor(strname_color); M5.Lcd.setFreeFont(FSSB12);; M5.Lcd.setTextSize(1);
    M5.Lcd.print(strname_currency);
    drawCandles();
    M5.Lcd.drawPngFile(SD, "/ccticker/data/currency.png", (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); // center for 100x100px png
  }

  // brighness button B
  if (M5.BtnB.wasPressed() && !current_Currency_changed && !current_timeframe_changed) {
    current_bright_changed = true;
    bright_btn_timeout_counter = currentMs + 750;
    Brightness_level++;
    if (Brightness_level > 4) Brightness_level = 1;                                       // center for 75x75px Png
    drawCandles();
    if (Brightness_level == 1) M5.Lcd.drawPngFile(SD, "/ccticker/data/brightness_low.png",    (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    if (Brightness_level == 2) M5.Lcd.drawPngFile(SD, "/ccticker/data/brightness_medlow.png", (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    if (Brightness_level == 3) M5.Lcd.drawPngFile(SD, "/ccticker/data/brightness_med.png",    (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
    if (Brightness_level == 4) M5.Lcd.drawPngFile(SD, "/ccticker/data/brightness_high.png",   (320 / 2) - (75 / 2), (240 / 2) - (75 / 2));
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
    M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_BLACK); M5.Lcd.setFreeFont(FSSB9);
    M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
    M5.Lcd.print(timeframe[0]);
    M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
    M5.Lcd.print(timeframe[1]);
    drawCandles();
    M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2));     // center for 100x100px Png
  }
  if (M5.BtnB.wasPressed() && current_timeframe_changed && !current_Currency_changed && !current_bright_changed) {   // set a higher timeframe
    if (current_Timeframe <= 7 && current_Timeframe >= 0) {
      timeframe_really_changed = true;
      timeframe_btn_timeout_counter1 = currentMs + 2000;
      timeframe_btn_timeout_counter2 = currentMs + 2000;
      current_Timeframe++;
      String timeframe = candlesTimeframes[current_Timeframe];
      M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_BLACK); M5.Lcd.setFreeFont(FSSB9);
      M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
      M5.Lcd.print(timeframe[0]);
      M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
      M5.Lcd.print(timeframe[1]);
      drawCandles();
      switch (current_Timeframe) {
        case 0: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1_m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;    // center for 100x100px
        case 1: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe3m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 2: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe5m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 3: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe15m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 4: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 5: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe4H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 6: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1D.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 7: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1W.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 8: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1M.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
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
      M5.Lcd.fillRect(301, 240 - bottomPanel, 19, bottomPanel, TFT_BLACK); M5.Lcd.setFreeFont(FSSB9);
      M5.Lcd.setTextSize(1); M5.Lcd.setTextColor(TFT_CYAN); M5.Lcd.setCursor(301, 254 - bottomPanel);
      M5.Lcd.print(timeframe[0]);
      M5.Lcd.setCursor(301, 255 - floor(bottomPanel / 2));
      M5.Lcd.print(timeframe[1]);
      drawCandles();
      switch (current_Timeframe) {
        case 0: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1_m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;    // center for 100x100px
        case 1: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe3m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 2: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe5m.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 3: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe15m.png",  (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 4: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 5: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe4H.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 6: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1D.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 7: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1W.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
        case 8: M5.Lcd.drawPngFile(SD, "/ccticker/data/timeframe1M.png",   (320 / 2) - (100 / 2), (240 / 2) - (100 / 2)); break;
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
  if (M5.BtnB.pressedFor(1700)) {
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
      preferences.putUInt("bright", Brightness_value);            // store setting to memory
      preferences.putUInt("briglv", Brightness_level);            // store setting to memory
      M5.Lcd.drawCircle(101, topPanel + (infoPanel / 2), 5, TFT_BLUE); // set blue status light when sleeptimer was activated
      drawCandles();
      M5.Lcd.drawPngFile(SD, "/ccticker/data/sleep.png", (320 / 2) - (100 / 2), (240 / 2) - ((100 / 2))); // sleep.png is 100x100px
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
      M5.Lcd.drawCircle(101, topPanel + (infoPanel / 2), 5, TFT_BLACK); // reset blue status light when sleeptimer was deactivated
      drawCandles();
      M5.Lcd.drawPngFile(SD, "/ccticker/data/cancel_sleep.png", (320 / 2) - (120 / 2), (240 / 2) - ((120 / 2))); //sleep.png is 120x120px
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
    M5.Lcd.drawCircle(101, topPanel + (infoPanel / 2), 5, TFT_BLACK); // reset status light when sleeptimer was finished
    M5.Lcd.drawPngFile(SD, "/ccticker/data/m5_logo_dark.png", 0 , 0);
    delay(1000);
    for (int i = Brightness_value; i > 0 ; i--) {                  // dimm LCD slowly before shutdown
      Brightness_value -= 1;
      M5.lcd.setBrightness(Brightness_value);
      delay(50);
    }
    M5.Lcd.fillScreen(TFT_BLACK);
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
    LEDbar.show();                                                // Update LED-bar with new contents
    delay(wait);                                                  // Pause for a moment
  }
}
// ##END##
