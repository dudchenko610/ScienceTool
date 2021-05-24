#pragma once

#include <string>

#include "../core/vertices/Vertices.h"

namespace parser
{
	namespace obj
	{
		class ObjParser
		{
		public:
			static Vertices* Parse(const std::string& filepath);
			static Vertices* Parse_P_N_T(const std::string& filepath);
		};
	}
}


