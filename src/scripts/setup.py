from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = [
	Extension(
		'vector', ["vector.pyx"],
		include_dirs=["../../include/"]
	),
	Extension(
		'op_string', ["op_string.pyx"],
		include_dirs=["../../include/"]
	),
]

setup(
	ext_modules=cythonize(extensions)
)
