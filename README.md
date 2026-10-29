# Task 2: Dual-Axis Solar Tracker

**TinkerCAD link:** https://www.tinkercad.com/things/2JKuQGGRBT9/editel?returnTo=%2Fdashboard%2Fdesigns%2Fall&sharecode=yo0gkKy3qsBSR-tZEfBIpp1jw538VVGkfukHHMrHAQw


## What I built

The task asks for 4 LDRs, but they aren't available in TinkerCAD anymore, so
I used an ultrasonic sensor instead. Two servos move the
panel: one turns it left and right (pan), the other tilts it up and down.

## Circuit

| Part | Connection |
|---|---|
| Ultrasonic sensor (3-pin) | 5V, GND, SIG to D7 |
| Pan servo | signal to D9 |
| Tilt servo | signal to D10 |
| LED (with 220 ohm resistor) | D13 |

Servos and the sensor are powered from the 5V and GND rails, and all
grounds are common.



## Additional functionality

- LED that turns on when the tracker is locked and off when it is searching
  or parked.
- Parking position when the object is lost.

## Real-world adaptation

A real tracker would use 4 LDRs in a 2x2 grid and compare left vs right and
top vs bottom. That follows the sun directly, so there is no scanning
needed. Things I would need to handle on an actual panel:

- **Clouds:** the light difference gets small, so the tracker shouldn't
  chase noise. A deadband like the one I used helps.
- **Wind:** the panel needs a safe position it can fold into.
- **Night:** reset to face east so it's ready for sunrise.
- **Power:** servos draw a lot of current, so they need their own supply
  rather than the Arduino's 5V pin.

## Further applications

Solar farms, satellite and antenna dishes, and cameras that follow a moving
subject all use the same idea of sensing a direction and adjusting two axes.
