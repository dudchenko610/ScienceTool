#pragma once

#include "node/Node.h"
#include "node/Node.cpp"

#include <iostream>


namespace collection
{
	template<typename T>
	class Queue
	{
	public:
		Queue();
		~Queue();
	public:
		void Push(T data);
		T Pop();

		template<typename F>
		void ForEach(F functor)
		{
			queue::Node<T>* current = this->last;
			queue::Node<T>* frst = &this->first;

			while (current != frst)
			{
				functor(current->data);
				current = current->prev;
			}
			
		}

		template<typename F>
		void ForEachRemoving(F functor)
		{
			queue::Node<T>* current = this->last;
			queue::Node<T>* frst = &this->first;
			queue::Node<T>* todel = nullptr;

			while (current != frst)
			{
				functor(current->data);
				todel = current;
				current = current->prev;

				this->last = current;
				this->size--;

				delete todel;
			}

			//this->last = frst;
			//this->size = 0;

		}

		void Clean();

		int GetSize() const;
	private:
		queue::Node<T> first;
		int size;
		queue::Node<T>* last;
	};
}



template<typename T>
collection::Queue<T>::Queue() : size(0), last(nullptr)
{
	std::cout << "Queue Constructor" << std::endl;

	this->first.prev = nullptr;
	this->first.data = 0;

	this->last = &first;
}

template<typename T>
collection::Queue<T>::~Queue()
{
	std::cout << "Queue Destructor" << std::endl;

	queue::Node<T>* node = this->last;
	queue::Node<T>* todel = nullptr;
	queue::Node<T>* frst = &this->first;

	while (node != frst)
	{
		todel = node;
		node = node->prev;
		delete todel;
	}
}

template<typename T>
void collection::Queue<T>::Clean()
{
	queue::Node<T>* node = this->last;
	queue::Node<T>* todel = nullptr;
	queue::Node<T>* frst = &this->first;

	while (node != frst)
	{
		todel = node;
		node = node->prev;
		delete todel;
	}

	this->size = 0;
}

template<typename T>
void collection::Queue<T>::Push(T data)
{

	queue::Node<T>* node = new queue::Node<T>();
	node->data = data;
	node->prev = this->last;

	this->last = node;

	this->size++;
}


template<typename T>
T collection::Queue<T>::Pop()
{
	if (this->size == 0)
	{
		return 0;
	}

	queue::Node<T>* node = this->last;
	this->last = node->prev;

	T data = node->data;
	delete node;

	this->size--;

	return data;
}

template<typename T>
int collection::Queue<T>::GetSize() const
{
	return this->size;
}
