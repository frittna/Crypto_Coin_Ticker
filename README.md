![](preview/IMG_M.jpg)


Multi Crypto Coin Price Ticker with candlestick chart

#SD-Card Version with configuation file#  (updated version)

only needs config file "ccticker.cfg" on SD-Card

Descriptin:
This App will show you  a 24 candle chart in changeable timeframes including min/max price and the volume as line. Date and time are from time.nist.gov timeserver. The price data is from the binance api.

For M5-Stack MCU , coded in ArduinoIDE 1.8.13 - last modified Aug.05.2021 23:54 CET - Version 1.0.5 "using spiffs + SDconfig + SHT30 Room Sensor"

**last edit:** added optional support for the SHT30 Temperature and Humidity sensor in the official grey vertical base - makes a bit larger info panel so only select if you want it.  -> when using the M5Burner just select regular or the sensor version (Celcius or Fahrenheit)

For users **without the vertical base i recommend to use the usual version**
you would loose some pixels from the candle chart for no benefit

--------> **NOTE:** Aug.5.2021: I fixed a **BUG** in the recent version where your settings was not saved

#
#
##Using the App:

- This version needs a SD-Card whith the your WiFi credentials, favourite coinpairs, timezone and language(eng/ger/esp) in a textfile - see installation

- ButtonA: switches through your favourite coinpair (up to 25) e.g: BTC/USDT or whatever is available on the spot exchange Binance.com

- ButtonB: changes the LCD-Brightness in 4 levels

- ButtonC: press buttonC then, within 2 sec press buttonA to switch down, or buttonB to switch up through the timeframes
           available timeframes are 1minute, 3m, 5m, 15m, 1h, 4h, 1d, 1w, 1Month

- To turn OFF the device press the red button once OR by holding ButtonC for over 1 second (e.g if USB is connected)

- if you hold ButtonC at Startup: it will start with alternative SSID2/WiFi2-password instead (e.g your mobile phone's hotspot)

- if you hold ButtonB at Startup: it will delete all user settings stored from internal memory




##Further description:

- The top infoPanel shows the WiFi-strength, batterylevel, colored indicators for "busy", SleepTimer, price moving and if charging from usb (can have delay up to 30s)

- right now: english, german, spanish (for day and month names only)

- SleepTimer: when holding ButtonB longer than 1,5 seconds it will start a user defined timer to powerOFF the device

- If WiFi is failing more than 2 minutes it reduces the reconnect interval and brightness level, after 10 minutes -> shutdown device

- SDMenu Loader compatible, if SD-Updater (menu.bin) is installed in your SD- Card hold buttonA while booting up to start MenuLoader to load your apps

- It is prepared for the use of a Neopixel RGB-LED bar (i use the built-in one in the Battery-Bottom Module for M5Stack/Fire with rgb 10 LEDs)

- The settings will remain stored in internal memory after a reset. No SD-Card is needed after you have loaded your settings once! (thats new!)

- If no SD-Card is installed it will load the last good settings again - to clear all stored settings from internal memory hold ButtonB at start-up

SEE CODE OR IN THE PACKAGE FOR FURTHER INSTRUCTIONS ABOUT THE SD-CARD AND CONFIG FILE



## -- THIS IS ALL FREE - you love it ? please feel free and support me with any crypto  -- ##
    -> bc1qvfjd0x2r7fk3p0lwwaxvjjzgdrgjnu5fhqjqcv               (Bitcoin/BTC - native segwit)
    -> 0x8D88c8ff9A467F0821E8d1941D206164b11F4056               (Ethereum/ETH - or any ERC20 or BSC/BEP20 tokens)
    -> D5QEBVqY3B9kUEMn9nRHexSbMaRgRafKiY                       (Doge/DOGE)
    -> THRo4ujdmD5eYVPD2XUodN7NowRZ88F9rN                       (Tron/TRX or any TRC10/TRC20 tokens)
    -> GDY2LAUPJFWCR4XWN5EHQ72VLYRY53CNCCA4IXTBBZBKMAGT2BSY7KUT (Stellar/XLM)


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

Here the Version with room sensor built in the M5 vertical base
![](preview/Sensor.jpg)
