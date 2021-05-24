#pragma once

#include <iostream>

namespace arraylist 
{
	template<typename T>
	class Node
	{
	public:
		Node();
		~Node();
	public:
		Node* next;
		Node* prev;
		T data;
	};
}

