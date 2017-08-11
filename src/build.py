from distutils.core import setup, Extension

libip = Extension('libip',
                    define_macros = [('MAJOR_VERSION', '1'),
                                     ('MINOR_VERSION', '0')],
                    include_dirs = ['../include'],
                    #libraries = ['tcl83'],
                    #library_dirs = ['/usr/local/lib'],
                    sources = ['ip_convert.c'])

setup (name = 'autogentoo',
       version = '5.0',
       ext_modules = [libip])
