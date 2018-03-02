from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

extensions = [
	Extension(
		'vector', ["vector.pyx"],
		include_dirs=["../../include/"],
		libraries=["autogentoo"],
		library_dirs=["../.libs/"]
	),
	Extension(
		'op_string', ["op_string.pyx"],
		include_dirs=["../../include/"],
		libraries=["autogentoo"],
		library_dirs=["../.libs/"]
	),
	Extension(
		'interface', ["interface.pyx"],
		include_dirs=["../../include/"],
		libraries=["autogentoo"],
		library_dirs=["../.libs/"]
	)
]

setup(
	ext_modules=cythonize(extensions)
)
