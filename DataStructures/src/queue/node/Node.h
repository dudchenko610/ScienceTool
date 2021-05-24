#pragma once



namespace queue
{
	template<typename T>
	class Node
	{
	public:
		Node();
		~Node();
	public:
		Node* prev;
		T data;
	};
}



