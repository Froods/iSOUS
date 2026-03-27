import tkinter as tk

class Page:
    def __init__(self, parent, app):
        self.app = app
        self.frame = tk.Frame(parent, width=320, height=180)

    def show(self):
        self.frame.tkraise()


class HomePage(Page):
    def __init__(self, parent, app):
        super().__init__(parent, app)

        title = tk.Label(self.frame, text="iSOUS", font=("Arial", 14))
        title.pack()

        content_row = tk.Frame(self.frame)
        content_row.pack(fill="x", padx=10, pady=10)

        controls_panel = tk.Frame(content_row)
        controls_panel.pack(side="left", anchor="n")

        settings_button = tk.Button(
            controls_panel,
            text="Indstillinger",
            width=15,
            height=2,
            command=self.app.show_settings
        )
        settings_button.pack(anchor="nw", pady=(0, 10))

        stopauto_button = tk.Button(
            controls_panel,
            text="Stop Automatisk \n Styring",
            width=15,
            height=2,
            bg="green"
            #tilfoj stopauto logik
            #command=
        )
        stopauto_button.pack(anchor="nw")

        #realtime panelet
        realtime_panel = tk.LabelFrame(content_row, text="Realtidsdata")
        realtime_panel.pack(side="left", fill="both", expand=True, padx=(12, 0), anchor="n")

        # Midlertidige variabler til visning af realtime-data
        self.temp_inside_var = tk.StringVar(value="Temperatur inde: 22.5 C")
        self.temp_outside_var = tk.StringVar(value="Temperatur ude: 9.8 C")
        self.co2_var = tk.StringVar(value="CO2 niveau: 640 ppm")
        self.light_var = tk.StringVar(value="Lysintensitet: 410 lux")

        tk.Label(realtime_panel, textvariable=self.temp_inside_var, anchor="w").pack(fill="x", padx=8, pady=(6, 2))
        tk.Label(realtime_panel, textvariable=self.temp_outside_var, anchor="w").pack(fill="x", padx=8, pady=2)
        tk.Label(realtime_panel, textvariable=self.co2_var, anchor="w").pack(fill="x", padx=8, pady=2)
        tk.Label(realtime_panel, textvariable=self.light_var, anchor="w").pack(fill="x", padx=8, pady=(2, 6))


class SettingsPage(Page):
    def __init__(self, parent, app):
        super().__init__(parent, app)

        title = tk.Label(self.frame, text="Settings", font=("Arial", 14))
        title.pack()

        back_button = tk.Button(
            self.frame,
            text="Tilbage",
            width=15,
            command=self.app.show_home
        )
        back_button.pack(anchor="s", padx=10, pady=10)