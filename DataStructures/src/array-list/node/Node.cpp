#include "Node.h"



template<typename T>
arraylist::Node<T>::Node()
{
	std::cout << "ArrayList Node Constructor" << std::endl;
}

template<typename T>
arraylist::Node<T>::~Node()
{
	std::cout << "ArrayList Node Destructor" << std::endl;
}
