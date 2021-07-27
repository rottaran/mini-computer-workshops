import machine
import servo
import time

i2c = machine.I2C(1, scl=machine.Pin(22), sda=machine.Pin(21), freq=400000)
pca = servo.PCA9685(i2c)
s1 = servo.Servo(pca, 15)
s1.position(180)

time.sleep(2)
s1.position(0)
time.sleep(2)
s1.position(90)
time.sleep(2)
s1.release()
