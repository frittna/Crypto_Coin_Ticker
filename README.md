 ######## Crypto Currency Ticker for M5Stack. Getting data over WiFi from Binance API/Websocket v3 ########

 shows 24 candles, min/max prize and volume as line. Date and Time are from time.nist.gov timeserver.
 original code was for SPI TFT display ILI9341 and NodeMCU Board from: https://github/goleh/bitcoin-ticker from 18.dec.2019
 Meanwhile i cannot find his project site and user on github but credits go to him and his code which builds the core of my project.

 this version is MODIFIED by frittna to use on M5-Stack with ArduinoIDE - created 22.Mar.2020 22:07 CET
 added the use of free fonts, changed format for small currencies, added the use of SPIFFS*) for jpg+png pics, settings will remain stored after a reset
 buttonA: switches through 8 (as many you want) preconfigured pairs e.g: BTC to USDT etc. which are available on Binance.com
 buttonB: changes the LCD-Brightness in 4 levels
 buttonC: 9 changeable Timeframes from 1 Minute to 1 Month
 press buttonC, then, within 2 sec press buttonA to switch down or buttonB to switch up through the timeframes
 available timeframes are 1m, 3m, 5m, 15m, 1h, 4h, 1d, 1w, 1M
 hold ButtonC at Startup: will start with alternative SSID/WiFi-password instead (e.g your mobile phone's hotspot)
 the new infoPanel shows: batterylevel and indicates charging (can have delay up to 30s), a colored "busy" light, sleeptimer-active light, changings over time
 SleepTimer: when holding ButtonB longer than 2 seconds it will start a 20 minutes timer to powerOFF the device
 if WiFi is failing more than 2 minutes it reduces the reconnect interval and brightness level, after 10 minutes -> shutdown device
 prepared for the use of a Neopixel RGB-LED bar (i use the built-in one in the Battery-Bottom Module for M5Stack/Fire with rgb 10 LEDs)
 Menu Loader compatible, if SD-Updater (menu.bin) is installed in your SD-card hold buttonA while booting up to start MenuLoader to load your apps
 the impoovements are based quick and dirty solutions - no complains please ;) - changings welcome :)
 known bugs: ButtonC needs better debouncing, slow response if wifi signal receiption is not stable

 Install and set up you Arduino like instructed in the M5-Stack manual:
 install the M5-Stack board in Arduino IDE: Tools -> Board -> Boards Manager -> search for M5-Stack
 install librarys in Arduino IDE: Sketch -> Include Library -> Manage Libraries -> seach for the correct ones (look carefully).
 if the esp32fs/Spiffs library was not found in the library manager you have to search it with google or use the github link bewlow.

 *) about SPIFFS Filesystem and ESP32 Sketch Data Uploader: to use the ESP32 Sketch Data Uploader in Arduino to upload png files
 into the SPIFFS filesystem instead using an SD-Card I hat do use older standalone Verion of Arduino 1.8.10 (just for uploading SPIFFS).
 The Arduino App ver 1.8.12 (Windows 10 Store Store 1.8.33.0) didn't recognizes the tool and shows no tool menu entry to upload.
 If you don't want to use SPIFFS at all you can change "drawPngFile(SPIFFS," to "drawPngFile(SD," and put the pics to your SDcard
