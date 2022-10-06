#pragma once

#ifdef Albedo_Platform_Windows
	#ifdef Albedo_Build_DLL
		#define Albedo_API __declspec(dllexport)
	#else
		#define Albedo_API __declspec(dllimport)
	#endif
#else
	#error Only supports Windows

#endif

#define BIT(x) (1 << x)