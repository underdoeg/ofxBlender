#include "Utils.h"

#include <algorithm>
#include <iostream>

namespace ofx{
namespace blender{

	// trim from start
	std::string &ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	// trim from end
	std::string &rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	// trim from both ends
	std::string &trim(std::string &s) {
		return ltrim(rtrim(s));
	}

}
}