import builtins
import os
import sys
import time
import types


GUI_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ""))
if GUI_DIR not in sys.path:
    sys.path.insert(0, GUI_DIR)


class FakeSerial:
    ROOM_TEMP_ID = 0x01
    ROOM_CO2_ID = 0x02
    OUTSIDE_TEMP_ID = 0x03
    LIGHT_ID = 0x04
    WINDOW_STATE_ID = 0x05
    CURTAIN_STATE_ID = 0x06
    STOPBYTE = 0xFF

    COMMAND_NAMES = {
        0x01: "CMD_SET_DESIRED_VALUES",
        0x02: "CMD_GET_ROOM_TEMP",
        0x03: "CMD_GET_ROOM_CO2",
        0x04: "CMD_GET_OUTSIDE_TEMP",
        0x05: "CMD_GET_LIGHT",
        0x06: "CMD_SET_WINDOW_STATE",
        0x07: "CMD_SET_CURTAIN_STATE",
        0x08: "CMD_TOGGLE_AUTO_MODE",
        0x09: "CMD_GET_WINDOW_OPEN",
        0x0A: "CMD_GET_CURTAIN_OPEN",
    }

    def __init__(self, port, baudrate, timeout):
        self.portstr = "TEST_PORT"
        self.baudrate = baudrate
        self.timeout = timeout
        self.is_open = True
        self._next_response = b""
        self.window_open = 0
        self.curtain_open = 1

    def reset_input_buffer(self):
        self._next_response = b""

    def write(self, packet):
        cmd = packet[0]
        command_name = self.COMMAND_NAMES.get(cmd, "UNKNOWN_COMMAND")
        test_print(f"Modtaget fra GUI ({command_name}): {packet.hex(' ')}")

        if cmd == 0x01:
            test_print(f"  Ønsket temperatur={packet[1]}, CO2={packet[2]}")
        elif cmd == 0x02:
            self._next_response = self._response(self.ROOM_TEMP_ID, 25)
        elif cmd == 0x03:
            self._next_response = self._response(self.ROOM_CO2_ID, 0)
        elif cmd == 0x04:
            self._next_response = self._response(self.OUTSIDE_TEMP_ID, 12)
        elif cmd == 0x05:
            self._next_response = self._response_int(self.LIGHT_ID, 325)
        elif cmd == 0x06:
            self.window_open = packet[1]
            test_print(f"  Vindue={'åbent' if self.window_open else 'lukket'}")
        elif cmd == 0x07:
            self.curtain_open = packet[1]
            test_print(f"  Gardin={'åbent' if self.curtain_open else 'lukket'}")
        elif cmd == 0x08:
            test_print(f"  Automatisk styring={'tændt' if packet[1] else 'slukket'}")
        elif cmd == 0x09:
            self._next_response = self._response(self.WINDOW_STATE_ID, self.window_open)
        elif cmd == 0x0A:
            self._next_response = self._response(self.CURTAIN_STATE_ID, self.curtain_open)

    def read(self, size):
        response = self._next_response[:size]
        self._next_response = b""

        if response:
            test_print(f"Sender til GUI: {response.hex(' ')}")

        return response

    def flush(self):
        return None

    def close(self):
        self.is_open = False

    def _response(self, response_id, value):
        payload = bytes((int(value) & 0xFF, 0x00, 0x00, 0x00))
        return bytes((response_id, *payload, self.STOPBYTE))

    def _response_int(self, response_id, value):
        payload = int(value).to_bytes(4, byteorder="big")
        return bytes((response_id, *payload, self.STOPBYTE))


original_print = builtins.print


def test_print(*args, **kwargs):
    kwargs.setdefault("flush", True)
    original_print(*args, **kwargs)


def filtered_print(*args, **kwargs):
    if not args or all(str(arg).strip() == "" for arg in args):
        return
    if args and str(args[0]).startswith("Packet sent:"):
        return
    if args and str(args[0]).startswith("Byte modtaget gennem UART:"):
        return

    test_print(*args, **kwargs)

builtins.print = filtered_print

try:
    import serial
except ModuleNotFoundError:
    serial = types.ModuleType("serial")
    sys.modules["serial"] = serial


serial.Serial = FakeSerial
time.sleep = lambda _seconds: None

test_print("Starter GUI")

import main