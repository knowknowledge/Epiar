#include "includes.h"
#include "Utilities/log.h"

class ArgParser {
	public:
		/**\brief Call this function to parse arguments.*/
		ArgParser( int argc, char **argv ){
			this->Parse( argc, argv );
		}
		
		/**\brief Set's a valid option and it's description.*/
		void SetOpt(const string& optname, const string& optdesc){
			validopt[optname]=optdesc;
		}

		/**\brief Checks to see if either short or long option exists.*/
		bool HaveOpt( const string& arg ){
			bool result1 = this->HaveShort(arg);
			bool result2 = this->HaveLong(arg);
			if ( result1 || result2 )
				return true;
			else
				return false;
		}

		/**\brief Checks to see if short option exists.*/
		bool HaveShort( const string& arg ){
			if( this->validopt.count( arg ) == 0 ){
				Log::Error("You tried to access an invalid option, please use SetOpt to define it first!");
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
		bool HaveLong( const string& arg ){
			if( this->validopt.count( arg ) == 0 ){
				Log::Error("You tried to access an invalid option, please use SetOpt to define it first!");
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
		string HaveValue( const string& arg ){
			if( this->validopt.count( arg ) == 0 ){
				Log::Error("You tried to access an invalid option, please use SetOpt to define it first!");
				return false;
			}
			if( this->args.count( arg ) > 0 ){
				string optval = this->valueopts[arg];
				// Set it to false so we know we've checked it.
				this->valueopts[arg].clear();
				return optval;
			}
			return string();
		}

		
		/**\brief Checks to see if argument exists.*/
		bool HaveArg( const string& arg ){
			if( this->validopt.count( arg ) == 0 ){
				Log::Error("You tried to access an invalid option, please use SetOpt to define it first!");
				return false;
			}
 			if( this->args.count( arg ) > 0 ){
				// Set it to false so we know we've checked it.
				this->args[arg] = false;
				return true;
			}
			return false;
		}

		/**\brief Prints a usage message dynamically generated.*/
		void PrintUsage( void ){
			map<string,string>::iterator it;
			cout<<"---------------------------------------------"<<endl;
			cout<<"Epiar Options:"<<endl;
			for ( it=validopt.begin(); it != validopt.end(); it++ ){
				/// \todo Make this distinguish between short, long, etc
				cout<< "\t--"<< setw(20)<<left<<(*it).first << "- "<<(*it).second<<endl;
			}
			cout<<"---------------------------------------------"<<endl;
		}

		/**\brief Returns unused options.*/
		list<string> GetUnused( void ){
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
				if ( (*itv).second.empty() )
					unused.push_back((*itv).first);
			}
			
			for ( ita=args.begin(); ita != args.end(); ita++ ){
				if ( (*ita).second == true )
					unused.push_back((*ita).first);
			}
			return unused;
		}
		
	private:
		typedef enum{NOTVALID,SHORT,LONG,VALUE,ARG} argType;
		map<string,string> validopt;			// Valid options
		map<string,bool> opts;					// Short options (I.E. -s)
		map<string,bool> longopts;				// Long options (I.E. --hello)
		map<string,string> valueopts;			// Option with values (I.E. a=1)
		map<string,bool> args;					// Unprocessed args (those without "-" prefix)

		/**\brief Parses the args and fills in the data.*/
		void Parse( int argc, char **argv ){
			string currarg;						// Current options
			for( int i=1; i < argc; i ++ ){
				currarg = argv[i];
				argType type = this->CheckArg( currarg );
				switch (type){
					case NOTVALID:
						cout<<"Invalid option: "<<currarg<<endl;
						break;
					case SHORT:
						this->opts[currarg.substr(1)] = true;
						cout<<"Found short option: "<<currarg.substr(1)<<endl;
						break;
					case LONG:
						this->longopts[currarg.substr(2)] = true;
						cout<<"Found long option: "<<currarg.substr(2)<<endl;
						break;
					case VALUE:{
						size_t splitpos = currarg.find( '=' );
						string valuearg = currarg.substr(2,splitpos-2);
						string valueval;
						if ( splitpos != currarg.npos )
							valueval = currarg.substr(splitpos+1);
						else
							valueval = "";
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
						Log::Error("An error occurred processing argument: %s",
							currarg.c_str());
				}
			}
		}

		/**\brief Checks that a single arg is valid and return it's type.*/
		argType CheckArg( const string& anarg ){
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
							return VALUE;
						else
							return LONG;

					}else{
					// It's '--' hmmm, we'll just say it's an ARG
						return ARG;
					}
				}else
					return SHORT;
			}else
			// Just an arg
				return ARG;
		}
};
