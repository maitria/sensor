pro-micro         = -mmcu=atmega32u4 -DF_CPU=16000000L
usb-config        = -DUSB_VID=0x2341 -DUSB_PID=0x8037 -DUSB_MANUFACTURER='"Unknown"' -DUSB_PRODUCT='"Arduino Micro"'
make-small-code   = -Os
include-paths     = -IArduinoCore-avr/cores/arduino -IArduinoCore-avr/libraries/Wire/src -IArduinoCore-avr/libraries/SPI/src -ISparkFun_Micro_OLED_Arduino_Library/src -I.
do-not-link       = -c
c-stuff-arduino-people-want = -g -std=gnu11 -ffunction-sections -fdata-sections -MMD -flto -fno-fat-lto-objects
cpp-stuff-arduino-people-want = -g -std=gnu++11 -fpermissive -fno-exceptions -ffunction-sections -fdata-sections -fno-threadsafe-statics -Wno-error-narrowing -MMD -flto


sensor_sources = \
		 SparkFun_RHT03.cpp\
		 respite-sensor.cpp\
		 $(wildcard ArduinoCore-avr/cores/arduino/*.cpp)\
		 $(wildcard ArduinoCore-avr/cores/arduino/*.c)\
		 $(wildcard ArduinoCore-avr/libraries/Wire/src/*.cpp)\
		 $(wildcard ArduinoCore-avr/libraries/Wire/src/utility/*.c)\
		 $(wildcard ArduinoCore-avr/libraries/SPI/src/*.cpp)\
		 $(wildcard SparkFun_Micro_OLED_Arduino_Library/src/*.cpp)

sensor_objects = $(patsubst %.c,out/%.o,$(patsubst %.cpp,out/%.o,$(sensor_sources)))

.PHONY: all flash clean
all: out/respite-sensor.hex

flash: out/respite-sensor.hex
	avrdude -p atmega32u4 -c avr109 -P $(TTY) -U flash:w:$<

clean:
	rm -f $(sensor_objects) *.hex

out/%.o: %.c
	mkdir -p $(dir $@)
	avr-gcc $(pro-micro) $(usb-config) $(make-small-code) $(include-paths) $(do-not-link) $(c-stuff-arduino-people-want) -o $@ $<

out/%.o: %.cpp
	mkdir -p $(dir $@)
	avr-c++ $(pro-micro) $(usb-config) $(make-small-code) $(include-paths) $(do-not-link) $(cpp-stuff-arduino-people-want) -o $@ $<

out/respite-sensor.elf: $(sensor_objects)
	avr-c++ $(pro-micro) -o out/respite-sensor.elf $(sensor_objects)

out/%.hex: out/%.elf
	avr-objcopy -O ihex -R .eeprom $< $@

