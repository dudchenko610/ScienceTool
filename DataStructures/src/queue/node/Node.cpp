#include "Node.h"

#include <iostream>

template<typename T>
queue::Node<T>::Node()
{
	std::cout << "Queue Node Constructor" << std::endl;
}

template<typename T>
queue::Node<T>::~Node()
{
	std::cout << "Queue Node Destructor" << std::endl;
}
