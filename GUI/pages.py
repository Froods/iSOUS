import tkinter as tk


class Page:
    def __init__(self, parent, app):
        self.app = app
        self.frame = tk.Frame(parent, width=640, height=360)

    def show(self):
        self.frame.tkraise()


class HomePage(Page):
    def __init__(self, parent, app):
        super().__init__(parent, app)
        title = tk.Label(self.frame, text="iSOUS", font=("Arial", 14, "bold"))
        title.pack()

        # Hele siden
        content_row = tk.Frame(self.frame)
        content_row.pack(fill="both", expand=True, padx=10, pady=10)

        # Realtime-panelet
        realtime_panel = tk.LabelFrame(content_row, text="Realtidsdata")
        realtime_panel.pack(fill="x", anchor="n")

        # Alle homepage-felter starter som dynamiske realtime-felter fra GUI-attributter.
        self.temp_inside_var = tk.StringVar(value="Temperatur inde: --")
        self.temp_outside_var = tk.StringVar(value="Temperatur ude: --")
        self.co2_var = tk.StringVar(value="CO2 inde: --")
        self.light_var = tk.StringVar(value="Lysintensitet: --")

        tk.Label(realtime_panel, textvariable=self.temp_inside_var, anchor="w").pack(fill="x", padx=8, pady=(6, 2))
        tk.Label(realtime_panel, textvariable=self.temp_outside_var, anchor="w").pack(fill="x", padx=8, pady=2)
        tk.Label(realtime_panel, textvariable=self.co2_var, anchor="w").pack(fill="x", padx=8, pady=2)
        tk.Label(realtime_panel, textvariable=self.light_var, anchor="w").pack(fill="x", padx=8, pady=(2, 6))

        # Knapper under realtime (centreret)
        knap_row = tk.Frame(content_row)
        knap_row.pack(fill="x", pady=(10, 0))

        knap_center = tk.Frame(knap_row)
        knap_center.pack()

        settings_button = tk.Button(
            knap_center,
            text="Indstillinger",
            width=15,
            height=2,
            command=self.app.show_settings,
        )
        settings_button.pack(side="left", padx=(0, 10))

        # Gemmer reference til knappen, så dens tekst og farve kan opdateres ud fra manual-state.
        self.stopauto_button = tk.Button(
            knap_center,
            text="Stop Automatisk \n Styring",
            width=15,
            height=2,
            bg="red",
            command=self.app.toggle_automatic_control,
        )
        self.stopauto_button.pack(side="left")
        self.refresh_control_mode()

    # HomePage opdaterer alle fire realtime-felter fra GUI-attributterne.
    def refresh_realtime_data(self):
        temp_inside_text = "--" if self.app.room_temp is None else f"{self.app.room_temp} C"
        temp_outside_text = "--" if self.app.temp_outside is None else f"{self.app.temp_outside} C"
        co2_value = "Lav" if self.app.room_co2 == 3 else "Normalt indeklima" if self.app.room_co2 == 2 else "Høj" if self.app.room_co2 == 1 else "Ureguleret"
        

        co2_text = "--" if self.app.room_co2 is None else f"{co2_value} ppm"
        light_text = "--" if self.app.light is None else f"{self.app.light} lux"

        # tkinter
        self.temp_inside_var.set(f"Temperatur inde: {temp_inside_text}")
        self.temp_outside_var.set(f"Temperatur ude: {temp_outside_text}")
        self.co2_var.set(f"CO2 inde: {co2_text}")
        self.light_var.set(f"Lysintensitet: {light_text}")

    # Opdaterer forsiden, så knappen viser om automatisk styring er aktiv eller stoppet.
    def refresh_control_mode(self):
        if self.app.manual:
            self.stopauto_button.config(
                text="Start Automatisk \n Styring",
                bg="green",
            )
        else:
            self.stopauto_button.config(
                text="Stop Automatisk \n Styring",
                bg="red",
            )


class SettingsPage(Page):
    def __init__(self, parent, app):
        super().__init__(parent, app)

        title = tk.Label(self.frame, text="Settings", font=("Arial", 14, "bold"))
        title.pack()

        # Hele siden
        content_row = tk.Frame(self.frame)
        content_row.pack(fill="both", expand=True, padx=10, pady=10)

        # Change panelet
        change_panel = tk.LabelFrame(content_row, text="Indstil miljø")
        change_panel.pack(side="left", fill="both", expand=True, anchor="n")

        # Midlertidig variabel til temperature input
        self.wanted_temp = tk.StringVar(value="")
        tk.Label(change_panel, text="Ønsket temperatur (C):", anchor="w").pack(fill="x", padx=8, pady=(8, 2))
        tk.Entry(change_panel, textvariable=self.wanted_temp).pack(fill="x", padx=8, pady=(0, 8))

        # CO2 niveau med 4 bullets
        self.wanted_co2 = tk.StringVar(value="Lav")
        tk.Label(change_panel, text="CO2 niveau:", anchor="w").pack(fill="x", padx=8, pady=(2, 2))

        tk.Radiobutton(change_panel, text="Ureguleret", variable=self.wanted_co2, value="Ureguleret").pack(anchor="w", padx=12)
        tk.Radiobutton(change_panel, text="Lav", variable=self.wanted_co2, value="Lav").pack(anchor="w", padx=12)
        tk.Radiobutton(change_panel, text="Mellem", variable=self.wanted_co2, value="Mellem").pack(anchor="w", padx=12)
        tk.Radiobutton(change_panel, text="Høj", variable=self.wanted_co2, value="Høj").pack(anchor="w", padx=12)

        # Spacer så knapperne bliver i bunden af change_panel
        tk.Frame(change_panel).pack(fill="both", expand=True)

        # Knapper
        side_controls = tk.LabelFrame(content_row, text="Manuel styring")
        side_controls.pack(side="left", padx=(10, 0), anchor="n")

        gardin_row = tk.Frame(side_controls)
        gardin_row.pack(side="left", padx=(0, 10))

        vindue_row = tk.Frame(side_controls)
        vindue_row.pack(side="left")

        # Gemmer knapperne som attributter, så de kan disables samlet i 10 sekunder.
        self.gardin_op = tk.Button(
            gardin_row,
            text="Op",
            width=8,
            height=6,
            command=self.handle_open_curtain,
        )
        self.gardin_op.pack()

        tk.Label(gardin_row, text="Gardin").pack(pady=4)

        self.gardin_ned = tk.Button(
            gardin_row,
            text="Ned",
            width=8,
            height=6,
            command=self.handle_close_curtain,
        )
        self.gardin_ned.pack()

        self.vindue_op = tk.Button(
            vindue_row,
            text="Åben",
            width=8,
            height=6,
            command=self.handle_open_window,
        )
        self.vindue_op.pack()

        tk.Label(vindue_row, text="Vindue").pack(pady=4)

        self.vindue_ned = tk.Button(
            vindue_row,
            text="Luk",
            width=8,
            height=6,
            command=self.handle_close_window,
        )
        self.vindue_ned.pack()

        # Samler alle manuelle knapper i en liste, så et tryk kan disable dem alle.
        self.manual_buttons = [
            self.gardin_op,
            self.gardin_ned,
            self.vindue_op,
            self.vindue_ned,
        ]
        self.enable_buttons(self.manual_buttons)

        knap_row = tk.Frame(change_panel)
        knap_row.pack(fill="x", padx=8, pady=(4, 8))

        back_button = tk.Button(
            knap_row,
            text="Tilbage",
            width=15,
            command=self.app.show_home,
        )
        back_button.pack(side="left")

        save_button = tk.Button(
            knap_row,
            text="Gem",
            width=15,
            command=self.save_settings,
        )
        save_button.pack(side="right")

    # læser settings-felterne og sender dem videre til GUI-logikken.
    def save_settings(self):
        self.app.save_desired_values(
            temp_text=self.wanted_temp.get(),
            co2_level=self.wanted_co2.get(),
        )

    # Hjælpefunktion til at disable en gruppe knapper i 10 sekunder.
    def disable_buttons(self, buttons):
        for button in buttons:
            button.config(state=tk.DISABLED)

        self.frame.after(10000, lambda: self.enable_buttons(buttons))

    # Efter cooldown gendannes den normale state ud fra vindue/gardin-status.
    def enable_buttons(self, buttons):
        self.vindue_op.config(state=tk.DISABLED if self.app.window_open else tk.NORMAL)
        self.vindue_ned.config(state=tk.NORMAL if self.app.window_open else tk.DISABLED)
        self.gardin_op.config(state=tk.DISABLED if self.app.curtain_open else tk.NORMAL)
        self.gardin_ned.config(state=tk.NORMAL if self.app.curtain_open else tk.DISABLED)

    # Alle manuelle knapper disables samlet efter tryk på en manuel knap.
    def handle_open_window(self):
        self.app.open_window()
        self.disable_buttons(self.manual_buttons)

    def handle_close_window(self):
        self.app.close_window()
        self.disable_buttons(self.manual_buttons)

    def handle_open_curtain(self):
        self.app.open_curtain()
        self.disable_buttons(self.manual_buttons)

    def handle_close_curtain(self):
        self.app.close_curtain()
        self.disable_buttons(self.manual_buttons)
