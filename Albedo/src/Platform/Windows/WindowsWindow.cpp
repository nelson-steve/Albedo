#include "AlbedoPreCompiledHeader.h"
#include "WindowsWindow.h"
#include "Albedo/Log.h"

namespace Albedo {
	static bool s_GLFWInitialized = false;

	Window* Window::Create(const WindowProps props)
	{
		return new WindowsWindow(props);
	}

	//WindowsWindow* WindowsWindow::Create(const WindowProps props) 
	//{
	//	return new WindowsWindow(props);
	//}

	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}
	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}
	void WindowsWindow::OnUpdate()
	{
		Albedo_Core_INFO("Window is running!");
		glfwPollEvents();
		glfwSwapBuffers(m_Window);
	}
	void WindowsWindow::SetVSync(bool enabled)
	{
		m_Data.VSync = enabled;
	}
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Width = props.Width;
		m_Data.Height = props.Height;
		m_Data.Title = props.Title;

		if (!s_GLFWInitialized)
		{
			int success = glfwInit();

			s_GLFWInitialized = true;
		}

		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(m_Window);
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);
	}
	void WindowsWindow::Shutdown()
	{
	}
}