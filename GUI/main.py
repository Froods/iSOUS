import tkinter as tk
from tkinter import ttk

class GUI:
    def __init__(self): #__init__() svarer til en contructor i C++
        self.root = tk.Tk()
        self.root.geometry("320x180")
        self.root.title("iSOUS")
    def run(self):
        self.root.mainloop()

class Page: #klassen som laver Frames og skifter frame
    def __init__(self,parent,app):       
        self.app = app
        self.frame = tk.Frame(parent)

    def show(self):
        self.frame.tkraise()

class HomePage(Page): #komposition
    def __init__(self,parent,app):
        super().__init__(parent,app)

        title = tk.label(self.frame, text="iSOUS")
        title.pack(pady=20)



app = GUI()
app.run()