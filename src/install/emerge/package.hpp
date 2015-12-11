#include <iostream>
#include <string>
#include <map>
#include "formatString.hpp"

using namespace std;

/*! \struct PackageProperties
 * This type is meant to store the boolean information
 * of a Gentoo ebuild/package
 */ 
typedef struct PackageProperties
{
	bool _new; //!< not yet installed
	bool _slot; //!< side-by-side versions
	bool _updating; //!< update to another version
	bool _downgrading; //!< best version seems lower
	bool _reinstall; //!< forced for some reason, possibly due to slot or sub-slot
	bool _replacing; //!< remerging same version
	bool _fetchman; //!< must be manually downloaded
	bool _fetchauto; //!< already downloaded
	bool _interactive; //!< requires user input
	bool _blockedman; //!< unresolved conflict
	bool _blockedauto; //!< automatically resolved conflict
	
	bool createdList; //!< Tells set and the [] operator whether to init the list
	map<string, bool> attrMap;
	
	void createList ( void )
	{
		attrMap["new"] = _new;
		attrMap["slot"] = _slot;
		attrMap["updating"] = _updating;
		attrMap["downgrading"] = _downgrading;
		attrMap["reinstall"] = _reinstall;
		attrMap["replacing"] = _replacing;
		attrMap["fetch_man"] = _fetchman;
		attrMap["fetch_auto"] = _fetchauto;
		attrMap["interactive"] = _interactive;
		attrMap["blocked_man"] = _blockedman;
		attrMap["blocked_auto"] = _blockedauto;
		createdList = true;
	}
	void set ( string in, bool val )
	{
		if ( !createdList )
		{
			createList ( );
		}
		if ( in.length() == 1 )
		{
			char inc = in.at(0);
			switch ( inc )
			{
				case 'N':
					_new = val;
					return;
				case 'S':
					_slot = val;
					return;
				case 'U':
					_updating = val;
					return;
				case 'D':
					_downgrading = val;
					return;
				case 'r':
					_reinstall = val;
					return;
				case 'R':
					_replacing = val;
					return;
				case 'F':
					_fetchman = val;
					return;
				case 'f':
					_fetchauto = val;
					return;
				case 'I':
					_interactive = val;
					return;
				case 'B':
					_blockedman = val;
					return;
				case 'b':
					_blockedauto = val;
					return;
			}
		}
		attrMap[in] = val;
	}
	bool operator [] ( string in )
	{
		if ( !createdList )
		{
			createList ( );
		}
		if ( in.length() == 1 )
		{
			char inc = in.at(0);
			switch ( inc )
			{
				case 'N':
					return _new;
				case 'S':
					return _slot;
				case 'U':
					return _updating;
				case 'D':
					return _downgrading;
				case 'r':
					return _reinstall;
				case 'R':
					return _replacing;
				case 'F':
					return _fetchman;
				case 'f':
					return _fetchauto;
				case 'I':
					return _interactive;
				case 'B':
					return _blockedman;
				case 'b':
					return _blockedauto;
			}
		}
		else { return attrMap[in]; }
	}
}PackageProperties;

class Package
{
	public:
	
	PackageProperties properties;
	string propertystr, packagestr, old;
	
	Package(const char *input)
	{
		string packageString = string(input);
		propertystr = packageString.substr(0, 15);
		strfmt::remove(propertystr, "[ebuild");
		strfmt::remove(propertystr, "]");
		const char *x;
		unsigned int i=0;
		while (i < propertystr.length())
		{
			x = propertystr.substr(i-1, i).c_str();
			properties.set(x, true);
			i++;
		}
		string rawpackagestr = packageString.substr(16, packageString.length());
		if (properties["updating"])
		{
			packagestr = strfmt::getSubStr(rawpackagestr, 0 , "[");
			string rawold = rawpackagestr.substr(packagestr.length(), rawpackagestr.length());
			strfmt::remove(rawold, "[");
			strfmt::remove(rawold, "]");
			old = rawold;
		}
		else
		{
			packagestr = rawpackagestr;
		}
	}
	
	string path()
	{
		string searchPackage = packagestr.substr(packagestr.find("/"), packagestr.length());
		string release = searchPackage.substr(0, strfmt::rfind(searchPackage, "-"));
		string path;
		if (packagestr.at(packagestr.length()-2) == 'r')
		{
			path = packagestr.substr(0, packagestr.find("/")) + release.substr(0, strfmt::rfind(release, "-"));
		}
		else
		{
			string secondHalf = searchPackage.substr(0, strfmt::rfind(searchPackage, "-"));
			path = packagestr.substr(0, packagestr.find("/")) + searchPackage.substr(0, searchPackage.rfind("-"));
		}
		return path;
	}
};
