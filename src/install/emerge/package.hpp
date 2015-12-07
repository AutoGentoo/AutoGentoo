#include <iostream>
#include <string>
#include "formatString.hpp"

using namespace std;

typedef struct PackageProperties
{
	bool _new;
	bool _slot;
	bool _updating;
	bool _downgrading;
	bool _reinstall;
	bool _replacing;
	bool _fetchman;
	bool _fetchauto;
	bool _interactive;
	bool _blockedman;
	bool _blockedauto;
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
		//[ebuild     U ] dev-lang/mono-4.0.5.1 [2.10.9-r2]
		char x;
		unsigned int i=0;
		while (i < propertystr.length())
		{
			x = propertystr.at(i);
			switch (x)
			{
				case 'N':
					properties._new = true;
				case 'S':
					properties._slot = true;
				case 'U':
					properties._updating = true;
				case 'D':
					properties._downgrading = true;
				case 'r':
					properties._reinstall = true;
				case 'R':
					properties._replacing = true;
				case 'F':
					properties._fetchman = true;
				case 'f':
					properties._fetchauto = true;
				case 'I':
					properties._interactive = true;
				case 'B':
					properties._blockedman = true;
				case 'b':
					properties._blockedauto = true;
			}
			i++;
		}
		string rawpackagestr = packageString.substr(16, packageString.length());
		if (properties._updating)
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
