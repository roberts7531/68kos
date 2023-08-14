import base64;
import time;
import serial;
ser = serial.Serial('/dev/tty.usbserial-120',115200)
filedest = open('C.b64','w')
with open('drive_c.img','rb') as file:
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
filetra = open('basic.b64','r')
i=0;
while 1:
    char = filetra.read(1)
    if not char:
        break;
    ser.write(char.encode("ASCII"))
    
    if char=='\n':
        print(i)
        ser.read()
        i = i+1
    else:
        ser.read()
