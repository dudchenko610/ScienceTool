#pragma once

#include "../Queue.h"
#include <mutex>

namespace collection 
{
	template<typename T>
	class QueueSafe : Queue<T>
	{
	public:
		void Push(T data)
		{
			mtx.lock();
			Queue<T>::Push(data);
			mtx.unlock();
		}

		T Pop()
		{
			mtx.lock();
			T data = Queue<T>::Pop();
			mtx.unlock();

			return data;
		}

		int GetSize()
		{
			mtx.lock();
			int size = Queue<T>::GetSize();
			mtx.unlock();

			return size;
		}

		template<typename Functor>
		void ForEach(Functor functor)
		{
			mtx.lock();
			Queue<T>::ForEach(functor);
			mtx.unlock();
		}

		template<typename Functor>
		void ForEachRemoving(Functor functor)
		{
			mtx.lock();
			Queue<T>::ForEachRemoving(functor);
			mtx.unlock();
		}

		void Clean()
		{
			mtx.lock();
			Queue<T>::Clean();
			mtx.unlock();
		}


	private:
		std::mutex mtx;
	};
}



