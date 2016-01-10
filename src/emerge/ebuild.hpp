/*
 * ebuild.hpp
 * 
 * Copyright 2015 Andrei Tumbar <atuser@Kronos>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <iostream>
#include <string>
#include <map>
#include <vector>

using namespace std;

class ebuild
{
	string P; //!< The name and version of the package.
	string PN; //!< The name of the package.
	string PV; //!< The version of the package.
	string PR; //!< Contains the revision number or r0 if no revision number exists.
	string PVR; //!< Contains the version number with the revision.
	string PF; //!< Contains the full package name ${PN}-${PVR}.
	string A; //!< Space delimited list of the filenames in SRC_URI. This does not contain the URL paths, just the filename.
	string DISTDIR; //!< Contains the path to the distfiles directory where all the files fetched for a package are stored. Typically, this is /usr/portage/distfiles.
	string FILESDIR; //!< Contains the path to the files sub folder in the package specific location in the portage tree. Do not modify this variable.
	string WORKDIR; //!< Base of the build root for the ebuild. Nothing should be built outside of this directory.
	string S; //!< The source directory for your package; commonly ${WORKDIR}/${P}. Portage will default to this value so you may not have to set it!
	string T; //!< The temporary directory for your package. It is used as a virtual /tmp directory while processing the ebuild.
	string D; //!< The root directory that the package is installed to, treat it as the virtual /.
	string SLOT; //!< Portage handles different versions of the same installed programs. If you would want, say GCC 2.95 and GCC 3.2 installed at the same time, you would specify the SLOT in each ebuild. Here we would set the SLOT of GCC 2.95 to 2 while we would set the SLOT of GCC 3.2 to 3. Note: Using 0 as the SLOT value signifies that this package only has 1 SLOT setting (in other words, this package is not SLOTable).
	string LICENSE; //!< This variable specifies what license the program is covered under, i.e. GPL-2, BSD, etc... This field must be set to a valid license (which is any license found in /usr/portage/license/). If the license does not already exist there, it must be added before the ebuild can be added to the portage tree. If the license does not allow redistribution, make sure you place RESTRICT="nomirror" in the ebuild.
	string KEYWORDS; //!< This variable now supports a couple of different functions. First of all, this variable specifies what architecture the ebuild is meant for. Some example keywords include: x86, ppc, sparc, mips, alpha, arm, hppa, amd64 and ia64. See the profiles/arch.list file in the Portage tree for more details. Obviously, you would set this to reflect the architecture of the target machine. Portage will not allow an x86 machine to build anything but x86, as specified by the KEYWORDS variable. Packages that do not support the native architecture are automatically masked by Portage. If the KEYWORDS flag has a preceding ~, then that indicates that the particular ebuild works, but needs to be tested in several environments before being moved to the stable profile with the given keyword. If the KEYWORDS flag has a preceding -, then the package does not work with the given keyword. If there is nothing leading KEYWORDS, then the package is considered stable. You can allow installation of these different types of packages through the ACCEPT_KEYWORDS variable in make.conf.
	string DESCRIPTION; //!< A short, one line description of your package.
	string SRC_URI; //!< The URLs for every source file in your package, separated by whitespace. You should try not to include version numbers in the SRC_URI and S. Always try to use ${PV} or ${P}, and if a version number is not consistent with the name of the source package, make a ${MY_P} variable and use that instead.
	string HOMEPAGE; //!< The homepage of the package. If you are unable to locate an official one, try to provide a link from freshmeat.net or a similar package tracking site. Never refer to a variable name in the string; include only raw text.
	string IUSE; //!< This is set to whatever USE variables your package utilizes. Remember that KEYWORDS should not be listed in here!
	string DEPEND; //!< The package's build dependencies are listed here. See the section Package Dependencies for more details on proper syntax.
	string RDEPEND; //!< The package's runtime dependencies are listed here. Once again, see Package Dependencies for more details.
	
	map < string, string& > varMap;
	
	ebuild ( Package input )
	{
		configFile = configFile.c_str();
		string prop_raw[] = { P, PN, PV, PR, PVR, PF, A, DISTDIR, FILESDIR, WORKDIR, S, T, D, SLOT, LICENSE, KEYWORDS, DESCRIPTION, SRC_URI, HOMEPAGE, IUSE, DEPEND, RDEPEND };
		string (&prop)[sizeof(prop_raw)/sizeof(string)] = prop_raw;
		
		//ebuild_properties properties ( P, PN, PV, PR, PVR, PF, A, DISTDIR, FILESDIR, WORKDIR, S, T, D, SLOT, LICENSE, KEYWORDS, DESCRIPTION, SRC_URI, HOMEPAGE, IUSE, DEPEND, RDEPEND );
		
	}
};
