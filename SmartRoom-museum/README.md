# SmartRoom-museum

Part of a 2-piece project demoing use of several peripheral devices attached to a Particle Argon and demonstrating publish()/subscribe() handling

The float "tempF", to be handled by Smartroom-insurance, is published periodically
Peripheral devices handled include:
A BME280 capable of reading local temperature, pressure, and relative humidity is used for temperature readings
A SSD1306 display is used for displaying local temperature and relative humitity, as well as messages reflecting museums "open" status
3 LEDs are used to show whether the temperature is above a certain cutoff (red), safe (green), or in a local security override state (blue)
A servo attached to a flag indicates that the museum is open; actually mounted within a lock frame
A simple button is used to toggle between automatic handling of security measures, or a meanual forced-open override state
