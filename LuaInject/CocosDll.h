#pragma once
#include <windows.h>
#ifndef GLAPI
#  if defined(__MINGW32__) || defined(__CYGWIN__)
#    define GLAPI extern
#  else
#    define GLAPI WINGDIAPI
#  endif
#endif

#ifndef GLAPIENTRY
#define GLAPIENTRY APIENTRY
#endif

typedef unsigned int GLenum;
typedef unsigned int GLbitfield;
typedef unsigned int GLuint;
typedef int GLint;
typedef int GLsizei;
typedef unsigned char GLboolean;
typedef signed char GLbyte;
typedef short GLshort;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned long GLulong;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef void GLvoid;


 

typedef void(*cocos_drawScene_t)				();
typedef void(*cocos_pause_t)					();
typedef void(*cocos_resume_t)				    ();

typedef void  (__stdcall *cocos_glDrawArrays_t)(GLenum mode, GLint first, GLsizei count);
typedef void  (__stdcall *cocos_glDrawElements_t)(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices);
void  cocos_cocos_pause_stdcall();
void  cocos_cocos_resume_stdcall();
void    InstallCocosHooker();