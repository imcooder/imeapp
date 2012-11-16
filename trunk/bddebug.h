
#ifndef BDDEBUG_H
#define	BDDEBUG_H

#include <crtdbg.h>
#include <assert.h>

#ifndef VERIFY
#ifdef _DEBUG
#define VERIFY(f)          assert((f))//_ASSERT((f))
#else
#define VERIFY(f)          ((void)(f))
#endif    
#endif
#define verify   VERIFY

#ifndef _abs
#define _abs(a) (((a)>0)?(a):(-(a)))
#endif

#ifndef _max
#define _max(a,b) ((a)>(b)?(a):(b))
#endif

#ifndef  _min
#define _min(a,b) ((a)<(b)?(a):(b))
#endif

#define _swap(a,b) {(a)^=(b);(b)^=(a);(a)^=(b);}


#ifndef _countof
#define _countof(_Array) (sizeof(_Array) / sizeof(*_Array))
#endif

#ifndef ZeroMemory 
#define ZeroMemory(pVoid, nLen) memset((pVoid), 0, (nLen))
#endif

//#define SAFE_DELETE(ptr)
#undef SAFE_DELETE
#define SAFE_DELETE(ptr) \
	if ((ptr))\
{ \
	delete (ptr);\
	(ptr) = NULL; \
}

//#define SAFE_DELETE_AR(ptr)
#undef SAFE_DELETE_AR
#define SAFE_DELETE_AR(ptr) \
	if ((ptr)) \
{ \
	delete [] (ptr); \
	(ptr) = NULL; \
}

#undef SAFE_FREE_LIBRARY
#define SAFE_FREE_LIBRARY(hModel) \
	if ((hModel))\
{\
	::FreeLibrary((hModel));\
	(hModel) = NULL;\
}

#undef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(hHandle) \
	if ((hHandle))\
{\
	::CloseHandle((hHandle));\
	(hHandle) = NULL;\
}

#undef SAFE_DELETE_DC
#define SAFE_DELETE_DC(hDC) \
	if ((hDC))\
{\
	::DeleteDC((hDC));\
	(hDC) = NULL;\
} 


#undef SAFE_CLOSE_HANDLE
#define SAFE_CLOSE_HANDLE(hHandle) \
	if ((hHandle))\
{\
	::CloseHandle((hHandle));\
	(hHandle) = NULL;\
} 

#undef SAFE_FREE_LIBRARY
#define SAFE_FREE_LIBRARY(hModel) \
	if ((hModel))\
{\
	::FreeLibrary((hModel));\
	(hModel) = NULL;\
}

#undef SAFE_DELETE_OBJECT
#define SAFE_DELETE_OBJECT(hObj) \
	if ((hObj)) \
{\
	::DeleteObject((hObj));\
	(hObj) = NULL;\
}

#define SAFE_RELEASE_DC					SAFE_RELEASEDC
#undef SAFE_RELEASEDC
#define SAFE_RELEASEDC(hWnd, hDC) \
	if (/*::IsWindow((hWnd)) && */(hDC))\
{\
	::ReleaseDC((hWnd), (hDC));\
	(hDC) = NULL;\
} 

#undef SAFE_REG_CLOSEKEY
#define SAFE_REG_CLOSEKEY(hKey) \
	if ((hKey))\
{\
	::RegCloseKey((hKey));\
	(hKey) = NULL;\
} 



#ifndef DEBUGMSG
#if defined(DEBUG) || defined(_DEBUG)
#define DEBUGMSG		XTrace
#else 
#define DEBUGMSG		__noop
//#define DEBUGMSG(cond, printf_exp) ((void)0)
#endif
#endif




#define RETAILMSG		XTrace

#define MSG_LEVEL_DEBUG					(0x1)
#define MSG_LEVEL_INFO					(0x2)
#define MSG_LEVEL_WARNING				(0x4)
#define MSG_LEVEL_ERROR					(0x8)
#define MSG_LEVEL_CRITICAL			(0x10)

#ifdef __cplusplus
extern "C"
{
#endif	
	void WINAPI XTrace(ULONG_PTR, LPCTSTR, ...);		
#ifdef __cplusplus
}
#endif





#ifndef BD_AUTO_RELEASE_CONCAT_INNER
#define BD_AUTO_RELEASE_CONCAT_INNER(a,b) a##b
#endif
#ifndef BD_AUTO_RELEASE_CONCAT
#define BD_AUTO_RELEASE_CONCAT(a,b) BD_AUTO_RELEASE_CONCAT_INNER(a,b)
#endif
#ifndef BDAR_LINE_NAME
#define BDAR_LINE_NAME(prefix) BD_AUTO_RELEASE_CONCAT(prefix,__LINE__)
#endif
#ifndef ON_LEAVE
#define ON_LEAVE(statement) \
struct BDAR_LINE_NAME(ols_) { \
	~BDAR_LINE_NAME(ols_)() { statement; } \
} BDAR_LINE_NAME(olv_);
#endif
#ifndef ON_LEAVE_1
#define ON_LEAVE_1(statement, type, var) \
struct BDAR_LINE_NAME(ols_) { \
	type var; \
	BDAR_LINE_NAME(ols_)(type v): var(v) {} \
	~BDAR_LINE_NAME(ols_)() { statement; } \
} BDAR_LINE_NAME(olv_)(var);
#endif
#ifndef ON_LEAVE_2
#define ON_LEAVE_2(statement, type1, var1, type2, var2) \
struct BDAR_LINE_NAME(ols_) { \
	type1 var1; type2 var2; \
	BDAR_LINE_NAME(ols_)(type1 v1, type2 v2): var1(v1), var2(v2) {} \
	~BDAR_LINE_NAME(ols_)() { statement; } \
} BDAR_LINE_NAME(olv_)(var1, var2);
#endif
#ifndef ON_LEAVE_3
#define ON_LEAVE_3(statement, type1, var1, type2, var2, type3, var3) \
struct BDAR_LINE_NAME(ols_) { \
	type1 var1; type2 var2; type3 var3; \
	BDAR_LINE_NAME(ols_)(type1 v1, type2 v2, type3 v3): var1(v1), var2(v2), var3(v3) {} \
	~BDAR_LINE_NAME(ols_)() { statement; } \
} BDAR_LINE_NAME(olv_)(var1, var2, var3);
#endif
#ifndef ON_LEAVE_4
#define ON_LEAVE_4(statement, type1, var1, type2, var2, type3, var3, type4, var4) \
struct BDAR_LINE_NAME(ols_) { \
	type1 var1; type2 var2; type3 var3; type4 var4; \
	BDAR_LINE_NAME(ols_)(type1 v1, type2 v2, type3 v3, type4 v4): var1(v1), var2(v2), var3(v3), var4(v4) {} \
	~BDAR_LINE_NAME(ols_)() { statement; } \
} BDAR_LINE_NAME(olv_)(var1, var2, var3, var4);
#endif

///////////////////////////////////////////////////////////////////////////////
//do not delete
#ifndef MEMLEAK_CHECK
#define MEMLEAK_CHECK									\
#ifndef _AFX													\
#ifdef _DEBUG													\
#define new     new(_NORMAL_BLOCK, __FILE__, __LINE__)							\
#endif																\
#else																	\
#define new			DEBUG_NEW							\
#endif																
#endif																

inline void EnableMemLeakCheck()
{
	_CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
}
///////////////////////////////////////////////////////////////////////////////







#endif