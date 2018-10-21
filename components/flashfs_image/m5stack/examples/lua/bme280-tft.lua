-- ----------------------------------------------------------------
-- WHITECAT ECOSYSTEM
--
-- Lua RTOS examples
-- ----------------------------------------------------------------
-- This sample shows the temperature, humidity and pressure values
-- readed from a BME280 sensor, on a TFT display
-- ----------------------------------------------------------------

thread.start(function()
	local line
	local line_spacing

	-- Init TFT
	gdisplay.attach(gdisplay.ILI9341, gdisplay.LANDSCAPE_FLIP, true)
	dw, dh = gdisplay.getscreensize()
	gdisplay.setbackground(gdisplay.BLUE)
	gdisplay.setforeground(gdisplay.WHITE)
	gdisplay.clear()
	
	gdisplay.setfont(gdisplay.FONT_DEJAVU18)
	line_spacing = gdisplay.getfontheight() + 4
	gdisplay.on()
	
	
	-- Instantiate the sensor
	bme280 = sensor.attach("BME280", i2c.I2C0, 0x76)

	while true do
	  -- Read temperature
	  temperature = bme280:read("temperature")

	  -- Read humidity
	  humidity = bme280:read("humidity")

	  -- Read preassure
	  pressure = bme280:read("pressure")

	  -- Print results
	  line = 1
	  gdisplay.write(2,line * line_spacing,"T: "..string.format("%4.2f",temperature))
	  
	  line = line + 1
	  gdisplay.write(2,line * line_spacing,"H: "..string.format("%4.2f",humidity))
	  
	  line = line + 1
	  gdisplay.write(2,line * line_spacing,"P: "..string.format("%4.2f",pressure))
  
	  tmr.delayms(500)
	end
end)