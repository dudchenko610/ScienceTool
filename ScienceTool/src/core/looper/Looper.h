#pragma once

#include "queue/thread-safe/QueueSafe.h"
#include "event/Event.h"

#include <iostream>

namespace core 
{
	class Looper
	{
	public:
		Looper(const Looper&) = delete;
	public:
		Looper();
	public:
		template<typename Func>
		void Loop(Func functor)
		{
			while (this->isRunning)
			{

				std::this_thread::sleep_for(std::chrono::milliseconds(50));

				events.ForEachRemoving(
					[&functor](Event* event)
					{
						functor(event);

						std::cout << "Cycle" << std::endl;

						delete event;
					}
				);
			}
		}
		void PushEvent(Event*);
		void Stop();
	private:
		collection::QueueSafe<Event*> events;
		bool isRunning;
	};
}



