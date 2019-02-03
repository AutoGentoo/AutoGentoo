from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
from Cython.Compiler import Options

ex = [
#	"log",
#	"op_string",
#	"op_socket",
	"cython_dynamic_binary",
	"d_malloc",
#	"interface",
#	"request",
#	"crypt",
#	"bignum"
#	"worker",
#	"job"
]

Options.language_level = "3"

extensions = []
for x in ex:
	extensions.append(Extension(
		'%s' % x, ["%s.pyx" % x],
		include_dirs=["../../include/"],
		extra_link_args=["-L../.libs/", "-l:libautogentoo.so", "-L../../hacksaw/src", "-l:libhacksaw.a"],
	))

setup(
	ext_modules=cythonize(extensions)
)
