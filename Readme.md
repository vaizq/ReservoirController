# Cultimatics ReservoirController
Software for hydroponics reservoir automation 


## Abilities
- Automatic PH control.
- Automatic Nutrient dosing and EC control. (Unlimited number of pumps)
- Automatic liquid-level control.
- RPC interface for remote control and monitoring.
- Extensible 


## Architecture
Core is platform and hardware agnostic. (Depends only on C++17)
ReservoirController is easy to port to different platforms and hardware, it's also easy to test and simulate. (see [simulation](./simulation/))
Current drivers are implemented using Arduino framework on ESP32 for my prototype.

Hardware will consists of two major parts: Controller and Dosers.
Dosers are connected to Controller and can be daisy chained together.
Controller uses scheduling when dosing to limit the number of simultaneusly running pumps and voltage spikes.
This way users need to use only one power supply and can add unlimited number of pumps to their system.


## Current state - V0.1
I have first prototype build using solder board and parts I had laying around.
Goal is to design PCBs for ReservoirController and pump modules. So that pumps can be daisy chained.