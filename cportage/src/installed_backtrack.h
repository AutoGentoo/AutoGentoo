//
// Created by atuser on 10/29/19.
//

#ifndef AUTOGENTOO_BACKTRACK_H
#define AUTOGENTOO_BACKTRACK_H

#include <autogentoo/hacksaw/vector.h>
#include "constants.h"
#include "atom.h"

struct __InstalledBacktrack {
	InstalledEbuild* required_by;
	Dependency* selected_by;
};

typedef enum {
	EBUILD_REBUILD_DEPEND = 0x1, //!< Part of DEPEND set
	EBUILD_REBUILD_RDEPEND = 0x2 //!< Part of RDEPEND set
} rebuild_t;

/**
 * A rebuilt request caused by a slot change and '=' operator
 */
struct __RebuildEbuild {
	InstalledEbuild* rebuild;
	Dependency* selector;
	rebuild_t type;
};

/**
 * Resolve all the rebuilds required by installing se
 * @param em parent emerge environment
 * @param se ebuild being installed
 * @param dependency_ordered vector to add selected packages to
 * @param dependency_selected currently selected packages (unordered)
 * @param dependency_blocks package blockers
 */
void installed_backtrack_rebuild(Emerge* em, ResolvedEbuild* se, Vector* dependency_ordered, Vector* dependency_selected,
                                 Vector* dependency_blocks);


/**
 * Generate a new backtracking request
 * @param required_by parent ebuild that requires this package
 * @param selected_by dependency that this is selected by
 * @return new backtracking request
 */
InstalledBacktrack* installed_backtrack_new(InstalledEbuild* required_by, Dependency* selected_by);

/**
 * Free a backtrack
 * @param bt
 */
void installed_backtrack_free(Backtrack* bt);

/**
 * Add this ebuild to the required_by of every dependency
 * @param ebuild to resovle the rdepends for
 */
void installed_backtrack_resolve(PortageDB* db, InstalledEbuild* ebuild);

void installed_backtrack_rebuild_search(PortageDB* db, InstalledEbuild* parent, Dependency* deptree, rebuild_t type);
void installed_backtrack_rebuild_new(PortageDB* db, InstalledEbuild* rebuild, Dependency* dep, rebuild_t type);
void installed_backtrack_rebuild_resolve(PortageDB* db, rebuild_t types);
void installed_backtrack_rebuild_free(RebuildEbuild* rebuild);

#endif //AUTOGENTOO_BACKTRACK_H
