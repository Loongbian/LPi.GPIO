from distutils.core import setup, Extension
setup(name = 'LPi', version = '1.0',  \
   ext_modules = [Extension('LPi', ['source/py_gpio.c'])])
