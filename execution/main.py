import os

path = os.path.dirname(os.path.realpath(__file__)) + "\\hephaestus.dll"
module = __import__("injector")
module.inject(path)