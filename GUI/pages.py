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

        settings_button = tk.Button(
            self.frame,
            text="Indstillinger",
            width=15,
            height= 2,
            command=self.app.show_settings
        )
        settings_button.pack(anchor="nw", padx=10, pady=10)
        
        stopauto_button = tk.Button(
            self.frame,
            text="Stop Automatisk \n Styring",
            width = 15,
            height = 2,
            bg = "green"
          
#tilføj stopauto logik
#            command=
        )
        stopauto_button.pack(anchor="nw", padx=10, pady=10)

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