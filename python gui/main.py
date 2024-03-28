import PySimpleGUI as sg
from win32api import GetSystemMetrics
from numpy import genfromtxt


class Map:

    def __init__(self):
        self.mapData = None
        self.spieleranzahl = None
        self.ueberschreibsteine = None
        self.bomben = None
        self.staerke = None
        self.width = None
        self.height = None

    def read_map(self):
        with open('../output/hash.csv', 'r') as infile:
            game_data = infile.readlines()[:4]
            game_data = genfromtxt(game_data, delimiter=',', dtype=int)
        with open('../output/hash.csv', 'r') as infile, open('../output/hash_edited.csv', 'w') as outfile:
            lines = infile.readlines()[4:]
            outfile.writelines(lines)
        self.mapData = genfromtxt('../output/hash_edited.csv', delimiter=',', dtype=int)
        self.spieleranzahl = game_data[0][0]
        self.ueberschreibsteine = game_data[1][0]
        self.bomben = game_data[2][0]
        self.staerke = game_data[2][1]
        self.width = game_data[3][0]
        self.height = game_data[3][1]

    def print_map(self):
        print(self.spieleranzahl)
        print(self.ueberschreibsteine)
        print(self.bomben, " ", self.staerke)
        print(self.width, " ", self.height)
        counter = 1
        for elem in self.mapData:
            print(chr(elem[0]) + ' ', end="")
            if counter % 50 == 0:
                print()
            counter += 1


class Window:

    def __init__(self):
        self.layout = [[sg.Button('Close')]]
        self.windowHeight = GetSystemMetrics(1)
        self.windowWidth = GetSystemMetrics(0)
        self.disLeft = (self.windowWidth - self.windowHeight) / 2
        self.window = sg.Window('Window Title', self.layout).Finalize()
        self.window.Maximize()

    def close(self):
        self.window.close()


my_map = Map()
my_map.read_map()
my_map.print_map()
