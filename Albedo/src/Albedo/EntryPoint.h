#pragma once

#ifdef Albedo_Platform_Windows

extern Albedo::Application* CreateAppliation();

int main() {
	auto app = Albedo::CreateApplication();
	app->Run();
	delete app;
	}

#endif