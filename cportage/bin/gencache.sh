#!/bin/bash

unalias -a
unset BASH_COMPAT

source ${PORTAGE_BIN_PATH}/ebuild.sh ""

if [ ! -d "$CACHE_DIR" ]; then
  mkdir -p $CACHE_DIR
  mkdir_stat=$?

  if [ ! $mkdir_stat == 0 ]; then
    exit $mkdir_stat
  fi
fi

echo $MD5_HASH > $CACHE_FILE
echo DEFINED_PHASES="${DEFINED_PHASES}" >> $CACHE_FILE

if [ ${#BDEPEND} -gt 0 ]; then
    echo BDEPEND="${BDEPEND//[$'\t\r\n']/ }" >> $CACHE_FILE
fi

if [ ${#DEPEND} -gt 0 ]; then
    echo DEPEND="${DEPEND//[$'\t\r\n']/ }" >> $CACHE_FILE
fi

if [ ${#RDEPEND} -gt 0 ]; then
    echo RDEPEND="${RDEPEND//[$'\t\r\n']/ }" >> $CACHE_FILE
fi

if [ ${#PDEPEND} -gt 0 ]; then
    echo PDEPEND="${PDEPEND//[$'\t\r\n']/ }" >> $CACHE_FILE
fi

echo EAPI="$EAPI" >> $CACHE_FILE
echo IUSE="$IUSE" >> $CACHE_FILE
echo KEYWORDS="$KEYWORDS" >> $CACHE_FILE
echo SLOT="$SLOT" >> $CACHE_FILE
echo SRC_URI="$SRC_URI" >> $CACHE_FILE
