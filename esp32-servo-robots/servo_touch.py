import machine
import servo
import mpr121
import time

i2c = machine.I2C(1, scl=machine.Pin(22), sda=machine.Pin(21), freq=400000)
pca = servo.PCA9685(i2c)
touch = mpr121.MPR121(i2c)

s1 = servo.Servo(pca, 15)
s2 = servo.Servo(pca, 14)
s3 = servo.Servo(pca, 13)
s4 = servo.Servo(pca, 12)

while True:
    t1 = touch.is_touched(0)
    print(t1)
    if t1:
        s1.position(180)
    else:
        s1.position(0)

    t2 = touch.is_touched(11)
    if t2:
        s2.position(180)
    else:
        s2.position(0)

    t3 = touch.is_touched(2)
    if t3:
        s3.position(180)
    else:
        s3.position(0)

    t4 = touch.is_touched(4)
    if t4:
        s4.position(180)
    else:
        s4.position(0)

    time.sleep_ms(100)
    

