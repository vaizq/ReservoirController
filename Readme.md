# ReservoirController
Software for hydroponics reservoir automation, targeted for ESP32 on arduino framework

## Abilities
- PH control
- Nutrient dosing and EC control
- Automatic liquid-level control
- Json-RPC interface over mqtt for remote control and monitoring

## Architecture
Core relays only on simple concepts of AnalogSensor (ph, ec), DigitalSensor (liquid-level) and Valve. Those are expressed using C++20 concepts.
There are two abstractions over Valve: Doser and DoserManager. Doser is just a valve with a known flowRate so it provides a simple interface
<code> doser.dose(float amount); </code> Running Dosers using DoserManager, user can select how many dosers are run parallel. DoserManager uses
queuing of doses to achieve this.
Existing drivers are implemented using Arduino framework on ESP32 for my current prototype.