import tkinter as tk

from client import Client
from pages import HomePage, SettingsPage


# self er objektet som man arbejder på
class GUI:
    # Svar-ID'er matcher embedded UARTinterface.h.
    ROOM_TEMP_ID = 0x01
    ROOM_CO2_ID = 0x02
    OUTSIDE_TEMP_ID = 0x03
    LIGHT_ID = 0x04
    STOPBYTE = 0xFF
    WINDOW_STATE_ID = 0x05
    CURTAIN_STATE_ID = 0x06

    # GUI'en initialiserer alle realtime-attributter, som forsiden bruger.
    def __init__(self, port="COM4", baudrate=9600, timeout=5):
        self.root = tk.Tk()
        self.root.geometry("640x360")
        self.root.title("iSOUS")

        self.client = self._create_client(port=port, baudrate=baudrate, timeout=timeout)
        self.room_temp = None
        self.temp_outside = None
        self.room_co2 = None
        self.light = None
        self.manual = False
        self.window_open = False
        self.curtain_open = False
        self.co2_values = {
            "Ureguleret": 0,
            "Lav": 1,
            "Mellem": 2,
            "Høj": 3,
        }

        container = tk.Frame(self.root)
        container.pack()

        # Lav begge pages
        self.home_page = HomePage(container, self)
        self.settings_page = SettingsPage(container, self)

        # Placer dem oven på hinanden
        self.home_page.frame.grid(row=0, column=0, sticky="nsew")
        self.settings_page.frame.grid(row=0, column=0, sticky="nsew")

        # Startside
        self.show_home()
        self.update_sensor_values()

    def set_manual_mode(self, is_manual):
        if self.manual != is_manual and self.client is not None:
            # Sender 1 når automatisk styring er tændt og 0 når den er slukket.
            self.client.toggle_auto_mode(not is_manual)

        self.manual = is_manual
        self.home_page.refresh_control_mode()

    # toggle til knap på forside
    def toggle_automatic_control(self):
        self.set_manual_mode(not self.manual)

    def open_window(self):
        if self.window_open:
            print("Vinduet er allerede åbent")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_window_state(True)
            self.window_open = True

    def close_window(self):
        if not self.window_open:
            print("Vinduet er allerede lukket")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_window_state(False)
            self.window_open = False

    def open_curtain(self):
        if self.curtain_open:
            print("Gardinet er allerede åbent")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_curtain_state(True)
            self.curtain_open = True

    def close_curtain(self):
        if not self.curtain_open:
            print("Gardinet er allerede lukket")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_curtain_state(False)
            self.curtain_open = False

    # send ønsket temperatur og CO2 via CMD_SET_DESIRED_VALUES.
    def save_desired_values(self, temp_text, co2_level):
        if self.client is None:
            print("Error: Ikke forbundet til nogen port")
            return

        try:
            temp_value = int(float(temp_text))
        except ValueError:
            temp_value = 255

        # Validerer at ønsket temperatur ligger i det tilladte interval.
        if temp_value < 0 or (temp_value > 40 and temp_value != 255):
            print("Fejl: Ugyldigt valg. Temperaturen skal være mellem 0 og 40")
            return

        # Bruger den fælles CO2-mapping, så værdierne kun skal ændres et sted.
        co2_value = self.co2_values.get(co2_level)
        if co2_value is None:
            print("Fejl: Ugyldigt CO2-niveau")
            return

        self.client.send_desired_values(temp=temp_value, co2=co2_value)
        # Når der gemmes ønskede værdier, genstartes automatisk styring.
        self.set_manual_mode(False)
        print(f"Sendte ønskede værdier: temperatur={temp_value}, co2={co2_value}")

    # Oprettelse af client, så GUI'en stadig kan starte uden seriel forbindelse.
    def _create_client(self, port, baudrate, timeout):
        try:
            return Client(port=port, baudrate=baudrate, timeout=timeout)
        except Exception as error:
            print(f"Kunne ikke oprette serial client: {error}")
            return None

    # show_home opdaterer forsiden med de nyeste gemte attributter for alle realtime-felter.
    def show_home(self):
        self.home_page.refresh_realtime_data()
        self.home_page.refresh_control_mode()
        self.home_page.show()

    def show_settings(self):
        self.settings_page.vindue_op.config(state=tk.DISABLED if self.window_open else tk.NORMAL)
        self.settings_page.vindue_ned.config(state=tk.NORMAL if self.window_open else tk.DISABLED)
        self.settings_page.gardin_op.config(state=tk.DISABLED if self.curtain_open else tk.NORMAL)
        self.settings_page.gardin_ned.config(state=tk.NORMAL if self.curtain_open else tk.DISABLED)
        self.settings_page.show()

    # Parser 6-byte svar fra embedded.
    def _parse_sensor_response(self, response, expected_type):
        if response is None:
            return None

        if len(response) != 6:
            return None

        response_type = response[0]
        payload = response[1:5]
        stopbyte = response[5]

        if stopbyte != self.STOPBYTE:
            return None

        if response_type != expected_type:
            return None

        if response_type == self.LIGHT_ID:
            return int.from_bytes(payload, byteorder="big")

        return payload[0]

    # update_sensor_values samler sensordata via de oprindelige client-metoder.
    def update_sensor_values(self):
        if self.client is not None:
            try:
                room_temp = self._parse_sensor_response(self.client.get_room_temp(), self.ROOM_TEMP_ID)
                room_co2 = self._parse_sensor_response(self.client.get_room_co2(), self.ROOM_CO2_ID)
                outside_temp = self._parse_sensor_response(self.client.get_outside_temp(), self.OUTSIDE_TEMP_ID)
                light = self._parse_sensor_response(self.client.get_light(), self.LIGHT_ID)

                returned_window_arr = self.client.get_window_open()
                if returned_window_arr[1] == 0x01:
                    self.window_open = True
                elif returned_window_arr[1] == 0x00:
                    self.window_open = False
                else:
                    print(f"error (window state) - byte recivied: {returned_window_arr[1]}")

                returned_curtain_arr = self.client.get_curtain_open()
                if returned_curtain_arr[1] == 0x01:
                    self.curtain_open = True
                elif returned_curtain_arr[1] == 0x00:
                    self.curtain_open = False
                else:
                    print(f"Error: (curtain state) - byte recivied: {returned_curtain_arr[1]}")

                if room_temp is not None:
                    self.room_temp = room_temp
                if room_co2 is not None:
                    self.room_co2 = room_co2
                if outside_temp is not None:
                    self.temp_outside = outside_temp
                if light is not None:
                    self.light = light

            except Exception as error:
                print(f"Fejl ved hentning af realtidsdata: {error}")

        self.home_page.refresh_realtime_data()
        self.settings_page.enable_buttons(self.settings_page.manual_buttons)
        self.root.after(5000, self.update_sensor_values)

    def run(self):
        self.root.mainloop()

app = GUI()
app.run()
