import serial
import time
import struct

###########################
##### UART Kommandoer #####
###########################
# - 0x00: Send ønskede værdier
# - 0x01: Læs nuværende temperatur
# - 0x02: Læs nuværende CO2-niveau

class Client:
	 
	def __init__(self, port, baudrate, timeout):
		# Initialiser attributter
		self.__port = port
		self.__baudrate = baudrate
		self.__timeout = timeout
		self.__manual = False

		# Initialiser Serial object fra pyserial
		self.ser = serial.Serial(
			port=port,
			baudrate=baudrate,
			timeout=timeout
		)

		# Vent på forbindelsen stabiliserer 
		time.sleep(2)

		# Print forbindelses status
		if self.ser.is_open:
			print(f"Forbundet til {self.ser.portstr}")
		else:
			print("Error: Ikke forbundet til nogen port")
	
	def send_desired_values(self, temp, co2):
		if self.ser.is_open:
			# Pak data i respektive bytes
			packet = self.__pack_desired_values(temp=temp, co2=co2)
			# Send data
			self.ser.write(packet)
			print(f"Byte sendt gennem UART: \n{packet}")
		else:
			print("Error: Ikke forbundet til nogen port")

	def get_room_temp(self): # Implementer
		pass

	def get_room_co2(self): # Implementer
		pass

	def __pack_desired_values(self, temp, co2):
		# 1. Parameter: Hvordan data skal pakkes
		# 2. Parameter: Kommando
		# 3. Parameter: Ønskede temperatur
		# 4. Parameter: Ønskede CO2-niveau
		return struct.pack('>BBH', 0x00, temp, co2)
