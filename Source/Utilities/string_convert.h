/**\file			string_convert.h
 * \author			Chris Thielen (chris@luethy.net)
 * \date			Created: Monday, April 21, 2008
 * \date			Modified: Monday, April 21, 2008
 * \brief			Converts a std::string into the template type (such as an int)
 * \details
 * Code learnt at http://www.parashift.com/c++-faq-lite/misc-technical-issues.html#faq-39.2
 */

#ifndef __H_STRING_CONVERT__
#define __H_STRING_CONVERT__

 #include <iostream>
 #include <sstream>
 #include <string>
 #include <typeinfo>
 #include <stdexcept>

template<typename T> inline std::string stringify( const T& x ) {
	std::ostringstream o;

	if (!(o << x)) {
		std::cout << std::string("stringify(") << typeid(x).name() << ")";
		return;
	}
		
	return o.str();
} 

template<typename T> inline void convert(const std::string& s, T& x, bool failIfLeftoverChars = true) {
	std::istringstream i( s );
	char c;

	if( !( i >> x ) || ( failIfLeftoverChars && i.get( c ) ) ) {
		std::cout << s;
		return;
	}
}

template<typename T> inline T convertTo(const std::string& s, bool failIfLeftoverChars = true){
	T x;
	
	convert( s, x, failIfLeftoverChars );
	
	return x;
}

#endif // __H_STRING_CONVERT__
