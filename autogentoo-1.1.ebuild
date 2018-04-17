# Copyright 1999-2018 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v2

EAPI=6

DESCRIPTION="A gentoo portage binhost manager"
HOMEPAGE="https://github.com/AutoGentoo/AutoGentoo"
SRC_URI="https://github.com/AutoGentoo/AutoGentoo/archive/v1.0.tar.gz"

LICENSE="GPL-3"
SLOT="0"
KEYWORDS="~amd64 ~x86"

src_configure() {
	econf --with-posix-regex
}

src_install() {
	emake DESTDIR="${D}" install
	dodoc FAQ NEWS README
}
