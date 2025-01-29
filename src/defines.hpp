#pragma once

#include <GL/glew.h>

#if defined(__APPLE__)
#include <OpenGL/gl3.h>
#else
#include <GL/gl.h>
#endif

#if defined(linux)
#include <GL/glx.h>
#endif

#define DEC_GL_FUNC(name, returnType, ...)		\
typedef returnType (*PFN##name)(__VA_ARGS__);	\
static PFN##name p##name = nullptr;

#define LOAD_GL_FUNC(name)				\
p##name = (PFN##name)loadFunc(#name);		\
if(!p##name) {					\
    std::cerr << "Failed to load " #name << "\n";   \
    dlclose(g_libGL);					\
    return false;					\
}
