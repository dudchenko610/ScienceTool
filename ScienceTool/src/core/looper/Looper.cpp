#include "Looper.h"
#include <thread>
#include <chrono>

core::Looper::Looper() : isRunning(true) 
{

}



void core::Looper::PushEvent(Event* event) 
{
	events.Push(event);	

	std::cout << "Event Pushed " << std::endl;
}

void core::Looper::Stop()
{
	this->isRunning = false;
}