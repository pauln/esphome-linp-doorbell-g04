# ESPHome custom component for Linptech G6L-WIFI (linp-doorbell-g04)

## Background
The Linptech G6L-WIFI is a wifi doorbell with a self-powered button.  It's part of the Mijia (Xiaomi smart home) ecosystem, but it unfortunately doesn't seem to announce events (such as the button being pressed) on the local network.  As such, this project aims to provide replacement firmware to enable full local control via [ESPHome](https://esphome.io/) - which provides for simple integration with [Home Assistant](https://www.home-assistant.io/).

As of early 2022, the latest version is the `linp-doorbell-g04`, which no longer contains a secondary microcontroller to handle the basic doorbell functionality; the ESP32 is now connected directly to the RF receiver and the doorbell chime chip.  If you have the earlier `linp-doorbell-g03` version with an STM8S005K6 microcontroller, see the [linp-doorbell-g03](https://github.com/pauln/esphome-linp-doorbell-g03/tree/feature/external_components) repository for a custom component for your device.

 
## Hardware
The doorbell's base unit is pressed/clipped together - there are no screws or other fasteners holding it together.  The seam between the two halves can be found running around the perimeter of the device.

**With the device unplugged**, carefully pry / pull the two halves apart.

Once you've got it open, you'll see an ESP32 module (which handles the wifi/network/cloud control, but not the basic doorbell functionality) with a row of test points beside it, marked (from top to bottom):
- RXD0
- TXD0
- IO0
- GND
- 3.3V

These pads can be used to communicate with the ESP32 via UART serial in order to initially flash ESPHome on to the ESP32.

If you lift the main circuit board out of the plastic case, you'll find "3.3V" and "GND" pads near the top on the back/bottom, which can be used to power the entire unit instead of the mains.  You can also power it from the 3.3V and GND pads next to the ESP32, but using these back pads can make it easier to use the other pads for flashing the ESP32.  I highly recommend that you power it via one of these sets of pads while working on the doorbell, as it's much safer to use an isolated 3.3V DC supply (I use a 3.3V regulator on the end of a USB cable plugged into a USB charger or battery pack) than to try powering it from the mains while reflashing the ESP32.  I desoldered the mains input wires from the circuit board, which allows the circuit board to be completely removed from the plastic case (if you unplug the speaker or remove it from the plastic case).

## ESP32 quirks
The ESP32 module used in this device is a single-core variant.  Additionally, the MAC address CRC written to the EFUSE at the factory seems to be incorrect, so a modification to the core libraries is required to disable the reset on MAC CRC check failure.  Recent versions of ESPHome provide configuration options to handle both of these quirks when using the ESP-IDF framework; see `doorbell.yaml` for the appropriate configuration.

## Requirements for flashing
- ESPHome
- A serial-to-USB adapter

## Configuration
- Set your own WiFi credentials in the `wifi:` section of the YAML file, so that it knows how to connect to your network.  The example config uses the !secret directive; [see the ESPHome docs for information](https://esphome.io/guides/faq.html) if you're not familiar with this.
- If you haven't already used a serial logger to get your button IDs from the stock firmware, ensure that you have the dumper enabled (`dump: linptech_g6l` in the `remote_receiver` configuration).

## Flashing the ESP32
In order to boot the ESP32 into flash mode (so that you can write ESPHome to it), you'll need to pull the two pins closest the bottom right corner of the ESP32 module to ground; I did this by soldering fine wires to the rightmost pin on the bottom edge, the `IO0` and `GND` test point beside the ESP32, but if you have an easier way to reliably short the two corner pads to ground temporarily, anything which is easy to undo should work fine.

1. Connect GND, TXD0, RXD0 to a serial-to-USB adapter (making sure to connect the ESP's RXD0 to your adapter's TX (or TXD) pin and the ESP's TXD0 to your adapter's RX (or RXD) pin)
2. Connect your serial-to-USB adapter to your computer, making note of the port which it shows up on (depending on your OS, this could be something along the lines of COM0 or /dev/ttyUSB0)
3. Short the corner pads (mentioned above) to GND
3. Power up the doorbell using the 3.3V and GND pads on the back of the board
4. Run `esphome doorbell.yaml run` (replacing "doorbell.yaml" with whatever your YAML file is called, if you've called it something else)
5. Once the compilation is complete, you'll be prompted to choose how to perform the update; choose your serial-to-USB adapter
6. Once the flashing is complete, unplug the doorbell from the 5V power supply, remove the short from the corner pads to GND, then power it back up to boot normally
7. The ESP32 should start up and connect to your WiFi network as configured in the yaml file; you can then add it to your Home Assistant and start integrating it into your home automation!

## Configuring your buttons
Once you've flashed ESPHome (with the dumper enabled), use `esphome logs doorbell.yaml` to connect (via serial or wifi) to the ESP32 and view the log output.  Pressing one of the buttons should result in a log message such as the following:

`[remote.linptech_g6l:068]: Received Linptech G6L: address=0x123456`

You can then use the part after `address=` (including the `0x` prefix) in a `remote_receiver` binary sensor configuration block (see `doorbell.yaml`).

## Home Assistant
Once you have your doorbell flashed with ESPHome and connected to Home Assistant, you should see the following services appear (if you included them in your configuration):

| Service name  | Description | Parameter 1 | Parameter 2 | Parameter 3 |
| ------------- | ----------- | ----------- | ----------- | ----------- |
| `esphome.doorbell_play_tune` | Play a tune/chime | `tune` \[int, 1-40] | `volume` \[int, 1-8] | `mode` \[int, 1-4] |
| `esphome.doorbell_stop_playing` | Stop the tune/chime, if one is currently playing |  |  |

For a list of the available "tunes", see the table in the [SZY8039B datasheet](https://raw.githubusercontent.com/pauln/esphome-linp-doorbell-g04/main/SZY8039B.pdf).  Your online translation service of choice might provide more meaningful English names for some of the options.

The `mode` parameter isn't especially useful, as in the G6L-WIFI, the LEDs are connected to the ESP32 and not the SZY8039B.  As such, modes 1, 2 and 4 play the music and mode 3 doesn't (which isn't much use).

Note that the `doorbell` prefix on all service names is the name of your ESPHome node, as defined in the `esphome:` block of your yaml file.


If these services don't appear in Home Assistant, try power cycling the doorbell so that it reconnects to Home Assistant.  I've found that the services don't seem to appear on first connect, but do on the second.
