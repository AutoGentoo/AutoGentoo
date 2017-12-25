#ifndef __AUTOGENTOO_BINPKG_H__
#define __AUTOGENTOO_BINPKG_H__

#include <stdio.h>
#include <hacksaw/tools.h>

typedef struct __BinaryPackage BinaryPackage;

BUILD_TIME: 1506728470
CPV: app-accessibility/at-spi2-atk-2.22.0
DEFINED_PHASES: compile configure install postinst postrm preinst prepare test
DEPEND: >=app-accessibility/at-spi2-core-2.17.90[abi_x86_64(-)] >=dev-libs/atk-2.15.4[abi_x86_64(-)] >=dev-libs/glib-2.32:2[abi_x86_64(-)] >=sys-apps/dbus-1.5[abi_x86_64(-)] virtual/pkgconfig[abi_x86_64(-)] >=app-portage/elt-patches-20170422 app-arch/xz-utils >=sys-apps/sed-4 dev-util/desktop-file-utils x11-misc/shared-mime-info
EAPI: 6
IUSE: test abi_x86_32 abi_x86_64 abi_x86_x32 abi_mips_n32 abi_mips_n64 abi_mips_o32 abi_ppc_32 abi_ppc_64 abi_s390_32 abi_s390_64 test
KEYWORDS: alpha amd64 arm ~arm64 hppa ia64 ~mips ppc ppc64 ~sh sparc x86 ~amd64-fbsd ~x86-fbsd ~amd64-linux ~arm-linux ~x86-linux ~x64-macos ~x86-macos
LICENSE: LGPL-2+
PROVIDES: x86_64: libatk-bridge-2.0.so.0 libatk-bridge.so
RDEPEND: >=app-accessibility/at-spi2-core-2.17.90[abi_x86_64(-)] >=dev-libs/atk-2.15.4[abi_x86_64(-)] >=dev-libs/glib-2.32:2[abi_x86_64(-)] >=sys-apps/dbus-1.5[abi_x86_64(-)] !<gnome-extra/at-spi-1.32.0-r1
REQUIRES: x86_64: libatk-1.0.so.0 libatspi.so.0 libc.so.6 libdbus-1.so.3 libglib-2.0.so.0 libgmodule-2.0.so.0 libgobject-2.0.so.0 libpthread.so.0
SIZE: 104892
SLOT: 2
USE: abi_x86_64 amd64 elibc_glibc kernel_linux userland_GNU
MTIME: 1511419260
REPO: gentoo

struct __BinaryPackage {
    char* cpv;
    long build_time;
    StringVector* defined_phases;
    
};

#endif