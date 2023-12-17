#pragma once

#ifdef Albedo_Platform_Windows

extern Albedo::Application* CreateAppliation();

int main(int argc, char** argv) {

	Albedo::Log::Init();
	
	auto app = Albedo::CreateApplication();	

	app->Run();

	delete app;
}

#endif