gdisplay.attach(gdisplay.ILI9341, gdisplay.LANDSCAPE_FLIP, false)
gdisplay.clear()
gdisplay.setfont(gdisplay.UBUNTU16_FONT)
gdisplay.write({gdisplay.CENTER,gdisplay.CENTER},"Lua-RTOS-ESP32")