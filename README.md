# TemperatureProbes 

This project is to measure temperature at the following heights compareed to ground level in a specific location:

* -5 cm (arduion A0)
* 0 cm (arduion A1)
* 30 cm (arduion A2)
* 200 cm (arduion A3)

# Parts list
* 1 - Adafruit's ChronoDot (or Jeelabs high precision clock)
* 1 - RocketScream proto-shield
* 1 - RocketScream Pro (8 MHz / 3V) - a low power Arduino
* 4 - 10K Thermistors (from RocketScream 1 meter wire) are used to read the temperature - Adafruit also makes good 10K thermistors (with shorter wires - thus more soldering will be needed)
* 1 - Adafruit microSD Card reader/writer (it works at 3 V)
* 2 - micro SD cards (one installed and one to exchange when collecting data)
* batteries to be dertmined (must be recharable and work in the winter -- probably nickel cadmium or specialized lithium ion)

# Measurements

* each probe is measured 5 times within one minute (each probes 5 measurements are then averaged -- this averaged result is the reported result)
* the date & time from the low power high precision RTC in the format of: 
