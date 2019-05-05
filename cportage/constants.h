//
// Created by atuser on 4/23/19.
//

#ifndef AUTOGENTOO_CONSTANTS_H
#define AUTOGENTOO_CONSTANTS_H

#define FILEDIR "${PORTDIR}/${CATEGORY}/${PN}/FILES"
#define WORKDIR "${PORTAGE_BUILDDIR}/WORK"
#define T "${PORTAGE_BUILDDIR}/TEMP"
#define D "${PORTAGE_BUILDDIR}/IMAGE"
#define HOME "${PORTAGE_BUILDDIR}/HOMEDIR"
#define DEFAULT_ROOT "/"
#define DEFAULT_DISTDIR "/usr/portage/distfiles/"

typedef unsigned char* sha_hash;

#endif //AUTOGENTOO_CONSTANTS_H
