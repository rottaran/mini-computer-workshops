import machine
import servo
import time

i2c = machine.I2C(1, scl=machine.Pin(22), sda=machine.Pin(21), freq=400000)
pca = servo.PCA9685(i2c)
motor1 = servo.DRV8833(pca, 0, 1)
motor2 = servo.DRV8833(pca, 2, 3)
motor3 = servo.DRV8833(pca, 4, 5)
motor4 = servo.DRV8833(pca, 6, 7)

while True:
    print("vorwärts")
    motor1.drive(100)
    time.sleep(10)

    print("stop")
    motor1.drive(0)
    time.sleep(10)

    print("rückwärts")
    motor1.drive(-100)
    time.sleep(10)

    print("stop")
    motor1.drive(0)
    time.sleep(10)
