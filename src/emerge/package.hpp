#include <iostream>
#include <string>
#include <map>
#include <boost/algorithm/string.hpp>
#include "parse_config.hpp"

using namespace std;
using namespace boost::algorithm;

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
	map<string, string> packageVar;
	vector<string> varNames;
	
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
	void set ( char in, bool val )
	{
		if ( !createdList )
		{
			createList ( );
		}
		switch ( in )
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
		attrMap[string(1, in)] = val;
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
		return attrMap[in];
	}
	void addVar ( string in )
	{
		int findOperator = in.find ( "=" );
		
		string varName = in.substr ( 0, findOperator );
		varNames.push_back ( varName );
		
		string value = in.substr ( findOperator, in.length ( ) );
		packageVar[varName] = value;
	}
}PackageProperties;

class Package
{
	public:
	
	PackageProperties properties;
	string propertystr, packagestr, old;
	map< string, vector<string> > flags;
	map< string, string > flags_str;
	
	Package(const char *input)
	{
		string packageString = string(input);
		propertystr = packageString.substr(0, 15);
		strfmt::remove(propertystr, "[ebuild");
		strfmt::remove(propertystr, "]");
		char x;
		for (unsigned int i=0; i < propertystr.length(); i++)
		{
			x = propertystr.at(i);
			properties.set(x, true);
		}
		string rawpackagestr = packageString.substr(16, packageString.length());
		string packagestrwithval;
		if (properties["updating"])
		{
			packagestrwithval = strfmt::getSubStr(rawpackagestr, 0 , '[');
			string rawold = strfmt::getSubStr(packagestr, 0 , ']');
			strfmt::remove(rawold, "[");
			strfmt::remove(rawold, "]");
			old = rawold;
		}
		else
		{
			packagestrwithval = rawpackagestr;
		}
		packagestrwithval = packagestrwithval.substr(1, packagestrwithval.length());
		int first_space = packagestrwithval.rfind(" ");
		string val_buff = packagestrwithval.substr ( 0, first_space );
		int second_space = val_buff.rfind(" ");
		string values = packagestrwithval.substr(packagestrwithval.find("  "), second_space);
		trim ( values );
		flags = get_variables_split ( values );
		flags_str = get_variables ( values );
		packagestr = packagestrwithval.substr(0, packagestrwithval.find("  "));
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
