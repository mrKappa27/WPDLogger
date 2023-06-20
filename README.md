# WPDLogger

ESP32 offline pressure datalogger to microsd.

:alarm_clock: The goal of this project is to log to a microsd card the readings of an analog water pressure transducer every 60S on a CSV file.
:electric_plug: It will be powered by an external battery (power bank) or a classic USB power supply.
:battery: Power consumption was a key point so I've used the [ESP32 deep sleep](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/sleep_modes.html) options to greatly reduce the power draw.

:watch: Right now there's no way to retrieve the time, we'll have a global incremental ID that increments each cycle.

## Future improvements

:satellite: Send data via LoraWan and/or WiFi to a remote endpoint

:sparkles: Custom PCB

:hourglass: External RTC

:blue_book: Bluetooth setup (Date and time + logging cadence)

## Contributing

Pull requests are welcome. For major changes, please open an issue first to discuss what you would like to change.
