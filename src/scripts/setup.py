from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize

ex = [
	"log",
	"op_string",
	"op_socket",
	"d_malloc",
	"interface",
	"request",
	"worker",
	"job"
]

extensions = []
for x in ex:
	extensions.append(Extension(
		'%s' % x, ["%s.pyx" % x],
		include_dirs=["../../include/"],
		libraries=["autogentoo"],
		library_dirs=["../.libs/"]
	))

setup(
	ext_modules=cythonize(extensions)
)
