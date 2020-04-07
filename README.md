# Aare Display

ESP8266 based display of the current Aare temperature on a seven-segment display

## Structure

- Uses [WifiManager](https://github.com/tzapu/WiFiManager) to search and connect to a WLAN.
- Uses the [Arduino Wire library](https://www.arduino.cc/en/Reference/Wire) to set up the I2C bus.
- A [MCP21017 I/O expander](https://www.microchip.com/wwwproducts/en/MCP23017) is put on the I2C bus.
- The MCP connects to the LEDs on the [Sparkfun 7-segment-display](https://www.sparkfun.com/products/11408).
- Uses the HTTP client to fetch the current Aare temperature in Bern, Schönau from the [Aare.guru API](https://aareguru.existenz.ch).

  TODO complete this.

## Known issues

- Display refresh.
