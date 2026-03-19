import tkinter as tk
from tkinter import ttk
from pages import HomePage, SettingsPage
#self er objektet som man arbejder på
class GUI:
    def __init__(self):
        self.root = tk.Tk()
        self.root.geometry("320x180")
        self.root.title("iSOUS")

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

    def show_home(self):
        self.home_page.show()

    def show_settings(self):
        self.settings_page.show()

    def run(self):
        self.root.mainloop()

app = GUI()
app.run()