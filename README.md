# ESP32_basdd_nerf_turret

🎯 Nerf Turret Web-Controlled System — Powered by ESP32
This Arduino sketch transforms an ESP32 into a WiFi-accessible control hub for a custom-built Nerf dart turret, featuring servo-actuated firing and dual-motor flywheel propulsion. The system hosts its own access point and dynamic HTML interface, allowing real-time user control from any browser-connected device.

🌐 Key Features
- WiFi Access Point Mode:
ESP32 acts as a standalone AP using WiFi.softAP(), serving a local web UI without needing an external router.
- Web-Based Control Panel:
Users can interact via a webpage to issue commands:
SINGLE FIRE, START/STOP CONTINUOUS, and STOP FIRING.
- Servo-Triggered Dart Release:
A servo motor (pin 18) simulates trigger pressing for each shot, with defined press duration.
- Dual DC Motor Flywheel System:
Two motors (controlled via four GPIOs) spin up the turret’s flywheel mechanism for propulsion.
- Continuous Firing Mode:
Activates repeated firing at intervals (FIRE_INTERVAL = 500ms) while keeping motors spinning.
- Fail-Safes and Timing Logic:
- Motors auto-stop after MOTOR_RUN_TIME = 2000ms in single fire mode.
- Servo reset after TRIGGER_PRESS_TIME = 200ms.
- Emergency stop resets all states and halts firing.


🔧 Web Routes and Their Actions
| Route | Action | 
| / | Displays the control panel web interface | 
| /fire | Fires a single dart | 
| /continuous | Starts or stops continuous firing mode | 
| /stop | Emergency halt for motors and servo | 
