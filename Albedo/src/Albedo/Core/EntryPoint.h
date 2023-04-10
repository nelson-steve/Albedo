#pragma once

#ifdef Albedo_Platform_Windows

extern Albedo::Application* CreateAppliation();

int main(int argc, char** argv) {

	Albedo::Log::Init();
	
	Albedo_PROFILE_BEGIN_SESSION("Startup", "AlbedoProfile-Startup.json");
	auto app = Albedo::CreateApplication();	
	Albedo_PROFILE_END_SESSION();

	Albedo_PROFILE_BEGIN_SESSION("Runtime", "AlbedoProfile-Runtime.json");
	app->Run();
	Albedo_PROFILE_END_SESSION();

	Albedo_PROFILE_BEGIN_SESSION("Shutdown", "AlbedoProfile-Shutdown.json");
	delete app;
	Albedo_PROFILE_END_SESSION();

}

#endif