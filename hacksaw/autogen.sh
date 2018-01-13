test -n "$srcdir" || srcdir=`dirname "$0"`
test -n "$srcdir" || srcdir=.

olddir=`pwd`
cd "$srcdir"

mkdir -p m4

PKGCONFIG=`which pkg-config`
if test -z "$PKGCONFIG"; then
        echo "*** pkg-config not found, please install it ***"
        exit 1
fi

# README and INSTALL are required by automake, but may be deleted by clean
# up rules. to get automake to work, simply touch these here, they will be
# regenerated from their corresponding *.in files by ./configure anyway.
touch README INSTALL

AUTORECONF=`which autoreconf`
if test -z $AUTORECONF; then
        echo "*** No autoreconf found, please install it ***"
        exit 1
else
        autoreconf --force --install --verbose || exit $?
fi
