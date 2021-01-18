# CPortage module

The goal of this module is to perform all of the I/O intensive operations from
a C-context. This will the compiler the ability to optimize and for the programmer
to be able to reduce performance impact of high frequency dynamic memory allocation.


## Module Layout
All backend objects are written in C and can be found in their
respective `[object].c`. Their header files will include `PyObject` structure
definitions.

All attributes of C objects are forced to be `READONLY`. If attributes need
to be updated, they should be changed from a method. Attempting to change
attributes from Python will raise a `AttributeError`.

### Portage language
Portage ebuild rely on a special syntax for their dependency expressions.
A [neoast](https://github.com/AutoGentoo/neoast) module is included to parse
these expressions and generate a  Python friendly object. `Dependency` (`depend.y`)
and `RequiredUse` (`required_use.y`) parsers are Python dependent as they use CPython
functions to allocate and initialize their data. This means the parser cannot
be stripped from the rest of the module without running into linking issues.

Because there are multiple different types of expressions used in portage,
a pseudo token will be appended to the input string during parsing to support
multiple expression types. Supported expressions follow:
  - Dependency expression: `use? ( category/pkg-name-3.4.1[-useflag,+useflag2] ... )`
  - Required use: `use? ( select1 select2 ) || ( or1 or2 )`
  - Portage atoms are parsed using the dependency expression parser. When
    `atom_parse()` is used, the atom is extracted from the expression

### I/O
> TODO

Initializing ebuild metadata is one of the slowest tasks of Gentoo's portage.
This is because Python creates a massive overhead for file reading and parsing.
Also because of Python's Global Interpreter Lock (GIL) portage cannot make use
of multiple threads.

Ebuild metadata I/O is completely encapsulated inside the `cportage` module.
A ebuild will only read its own metadata during resolve-time. This means
that after initial repository initialization, the only information we have
about an ebuild is its identification (category, name etc.) and its version.

Metadata parsing is non-threadblocking meaning Python GIL will be released
during this time. This will allow Python to setup multiple threads to perform
other tasks during I/O.

### Constants
There are a number of enums defined `portage.h`. These enums are also defined
in the `cportage` python module as `enum.IntEnum` or `enum.IntFlag` as needed.

These definitions can be found in `__init__.py`. Portage Python objects are
wrapped in `__init__.py` to convert integers used in C to their Python enum
equivelents.

## Dependency resolution
> TODO