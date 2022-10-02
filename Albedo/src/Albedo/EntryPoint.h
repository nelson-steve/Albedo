#pragma once

#ifdef Albedo_Platform_Windows

extern Albedo::Application* CreateAppliation();

int main(int argc, char** argv) {

	Albedo::Log::Init();
	Albedo_Core_INFO("spdlog is working!");
	Albedo_Core_WARN("Hello");


	auto app = Albedo::CreateApplication();
	app->Run();
	delete app;
	}

#endif