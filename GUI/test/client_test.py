import sys
import os
import time
# This adds the 'GUI' directory to your search path
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..')))
from client import Client  # Notice the '..' is gone from the import line

port = "/dev/ttys002"
temp = 17
co2 = 33

c = Client(port=port, baudrate=9600, timeout=1)
c.send_desired_values(temp=temp, co2=co2)

while True:
    time.sleep(1) # Sends once per second
    
## TESTING ##

# I terminal: socat -d -d PTY,link=/tmp/virtual_port_v0,raw,echo=0 PTY,link=/tmp/virtual_port_v1,raw,echo=0
# Du får følgende:
# 2026/03/27 11:59:39 socat[85741] N PTY is /dev/ttys00x <- x er et arbitrært tal
# 2026/03/27 11:59:39 socat[85741] N PTY is /dev/ttys00x <- x er et arbitrært tal

# I denne fil > ændr port til /dev/ttys00x fra første linje

# Åbn serial tools > serial ports > add serial port manually > kopier /dev/ttys00x fra anden linje ind 
# > tryk på change serial til serial 1 > tryk OPEN > tryk HEX terminal