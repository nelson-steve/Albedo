#pragma once

#include <memory>
#include "Log.h"

#ifdef _MSVC_LANG
	#define _CRT_SECURE_NO_WARNINGS
#endif

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
#define Albedo_CORE_ASSERT(x, ...) { if(!(x)) { Albedo_Core_ERROR("Assertion Failed: {0}", __VA_ARGS__); __debugbreak(); } }
#else
#define Albedo_ASSERT(x, ...)
#define Albedo_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

//#define BIND_EVENT_FN(x) std::bind(&x, this, std::placeholders::_1)
#define BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Albedo {

	template<typename T>
	using Scope = std::unique_ptr<T>;

	template<typename T>
	using Ref = std::shared_ptr<T>;

}

#define BATCH