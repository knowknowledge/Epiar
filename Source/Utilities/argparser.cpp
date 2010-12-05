/**\file		argparser.cpp
 * \author		Maoserr
 * \date		Created: Saturday, March 20, 2010
 * \date		Modified: Saturday, March 20, 2010
 * \brief		Command line argument parsing routines.
 */

#include "includes.h"
#include "Utilities/argparser.h"
#include "Utilities/log.h"

/**\class ArgParser
 * \brief The argument parser class, pass argc and argv to the constructor.
 * \details
 * There steps to use this class is as follows:
 * 1. Created an ArgParser instance.
 * 2. Set valid options using SetOpt
 * 3. Check for options using
 *		- HaveOpt - Check for either short or long options
 *		- HaveShort - Check for short options ( Example: -help )
 *		- HaveLong - Check for long options ( Example: --help )
 *		- HaveValue - Check and returns values ( Example: --help=ui )
 *		- HaveArg - Check for non-prefixed options ( Example: help )
 * 4. (Optional) Retrieve unused options via GetUnused and deal with them.
 * 5. (Optional) You can also use PrintUsage to list all valid options.
 *
 * Example:
 * \code
 * #include "ArgParser.h"
 * int main( int argc, char **argv ){
 *		ArgParser argpInst(argc,argv);
 *
 *		// Test for arguments
 *		argpInst.SetOpt(SHORTOPT,"h","Display help screen");
 *		argpInst.SetOpt(LONGOPT,"help","Display help screen");
 *		argpInst.SetOpt(VALUEOPT,"answer","Display the answer to life");
 *
 *		if ( argpInst.HaveShort("h") || argpInst.HaveLong("help") )
 *			argpInst.PrintUsage(); // This prints the list of defined options
 *
 *		string ans=argpInst.HaveValue("answer");
 *		if ( !ans.empty() )
 *			cout<<"The answer to life is set to: "<<ans<<endl;
 *
 *		// Note that we don't have to use SetOpt for arguments without "-"
 *		if ( argpInst.HaveArg("an-argument") )
 *			cout<<"There was an argument"<<endl;
 *
 *		return 0;
 * }
 * \endcode
 */

/**\brief Constructor: Call this function to parse arguments.*/
ArgParser::ArgParser( int argc, char **argv ){
	this->Parse( argc, argv );
}

/**\brief Set's a valid option and it's description.
 * \details
 * Valid types are:
 *		- SHORTOPT
 *		- LONGOPT
 *		- VALUEOPT
 *
 *	\note You don't need to set ARG type as valid.
 */
void ArgParser::SetOpt(argType type, const string& optname, const string& optdesc){
	validopttype[optname]=type;
	validopt[optname]=optdesc;
}

/**\brief Checks to see if either short or long option exists.*/
bool ArgParser::HaveOpt( const string& arg ){
	bool result1 = this->HaveShort(arg);
	bool result2 = this->HaveLong(arg);
	if ( result1 || result2 )
		return true;
	else
		return false;
}

/**\brief Checks to see if short option exists.*/
bool ArgParser::HaveShort( const string& arg ){
	if( this->validopt.count( arg ) == 0 ){
		LogMsg(ERR,"You tried to access an invalid option ('%s'), please use SetOpt to define it first!", arg.c_str());
		return false;
	}
	if( this->validopttype[ arg ] != SHORTOPT ){
		// Option exists, but it's the wrong type
		return false;
	}
	if( this->opts.count( arg ) > 0 ){
		// Set it to false so we know we've checked it.
		this->opts[arg] = false;
		return true;
	}
	return false;
}

/**\brief Checks to see if long option exists.*/
bool ArgParser::HaveLong( const string& arg ){
	if( this->validopt.count( arg ) == 0 ){
		LogMsg(ERR,"You tried to access an invalid option, please use SetOpt to define it first!");
		return false;
	}
	if( this->validopttype[ arg ] != LONGOPT ){
		// Option exists, but it's the wrong type
		return false;
	}
	if( this->longopts.count( arg ) > 0 ){
		// Set it to false so we know we've checked it.
		this->longopts[arg] = false;
		return true;
	}
	return false;
}

/**\brief Checks to see if value exists and return its value.*/
string ArgParser::HaveValue( const string& arg ){
	if( this->validopt.count( arg ) == 0 ){
		LogMsg(ERR,"You tried to access an invalid option, please use SetOpt to define it first!");
		return "";
	}
	if( this->validopttype[ arg ] != VALUEOPT ){
		// Option exists, but it's the wrong type
		return "";
	}

	if( this->valueopts.count( arg ) > 0 ){
		string optval = this->valueopts[arg];
		// Clear it so we know we've checked it.
		this->valueopts[arg].clear();
		return optval;
	}
	return "";
}


/**\brief Checks to see if argument exists.*/
bool ArgParser::HaveArg( const string& arg ){
	if( this->args.count( arg ) > 0 ){
		// Set it to false so we know we've checked it.
		this->args[arg] = false;
		return true;
	}
	return false;
}

/**\brief Prints a usage message dynamically generated.*/
void ArgParser::PrintUsage( void ){
	map<string,string>::iterator it;
	cout<<"---------------------------------------------"<<endl;
	cout<<"Epiar Options:"<<endl;
	for ( it=validopt.begin(); it != validopt.end(); it++ ){
		switch (validopttype[(*it).first]){
			case SHORTOPT:
				cout<< "\t-"<< setw(21)<<left<<(*it).first<< "- "<<(*it).second<<endl;
				break;
			case LONGOPT:
				cout<< "\t--"<< setw(20)<<left<<(*it).first << "- "<<(*it).second<<endl;
				break;
			case VALUEOPT:
				cout<< "\t--"<< setw(20)<<left<<(*it).first+"=<value>" << "- "<<(*it).second<<endl;
				break;
			default:
				break;
		}
	}
	cout<<"---------------------------------------------"<<endl;
}

/**\brief Returns unused options.*/
list<string> ArgParser::GetUnused( void ){
	list<string> unused;
	map<string,bool>::iterator its;
	map<string,bool>::iterator itl;
	map<string,string>::iterator itv;
	map<string,bool>::iterator ita;

	for ( its=opts.begin(); its != opts.end(); its++ ){
		if ( (*its).second == true )
			unused.push_back((*its).first);
	}
	
	for ( itl=longopts.begin(); itl != longopts.end(); itl++ ){
		if ( (*itl).second == true )
			unused.push_back((*itl).first);
	}
	
	for ( itv=valueopts.begin(); itv != valueopts.end(); itv++ ){
		if ( !(*itv).second.empty() )
			unused.push_back((*itv).first);
	}
	
	for ( ita=args.begin(); ita != args.end(); ita++ ){
		if ( (*ita).second == true )
			unused.push_back((*ita).first);
	}
	return unused;
}

/**\brief Parses the args and fills in the data.*/
void ArgParser::Parse( int argc, char **argv ){
	string currarg;						// Current options
	path = argv[0];
	for( int i=1; i < argc; i ++ ){
		currarg = argv[i];
		argType type = this->CheckArg( currarg );
		switch (type){
			case NOTVALID:
				cout<<"Invalid option: "<<currarg<<endl;
				break;
			case SHORTOPT:
				this->opts[currarg.substr(1)] = true;
				cout<<"Found short option: "<<currarg.substr(1)<<endl;
				break;
			case LONGOPT:
				this->longopts[currarg.substr(2)] = true;
				cout<<"Found long option: "<<currarg.substr(2)<<endl;
				break;
			case VALUEOPT:{
				size_t splitpos = currarg.find( '=' );
				string valuearg = currarg.substr(2,splitpos-2);
				string valueval;
				assert ( splitpos != currarg.npos );
				valueval = currarg.substr(splitpos+1);
				if( valueval.empty() )
					valueval = "NONE";
				this->valueopts[valuearg] = valueval;
				cout<<"Found value option: "<<valuearg<<" value:"<<valueval<<endl;
				break;
			}
			case ARG:
				this->args[currarg] = true;
				cout<<"Found argument: "<<currarg<<endl;
				break;
			default:
				// Throw an error
				LogMsg(ERR,"An error occurred processing argument: %s",
					currarg.c_str());
		}
	}
}

/**\brief Checks that a single arg is valid and return it's type.*/
argType ArgParser::CheckArg( const string& anarg ){
	if (anarg.empty())
		return NOTVALID;

	int len = anarg.size();
	if ( len < 1 )
		return NOTVALID;

	// Single character, can't be short or long or value
	if ( len == 1 )
		return ARG;

	if ( anarg[0] == '-' ){
		// It's a negative number, not an option
		if ( isdigit(anarg[1]) )
			return ARG;
		
		// Could be a long/value option
		if ( anarg[1] == '-' ){
			// It is a long or value
			if ( len > 2 ){
				if ( anarg.find('=') != anarg.npos )
					return VALUEOPT;
				else
					return LONGOPT;

			}else{
			// It's '--' hmmm, we'll just say it's an ARG
				return ARG;
			}
		}else
			return SHORTOPT;
	}else
	// Just an arg
		return ARG;
}

