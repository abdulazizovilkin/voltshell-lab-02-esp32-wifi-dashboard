# VoltShell Lab 02 — ESP32 Wi-Fi Web Dashboard

This project is an upgraded version of my ESP32 mini alarm system.

In this lab, I added a Wi-Fi web dashboard to monitor and control the alarm system through a browser.

## Features

* ESP32 Access Point mode
* Web dashboard at `192.168.4.1`
* Browser-based ARM / DISARM control
* OLED status display
* Physical button control
* Potentiometer-based sensor level monitoring
* Danger detection logic
* LED and buzzer alert

## Hardware Used

* ESP32 DevKit
* OLED 128x64 I2C display
* Potentiometer
* 2 LEDs
* Buzzer
* Push button
* Resistors
* Breadboard
* Jumper wires

## Pin Mapping

| Component     | ESP32 Pin |
| ------------- | --------- |
| OLED SDA      | GPIO21    |
| OLED SCL      | GPIO22    |
| Armed LED     | GPIO23    |
| Danger LED    | GPIO19    |
| Buzzer        | GPIO25    |
| Button        | GPIO27    |
| Potentiometer | GPIO34    |

## How It Works

The ESP32 creates its own Wi-Fi network called `VoltShell-Lab02`.

After connecting to this Wi-Fi network, the dashboard can be opened from a browser using:

`http://192.168.4.1`

From the dashboard, the system can be armed or disarmed.
The OLED display shows the current sensor level, system status, and danger status.

When the system is armed and the sensor level passes the danger threshold, the danger LED turns on and the buzzer starts beeping.

## What I Learned

* ESP32 Wi-Fi Access Point mode
* Basic web server logic on ESP32
* Browser-based device control
* GPIO input and output
* Analog input reading
* OLED display usage
* Button debounce logic
* Basic embedded system state management


