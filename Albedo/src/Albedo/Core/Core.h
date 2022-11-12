#pragma once

#include <memory>

#ifdef Albedo_Platform_Windows
#ifdef Albedo_Dynamic_LINK
	#ifdef Albedo_Build_DLL
		#define Albedo_API __declspec(dllexport)
	#else
		#define Albedo_API __declspec(dllimport)
	#endif
#else
	#define Albedo_API
#endif
#else
	#error Only supports Windows

#endif

#ifdef Albedo_DEBUG
#define Albedo_ENABLE_ASSERTS
#endif

#ifdef Albedo_ENABLE_ASSERTS
#define Albedo_ASSERT(x, ...) { if(!(x)) { Albedo_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#define Albedo_CORE_ASSERT(x, ...) { if(!(x)) { Albedo_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define Albedo_ASSERT(x, ...)
#define Albedo_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)

namespace Albedo {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}