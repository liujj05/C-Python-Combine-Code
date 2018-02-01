from distutils.core import setup, Extension

MOD = 'spam'
setup(name=MOD, ext_modules=[Extension(MOD, sources=['spammodule.c'])])