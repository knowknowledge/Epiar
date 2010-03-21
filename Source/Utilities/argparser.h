/**\file		ArgParser.h
 * \author		Maoserr
 * \date		Created: Saturday, March 20, 2010
 * \date		Modified: Saturday, March 20, 2010
 * \brief		Command line argument parsing routines.
 */

typedef enum{NOTVALID,SHORT,LONG,VALUE,ARG} argType;

class ArgParser {
	public:
		ArgParser( int argc, char **argv );
		void SetOpt(argType type, const string& optname, const string& optdesc);
		bool HaveOpt( const string& arg );
		bool HaveShort( const string& arg );
		bool HaveLong( const string& arg );
		string HaveValue( const string& arg );
		bool HaveArg( const string& arg );
		void PrintUsage( void );
		list<string> GetUnused( void );

	private:
		void Parse( int argc, char **argv );
		argType CheckArg( const string& anarg );

		map<string,string> validopt;			// Valid options
		map<string,argType> validopttype;		// Types for valid options
		map<string,bool> opts;					// Short options (I.E. -s)
		map<string,bool> longopts;				// Long options (I.E. --hello)
		map<string,string> valueopts;			// Option with values (I.E. a=1)
		map<string,bool> args;					// Unprocessed args (those without "-" prefix)
};
