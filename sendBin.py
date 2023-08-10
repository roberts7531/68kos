import base64;
import serial;
import time;
ser = serial.Serial('/dev/ttyUSB0',115200)
filedest = open('basic.b64','w')
with open('basic.srec','rb') as file:
    while True:
        chunk = file.read(100)
        if not chunk:
            break
        print(base64.b64encode(chunk))
        encoded = base64.b64encode(chunk)
        filedest.write(encoded.decode("ASCII"))
        filedest.write('\n');
filedest.write('\0');
filedest.close()
