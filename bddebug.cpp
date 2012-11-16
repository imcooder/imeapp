
#include "stdafx.h"
#include "bddebug.h"
#include <memory>
#include <StrSafe.h>


const ULONG_PTR gdwDebugFilter = ULONG_PTR(-1);

void WINAPI XTrace( ULONG_PTR dwLevel, LPCTSTR pwhFormat, ... )
{
	if (dwLevel & gdwDebugFilter)
	{
		TCHAR szBuffer[512] = {0};		
		LPTSTR pwhBuffer = NULL;
		LONG nLen = 0;
		va_list argList;
		va_start(argList, pwhFormat);
		nLen = _vsctprintf(pwhFormat, argList);
		if (nLen	< 0)
		{
			va_end(argList);	
			return;
		}
		if (nLen < _countof(szBuffer))
		{
			pwhBuffer = szBuffer;
			StringCchVPrintf(szBuffer, _countof(szBuffer) - 1, pwhFormat, argList); 
			OutputDebugString(pwhBuffer);
		}
		else
		{
            pwhBuffer = (LPTSTR)_alloca((nLen + 2) * sizeof(TCHAR));
			if(pwhBuffer)
			{
				StringCchVPrintf(pwhBuffer, (size_t)(nLen + 1), pwhFormat, argList);
				OutputDebugString(pwhBuffer);
			}
		}	
		va_end(argList);	
	}
}

