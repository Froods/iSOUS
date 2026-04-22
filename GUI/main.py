import tkinter as tk

from client import Client
from pages import HomePage, SettingsPage


# self er objektet som man arbejder på
class GUI:
    # GUI'en initialiserer alle realtime-attributter, som forsiden bruger.
    def __init__(self, port="COM3", baudrate=9600, timeout=1):
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

    #Vindue og gardin åben luk metoder
    def open_window(self):
        if self.window_open:
            print("Vinduet er allerede åbent")
            return

        if self.client is not None:
            self.client.set_window_state(True)
            self.window_open = True

    def close_window(self):
        if not self.window_open:
            print("Vinduet er allerede lukket")
            return

        if self.client is not None:
            self.client.set_window_state(False)
            self.window_open = False

    def open_curtain(self):
        if self.curtain_open:
            print("Gardinet er allerede åbent")
            return

        if self.client is not None:
            self.client.set_curtain_state(True)
            self.curtain_open = True

    def close_curtain(self):
        if not self.curtain_open:
            print("Gardinet er allerede lukket")
            return

        if self.client is not None:
            self.client.set_curtain_state(False)
            self.curtain_open = False

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
        self.home_page.show()

    def show_settings(self):
        self.settings_page.show()

    # GUI'en parser rå sensor-bytes, så client.py kan blive i sin oprindelige form.
    def _parse_sensor_response(self, response):
        if response is None:
            return None
    # er alle disse if's nødvendige?
        if len(response) != 4:
            return None

        if response[-1] == 0xFF:
            return int.from_bytes(response[1:3], byteorder="big")

        return int.from_bytes(response, byteorder="big")

    # update_sensor_values samler sensordata via de oprindelige client-metoder.
    def update_sensor_values(self):
        if self.client is not None:
            try:
                room_temp = self._parse_sensor_response(self.client.get_room_temp())
                room_co2 = self._parse_sensor_response(self.client.get_room_co2())
                outside_temp=self._parse_sensor_response(self.client.get_outside_temp())
                light = self._parse_sensor_response(self.client.get_light())

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
        self.root.after(5000, self.update_sensor_values)

    def run(self):
        self.root.mainloop()

app = GUI()
app.run()