from machine import Pin
from neopixel import NeoPixel

pin = Pin(16, Pin.OUT)   # set GPIO0 to output to drive NeoPixels
np = NeoPixel(pin, 4)   # create NeoPixel driver on GPIO0 for 8 pixels
np[0] = (255, 255, 255) # set the first pixel to white
np[1] = (255, 0, 0) # set the first pixel to white
np[2] = (0, 255, 0) # set the first pixel to white
np[3] = (0, 0, 255) # set the first pixel to white
np.write()              # write data to all pixels
r, g, b = np[0]         # get first pixel colour
