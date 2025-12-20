obj-y += src/main.o


obj-y += src/klipper_r528/bus/msgboxx.o
obj-y += src/klipper_r528/bus/share_space.o
obj-y += src/klipper_r528/bus/usb_bulk.o
obj-y += src/klipper_r528/bus/uart.o
obj-y += src/klipper_r528/hal_call/adccmds.o
obj-y += src/klipper_r528/hal_call/endstop.o
obj-y += src/klipper_r528/hal_call/gpiocmds.o
obj-y += src/klipper_r528/hal_call/i2ccmds.o
obj-y += src/klipper_r528/hal_call/initial_pins.o
obj-y += src/klipper_r528/hal_call/pwmcmds.o
obj-y += src/klipper_r528/hal_call/sensor_adxl345.o
obj-y += src/klipper_r528/hal_call/spi_software.o
obj-y += src/klipper_r528/hal_call/spicmds.o
obj-y += src/klipper_r528/hal_call/thermocouple.o
obj-y += src/klipper_r528/hal_call/tmcuart.o


obj-y += src/klipper_r528/printer/basecmd.o
obj-y += src/klipper_r528/printer/command.o
obj-y += src/klipper_r528/printer/compile_time_request.o
obj-y += src/klipper_r528/printer/debugcmds.o
obj-y += src/klipper_r528/printer/pulse_counter.o
obj-y += src/klipper_r528/printer/scheder.o
obj-y += src/klipper_r528/printer/stepper.o
obj-y += src/klipper_r528/printer/trsync.o
obj-y += src/klipper_r528/printer/neopixel.o
obj-y += src/klipper_r528/ui/buttons.o
obj-y += src/klipper_r528/ui/lcd_hd44780.o
obj-y += src/klipper_r528/ui/lcd_st7920.o



obj-y += src/klipper_r528/generic/alloc.o
obj-y += src/klipper_r528/generic/armcm_reset.o
obj-y += src/klipper_r528/generic/armcm_timer.o
obj-y += src/klipper_r528/generic/armcm_irq.o
obj-y += src/klipper_r528/generic/crc16_ccitt.o
obj-y += src/klipper_r528/generic/usb_cdc.o


obj-y += src/klipper_r528/board/adc.o
obj-y += src/klipper_r528/board/chipid.o
obj-y += src/klipper_r528/board/gpio.o
obj-y += src/klipper_r528/board/hard_pwm.o
obj-y += src/klipper_r528/board/i2c.o

obj-y += src/klipper_r528/board/spi.o
obj-y += src/klipper_r528/board/watchdog.o







Q = @
clean:
	$(Q)echo CLEAN objects
	$(Q)find -L ./ -path './src*' -prune -o -name '*.o' -exec rm {} +
	$(Q)find -L ./ -path './src*' -prune -o -name '.*.cmd' -exec rm {} +
	$(Q)find -L ./ -path './src*' -prune -o -name '.*.d' -exec rm {} +
	$(Q)find -L ./ -path './src*' -prune -o -name '*.tmp' -exec rm {} +
	$(Q)rm -rf  ./src/*.o  ./src/*.o.d ./src/*.o.cmd  ./src/.*.o.d ./src/.*.o.cmd
	$(Q)rm -rf  ./src/klipper_r528/*.o    ./src/klipper_r528/.*.o.d ./src/klipper_r528/*.o.cmd  ./src/klipper_r528/.*.o.cmd
	$(Q)rm -rf  ./src/klipper_r528/*/*.o    ./src/klipper_r528/*/.*.o.d ./src/klipper_r528/*/*.o.cmd ./src/klipper_r528/*/.*.o.cmd





