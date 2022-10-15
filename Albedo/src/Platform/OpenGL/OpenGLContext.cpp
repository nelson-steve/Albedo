#include "AlbedoPreCompiledHeader.h"

#include "OpenGLContext.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <GL/GL.h>

namespace Albedo {

	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		:m_WindowHandle(windowHandle) {}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(m_WindowHandle);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	}
	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(m_WindowHandle);
	}

}