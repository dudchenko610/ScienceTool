#pragma once

#include<iostream>

namespace core
{
	struct Event
	{
	public:
		Event() 
		{
			std::cout << "Event Constructor " << std::endl;
		}
		~Event()
		{
			std::cout << "Event Destructor " << std::endl;
		}

		int code;
		void* data;
	};

}