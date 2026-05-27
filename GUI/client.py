import serial
import time
import struct

## @package client
# UART-klient brugt af GUI'en til kommunikation med embedded-controlleren.

###########################
##### UART Kommandoer #####
###########################

# - 0x01: Send ønskede værdier
#   - Parameter 1: Temperatur
#   - Parameter 2: CO2
CMD_SET_DESIRED_VALUES = 0x01

# - 0x02: Læs nuværende inde temperatur
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_ROOM_TEMP = 0x02

# - 0x03: Læs nuværende CO2-niveau
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_ROOM_CO2 = 0x03

# - 0x04: Læs nuværende ude temperatur
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_OUTSIDE_TEMP = 0x04

# - 0x05: Læs nuværende lys-niveau
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_LIGHT = 0x05

# - 0x06: Send vindue status
#   - Parameter 1: åben(1)/lukket(0)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_SET_WINDOW_STATE = 0x06

# - 0x07: Set gardin status
#   - Parameter 1: åben(1)/lukket(0)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_SET_CURTAIN_STATE = 0x07

# - 0x08: Set toggle auto mode
#   - Parameter 1: Tændt(1)/Slukket(0)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_TOGGLE_AUTO_MODE = 0x08

# - 0x09: Læs window state
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_WINDOW_OPEN = 0x09

# - 0x0A: Læs curtain state
#   - Parameter 1: Intet parameter (PARAMETER_OMITTED)
#   - Parameter 2: Intet parameter (PARAMETER_OMITTED)
CMD_GET_CURTAIN_OPEN = 0x0A

###########################

# Vi vil også sende en stop byte til sidst med værdien:
#  - Stopbyte
STOPBYTE = 0xFF
#  - Intet parameter
PARAMETER_OMITTED = 0x00

###########################

# Embedded svarer med 6-byte datapakker.
EXPECTED_RESPONSE_BYTES = 6

# Konstant ID'er til parser
ROOM_TEMP_ID = 0x01
ROOM_CO2_ID = 0x02
OUTSIDE_TEMP_ID = 0x03
LIGHT_ID = 0x04
WINDOW_STATE_ID = 0x05
CURTAIN_STATE_ID = 0x06

## Håndterer pakning, afsendelse og læsning af UART-kommandoer.
#
# Client indkapsler pyserial og udstiller en metode pr. GUI-kommando eller
# sensor request. Sender kommandopakker på 4 bytes: kommando,
# parameter 1, parameter 2 og STOPBYTE. Sensor og state læsninger forventer
# seks-byte svar fra embedded.
class Client:
    ## Åbner den serielle forbindelse og venter på at den stabiliserer sig.
    #
    # @param port Seriel port for embedded UART-forbindelse.
    # @param baudrate UART baudrate.
    # @param timeout Timeout for seriel læsning i sek.
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

    ## Sender ønsket temperatur og CO2-niveau til embedded.
    #
    # @param temp Byte for ønsket temperatur.
    # @param co2 Byte for ønsket CO2-niveau.
    def send_desired_values(self, temp, co2):
        if self.ser.is_open:
            # Send kommando med data
            self.__send_command_UART(cmd=CMD_SET_DESIRED_VALUES, par1=temp, par2=co2)
        else:
            print("Error: Ikke forbundet til nogen port")

    ## Få den aktuelle vinduestilstand fra embedded.
    #
    # @return 6 byte svar når det er gyldigt, ellers None.
    def get_window_open(self):
        # Fjern alle garbage værdier i RX buffer
        self.ser.reset_input_buffer()
        # Send kommando
        self.__send_command_UART(cmd=CMD_GET_WINDOW_OPEN, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
        # Gem modtaget data i variabel
        response = self.ser.read(EXPECTED_RESPONSE_BYTES)

        # Hvis respons er valid -> Returner
        # Ellers -> Print fejl
        if len(response) == EXPECTED_RESPONSE_BYTES and (response[0] == 0x05):
            print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
            return response
        else:
            print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
            return None

    ## Få den aktuelle gardintilstand fra embedded.
    #
    # @return 6 byte svar når det er gyldigt, ellers None.
    def get_curtain_open(self):
        # Fjern alle garbage værdier i RX buffer
        self.ser.reset_input_buffer()
        # Send kommando
        self.__send_command_UART(cmd=CMD_GET_CURTAIN_OPEN, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
        # Gem modtaget data i variabel
        response = self.ser.read(EXPECTED_RESPONSE_BYTES)

        # Hvis respons er valid -> Returner
        # Ellers -> Print fejl
        if len(response) == EXPECTED_RESPONSE_BYTES and (response[0] == 0x06):
            print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
            return response
        else:
            print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
            return None

    ## Få den aktuelle indetemperatur.
    #
    # @return 6 byte svar når det er gyldigt, ellers None.
    def get_room_temp(self):
        # Fjern alle garbage værdier i RX buffer
        self.ser.reset_input_buffer()
        # Send kommando
        self.__send_command_UART(cmd=CMD_GET_ROOM_TEMP, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
        # Gem modtaget data i variabel
        response = self.ser.read(EXPECTED_RESPONSE_BYTES)

        # Hvis respons er valid -> Returner
        # Ellers -> Print fejl
        if len(response) == EXPECTED_RESPONSE_BYTES and (response[0] == 0x01):
            print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
            return response
        else:
            print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
            return None

    ## Få det aktuelle CO2-niveau indendørs.
    #
    # @return 6 byte svar når det er gyldigt, ellers None.
    def get_room_co2(self):
        # Fjern alle garbage værdier i RX buffer
        self.ser.reset_input_buffer()
        # Send kommando
        self.__send_command_UART(cmd=CMD_GET_ROOM_CO2, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
        # Gem modtaget data i variabel
        response = self.ser.read(EXPECTED_RESPONSE_BYTES)

        # Hvis respons er valid -> Returner
        # Ellers -> Print fejl
        if len(response) == EXPECTED_RESPONSE_BYTES and (response[0] == 0x02):
            print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
            return response
        else:
            print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
            return None

    ## Få den aktuelle udetemperatur.
    #
    # @return 6 byte svar når det er gyldigt, ellers None.
    def get_outside_temp(self):
        self.ser.reset_input_buffer()
        self.__send_command_UART(cmd=CMD_GET_OUTSIDE_TEMP, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
        response = self.ser.read(EXPECTED_RESPONSE_BYTES)

        if len(response) == EXPECTED_RESPONSE_BYTES and (response[0] == 0x03):
            print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
            return response
        else:
            print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
            return None

    ## Få den aktuelle lysintensitet.
    #
    # @return 6 byte svar når det er gyldigt, ellers None.
    def get_light(self):
        self.ser.reset_input_buffer()
        self.__send_command_UART(cmd=CMD_GET_LIGHT, par1=PARAMETER_OMITTED, par2=PARAMETER_OMITTED)
        response = self.ser.read(EXPECTED_RESPONSE_BYTES)
        if len(response) == EXPECTED_RESPONSE_BYTES and (response[0] == 0x04):
            print(f"Byte modtaget gennem UART: \n{response.hex(' ')}")
            return response
        else:
             print(f"Error: Arduino svarede ikke i tide ({self.__timeout} sekunder)")
             return None

    ## Sender en manuel åbn/luk-kommando til vinduet.
    #
    # @param is_open True for at åbne vinduet, False for at lukke det.
    def set_window_state(self, is_open):
        if self.ser.is_open:
            par1 = 1 if is_open else 0
            self.__send_command_UART(cmd=CMD_SET_WINDOW_STATE, par1=par1, par2=PARAMETER_OMITTED)
        else:
            print("Error: Ikke forbundet til nogen port")

    ## Sender en manuel åbn/luk-kommando til gardinet.
    #
    # @param is_open True for at åbne gardinet, False for at lukke det.
    def set_curtain_state(self, is_open):
        if self.ser.is_open:
            par1 = 1 if is_open else 0
            self.__send_command_UART(cmd=CMD_SET_CURTAIN_STATE, par1=par1, par2=PARAMETER_OMITTED)
        else:
            print("Error: Ikke forbundet til nogen port")

    ## Slår embedded automatisk styring til eller fra.
    #
    # @param is_auto True for at aktivere automatisk styring, False for at deaktivere den.
    def  toggle_auto_mode(self, is_auto):
        if self.ser.is_open:
            par1 = 1 if is_auto else 0
            self.__send_command_UART(cmd=CMD_TOGGLE_AUTO_MODE, par1=par1, par2=PARAMETER_OMITTED)
        else:
            print("Error: Ikke forbundet til nogen port")

    # parser 6 byte svar fra embedded
    def parse_sensor_response(self, response, expected_type):
        if response is None:
            return None

        if len(response) != 6:
            return None

        response_type = response[0]
        payload = response[1:5]
        stopbyte = response[5]

        if stopbyte != STOPBYTE:
            return None

        if response_type != expected_type:
            return None

        if response_type == LIGHT_ID:
            return int.from_bytes(payload, byteorder="big")

        return payload[0]

    # --- Private methods ---

    ## Pakker en 4 byte UART-kommandopakke.
    #
    # @param cmd Kommandobyte.
    # @param par1 Første parameterbyte.
    # @param par2 Anden parameterbyte.
    # @return Pakkede bytes klar til afsendelse over UART.
    def __pack_values(self, cmd, par1, par2):
        # --- Parametre til struct.pack ---
        # 1. Parameter: Hvordan data skal pakkes
        # 2. Parameter: Kommando
        # 3. Parameter: Parameter 1 (til kommando)
        # 4. Parameter: Parameter 2 (til kommando)
        # 5. Parameter: STOPBYTE
        return struct.pack(">BBBB", cmd, par1, par2, STOPBYTE)

    ## Sender en pakket kommandopakke over UART og tømmer den serielle strøm.
    #
    # @param cmd Kommandobyte.
    # @param par1 Første parameterbyte.
    # @param par2 Anden parameterbyte.
    def __send_command_UART(self, cmd, par1, par2):
        # Pak data i respektive bytes
        packet = self.__pack_values(cmd=cmd, par1=par1, par2=par2)
        # Send data
        self.ser.write(packet)
        print("Packet sent: \n",packet.hex(' '))
        print("\n")
        self.ser.flush()
