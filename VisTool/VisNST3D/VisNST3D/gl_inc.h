#pragma once
#ifndef _GL_INC_
#define _GL_INC_


#ifdef _DLL	// dynamic link
	#ifdef _DEBUG
		#pragma comment (lib, "glew32d.lib")
		#pragma comment (lib, "freeglutd.lib")
	#else
		#pragma comment (lib, "glew32.lib")
		#pragma comment (lib, "freeglut.lib")
	#endif
#else		// static link
	#ifdef _DEBUG
		#pragma comment (lib, "glew32sd.lib")
		#pragma comment (lib, "freeglutsd.lib")
		//#pragma comment (linker, "/NODEFAULTLIB:MSVCRTD.lib")
	#else
		#pragma comment (lib, "glew32s.lib")
		#pragma comment (lib, "freegluts.lib")
		//#pragma comment (linker, "/NODEFAULTLIB:MSVCRT.lib")
	#endif
	#define GLEW_STATIC
	#define FREEGLUT_STATIC
#endif

#include "GL/glew.h"
#include "GL/freeglut.h"
#include "glm/glm.hpp"
#include "glm/ext.hpp"


#endif // #ifndef _GL_INC_