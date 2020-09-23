#ifndef XPLAT_GL_H
#define XPLAT_GL_H

/**
 * @file xplat_gl.h
 * A cross platform OpenGL include header.
 */

#ifdef _WIN32
#   define WIN32_LEAN_AND_MEAN
#	include <windows.h>
#endif

#define FREEGLUT_STATIC

#if _MSC_VER >= 1400
	// disable the warning for "non-safe" functions
    #pragma warning ( push )
	#pragma warning ( disable : 4996 )
#endif // _MSC_VER >= 1400

#define GL_GLEXT_PROTOTYPES

#if defined(__APPLE__) && defined(__MACH__)
#include <GLUT/glut.h>
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#endif

#include "glext.h"
#include <glui.h>

#undef max
#undef min

// setup YASMIC libraries to be verbose as well
#define YASMIC_USE_VERBOSE

#include <yasmic/verbose_util.hpp>

#if _MSC_VER >= 1400
	// enable the warning for "non-safe" functions
    #pragma warning ( pop )
#endif // _MSC_VER >= 1400


#endif /* XPLAT_GL_H */
