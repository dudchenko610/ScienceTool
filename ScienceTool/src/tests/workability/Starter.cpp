#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <thread>
#include <mutex>

#include <string>

/*#include "array-list/ArrayList.h"*/

#include "array-list/thread-safe/ArrayListSafe.h"
#include "queue/Queue.h"

#include "../../core/application/Application.h"

std::mutex mtx;



void error_callback(int error, const char* description)
{
	std::cout << " - error_callback - " << description << std::endl;
}

static void key_callback_(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	std::thread::id this_id = std::this_thread::get_id();

	std::cout << "key_callback_ thread " << this_id << " \n";

	std::cout << " - key_callback_ - Key was pressed " << key << std::endl;

	if (key == GLFW_KEY_0 && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GLFW_TRUE);
}


void differendThreadWin(const char* label)
{

	
	mtx.lock();

	std::thread::id this_id = std::this_thread::get_id();
	std::cout << "differendThreadWin " << this_id << " \n";

	/*A a;

	glfwSetErrorCallback(error_callback);
	glfwSetErrorCallback(&A::mth);*/

/*	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);*/

	GLFWwindow* window = glfwCreateWindow(640, 400, label, NULL, NULL);

	if (!window)
	{
		mtx.unlock();
		return;
	}

	glfwSetKeyCallback(window, key_callback_);

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	mtx.unlock();

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Error while GLEW init!" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;


	while (!glfwWindowShouldClose(window))
	{
		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(1.0f, 0.0f, 0.0f, 0.0f);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);
}

/*
struct Vec3i {
	int x;
	int y;
	int z;
};

std::string delimiter = " ";
std::string delimiterF = "//";

void SplitLine_F_(Vec3i* vec, std::string& line, std::string& frst_smbl)
{
	size_t pos = 0;
	std::string token;

	int* res = (int*)vec;

	line.erase(0, frst_smbl.length() + 1);

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);

		*res = std::stoi(token.substr(0, token.find(delimiterF)));

		line.erase(0, pos + delimiter.length()); // truncates string

		res++;
	}

	*res = std::stoi(line.substr(0, line.find(delimiterF)));

}
*/


class A
{
public:
	A()
	{
		std::cout << "A Constructor " << std::endl;
	}

	virtual ~A()
	{
		std::cout << "A Destructor " << std::endl;
	}

};

class B
{
public:
	B()
	{
		std::cout << "B Constructor " << std::endl;
	}

	virtual ~B()
	{
		std::cout << "B Destructor " << std::endl;
	}

};




template<class T>
class Node_
{
public:
	T data;
};

void func(A a) 
{
	std::cout << "func a size = " << sizeof(a) << std::endl;
}

void func1(A a)
{

}





enum struct Type
{
	NONE,
	SYSTEM_EVENT,
	THREAD_EVENT,

	MOUSE_CLICK_EVENT

};

class IEventData
{
public:
	Type type;
};


class EventData : public IEventData
{
public:
	void Init(int a, char b, short c)
	{
		type = Type::THREAD_EVENT;
		this->a = a;
		this->b = b;
		this->c = c;
	}
public:
	int a;
	char b;
	short c;
};



template<class T>
class Event
{
public:
	template<class ...TParams>
	Event(TParams... params)
	{
		T* ed = (T*) this->data;
		ed->Init(params...);
	}
public:
	T* Get();

private:
	void* data[10];
};

template<typename T>
T* Event<T>::Get()
{
	return (T*)this->data;
}

class C
{
public:
	int a;
};

int main() 
{	
	/*
	Event<EventData> e;

	Event<EventData> e(250, 65, 30000);
	EventData* ed = e.Get();

	std::cout << "ed->type = " << (int) ed->type << std::endl;
	std::cout << "ed->a = " << ed->a << std::endl;
	std::cout << "ed->b = " << ed->b << std::endl;
	std::cout << "ed->c = " << ed->c << std::endl;

	*/

	/*
	C* c = new C();

	c->a = 4;

	delete c;




	int b = 10;

	

	int* a = new int[3];

	a[0] = 1;
	a[1] = 2;
	a[2] = 3;

	C* ptr = (C*) a;

	std::cout << "a[0] = " << *a << std::endl;
	std::cout << "a[1] = " << *(a + 1) << std::endl;

	std::cout << "ptr->a = " << ptr->a << std::endl;

	int k = 0;

	delete ptr;

	int y = 0;

	delete[] a;

	int i = 0;

	std::cout << "a[0] = " << *a << std::endl;


	/*A a;

	std::cout << "a size = " << sizeof(a) << std::endl;

	func(a);

	Node_<A> n;

	std::cout << "n size = " << sizeof(n) << std::endl;*/

	/*A a;

	a.i = 5;

	std::cout << "a size = " << sizeof(a) << std::endl;
	
	void* data[2];

	std::cout << "data [0] = " << data[0] << std::endl;
	std::cout << "data [1] = " << data[1] << std::endl;

	std::cout << "data size = " << sizeof(data) << std::endl;

	memcpy(data, &a, 8);

	std::cout << "data [0] = " << data[0] << std::endl;
	std::cout << "data [1] = " << data[1] << std::endl;*/


	

	if (!glfwInit())
	{
		return -1;
	}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

	glfwWindowHint(GLFW_SAMPLES, 8);


	core::Application::Get().Loop();

	return 0;
	



	



	/*std::cout << B::Get().mth() << std::endl;


	B::Get().i = 7;

	std::cout << B::Get().mth() << std::endl;*/

	/*{

		AA<int> aa;
		int c = 100;
		aa.mth(
			[&c](int i)
			{
				std::cout << "i = " << i + c << std::endl;
			}
		);

		A<int> a;
	}*/

	/*{
		collection::Queue<int> queue;

		queue.Push(1);
		queue.Push(2);
		queue.Push(3);
		queue.Push(4);
		queue.Push(5);

		queue.ForEachRemoving(
			[&queue](int value)
			{
				std::cout << "val rem = " << value << std::endl;
				std::cout << "size = " << queue.GetSize() << std::endl;
			}
		);

		std::cout << "size = " << queue.GetSize() << std::endl;

		queue.ForEach(
			[&queue](int value)
			{
				std::cout << "val jst = " << value << std::endl;
			}
		);

	}*/




	/*	{
		collection::ArrayListSafe<short> als;
		als.Add(-1);
		als.Add(-2);
		als.Add(-3);


		int b = -4;

		als.ForEach(
			[&b] (short value) 
			{
				std::cout << "value safe = " << value << std::endl;
			}
		);


	}

	
	{


		collection::ArrayList<int> arrayList;

		arrayList.Add(4);
		arrayList.Add(5);
		arrayList.Add(6);
		arrayList.Add(7);
		arrayList.Add(8);

		arrayList.RemoveByValue(4);

		arrayList.ForEach(
			[] (int value) 
			{
				std::cout << "value = " << value << std::endl;
			}
		);



	}*/

	/*int** pp = new int*[1];

	*pp = new int;

	**pp = 6;

	std::cout << **pp << std::endl;

	float** pa =  (new float* [1]);

	*pa = new float[6]
	{
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	std::cout << *(*pa + 3) << std::endl;*/

//	
	

/*	std::string line("f 4//4 113//113 5//5");
	std::string f_smbl("f");

	Vec3i result;

	SplitLine_F_(&result, line, f_smbl);

	std::cout << "x = " << result.x << std::endl;
	std::cout << "y = " << result.y << std::endl;
	std::cout << "z = " << result.z << std::endl;*/


	/*struct Vec3f {
		float x;
		float y;
		float z;
	};

	Vec3f result;
	float* res = (float*)&result;

	std::string line("v 0.38268343 0.92387953 0.0000000e+0");
	float num;

	std::string delimiter = " ";

	size_t pos = 0;
	std::string token;

	line.erase(0, 2);

	while ((pos = line.find(delimiter)) != std::string::npos) {
		token = line.substr(0, pos);
		
		*res = std::stof(token);
		num = std::stof(token);
		std::cout << num << std::endl;

		line.erase(0, pos + delimiter.length()); // truncates string

		res++;
	}

	*res = std::stof(line);
	num = std::stof(line);
	std::cout << num << std::endl;

	std::cout << "x = " << result.x << std::endl;
	std::cout << "y = " << result.y << std::endl;
	std::cout << "z = " << result.z << std::endl;


	//token = line.substr(token.find(delimiter) + 1, token.length());
	//std::cout << token << std::endl;

/*	std::string line("v 0.38268343 0.92387953 0.0000000e+0");
	const char* v_str = "v";

	if (line.c_str()[0] == v_str[0])
	{
		std::cout << "true" << std::endl;
	}
	else
	{
		std::cout << "false" << std::endl;
	}*/


	/*
	std::thread::id this_id = std::this_thread::get_id();
	std::cout << "mainThread " << this_id << " \n";

	collection::ArrayList<std::thread*> threads;

	std::thread* th1 = new std::thread(differendThreadWin, "Fwindow1");
	std::thread* th2 = new std::thread(differendThreadWin, "Fwindow2");

	threads.Add(th1);
	threads.Add(th2);

	threads.ForEach(
		[] (std::thread* value)
		{
			if (value->joinable()) {
				value->join();
			}
		}
	);

	delete th1;
	delete th2;*/

    /*	std::thread::id this_id = std::this_thread::get_id();
	std::cout << "mainThread " << this_id << " \n";


	std::thread threads[] = {
		std::thread(differendThreadWin, "Fwindow1"),
		//std::thread(differendThreadWin, "Fwindow2")
	};

	int count = sizeof(threads) / sizeof(std::thread);

	std::cout << "Count: " << count << std::endl;

	for (int i = 0; i < count; i ++) {
		if (threads[i].joinable()) {
			threads[i].join();
		}
	}


/*	const char* name_bbk = "Cherno";
	char name2[7] = { 'C', 'h', 'e', 'r', 'n', 'o', 0 };

	std::cout << name2 << std::endl;

	std::string name = std::string("Cherno") + " hello";
	bool contains = name.find("no") != std::string::npos;
	std::cout << name << std::endl;
	*/

	//std::cout << name;
	//name[2] = 'a';

	
//	glfwTerminate();

	//std::cin.get();

}