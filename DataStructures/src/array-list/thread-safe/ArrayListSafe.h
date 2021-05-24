#pragma once

#include <mutex>
#include <iostream>

#include "../ArrayList.h"


namespace collection
{
	template<typename T>
	class ArrayListSafe : ArrayList<T>
	{
		
	public:
		void Add(T data) 
		{
			mtx.lock();
			collection::ArrayList<T>::Add(data);
			mtx.unlock();
		}

		T   Get(int index) const 
		{
			mtx.lock();
			T data = collection::ArrayList<T>::Get(index);
			mtx.unlock();

			return data;
		}

		T   RemoveByIndex(int index)
		{
			mtx.lock();
			T data = collection::ArrayList<T>::RemoveByIndex(index);
			mtx.unlock();

			return data;
		}

		bool RemoveByValue(T data)
		{
			mtx.lock();
			T data = collection::ArrayList<T>::RemoveByValue(data);
			mtx.unlock();

			return data;
		}

		template<typename F>
		void ForEach(F functor) 
		{
			mtx.lock();
			collection::ArrayList<T>::ForEach(functor);
			mtx.unlock();
		}

		int IndexOf(T data)
		{
			mtx.lock();
			int index = collection::ArrayList<T>::GetIndex(data);
			mtx.unlock();

			return index;
		}

		int  GetSize() const 
		{
			mtx.lock();
			collection::ArrayList<T>::GetSize();
			mtx.unlock();
		}
	private:
		std::mutex mtx;
	};
}