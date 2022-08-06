<h1 align="center">gpgate</h1>
<p align="center">aka *Global Positioning GATE* (remote controller)</p>

This sketch implements a gate remote control system based on GPS position. I coded this back in 2020, but I think it is an idea worthy to be published, even if the project is still not completed.

**Current dev status**: beta





## How it works




## TODO list

**High priority**
- use HDOP as a criteria for GPS fix control
- implement a suspension/wake-up system to save energy

**Really extra**
- solder the circuit on PCB and build a 3D printable case


## Technical remarks

### Adapt the software to your hardware



## Setup

You have to create your own `mygate.h` file, which will provide your waypoints and gates to the sketch. Use the **tool make_config** to create such file, or edit the template file you find when you download the repo.


