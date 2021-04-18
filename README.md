![](preview/IMG_9.jpg)
 
CRYPTO CURRENCY PRICE TICKER with 24 candlesticks chart for M5Stack
##SD-CARD Version with configuation file##
receiving WiFi data from Binance API/Websocket_v3 - by frittna (https://github.com/frittna/Crypto_Coin_Ticker)


This will show a 24 candles, the min/max price and the volume as line, date and time are from time.nist.gov timeserver.
For M5-Stack MCU , coded in ArduinoIDE 1.8.13 - last modified Apr.18.2021 12:10 CET - Version 1.0.4



##Using the App:

- This version needs a SD-Card whith the your WiFi credentials, favourite coinpairs, timezone and language(eng/ger/esp) in a textfile - see installation

- ButtonA: switches through your favourite Coinpair (as many you want) e.g: BTC/USDT etc. which are available on Binance.com

- ButtonB: changes the LCD-Brightness in 4 levels

- ButtonC: 9 changeable Timeframes from 1 Minute to 1 Month

- turn OFF the device pressing the red button once OR by holding ButtonC for over 1 second if USB is connected

- Press buttonC, then, within 2 sec press buttonA to switch down, or buttonB to switch up through the timeframes: 1min->15mins->1hour->..

- available timeframes are 1minute, 3m, 5m, 15m, 1h, 4h, 1d, 1w, 1Month

- if you hold ButtonC at Startup: it will start with alternative SSID/WiFi-password instead (e.g your mobile phone's hotspot)





##Further description:

- The top infoPanel shows the WiFi-strength, batterylevel, colored indicators for "busy", SleepTimer, price moving and if charging from usb (can have delay up to 30s)

- right now: english, german, spanish (day and month names)

- SleepTimer: when holding ButtonB longer than 1,5 seconds it will start a user defined timer to powerOFF the device

- If WiFi is failing more than 2 minutes it reduces the reconnect interval and brightness level, after 10 minutes -> shutdown device

- SDMenu Loader compatible, if SD-Updater (menu.bin) is installed in your SD-card hold buttonA while booting up to start MenuLoader to load your apps

- It is prepared for the use of a Neopixel RGB-LED bar (i use the built-in one in the Battery-Bottom Module for M5Stack/Fire with rgb 10 LEDs)

- Some settings like current timeframe, brightness level, active coinpair and the last wifi credentials will remain stored in internal memory after a reset.


--->   !!! SEE CODE FOR FURTHER INSTRUCTIONS ABOUT THE SD-CARD DATA FOLDER AND CONFIG FILE !!!   <---


![](preview/IMG_1.jpg)
![](preview/IMG_0new.jpg)
![](preview/IMG_0.jpg)
![](preview/IMG_2.jpg)
![](preview/IMG_3.jpg)
![](preview/IMG_4.jpg)
![](preview/IMG_5.jpg)
![](preview/IMG_6.jpg)
![](preview/IMG_7.jpg)
![](preview/IMG_81.jpg)
![](preview/IMG_8.jpg)










-- If someone wants to spend some DOGE to say -> "i love crypto tickers" <- i will appreciate that a lot -- 

address: DRUxU8yWKnWuWmhDCJKmn8eBveXANk85Mw (DOGE)

![](preview/IMG_qrd.jpg)                     ![](preview/doge.png)
