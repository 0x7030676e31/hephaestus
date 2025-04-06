from setuptools import setup, Extension

packageName = "injector"

ext_module = Extension(name=packageName, sources=["injector.c"])
setup(name=packageName, version="0.1", ext_modules=[ext_module])

print("Setup complete!")