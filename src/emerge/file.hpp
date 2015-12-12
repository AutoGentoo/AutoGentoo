#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

/*! \class File
 *
 * class File is meant to create vector of strings to be used
 * during reading files line by line
 *
 */
class File
{
	public:
	
	/// Empty file constructor
	ifstream file;
	
	/** String with whole file stored affecter File () is
	 * called
	 */
	string text;
	
	/** Used during readline () function
	 * Every time this function is run then currentLine will increase by 1
	 */
	unsigned int currentLine;
	
	File ( const char *fileName )
	{
		/// Open the file
		file.open ( fileName );
		
		/// Send text from file to text
		file >> text;
	}
	
	/// Return a vector of strings with each element as a line in the file
	vector<string> readlines ( const char *endChars="" )
	{
		vector < string > returnList;
		for ( string str; getline ( file, str ); )
		{
			/** Add chars to end of str
			 * Usually will be a \n
			 * Append this item to list
			 */
			returnList.push_back ( str + endChars );
		}
		/// Return the generated list
		return returnList;
	}
	
	/** Return a string of the currentLine in file **/
	string readline ( )
	{
		/// Create a buffer to later be returned
		string buffstr;
		
		vector<string> readFile = readlines ( );
		
		if ( currentLine > readFile.size ( ) )
		{
			/// Set buffstr to line
			buffstr = readFile [ currentLine ];
		}
		
		/// Add one to currentLine
		currentLine++;
		
		/// Return the str
		return buffstr;
	}
	
	/** Get a specific line in the file
	 * Will use readlines to create an array and then
	 * get specific line
	*/
	string get ( int line )
	{
		string buffstr; //!< Create a buffer to later be returned
		
		buffstr = readlines ( ) [line]; /// Set buffstr to line
		
		currentLine++; /// Add one to currentLine
		
		/// Return the str
		return buffstr;
	}
};
