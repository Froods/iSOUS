import os
import queue
import sys
import threading
import time
import types
import builtins


GUI_DIR = os.path.abspath(os.path.join(os.path.dirname(__file__), ".."))
if GUI_DIR not in sys.path:
    sys.path.insert(0, GUI_DIR)


class VirtualUARTState:
    def __init__(self):
        self.room_temp = 22
        self.room_co2 = 450
        self.outside_temp = 11
        self.light = 325
        self.window_open = 0
        self.curtain_open = 0
        self.desired_temp = None
        self.desired_co2 = None
        self.command_log = []

    def snapshot(self):
        return {
            "room_temp": self.room_temp,
            "room_co2": self.room_co2,
            "outside_temp": self.outside_temp,
            "light": self.light,
            "window_open": self.window_open,
            "curtain_open": self.curtain_open,
            "desired_temp": self.desired_temp,
            "desired_co2": self.desired_co2,
        }


class FakeSerial:
    CMD_SET_DESIRED_VALUES = 0x01
    CMD_GET_ROOM_TEMP = 0x02
    CMD_GET_ROOM_CO2 = 0x03
    CMD_GET_OUTSIDE_TEMP = 0x04
    CMD_GET_LIGHT = 0x05
    CMD_SET_WINDOW_STATE = 0x06
    CMD_SET_CURTAIN_STATE = 0x07
    STOPBYTE = 0xFF

    def __init__(self, port, baudrate, timeout):
        self.portstr = port
        self.baudrate = baudrate
        self.timeout = timeout
        self.is_open = True
        self._state = VIRTUAL_UART
        self._next_response = b""

    def reset_input_buffer(self):
        self._next_response = b""

    def write(self, packet):
        cmd, par1, par2, stop = packet
        if stop != self.STOPBYTE:
            raise ValueError("Stopbyte mangler i virtuel UART-pakke")

        self._state.command_log.append((cmd, par1, par2))

        if cmd == self.CMD_SET_DESIRED_VALUES:
            self._state.desired_temp = par1
            self._state.desired_co2 = par2
            self._state.room_temp = par1
            self._state.room_co2 = par2
            print(f"[EMULATOR] Modtog ønskede værdier: temp={par1}, co2={par2}")
            self._next_response = b""
        elif cmd == self.CMD_GET_ROOM_TEMP:
            self._next_response = self._pack_value(self._state.room_temp)
        elif cmd == self.CMD_GET_ROOM_CO2:
            self._next_response = self._pack_value(self._state.room_co2)
        elif cmd == self.CMD_GET_OUTSIDE_TEMP:
            self._next_response = self._pack_value(self._state.outside_temp)
        elif cmd == self.CMD_GET_LIGHT:
            self._next_response = self._pack_value(self._state.light)
        elif cmd == self.CMD_SET_WINDOW_STATE:
            self._state.window_open = par1
            print(f"[EMULATOR] Vindue sat til: {'åben' if par1 else 'lukket'}")
            self._next_response = b""
        elif cmd == self.CMD_SET_CURTAIN_STATE:
            self._state.curtain_open = par1
            print(f"[EMULATOR] Gardin sat til: {'åbent' if par1 else 'lukket'}")
            self._next_response = b""
        else:
            raise ValueError(f"Ukendt virtuel kommando: {cmd}")

    def read(self, size):
        response = self._next_response[:size]
        self._next_response = b""
        return response

    def flush(self):
        return None

    def close(self):
        self.is_open = False

    def _pack_value(self, value):
        high = (value >> 8) & 0xFF
        low = value & 0xFF
        return bytes((0x00, high, low, self.STOPBYTE))


VIRTUAL_UART = VirtualUARTState()
COMMAND_QUEUE = queue.Queue()
UART_VERBOSE = False
ORIGINAL_PRINT = builtins.print


def filtered_print(*args, **kwargs):
    message = " ".join(str(arg) for arg in args)
    if not UART_VERBOSE and (
        "Byte modtaget gennem UART:" in message or
        "Byte sendt gennem UART:" in message
    ):
        return
    ORIGINAL_PRINT(*args, **kwargs)


builtins.print = filtered_print


import tkinter as tk  # noqa: E402

try:
    import serial  # noqa: E402
except ModuleNotFoundError:
    serial = types.ModuleType("serial")
    sys.modules["serial"] = serial


ORIGINAL_MAINLOOP = tk.Tk.mainloop
serial.Serial = FakeSerial
time.sleep = lambda _seconds: None
tk.Tk.mainloop = lambda self: None

import main as gui_main  # noqa: E402

tk.Tk.mainloop = ORIGINAL_MAINLOOP


def print_help():
    print(
        "\nKommandoer:\n"
        "  help                    Vis hjælp\n"
        "  show                    Vis nuværende emulator-state\n"
        "  buttons                 Vis state for vindue/gardin-knapper i settings\n"
        "  uart on|off             Tænd/sluk UART debug i emulatoren\n"
        "  temp <0-65535>          Sæt rumtemperatur\n"
        "  co2 <0-65535>           Sæt rum-CO2\n"
        "  outside <0-65535>       Sæt udetemperatur\n"
        "  light <0-65535>         Sæt lysniveau\n"
        "  desired <temp> <co2>    Sæt ønskede værdier direkte i emulatoren\n"
        "  window open|close       Sæt vinduestilstand i emulatoren\n"
        "  curtain open|close      Sæt gardintilstand i emulatoren\n"
        "  press window open|close Tryk på vindue-knap i settings (starter cooldown)\n"
        "  press curtain open|close Tryk på gardin-knap i settings (starter cooldown)\n"
        "  log                     Vis UART kommando-log\n"
        "  refresh                 Tving GUI til at hente nye sensordata nu\n"
        "  quit                    Luk emulator og GUI\n"
    )


def print_state():
    state = VIRTUAL_UART.snapshot()
    print("\nAktuel emulator-state:")
    for key, value in state.items():
        print(f"  {key}: {value}")


def print_button_states(app):
    print("\nKnap-states i settings:")
    print(f"  vindue_op: {app.settings_page.vindue_op.cget('state')}")
    print(f"  vindue_ned: {app.settings_page.vindue_ned.cget('state')}")
    print(f"  gardin_op: {app.settings_page.gardin_op.cget('state')}")
    print(f"  gardin_ned: {app.settings_page.gardin_ned.cget('state')}")


def parse_int(value, field_name):
    try:
        parsed = int(value)
    except ValueError:
        print(f"Ugyldig værdi for {field_name}: {value}")
        return None

    if parsed < 0 or parsed > 65535:
        print(f"{field_name} skal være mellem 0 og 65535")
        return None

    return parsed


def handle_command(app, command_line):
    parts = command_line.strip().split()
    if not parts:
        return True

    cmd = parts[0].lower()

    if cmd == "help":
        print_help()
        return True

    if cmd == "show":
        print_state()
        return True

    if cmd == "buttons":
        print_button_states(app)
        return True

    if cmd == "uart" and len(parts) == 2:
        global UART_VERBOSE
        if parts[1].lower() == "on":
            UART_VERBOSE = True
            print("UART debug er nu tændt")
        elif parts[1].lower() == "off":
            UART_VERBOSE = False
            print("UART debug er nu slukket")
        else:
            print("Brug: uart on|off")
        return True

    if cmd == "temp" and len(parts) == 2:
        value = parse_int(parts[1], "temp")
        if value is not None:
            VIRTUAL_UART.room_temp = value
            print(f"Rumtemperatur sat til {value}")
        return True

    if cmd == "co2" and len(parts) == 2:
        value = parse_int(parts[1], "co2")
        if value is not None:
            VIRTUAL_UART.room_co2 = value
            print(f"CO2 sat til {value}")
        return True

    if cmd == "outside" and len(parts) == 2:
        value = parse_int(parts[1], "outside")
        if value is not None:
            VIRTUAL_UART.outside_temp = value
            print(f"Udetemperatur sat til {value}")
        return True

    if cmd == "light" and len(parts) == 2:
        value = parse_int(parts[1], "light")
        if value is not None:
            VIRTUAL_UART.light = value
            print(f"Lys sat til {value}")
        return True

    if cmd == "desired" and len(parts) == 3:
        temp_value = parse_int(parts[1], "desired temp")
        co2_value = parse_int(parts[2], "desired co2")
        if temp_value is not None and co2_value is not None:
            VIRTUAL_UART.desired_temp = temp_value
            VIRTUAL_UART.desired_co2 = co2_value
            print(f"Ønskede værdier sat til temp={temp_value}, co2={co2_value}")
        return True

    if cmd == "window" and len(parts) == 2:
        if parts[1].lower() == "open":
            VIRTUAL_UART.window_open = 1
            app.window_open = True
            print("Vindue sat til åben")
        elif parts[1].lower() == "close":
            VIRTUAL_UART.window_open = 0
            app.window_open = False
            print("Vindue sat til lukket")
        else:
            print("Brug: window open|close")
        return True

    if cmd == "curtain" and len(parts) == 2:
        if parts[1].lower() == "open":
            VIRTUAL_UART.curtain_open = 1
            app.curtain_open = True
            print("Gardin sat til åbent")
        elif parts[1].lower() == "close":
            VIRTUAL_UART.curtain_open = 0
            app.curtain_open = False
            print("Gardin sat til lukket")
        else:
            print("Brug: curtain open|close")
        return True

    if cmd == "press" and len(parts) == 3:
        target = parts[1].lower()
        action = parts[2].lower()

        if target == "window":
            if action == "open":
                app.settings_page.handle_open_window()
                print("Trykkede på vindue Åben-knappen")
            elif action == "close":
                app.settings_page.handle_close_window()
                print("Trykkede på vindue Luk-knappen")
            else:
                print("Brug: press window open|close")
                return True

            print_button_states(app)
            return True

        if target == "curtain":
            if action == "open":
                app.settings_page.handle_open_curtain()
                print("Trykkede på gardin Op-knappen")
            elif action == "close":
                app.settings_page.handle_close_curtain()
                print("Trykkede på gardin Ned-knappen")
            else:
                print("Brug: press curtain open|close")
                return True

            print_button_states(app)
            return True

        print("Brug: press window open|close eller press curtain open|close")
        return True

    if cmd == "log":
        print("\nUART kommando-log:")
        for index, entry in enumerate(VIRTUAL_UART.command_log, start=1):
            print(f"  {index:02d}: cmd=0x{entry[0]:02X}, par1={entry[1]}, par2={entry[2]}")
        if not VIRTUAL_UART.command_log:
            print("  (ingen kommandoer endnu)")
        return True

    if cmd == "refresh":
        app.update_sensor_values()
        print("GUI opdateret med nuværende emulator-data")
        return True

    if cmd == "quit":
        print("Lukker emulator...")
        app.root.destroy()
        return False

    print("Ukendt kommando. Skriv 'help' for at se muligheder.")
    return True


def input_loop():
    while True:
        try:
            command_line = input("> ")
        except EOFError:
            COMMAND_QUEUE.put("quit")
            break
        COMMAND_QUEUE.put(command_line)
        if command_line.strip().lower() == "quit":
            break


def process_command_queue(app):
    keep_running = True
    while True:
        try:
            command_line = COMMAND_QUEUE.get_nowait()
        except queue.Empty:
            break
        keep_running = handle_command(app, command_line)
        if not keep_running:
            return

    app.root.after(200, lambda: process_command_queue(app))


def run_emulator():
    app = gui_main.app

    # Byte-gyldige værdier til Gem-flowet i den nuværende UART-pakke.
    app.co2_values = {
        "Lav": 100,
        "Mellem": 200,
        "Høj": 250,
    }
    if app.client is not None and hasattr(app.client, "set_debug_uart"):
        app.client.set_debug_uart(False)

    print("=== Virtuel sensor-emulator startet ===")
    print("GUI-vinduet bruger nu virtuel UART i stedet for rigtig hardware.")
    print_help()
    print_state()

    thread = threading.Thread(target=input_loop, daemon=True)
    thread.start()

    process_command_queue(app)
    app.root.mainloop()


if __name__ == "__main__":
    run_emulator()
