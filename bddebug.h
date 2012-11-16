 
#ifndef BDDEBUG_H
#define	BDDEBUG_H

#include <crtdbg.h>
#include <assert.h>
#include <malloc.h>
#include <tchar.h>
#include <Strsafe.h>
#include <wctype.h>
#ifndef VERIFY
#ifdef _DEBUG
#define VERIFY(f)          assert((f))//_ASSERT((f))
#else
#define VERIFY(f)          ((void)(f))
#endif    
#endif
#define verify   VERIFY
#ifndef ASSERT
#define ASSERT							assert
#endif

#ifndef ASSERT
#define ASSERT assert
#endif


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

#ifndef MAKEULONG
#define MAKEULONG(a, b)      ((ULONG)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif

#ifndef MAKEDWORD
#define MAKEDWORD(a, b)      ((DWORD)(((WORD)(((DWORD_PTR)(a)) & 0xffff)) | ((DWORD)((WORD)(((DWORD_PTR)(b)) & 0xffff))) << 16))
#endif


#ifndef GetAValue
#define GetAValue(rgba)      (LOBYTE((rgba)>>24))
#endif
#ifndef RGBA
#define RGBA(r, g, b, a)     ((COLORREF)((((BYTE)(r)|((WORD)((BYTE)(g))<<8))|(((DWORD)(BYTE)(b))<<16))| (((DWORD)(BYTE)(a))<<24)))
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

#undef SAFE_DELETE_HMENU
#define SAFE_DELETE_HMENU(hMenu) \
	if ((hMenu)) \
{\
	::DestroyMenu((hMenu));\
	(hMenu) = NULL;\
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

#undef SAFE_CLOSEFILE
#define SAFE_CLOSEFILE(pFile) \
	if ((pFile))\
{\
	fclose((pFile));\
	(pFile) = NULL;\
} 

#undef SAFE_RELEASE
#define SAFE_RELEASE(punk) \
	if ((punk))\
{\
	(punk)->Release();\
	(punk) = NULL;\
} 

#define MSG_LEVEL_DEBUG					(0x10)
#define MSG_LEVEL_INFO					(0x20)
#define MSG_LEVEL_WARNING				(0x30)
#define MSG_LEVEL_ERROR					(0x40)
#define MSG_LEVEL_CRITICAL			(0x50)

#ifndef DEBUGMSG
#if defined(DEBUG) || defined(_DEBUG)
#define DEBUGMSG		CDBG::GetInstance().Trace
#else 
#define DEBUGMSG		__noop
//#define DEBUGMSG(cond, printf_exp) ((void)0)
#endif
#endif

#ifndef TRACE
#define TRACE(...)	DEBUGMSG(MSG_LEVEL_DEBUG, __VA_ARGS__)
#endif

#define RETAILMSG		CDBG::GetInstance().Trace


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

#ifndef MULTI_THREAD_GUARD
#define MULTI_THREAD_GUARD(cs) \
	EnterCriticalSection(&cs); \
	ON_LEAVE_1(LeaveCriticalSection(&cs), CRITICAL_SECTION&, cs)
#endif
///////////////////////////////////////////////////////////////////////////////
class CDBG
{	
public:
	~CDBG()
	{
		::DeleteCriticalSection(&m_CS);
	}
private:
	CDBG()
		: m_uFilter((ULONG_PTR)-1)
		, m_bJsonData(true)
	{
		::InitializeCriticalSection(&m_CS);
		TCHAR szProcess[MAX_PATH] = {0};
		GetModuleFileName(NULL, szProcess, _countof(szProcess));
		LPCTSTR pszName = _tcsrchr(szProcess, _T('\\'));
		if (!pszName)
		{
			pszName = szProcess;
		}
		else
		{
			pszName ++;
		}
		StringCchCopy(m_szPName, _countof(m_szPName), pszName);
		m_dwPID = GetCurrentProcessId();
	}
	static bool isControlCharacter(TCHAR ch)
	{
		return ch > 0 && ch <= 0x1F;
	}

	static bool containsControlCharacter( const TCHAR* str )
	{
		while ( *str ) 
		{
			if ( isControlCharacter( *(str++) ) )
				return true;
		}
		return false;
	}
	static bool valueToQuotedString( const TCHAR *value, TCHAR* pszOut, int nlen )
	{
		// Not sure how to handle unicode...
		if (pszOut == NULL || nlen < 10)
		{
			return false;
		}
		if (_tcspbrk(value, _T("\"\\\b\f\n\r\t")) == NULL && !containsControlCharacter( value ))
		{			
			StringCchCopy(pszOut, nlen, _T("\""));
			StringCchCat(pszOut, nlen - 2, value);
			StringCchCat(pszOut, nlen, _T("\""));
			return true;
		}
		StringCchCat(pszOut, nlen, _T("\""));
		for (const TCHAR* c = value; *c != 0; ++c)
		{
			TCHAR sz[2] = { *c, 0};
			switch(*c)
			{
			case '\"':
				StringCchCat(pszOut, nlen - 2, _T("\\\""));
				break;
			case '\\':				
				StringCchCat(pszOut, nlen - 2, _T("\\\\"));
				break;
			case '\b':				
				StringCchCat(pszOut, nlen - 2, _T("\\b"));
				break;
			case '\f':				
				StringCchCat(pszOut, nlen - 2, _T("\\f"));
				break;
			case '\n':			
				StringCchCat(pszOut, nlen - 2, _T("\\n"));
				break;
			case '\r':
				StringCchCat(pszOut, nlen - 2, _T("\\r"));				
				break;
			case '\t':
				StringCchCat(pszOut, nlen - 2, _T("\\t"));
				break;				
			default:
				if ( isControlCharacter( *c ) )
				{
					TCHAR szTmp[16] = {0};					
					StringCchPrintf(szTmp, _countof(szTmp), _T("\\u%04X"), sz);
					StringCchCat(pszOut, nlen - 2, szTmp);
				}
				else
				{
					StringCchCat(pszOut, nlen - 2, sz);
				}
				break;
			}
		}		
		StringCchCat(pszOut, nlen, _T("\""));
		return true;
	}
	void Output(ULONG_PTR uLevel, LPCTSTR psz)
	{
		if (psz == NULL || *psz == 0)
		{
			return;
		}
		if (!m_bJsonData)
		{
			OutputDebugString(psz);
			return;
		}
		TCHAR szContext2[1024 * 2] = {0};
		valueToQuotedString(psz, szContext2, _countof(szContext2));		
		SYSTEMTIME time;
		GetLocalTime(&time);

		TCHAR szFormat[1024 * 4] = {0};
		StringCchPrintf(szFormat, _countof(szFormat),
			_T("{\n\
			\"class\" : %d,\n\
			\"content\" : %s,\n\
			\"pid\" : %d,\n\
			\"pname\" : \"%s\",\n\
			\"tid\" : %d,\n\
			\"time\" : \"%d-%d-%d %d:%d:%d.%d\"\n\
			}"), 
			uLevel, szContext2, m_dwPID, m_szPName, GetCurrentThreadId(), time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute, time.wSecond, time.wMilliseconds);	
		OutputDebugString(szFormat);
	}
public:
	static CDBG& GetInstance()
	{
		static CDBG dbg;
		return dbg;
	}
	void WINAPI SetOutputFilter(ULONG_PTR uFilter)
	{
		CRITICAL_SECTION* pCS = &m_CS;
		EnterCriticalSection(pCS); \
		ON_LEAVE_1(LeaveCriticalSection(pCS), CRITICAL_SECTION*, pCS)		
		m_uFilter = static_cast<ULONG>(uFilter);
		return;
	}
	void WINAPI SetOutputJsonType(bool bJson)
	{
		CRITICAL_SECTION* pCS = &m_CS;
		EnterCriticalSection(pCS); \
			ON_LEAVE_1(LeaveCriticalSection(pCS), CRITICAL_SECTION*, pCS)		
			m_bJsonData = bJson;
		return;
	}		
	void WINAPI Trace( ULONG_PTR dwLevel, LPCTSTR pwhFormat, ... )
	{
		CRITICAL_SECTION* pCS = &m_CS;
		EnterCriticalSection(pCS);
		ON_LEAVE_1(LeaveCriticalSection(pCS), CRITICAL_SECTION*, pCS);
		if (!(dwLevel & m_uFilter))
		{
			return;
		}
		TCHAR szBuffer[1024 * 1] = {0};
		va_list argList;
		va_start(argList, pwhFormat);			
		StringCchVPrintf(szBuffer, _countof(szBuffer) - 1, pwhFormat, argList); 		
		va_end(argList);		
		Output(dwLevel, szBuffer);
		return;
	}
	void WINAPI TraceV(ULONG_PTR dwLevel, LPCTSTR pszFmt, va_list args)
	{
		CRITICAL_SECTION* pCS = &m_CS;
		EnterCriticalSection(pCS);
		ON_LEAVE_1(LeaveCriticalSection(pCS), CRITICAL_SECTION*, pCS);
		if (!(dwLevel & m_uFilter))
		{
			return;
		}
		TCHAR szBuffer[2048] = {0};
		StringCchVPrintf(szBuffer, _countof(szBuffer) - 1, pszFmt, args); 
		Output(dwLevel, szBuffer);	
		return;
	}
private:
	CRITICAL_SECTION m_CS;
	ULONG_PTR m_uFilter;
	bool m_bJsonData;
	DWORD m_dwPID;
	TCHAR m_szPName[MAX_PATH];
};
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




#define DISALLOW_COPY_AND_ASSIGN(TypeName)    \
	TypeName(const TypeName&);                    \
	void operator=(const TypeName&)

#ifndef DO_MSG4
#define DO_MSG4(message, fn)    \
		case (message): return fn((hWnd), (message), (wParam), (lParam))
#endif
#ifndef DO_MSG2
#define DO_MSG2(message, fn)    \
		case (message): return fn((wParam), (lParam))
#endif

////////////////////////////////////////////////////////////////////////////////////////////
//like MAKEHOTKEY(1, VK_K, MOD_CONTROL | MOD_SHIFT)
#ifndef MakeHotkey	
#define MakeHotkey(enable, vk, mod)				(MAKEDWORD((vk) & 0xFFFF, MAKEWORD((mod) & 0xFF, (!!(enable) ? 0x80 : 0x00))))
#endif

#ifndef GetHotkeyMod
#define GetHotkeyMod(key)									(LOBYTE(HIWORD((key))))
#endif

#ifndef GetHotkeyVK
#define GetHotkeyVK(key)									(LOWORD((key)))	
#endif

#ifndef IsHotkeyEnable
#define IsHotkeyEnable(key)								((key) & 0x80000000)
#endif

UINT HotkeyToString(DWORD dwHotkey, BOOL isCharMod, LPWSTR pszStr, UINT uCount);
////////////////////////////////////////////////////////////////////////////////////////////



class CNoHeapObj
{
protected:
	void* operator new(size_t size)
	{
		return ::operator new(size);
	}
	void operator delete(void* p)
	{
		::operator delete(p);
	}	
};

template<typename T> FORCEINLINE T Bound(T x, T m, T M) { return (x < m) ? m : ((x > M) ? M : x); }

#define NOCOPY(T)            T(const T&); T& operator=(const T&)
#endif//file

