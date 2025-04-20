from machine import Pin, I2C
from time import sleep
import _thread
from ssd1306 import SSD1306_I2C as SSD1306

i2c = I2C(1, sda=Pin(14), scl=Pin(15), freq=100000)
display = SSD1306(128, 32, i2c)

def second_thread():
    print("Hello, World! from secondary thread")
    
    global i2c, display
    
    display.text('Hello, World!', 0, 0, 1)
    display.show()
    sleep(1)
    display.text('Bye, World!', 0, 16, 1)
    display.show()
    sleep(1)
    display.fill(0x0000)
    display.show()
    
    
_thread.start_new_thread(second_thread, ())

print("Hello, World! from primary thread")
for i in range(10000):
    print(i)



