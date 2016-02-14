#!/usr/bin/env python
# -*- coding: utf-8 -*-
#
#  config.py
#  
#  Copyright 2016 Andrei Tumbar <atuser@Kronos>
#  
#  This program is free software; you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation; either version 2 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
#  MA 02110-1301, USA.
#  
#  



class config:
	def __init__ ( self, CFLAGS, USE, GPU ):
		self.localegen = """# /etc/locale.gen: list all of the locales you want to have on your system
#
# The format of each line:
# <locale> <charmap>
#
# Where <locale> is a locale located in /usr/share/i18n/locales/ and
# where <charmap> is a charmap located in /usr/share/i18n/charmaps/.
#
# All blank lines and lines starting with # are ignored.
#
# For the default list of supported combinations, see the file:
# /usr/share/i18n/SUPPORTED
#
# Whenever glibc is emerged, the locales listed here will be automatically
# rebuilt for you.  After updating this file, you can simply run `locale-gen`
# yourself instead of re-emerging glibc.

en_US ISO-8859-1"
en_US.UTF-8 UTF-8"
ja_JP.EUC-JP EUC-JP"
ja_JP.UTF-8 UTF-8"
ja_JP EUC-JP"
en_HK ISO-8859-1"
en_PH ISO-8859-1"
de_DE ISO-8859-1"
de_DE@euro ISO-8859-15"
es_MX ISO-8859-1"
fa_IR UTF-8"
fr_FR ISO-8859-1"
fr_FR@euro ISO-8859-15"
it_IT ISO-8859-1"""
		self.makeconf = """# These settings were set by the catalyst build script that automatically
# built this stage.
# Please consult /usr/share/portage/config/make.conf.example for a more
# detailed example.
CFLAGS='%s'
CXXFLAGS='${CFLAGS}'
# WARNING: Changing your CHOST is not something that should be done lightly.
# Please consult http://www.gentoo.org/doc/en/change-chost.xml before changing.
CHOST='x86_64-pc-linux-gnu'
# These are the USE flags that were used in addition to what is provided by the
# profile used for building.
USE='%s'
VIDEO_CARDS='%s'
INPUT_DEVICES='evdev synaptics'
PORTDIR='/usr/portage'
DISTDIR='${PORTDIR}/distfiles'
PKGDIR='${PORTDIR}/packages'
PORTDIR_OVERLAY=/usr/local/portage""" % ( CFLAGS, USE, GPU )
