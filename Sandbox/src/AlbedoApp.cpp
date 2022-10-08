#include <Albedo.h>

class Sandbox : public Albedo::Application {
public:
	Sandbox()
	{
	}
	~Sandbox()
	{
	}
};

Albedo::Application* Albedo::CreateApplication()
{
	return new Sandbox();
}