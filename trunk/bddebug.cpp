
#include "stdafx.h"
#include "bddebug.h"
#include <memory>
#include <StrSafe.h>


const ULONG_PTR gdwDebugFilter = ULONG_PTR(-1);

void WINAPI XTrace( ULONG_PTR dwLevel, LPCWSTR pwhFormat, ... )
{
	if (dwLevel & gdwDebugFilter)
	{
		WCHAR szBuffer[512] = {0};		
		LPWSTR pwhBuffer = NULL;
		LONG nLen = 0;
		va_list argList;
		va_start(argList, pwhFormat);
		nLen = _vscwprintf(pwhFormat, argList);
		if (nLen	< 0)
		{
			va_end(argList);	
			return;
		}
		if (nLen < _countof(szBuffer))
		{
			pwhBuffer = szBuffer;
			StringCchVPrintfW(szBuffer, _countof(szBuffer) - 1, pwhFormat, argList); 
			OutputDebugStringW(pwhBuffer);
		}
		else
		{
            pwhBuffer = (LPWSTR)_alloca((nLen + 2) * sizeof(WCHAR));
			if(pwhBuffer)
			{
				StringCchVPrintfW(pwhBuffer, (size_t)(nLen + 1), pwhFormat, argList);
				OutputDebugStringW(pwhBuffer);
			}
		}	
		va_end(argList);	
	}
}

