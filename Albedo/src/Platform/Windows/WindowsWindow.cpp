#include "AlbedoPreCompiledHeader.h"
#include "WindowsWindow.h"
#include "Albedo/Core/Log.h"

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
		Albedo_PROFILE_FUNCTION();
		Init(props);
	}
	WindowsWindow::~WindowsWindow()
	{
		Albedo_PROFILE_FUNCTION();
		Shutdown();
	}
	void WindowsWindow::OnUpdate()
	{
		Albedo_PROFILE_FUNCTION();
		glfwPollEvents();
		m_GraphicsContext->SwapBuffers();
		//glfwSwapBuffers(m_Window);
	}
	void WindowsWindow::SetVSync(bool enabled)
	{
		if (enabled)
			glfwSwapInterval(1);
		else
			glfwSwapInterval(0);

		m_Data.VSync = enabled;
	}
	bool WindowsWindow::IsVSync() const
	{
		return m_Data.VSync;
	}
	void WindowsWindow::Init(const WindowProps& props)
	{
		m_Data.Title  = props.Title;
		m_Data.Width  = props.Width;
		m_Data.Height = props.Height;

		if (!s_GLFWInitialized)
		{
			Albedo_PROFILE_FUNCTION("glfw Init");
			int success = glfwInit();
			//Albedo_CORE_ASSERT(success, "Could not intialize GLFW!");
			//glfwSetErrorCallback();

			//s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

		//Albedo_PROFILE_FUNCTION();
		m_Window = glfwCreateWindow((int)props.Width, (int)props.Height, props.Title.c_str(), nullptr, nullptr);

		m_GraphicsContext = GraphicsContext::Create(m_Window);
		m_GraphicsContext->Init();

		//glfwMakeContextCurrent(m_Window);
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
				Albedo_Core_TRACE("{0}, {1}", width, height);
				data.EventCallBack(event);
			});

		glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window) 
			{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallBack(event);
			Albedo_Core_TRACE("Window Close");
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
		glfwDestroyWindow(m_Window);
	}
}