#include "AlbedoPreCompiledHeader.h"
#include "WindowsWindow.h"
#include "Albedo/Log.h"

#include "Albedo/Events/ApplicationEvent.h"
#include "Albedo/Events/KeyEvent.h"
#include "Albedo/Events/MouseEvent.h"

#include <glad/glad.h>

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
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		if (!status) Albedo_ERROR("glad not working!");
		glfwSetWindowUserPointer(m_Window, &m_Data);
		SetVSync(true);

		glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Width = width;
				data.Height = height;

				WindowResizeEvent event(width, height);
				data.EventCallBack(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallBack(event);
			});

		glfwSetWindowFocusCallback(m_Window, [](GLFWwindow* window, int focus) 
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
				data.Focus = focus;
				
				switch (focus)
				{
					case 0:
					{
						WindowFocusEvent event(focus);
						data.EventCallBack(event);
					}
					case 1:
					{
						WindowLoseFocusEvent event(focus);
						data.EventCallBack(event);
					}
				}
				
				
			});

		//glfwSetWindowPosCallback(m_Window, [](GLFWwindow* window, int xPos, int yPos)
		//	{
		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		//
		//		WindowPosChangeEvent event;
		//		data.EventCallBack(event);
		//	});

		//glfwSetWindowRefreshCallBack(m_Window, [](GLFWwindow* window)
		//	{
		//		WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
		//
		//		WindowRefreshEvent event;
		//		data.EventCallBack(event);
		//	});

		glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						MouseButtonPressedEvent event(button);
						data.EventCallBack(event);
						break;
					}
					case GLFW_RELEASE:
					{
						MouseButtonReleasedEvent event(button);
						data.EventCallBack(event);
						break;
					}
				}

			});

		glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseScrolledEvent event((float)xOffset, (float)yOffset);
				data.EventCallBack(event);
			});

		glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				MouseMovedEvent event((float)xPos, (float)yPos);
				data.EventCallBack(event);
			});

		glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
			{
				WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

				switch (action)
				{
					case GLFW_PRESS:
					{
						KeyPressedEvent event(key, 0);
						data.EventCallBack(event);
						break;
					}
					case GLFW_RELEASE:
					{
						KeyReleasedEvent event(key);
						data.EventCallBack(event);
						break;
					}
					case GLFW_REPEAT:
					{
						KeyPressedEvent event(key, 1);
						data.EventCallBack(event);
						break;
					}
				}
				
			});
	}
	void WindowsWindow::Shutdown()
	{
	}
}