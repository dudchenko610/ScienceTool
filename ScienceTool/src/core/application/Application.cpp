#include "Application.h"

#include "stb_image/stb_image.h"

//#include "../../tests/workability/TestWindow.h"

//#include "../../tests/opengl-cookbook/chapter2/diffuse-per-vertex-shading-with-one-light-source/Example_2_1_Window.h"
//#include "../../tests/opengl-cookbook/chapter2/ambiend-diffuse-specular-shading/Example_2_2_Window.h"
//#include "../../tests/opengl-cookbook/chapter2/subroutine/Example_2_3_Window.h"
//#include "../../tests/opengl-cookbook/chapter2/discard-fragments/Example_2_4_Window.h"

#include "../../tests/opengl-cookbook/chapter3/multiple-positional-lights/Example_3_1_Window.h"
//#include "../../tests/opengl-cookbook/chapter3/per-fragment-shading/Example_3_2_Window.h"
//#include "../../tests/opengl-cookbook/chapter3/spotlight/Example_3_3_Window.h"
//#include "../../tests/opengl-cookbook/chapter3/cartoon-effect/Example_3_4_Window.h"
//#include "../../tests/opengl-cookbook/chapter3/fog-effect/Example_3_5_Window.h"

//#include "../../tests/opengl-cookbook/chapter4/2d-texture/Example_4_1_Window.h"
//#include "../../tests/opengl-cookbook/chapter4/multiple-textures/Example_4_2_Window.h"
#include "../../tests/opengl-cookbook/chapter4/normal-maps/Example_4_3_Window.h"
//#include "../../tests/opengl-cookbook/chapter4/cube-map-reflection/Example_4_4_Window.h"
//#include "../../tests/opengl-cookbook/chapter4/cube-map-refraction/Example_4_5_Window.h"
#include "../../tests/opengl-cookbook/chapter4/projected-texture/Example_4_6_Window.h"
//#include "../../tests/opengl-cookbook/chapter4/rendering-to-a-texture/Example_4_7_Window.h"
//#include "../../tests/opengl-cookbook/chapter4/sampler-objects/Example_4_8_Window.h"

//#include "../../tests/opengl-cookbook/chapter5/edge-detection-filter/Example_5_1_Window.h"
//#include "../../tests/opengl-cookbook/chapter5/gaussian-blur-filter/Example_5_2_Window.h"
//#include "../../tests/opengl-cookbook/chapter5/hdr-lighting/Example_5_3_Window.h"
//#include "../../tests/opengl-cookbook/chapter5/bloom-effect/Example_5_4_Window.h"
//#include "../../tests/opengl-cookbook/chapter5/deffered-shading/Example_5_5_Window.h"
#include "../../tests/opengl-cookbook/chapter5/order-independent-tansparency/Example_5_6_Window.h"


#include "../../tests/opengl-cookbook/chapter6/point-sprites-with-gs/Example_6_1_Window.h"
#include "../../tests/opengl-cookbook/chapter6/wireframe-on-top-of-shaded/Example_6_2_Window.h"
#include "../../tests/opengl-cookbook/chapter6/tesselating-a-curve/Example_6_3_Window.h"
#include "../../tests/opengl-cookbook/chapter6/tesselating-a-quad/Example_6_4_Window.h"
#include "../../tests/opengl-cookbook/chapter6/tesselating-a-3d-surface/Example_6_5_Window.h"

#include "../../tests/opengl-cookbook/chapter7/shadow-maps/Example_7_1_Window.h"
#include "../../tests/opengl-cookbook/chapter7/anti-aliasing-with-PCF/Example_7_2_Window.h"
#include "../../tests/opengl-cookbook/chapter7/soft-shadow-edges/Example_7_3_Window.h"

#include "../../tests/opengl-cookbook/chapter9/animating-surface/Example_9_1_Window.h"
#include "../../tests/opengl-cookbook/chapter9/particle-fountain/Example_9_2_Window.h"
#include "../../tests/opengl-cookbook/chapter9/fountain-transform-feedback/Example_9_3_Window.h"

#include "../../tests/opengl-cookbook/chapter10/particle-simulation/Example_10_1_Window.h"
#include "../../tests/opengl-cookbook/chapter10/cloth-simulation/Example_10_2_Window.h"




#include "../../tests/ui/draw-order/DrawOrderWindow.h"




core::Application::Application()
{

	std::cout << "Application Constructor" << std::endl;
	stbi_set_flip_vertically_on_load(1); // flip loading image because of open gl requires image to be flipped


	//Window* baseWnd = (Window*) new core::Example_5_6_Window(1000, 600, "Example_5_6_Window");
	Window* baseWnd = (Window*) new core::Example_6_5_Window(1000, 600);
//	Window* baseWnd = (Window*) new core::DrawOrderWindow(1000, 600);
	this->OpenWindow(baseWnd);
	
}

core::Application::~Application()
{
	std::cout << "Application Destructor" << std::endl;
}

core::Application& core::Application::Get()
{
	std::cout << "Application GET" << std::endl;

	static Application application;
	return application;
}

core::Looper& core::Application::GetLooper()
{
	return this->looper;
}

void core::Application::Loop()
{

	looper.Loop(
		[](Event* event)
		{

			Window* wnd;
			switch (event->code)
			{
			case 0:

				std::cout << "Close window" << std::endl;

				wnd = reinterpret_cast<Window*> (event->data);
				Application::Get().CloseWindow(wnd);


				break;
			case 1:

				std::cout << "Open window" << std::endl;

				wnd = reinterpret_cast<Window*> (event->data);
				Application::Get().OpenWindow(wnd);

				break;
			default:
				break;
			}

			if (Application::Get().windows.GetSize() == 0)
			{
				Application::Get().looper.Stop();
			}

		}
	);
}

void core::Application::OpenWindow(Window* window)
{
	windows.Add(window);

	std::thread thread(&core::Window::Run, window);
	if (thread.joinable())
	{
		thread.detach();
	}

}

void core::Application::CloseWindow(Window* window)
{
	windows.RemoveByValue(window);

	delete window;
}

