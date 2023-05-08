#pragma once

#include "mono/jit/jit.h"
#include "mono/metadata/assembly.h"
#include "mono/metadata/object.h"

namespace Albedo {

	class ScriptEngine
	{
	public:
		static void Init();
		static void Shutdown();
	private:
		static void InitMono();
		static void ShutdownMono();
		static char* ReadBytes(const std::string& filepath, uint32_t* outSize);
		static MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
		static void PrintAssemblyTypes(MonoAssembly* assembly);
	};

}