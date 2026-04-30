import serial
import time
import struct

###########################
##### UART Kommandoer #####
###########################

# - 0x01: Send ønskede værdier
#   - Parameter 1: temperatur
#   - Parameter 2: co2
CMD_SET_DESIRED_VALUES = 0x01

# - 0x02: Læs nuværende temperatur
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_ROOM_TEMP = 0x02

# - 0x03: Læs nuværende CO2-niveau
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_ROOM_CO2 = 0x03

###########################

# Vi vil også sende en stop byte til sidst med værdien:
#  - Stopbyte
STOPBYTE = 0xFF
#  - Intet parameter
PARAMETER_OMITTED = 0x00

###########################

# Vi vil også indstille en konstant størrelse af bytes til at blive modtaget fra arduinoen
#  - Expected bytes
EXPECTED_BYTES = 4

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
	
	# --- Public methods ---

	def send_desired_values(self, temp, co2):
		if self.ser.is_open:
			# Send kommando med data
			self.__send_command_UART(cmd=CMD_SET_DESIRED_VALUES, par1=temp, par2=co2)
		else:
			# Ved fejl: informer udvikler
			print("Error: Ikke forbundet til nogen port")

	def get_room_temp(self):
		# Fjern alle garbage værdier i RX buffer
		self.ser.reset_input_buffer()
		# Send kommando
		self.__send_command_UART(cmd=CMD_GET_ROOM_TEMP, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
		# Gem modtaget data i variabel
		response = self.ser.read(EXPECTED_BYTES)

		# Hvis respons er valid -> Returner
		# Ellers -> Print fejl
		if len(response) == EXPECTED_BYTES:
			print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
			return response
		else:
			print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
			return None
		

	def get_room_co2(self):
		# Fjern alle garbage værdier i RX buffer
		self.ser.reset_input_buffer()
		# Send kommando
		self.__send_command_UART(cmd=CMD_GET_ROOM_CO2, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
		# Gem modtaget data i variabel
		response = self.ser.read(EXPECTED_BYTES)

		# Hvis respons er valid -> Returner
		# Ellers -> Print fejl
		if len(response) == EXPECTED_BYTES:
			print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
			return response
		else:
			print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
			return None

	# --- Private methods ---

	def __pack_values(self, cmd, par1, par2):
		# --- Parametre til strcut.pack ---
		# 1. Parameter: Hvordan data skal pakkes
		# 2. Parameter: Kommando
		# 3. Parameter: Parameter 1 (til kommando)
		# 4. Parameter: Parameter 2 (til kommando)
		# 4. Parameter: STOPBYTE
		return struct.pack('>BBBB', cmd, par1, par2, STOPBYTE)
	
	def __send_command_UART(self, cmd, par1, par2):
		# Pak data i respektive bytes
		packet = self.__pack_values(cmd=cmd, par1=par1, par2=par2)

		# Send data
		self.ser.write(packet)
		self.ser.flush()

		# Print sendt byte
		print(f"Byte sendt gennem UART: \n{packet.hex(' ')}")
