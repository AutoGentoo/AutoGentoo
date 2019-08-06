from distutils.core import setup
from distutils.extension import Extension
from Cython.Build import cythonize
from Cython.Compiler import Options
from Cython.Distutils import build_ext

ex = [
	"vector",
	"dynamic_binary",
	"d_malloc",
	"request",
	"stream"
]

extensions = [
	Extension(
		"autogentoo_api.%s" % x,
		["${CMAKE_CURRENT_SOURCE_DIR}/autogentoo_api/%s.pyx" % x],
		extra_link_args=["-lautogentoo", "-lhacksaw", "-lssl"],
		include_dirs=["${CMAKE_SOURCE_DIR}/include/"]
	)
	for x in ex]
Options.language_level = "3"

setup(
	name="${PACKAGE_NAME}",
	version="2.01",
	package_dir={'': '${CMAKE_CURRENT_SOURCE_DIR}'},
	ext_modules=cythonize(extensions, compiler_directives={'language_level': "3"}, gdb_debug=True),
	cmdclass={'build_ext': build_ext},
	include_dirs=["."],
	# ext_package="autogentoo"
)
