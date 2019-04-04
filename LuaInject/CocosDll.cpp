#include "CocosDll.h"
#include "DebugHelp.h"
#include "Hook.h"
#include "DebugBackend.h"

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void *)0)
#endif
#endif

class CCCDirector;
 


cocos_drawScene_t                cocos_drawScene_dll = NULL;
cocos_pause_t                    cocos_pause_dll = NULL;
cocos_resume_t                   cocos_resume_dll = NULL;
cocos_glDrawArrays_t             cocos_glDrawArrays_dll = NULL;
cocos_glDrawElements_t           cocos_glDrawElements_dll = NULL;
CCCDirector*                        director = NULL;
int		cocosCurDrawCallIndex = 0;
int		cocosCurDrawCallSetIndex = 0;
int		cocosCurDrawCallMax   = 0;
bool    cocosGamePause = false;

/**
* This macro outputs the prolog code for a naked intercept function. It
* should be the first code in the function.
*/
#define INTERCEPT_PROLOG()                          \
	__asm                                           \
    {                                               \
        __asm push    ebp                           \
        __asm mov     ebp,            esp           \
        __asm sub     esp,            __LOCAL_SIZE  \
    }

/**
* This macro outputs the epilog code for a naked intercept function. It
* should be the last code in the function. argsSize is the number of
* bytes for the arguments to the function (not including the the api parameter).
* The return from the function should be stored in the "result" variable, and
* the "stdcall" bool variable determines if the function was called using the
* stdcall or cdecl calling convention.
*/
#define INTERCEPT_EPILOG(argsSize)                  \
    __asm                                           \
    {                                               \
        __asm mov     eax,            result        \
        __asm cmp     stdcall,        0             \
        __asm mov     esp,            ebp           \
        __asm pop     ebp                           \
        __asm jne     stdcall_ret                   \
        __asm ret     4                             \
    __asm stdcall_ret:                              \
        __asm ret     (4 + argsSize)                \
    }

/**
* This macro outputs the epilog code for a naked intercept function that doesn't
* have a return value. It should be the last code in the function. argsSize is the
* number of  bytes for the arguments to the function (not including the the api
* parameter). The "stdcall" bool variable determines if the function was called using
* the stdcall or cdecl calling convention.
*/
#define INTERCEPT_EPILOG_NO_RETURN(argsSize)        \
    __asm                                           \
    {                                               \
        __asm cmp     stdcall,        0             \
        __asm mov     esp,            ebp           \
        __asm pop     ebp                           \
        __asm jne     stdcall_ret                   \
        __asm ret     4                             \
    __asm stdcall_ret:                              \
        __asm ret     (4 + argsSize)                \
    }



#define COCOS_INTERCEPT_PROLOG()					         \
			__asm {  /* prolog */						     \
			__asm push        ebp							 \
			__asm mov         ebp, esp						 \
			__asm push        ecx							 \
			__asm mov         dword ptr[_this], 0CCCCCCCCh	 \
			__asm mov         dword ptr[_this], ecx			 \
			__asm mov ecx, dword ptr[_this]					 \
		}


#define COCOS_INTERCEPT_EPILOG()				      \
		__asm {  /* epilog */				          \
			__asm	mov         esp, ebp			  \
			__asm	pop         ebp					  \
			__asm	ret 							  \
		}

#define CALL_COCOS_FUNC(classPtr,funAddr) \
	if (classPtr)										\
	{													  \
		_asm   											 \
		{												  \
			__asm push eax                   /*保护eax */ 		\
			__asm push ecx									  \
			__asm mov eax, classPtr							  \
			__asm mov ecx, classPtr							  \
			__asm call funAddr 						  \
			__asm pop ecx                    /*还原ecx */ 		\
			__asm pop eax                  /*  还原eax*/  	\
		}												  \
	}

void __stdcall   cocos_glDrawArrays_intercept(GLenum mode, GLint first, GLsizei count)
{
	  if (!cocosGamePause)
	  {
		 
	      cocos_glDrawArrays_dll(mode, first, count);
		  cocosCurDrawCallMax++;
	  }
	  else
	  {
		  if (cocosCurDrawCallIndex <= cocosCurDrawCallSetIndex && cocosCurDrawCallIndex <= cocosCurDrawCallMax)
		  {
			  cocos_glDrawArrays_dll(mode, first, count);
			  cocosCurDrawCallIndex++;
		  }
	  }
}

void __stdcall  cocos_glDrawElements_intercept(GLenum mode, GLsizei count, GLenum type, const GLvoid *indices)
{
	  if (!cocosGamePause)
	  {
		
		 cocos_glDrawElements_dll(mode, count, type, indices);
		 cocosCurDrawCallMax++;
	  }
	  else
	  {
		  if (cocosCurDrawCallIndex <= cocosCurDrawCallSetIndex && cocosCurDrawCallIndex <= cocosCurDrawCallMax)
		  {
			  cocos_glDrawElements_dll(mode, count, type, indices);
			  cocosCurDrawCallIndex++;
		  }
	  }
}

void tryDescodeClassname(CCCDirector* ptr)
{
	try {
		std::string name = (typeid ( ptr)).name();
		std::string name1 = name;
	}
	catch (std::exception e)
	{

	}

}

__declspec(naked) void cocos_drawScene_intercept()
{
	void* _this;
	
	cocosCurDrawCallIndex = 0;
	if (!cocosGamePause)
		cocosCurDrawCallMax=0;
	COCOS_INTERCEPT_PROLOG()
		if (!director)
		{
			director = (CCCDirector*)_this;

		}
		__asm {
		call cocos_drawScene_dll;
			}
		tryDescodeClassname(director);

	if (!cocosGamePause)
		cocosCurDrawCallIndex=cocosCurDrawCallMax;
	 DebugBackend::Get().cocosDrawSceneTriggle();
	COCOS_INTERCEPT_EPILOG()
}


__declspec(naked) void cocos_pause_intercept()
{
	void* _this;
	
	COCOS_INTERCEPT_PROLOG()
		__asm {
		call cocos_pause_dll;
	}
	COCOS_INTERCEPT_EPILOG()
}

__declspec(naked) void cocos_resume_intercept()
{
	void* _this;
	
	COCOS_INTERCEPT_PROLOG()
		__asm {
		call cocos_resume_dll;
	}
	COCOS_INTERCEPT_EPILOG()
}

  void cocos_cocos_pause_stdcall()
{
	  cocosGamePause = true;
	  CALL_COCOS_FUNC(director, cocos_pause_dll);
}

 void cocos_cocos_resume_stdcall()
{
	 cocosGamePause = false;
	 CALL_COCOS_FUNC(director, cocos_resume_dll);
}


void InstallCocosHooker()
{
	static bool hookDone = false;
	if (hookDone)
		return;
#define GET_COCOS_FUNCTION(function,model,predllStr,dllStr)                                                                     \
        {																														\
          	cocos_##function##_t func_impl = (cocos_##function##_t)GetProcAddress(hModuleCocos, ""##predllStr#function##dllStr);\
			if (func_impl)																										\
			{																													\
				cocos_##function##_dll = (cocos_##function##_t)HookFunction(func_impl, cocos_##function##_intercept);			\
																																\
			}                                                                                                                   \
        }

#define GET_GL_FUNCTION(function,model)                                                                                 \
        {																															\
          	cocos_##function##_t func_impl = (cocos_##function##_t)GetProcAddress(model,""#function);\
			if (func_impl == NULL)																								\
			{																														\
				func_impl = (cocos_##function##_t)GetProcAddress(model,""#function);			\
			}																														\
			if (func_impl)																										\
			{																														\
				cocos_##function##_dll = (cocos_##function##_t)HookFunction(func_impl, cocos_##function##_intercept);						\
																																\
			}                                                                                                                    	\
        }
	HMODULE hModuleCocos = GetModuleHandle("libcocos2d.dll");
	if (hModuleCocos != NULL)
	{
		GET_COCOS_FUNCTION(drawScene, hModuleCocos,"?","@Director@cocos2d@@QAEXXZ");
		GET_COCOS_FUNCTION(pause, hModuleCocos, "?", "@Director@cocos2d@@QAEXXZ");
		GET_COCOS_FUNCTION(resume, hModuleCocos, "?", "@Director@cocos2d@@QAEXXZ");
	}

	HMODULE hModuleGlew = GetModuleHandle("opengl32.dll");
	if (hModuleGlew != NULL)
	{
		cocos_glDrawArrays_t temp = (cocos_glDrawArrays_t)GetProcAddress(hModuleGlew, "glDrawArrays");
		cocos_glDrawArrays_t temp2 = (cocos_glDrawArrays_t)GetProcAddress(hModuleGlew, "glDrawElements");
		GET_GL_FUNCTION(glDrawArrays, hModuleGlew );
		GET_GL_FUNCTION(glDrawElements, hModuleGlew );
	}
	hookDone = true;
}