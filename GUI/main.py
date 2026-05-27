import tkinter as tk
from datetime import datetime, timedelta
from pathlib import Path

from client import Client
from pages import HomePage, SettingsPage

## @package main
# Hovedmodul for iSOUS GUI.
#
# Forbinder tkinter-siderne med UART-klienten og holder styr på data for
# sensor og states af vindue og gardin, som brugerfladen viser.

## GUI-sider, UART-kommunikation, tilstandsopdatering og logging.
#
# GUI ejer tkinter-hovedvinduet, opretter sideobjekterne, kommunikerer med
# embedded-controlleren gennem Client.py og opdaterer realtidsdata.
class GUI:
    # Svar ID'er matcher embedded UARTinterface.h.
    ROOM_TEMP_ID = 0x01
    ROOM_CO2_ID = 0x02
    OUTSIDE_TEMP_ID = 0x03
    LIGHT_ID = 0x04
    WINDOW_STATE_ID = 0x05
    CURTAIN_STATE_ID = 0x06
    STOPBYTE = 0xFF
    LOG_INTERVAL_HOURS = 48

    ## Initialiserer tkinter-vinduet, UART-klienten, siderne og gemt tilstand.
    #
    # @param port Seriel port for embedded UART-forbindelse.
    # @param baudrate UART baudrate.
    # @param timeout Timeout for seriel læsning i sek.
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
        self.curtain_open = True
        self.log_path = Path(__file__).resolve().parent.parent / "isous.log"
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

        # Opstart
        self.show_home()
        self.load_logged_values()
        self.update_sensor_values()

    ## Slår manuel tilstand til eller fra og giver embedded-controlleren besked.
    #
    # @param is_manual True når manuel styring skal være aktiv.
    def set_manual_mode(self, is_manual):
        if self.manual != is_manual and self.client is not None:
            # Sender 1 når automatisk styring er tændt og 0 når den er slukket.
            self.client.toggle_auto_mode(not is_manual)

        self.manual = is_manual
        self.home_page.refresh_control_mode()

    ## Skifter automatisk styring fra knappen på forsiden.
    def toggle_automatic_control(self):
        self.set_manual_mode(not self.manual)

    ## Åbner vinduet gennem embedded-controlleren.
    def open_window(self):
        if self.window_open:
            print("Vinduet er allerede åbent")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_window_state(True)
            self.window_open = True

    ## Lukker vinduet gennem embedded-controlleren.
    def close_window(self):
        if not self.window_open:
            print("Vinduet er allerede lukket")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_window_state(False)
            self.window_open = False

    ## Åbner gardinet gennem embedded-controlleren.
    def open_curtain(self):
        if self.curtain_open:
            print("Gardinet er allerede åbent")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_curtain_state(True)
            self.curtain_open = True

    ## Lukker gardinet gennem embedded-controlleren.
    def close_curtain(self):
        if not self.curtain_open:
            print("Gardinet er allerede lukket")
            return

        if self.client is not None:
            self.set_manual_mode(True)
            self.client.set_curtain_state(False)
            self.curtain_open = False

    ## Validerer og sender ønsket temperatur og CO2-indstilling.
    #
    # Sender de ønskede værdier via CMD_SET_DESIRED_VALUES, logger de valgte
    # mål-værdier og sætter systemet til automatisk styring efter gem.
    #
    # @param temp_text Temperatur fra tekstfeltet.
    # @param co2_level CO2 niveau valgt.
    def save_desired_values(self, temp_text, co2_level):
        if self.client is None:
            print("Error: Ikke forbundet til nogen port")
            return

        try:
            temp_value = int(float(temp_text))
        except ValueError:
            temp_value = 255

        # Validerer at ønsket temperatur ligger i det tilladte interval.
        if temp_value < 10 or (temp_value > 30 and temp_value != 255):
            print("Fejl: Ugyldigt valg. Temperaturen skal være mellem 0 og 40")
            return

        # CO2 værdierne kun skal ændres et sted, TJEK LIGE OP ..........................................................
        co2_value = self.co2_values.get(co2_level)
        if co2_value is None:
            print("Fejl: Ugyldigt CO2-niveau")
            return

        self.client.send_desired_values(temp=temp_value, co2=co2_value)
        self.log_target_values(temp=temp_value, co2=co2_value)
        # Når der gemmes ønskede værdier, genstartes automatisk styring.
        self.set_manual_mode(False)
        print(f"Sendte ønskede værdier: temperatur={temp_value}, co2={co2_value}")

    ## Gemmer de seneste ønskede mål-værdier i den lokale .isous-fil.
    #
    # @param temp Ønsket temperaturværdi.
    # @param co2 Ønsket CO2-niveau.
    def log_target_values(self, temp, co2):
        with open(".isous", "r") as f:
            lines = f.readlines()
        
        lines[0] = f"{temp}\n"
        lines[1] = f"{co2}\n"

        with open(".isous", "w") as f:
            f.writelines(lines)

    ## Indlæser tidligere gemte mål-værdier og sender dem til embedded.
    def load_logged_values(self):
        with open(".isous", "r") as f:
            lines = f.readlines()
        
        temp = lines[0].strip()
        co2 = lines[1].strip()

        self.save_desired_values(temp_text=temp, co2_level=co2)
        print(f"loaded previous values into embedded system: \n temp: {temp} \n co2: {co2}")

    ## Opretter UART-klienten og lader GUI'en starte uden hardware.
    #
    # @param port Seriel port for embedded UART-forbindelse.
    # @param baudrate UART baudrate.
    # @param timeout Timeout for seriel læsning i sek.
    # @return Client-instans hvis forbindelsen lykkes, ellers None.
    def _create_client(self, port, baudrate, timeout):
        try:
            return Client(port=port, baudrate=baudrate, timeout=timeout)
        except Exception as error:
            print(f"Kunne ikke oprette serial client: {error}")
            return None

    ## Viser forsiden efter opdatering af realtidsværdier og styringstilstand.
    def show_home(self):
        self.home_page.refresh_realtime_data()
        self.home_page.refresh_control_mode()
        self.home_page.frame.tkraise()

    ## Viser indstillingssiden og opdaterer knapper til manuel styring.
    def show_settings(self):
        self.settings_page.vindue_op.config(state=tk.DISABLED if self.window_open else tk.NORMAL)
        self.settings_page.vindue_ned.config(state=tk.NORMAL if self.window_open else tk.DISABLED)
        self.settings_page.gardin_op.config(state=tk.DISABLED if self.curtain_open else tk.NORMAL)
        self.settings_page.gardin_ned.config(state=tk.NORMAL if self.curtain_open else tk.DISABLED)
        self.settings_page.frame.tkraise()

    ## Parser et 6-byte sensorsvar fra embedded-controlleren.
    #
    # @param response UART-svarbytes.
    # @param expected_type Forventet svartype-byte.
    # @return Parset sensorværdi eller None hvis svaret er ugyldigt.
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

    ## Tilføjer de seneste realtidsdata til logfilen og sletter de gamle linjer.
    def _log_realtime_data(self):
        now = datetime.now()
        line = (
            f"{now:%Y-%m-%d %H:%M:%S} -- "
            f"room_temp={self.room_temp}, "
            f"temp_outside={self.temp_outside}, "
            f"room_co2={self.room_co2}, "
            f"light={self.light}, "
            f"manual={self.manual}, "
            f"window_open={self.window_open}, "
            f"curtain_open={self.curtain_open}\n"
        )

        with self.log_path.open("a", encoding="utf-8") as log_file:
            log_file.write(line)

        self._remove_old_log_lines(now)

    ## Fjerner gamle log linjer der er ældre end LOG_INTERVAL_HOURS.
    #
    # @param now Aktuelt tidspunkt brugt som reference for oprydning.
    def _remove_old_log_lines(self, now):
        oldest_allowed = now - timedelta(hours=self.LOG_INTERVAL_HOURS)
        kept_lines = []

        with self.log_path.open("r", encoding="utf-8") as log_file:
            for line in log_file:
                try:
                    line_time = datetime.strptime(line[:19], "%Y-%m-%d %H:%M:%S")
                except ValueError:
                    kept_lines.append(line)
                    continue

                if line_time >= oldest_allowed:
                    kept_lines.append(line)

        with self.log_path.open("w", encoding="utf-8") as log_file:
            log_file.writelines(kept_lines)

    ## Henter sensor og motor tilstand fra embedded og opdaterer GUI'en.
    #
    # Metoden planlægger selv næste kørsel med tkinter, og opdatere GUI'ens værdier.
    def update_sensor_values(self):
        if self.client is not None:
            try:
                room_temp = self.client.parse_sensor_response(self.client.get_room_temp(), self.ROOM_TEMP_ID)
                room_co2 = self.client.parse_sensor_response(self.client.get_room_co2(), self.ROOM_CO2_ID)
                outside_temp = self.client.parse_sensor_response(self.client.get_outside_temp(), self.OUTSIDE_TEMP_ID)
                light = self.client.parse_sensor_response(self.client.get_light(), self.LIGHT_ID)

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
        self._log_realtime_data()
        self.root.after(5000, self.update_sensor_values)

    ## Starter tkinter event-loopet.
    def run(self):
        self.root.mainloop()

app = GUI()
app.run()
