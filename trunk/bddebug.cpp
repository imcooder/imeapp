
#include "stdafx.h"
#include "bddebug.h"
#include <memory>
#include <StrSafe.h>

UINT HotkeyToString( DWORD dwHotkey, BOOL isCharMod, LPWSTR pszStr, UINT uCount )
{
	if (!IsHotkeyEnable(dwHotkey))
	{
		return 0;
	}
	WCHAR szBuf[64] = {0};
	BYTE bMod = GetHotkeyMod(dwHotkey);
	WORD wVK = GetHotkeyVK(dwHotkey);
	if (bMod & MOD_CONTROL)
	{
		StringCchCat(szBuf, _countof(szBuf), L"Ctrl+");
	}
	if (bMod & MOD_SHIFT)
	{
		StringCchCat(szBuf, _countof(szBuf), L"Shift+");
	}
	if (bMod & MOD_ALT)
	{
		StringCchCat(szBuf, _countof(szBuf), L"Alt+");
	}
	if (wVK)
	{
		WCHAR szVK[16] = {0};		
		switch(wVK)
		{
		case VK_SPACE:
			{
				StringCchCopy(szVK, _countof(szVK), L"Space");
			}
			break;
		case VK_RETURN:
			{
				StringCchCopy(szVK, _countof(szVK), L"Enter");
			}
			break;
		case VK_TAB:
			{
				StringCchCopy(szVK, _countof(szVK), L"Tab");
			}
			break;
		case VK_ESCAPE:
			{
				StringCchCopy(szVK, _countof(szVK), L"Esc");
			}
			break;
		case VK_OEM_PERIOD:
			{
				if (isCharMod)
				{
					StringCchCopy(szVK, _countof(szVK), L"&.");
				}
				else
				{
					StringCchCopy(szVK, _countof(szVK), L".");
				}
			}
			break;
		default:
			{
				if (isCharMod)
				{
					StringCchPrintf(szVK, _countof(szVK), L"&%c", wVK);
				}
				else
				{
					StringCchPrintf(szVK, _countof(szVK), L"%c", wVK);
				}
			}
		}
		StringCchCat(szBuf, _countof(szBuf), szVK);
	}
	else
	{
		if (szBuf[0])
		{
			szBuf[wcslen(szBuf) - 1] = 0;
		}
	}
	if (pszStr && uCount)
	{
		StringCchCopy(pszStr, uCount, szBuf);
		return (UINT)wcslen(pszStr);
	}
	return (UINT)wcslen(szBuf);
}

