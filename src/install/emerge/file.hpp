#include <iostream>
#include <fstream>
#include <list>

using namespace std;

class File
{
	public:
	ifstream file;
	string text;
	
	File( const char *fileName )
	{
		file.open ( fileName );
		file >> text;
	}
	
	vector<string> readlines( )
	{
		vector<string> returnList;
		for ( string str; getline ( file, str ); )
		{
			string::size_type findNewLine = str.find ( "\n" );
			if ( findNewLine != string::npos )
			{
				returnList.push_back ( str );
			}
			else
			{
				returnList.push_back ( str + "\n" ) ;
			}
		}
		return returnList;
	}
};
