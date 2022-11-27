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
		Albedo_PROFILE_FUNCTION();
		glfwMakeContextCurrent(m_WindowHandle);
		gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	}
	void OpenGLContext::SwapBuffers()
	{
		Albedo_PROFILE_FUNCTION();
		glfwSwapBuffers(m_WindowHandle);
	}

}