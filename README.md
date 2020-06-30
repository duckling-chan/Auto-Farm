# Automated Vision-based Indoor Farming System
The codes released is part of a Final Year Project. The project aims to automate indoor farming by monitoring and controlling air temperature, humidity, lighting, water cycles and records the data onto an SD card for further data processing. A camera is mounted onto Cartesian-style rails to periodically capture images of the plants from a top-down perspective.

## Hardware requirements
### Microcontroller
1 x Arduino Mega

### Basic interface
1 x 2x16 LCD Screen\
4 x Tactile push buttons\
4 x 4.7k ohm resistor

### Lighting
1 x 5-meter RGB LED Strip\
3 x Push buttons\
3 x FQP30N06L Logic N-Channel MOSFET\
3 x 4.7k ohm resistor

### Water control/measurement
1 x 12 V Water pump\
1 x 12 V Magnetic valve\
2 x Magnetic float water level sensor\
1 x Flyback diode

### Humidity control/measurement
1 x DHTxx\
1 x 12 V Fan\
1 x Flyback diode\
1 x 4.7k ohm resistor

### Temperature measurement
1 x DHTxx (covered under Humidity measurement)\
1 x DS18B20 (for water temp. measurement)\
1 x 4.7k ohm resistor

### Data logging
1 x Micro SD TF Card Adapter Module

### Power
1 x 12 V, 10 A Switch-mode power supply\
1 x 5 V Opto-isolated relay

### Camera
1 x VC0706 Serial camera

### Cartesian-style rails
2 x NEMA 17 Stepper motors
2 x A4988 Stepper motor driver
1 x GT2 Timing belt
4 x 8 mm bore pulley
4 x 8 mm hardened steel rods

### Misc.
An assortment of jumper cables of different lengths

# To-do:
1. Refactor code
2. Update with schematics
3. Add images
4. Add Raspberry Pi integration codes
