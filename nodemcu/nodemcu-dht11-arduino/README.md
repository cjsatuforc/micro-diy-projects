# NodeMCU IoT Monitor

Simple IoT node that monitors the temperature, humidity and light level and submits them to a ThingSpeak channel.

# Prerequisites
* Arduino IDE configured for NodeMCU development
* NodeMCU v0.9 (other versions should work as well, be sure to check their pinouts though)
* DHT11
* Led
* Photoresistor (I've used LDR07, others should be working as well - be sure to check their specs).
* ThingSpeak account

## Arduino Libraries:
* Adafruit DHT sensor library - install it from the Arduino IDE

# Setup

TODO - Diagram

The following PIN configuration is used by this sketch:
* GPIO04 - LDR Power
* GPIO05 - LED
* GPIO14 - DHT Data

For deep sleep,
* GPIO16 - RST
* GPIO00 - Pull Up - 4.7K - VCC (not really needed)
* GPIO15 - Pull Down - 4.7K GND (not really needed)

