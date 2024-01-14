# Core: drivers, devices and controllers

Hardware agnostic layer that provides controllers for liquid-level, ph-level, ec-level and nutrient dosing.


# Architecture

v0.1
# Drivers
Concepts that define required semantics for drivers.
Actuators: Valve
Sensors: LiquidLevel, PH, EC

# Devices
Objects that use drivers to extend their functionality.
DosingPump

# Controllers
Controllers encapsulate sensor and actuators, to control some variable measured by that sensor.


v0.2
# Requirements
Users need to be able to create different implementations for sensors.
Some ph sensors are analog and some user RS485 for example.

Sensors job is to produce output, but different sensors produce different kind of output.
Ph sensor outputs value in range [0, 14], where LL sensor produces boolean value.

Only actuators that we really need is valve. We can use it as is to control liquid level and
we can build dosers from it to naturally dose exact amounts of liquids.


Controllers take responsibility of the sensor and actuators that it needs to operate.
Controllers use actuators to change sensor output in some direction.
ControlAlgorithms define the amount of "use".
Together controllers and control algorithms are used to achieve some sensor output.