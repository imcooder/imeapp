/**********************************************************************/
/*                                                                    */
/*      COMP.C                                                        */
/*                                                                    */
/*      Copyright (c) 1995 - 2000  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/
#include "stdafx.h"
#include <windows.h>
#include <strsafe.h>
#include <imm.h>
#include "resource.h"
#include "imeapps.h"

LRESULT HandleStartComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	COMPOSITIONFORM cpf = {0};
	HIMC hIMC = NULL;
	if (fdwProperty & IME_PROP_SPECIAL_UI)
	{
		// Normally, we need to set the composition window
		// position to caret position for a special UI IME
	}
	else if (fdwProperty & IME_PROP_AT_CARET)
	{
		// If an application show composition string by itself, we do not
		// need to set the position of composition window for an at caret
		// IME.

		return 1;
	}
	else
	{
		// Normally, we need to set the composition window
		// position to caret position for a near caret IME
	}

	hIMC = ImmGetContext(hWnd);

	if (!hIMC)
	{
		return 1;
	}

	cpf.dwStyle = CFS_POINT;
	cpf.ptCurrentPos.x = ptImeUIPos.x;
	cpf.ptCurrentPos.y = ptImeUIPos.y;

	ImmSetCompositionWindow(hIMC,&cpf);

	ImmReleaseContext(hWnd,hIMC);

	return 1;
}

LRESULT HandleEndComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	dwCompStrLen      = 0;
	dwCompAttrLen     = 0;
	dwCompClsLen      = 0;
	dwCompReadStrLen  = 0;
	dwCompReadAttrLen = 0;
	dwCompReadClsLen  = 0;
	dwResultStrLen      = 0;
	dwResultClsLen      = 0;
	dwResultReadStrLen  = 0;
	dwResultReadClsLen  = 0;
	CGlobalData::GetInstance().NotifyUpdateUI();

	return 1;
}

void MakePaintString(HWND hWnd, LPTSTR lpStr, DWORD dwStrLen, LPDWORD lpCls, DWORD dwClsLen, LPTSTR lpPaintStr, DWORD dwPaintStrSize)
{
	LPTSTR lpPaintStart = lpPaintStr;

	lpPaintStr += lstrlen(lpPaintStr);

	if (dwStrLen)
	{
		if (dwClsLen)
		{
			lpCls[127] = 0;

			while (*(lpCls+1) && *lpCls < dwStrLen)
			{
				DWORD dwTextLen = *(lpCls+1) - *lpCls;
				LPTSTR lpT = lpStr + *lpCls;

				memcpy(lpPaintStr,lpT,dwTextLen * sizeof(TCHAR));
				lpPaintStr += dwTextLen;
				*lpPaintStr = TEXT(',');
				lpPaintStr++;
				lpCls++;
			}
			*lpPaintStr = TEXT('\0');
		}
		else
		{
			StringCchCopy((LPTSTR)lpPaintStr,dwPaintStrSize, (LPTSTR)lpStr);
			StringCchCat((LPTSTR)lpPaintStr,dwPaintStrSize, (LPTSTR)TEXT(","));
		}
	}

}

LRESULT HandleComposition(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HIMC hIMC = NULL;
	BOOL fRedraw = FALSE;

	hIMC = ImmGetContext(hWnd);	
	/*
	{
	CANDIDATEFORM cdf = {0};

	cdf.dwIndex = 0;
	cdf.dwStyle = CFS_CANDIDATEPOS;
	cdf.ptCurrentPos.x = 500;
	cdf.ptCurrentPos.y = 200;
	ImmSetCandidateWindow(hIMC, &cdf);
	UINT i;
	for (i = 0; i < 4; i++)
	{
	if (!ImmGetCandidateWindow(hIMC, i, &cdf))
	{
	continue;
	}				
	cdf.dwStyle = CFS_CANDIDATEPOS;
	ImmSetCandidateWindow(hIMC, &cdf);
	}		
	COMPOSITIONFORM cpf = {0};
	cpf.dwStyle = CFS_POINT;
	cpf.ptCurrentPos.x = 500;
	cpf.ptCurrentPos.y = 200;
	ImmSetCompositionWindow(hIMC, &cpf);
	}
	*/
	{
		LPINPUTCONTEXT lpIMC = NULL;
		LPCANDIDATEINFO lpCandInfo = NULL;
		lpIMC = ImmLockIMC(hIMC);
		ON_LEAVE_1(ImmUnlockIMC(hIMC), HIMC, hIMC);	
		if (lpIMC)
		{
			HIMCC hIMCC = lpIMC->hCompStr;
			LPVOID pIMCC = ImmLockIMCC(hIMCC);
			ON_LEAVE_1(ImmUnlockIMCC(hIMCC), HIMCC, hIMCC);
			COMPOSITIONSTRING* cs = (COMPOSITIONSTRING*)pIMCC;			
			{
				RETAILMSG(MSG_LEVEL_DEBUG, _T("COMPOSITIONSTRING size[%08X]"), cs->dwSize);
				RETAILMSG(MSG_LEVEL_DEBUG, _T("Address: dwCompReadAttrOffset=%08X,dwCompReadClauseOffset=%08X, dwCompReadStrOffset=%08X, dwCompAttrOffset=%08X, dwCompClauseOffset=%08X, dwCompStrOffset=%08X dwResultReadClauseOffset=%08X dwResultReadStrOffset=%08X dwResultClauseOffset=%08X dwResultStrOffset=%08X dwPrivateOffset=%08X"), cs->dwCompReadAttrOffset, cs->dwCompReadClauseOffset, cs->dwCompReadStrOffset, cs->dwCompAttrOffset, cs->dwCompClauseOffset, cs->dwCompStrOffset, cs->dwResultReadClauseOffset, cs->dwResultReadStrOffset, cs->dwResultClauseOffset, cs->dwResultStrOffset, cs->dwPrivateOffset);
				TCHAR szBuf[512] = {0};
				RETAILMSG(MSG_LEVEL_DEBUG, TEXT("dwCompReadAttrLen %d\n"), cs->dwCompReadAttrLen);
				if (cs->dwCompReadAttrLen)
				{				
					StringCchPrintf(szBuf, _countof(szBuf), _T("CompReadAttr:"));
					LPBYTE pbAttr = (LPBYTE)cs + cs->dwCompReadAttrOffset;
					for (int i = 0; i < cs->dwCompReadAttrLen; i ++)
					{
						TCHAR szTmp[8] = {0};
						StringCchPrintf(szTmp, _countof(szTmp), _T("%02X-"), pbAttr[i]);
						StringCchCat(szBuf, _countof(szBuf), szTmp);
					}
					RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), szBuf);
				}
				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwCompReadClauseLen %d\n"), cs->dwCompReadClauseLen);
				if (cs->dwCompReadClauseLen)
				{				
					StringCchPrintf(szBuf, _countof(szBuf), _T("CompReadClause:"));
					LPBYTE pbAttr = (LPBYTE)cs + cs->dwCompReadClauseOffset;
					for (int i = 0; i < cs->dwCompReadClauseLen; i ++)
					{
						TCHAR szTmp[8] = {0};
						StringCchPrintf(szTmp, _countof(szTmp), _T("%02X-"), pbAttr[i]);
						StringCchCat(szBuf, _countof(szBuf), szTmp);
					}
					RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), szBuf);
				}

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwCompReadStrLen %d\n"), cs->dwCompReadStrLen);
				if (cs->dwCompReadStrLen)
				{
					LPCTSTR pszStr = (LPCTSTR)((LPBYTE)cs + cs->dwCompReadStrOffset);					
					RETAILMSG(MSG_LEVEL_DEBUG, _T("CompReadStr:%s\n"), pszStr);
				}

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwCompAttrLen %d\n"), cs->dwCompAttrLen);
				if (cs->dwCompAttrLen)
				{				
					StringCchPrintf(szBuf, _countof(szBuf), _T("CompAttr:"));
					LPBYTE pbAttr = (LPBYTE)cs + cs->dwCompAttrOffset;
					for (int i = 0; i < cs->dwCompAttrLen; i ++)
					{
						TCHAR szTmp[8] = {0};
						StringCchPrintf(szTmp, _countof(szTmp), _T("%02X-"), pbAttr[i]);
						StringCchCat(szBuf, _countof(szBuf), szTmp);
					}
					RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), szBuf);
				}

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwCompClauseLen %d\n"), cs->dwCompClauseLen);
				if (cs->dwCompClauseLen)
				{				
					StringCchPrintf(szBuf, _countof(szBuf), _T("CompClaus:"));
					LPBYTE pbAttr = (LPBYTE)cs + cs->dwCompClauseOffset;
					for (int i = 0; i < cs->dwCompClauseLen; i ++)
					{
						TCHAR szTmp[8] = {0};
						StringCchPrintf(szTmp, _countof(szTmp), _T("%02X-"), pbAttr[i]);
						StringCchCat(szBuf, _countof(szBuf), szTmp);
					}
					RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), szBuf);
				}

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwCompStrLen %d\n"), cs->dwCompStrLen);
				if (cs->dwCompStrLen)
				{
					LPCTSTR pszStr = (LPCTSTR)((LPBYTE)cs + cs->dwCompStrOffset);					
					RETAILMSG(MSG_LEVEL_DEBUG, _T("CompStr:%s\n"), pszStr);
				}

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwCursorPos %d\n"), cs->dwCursorPos);
				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwDeltaStart %d\n"), cs->dwDeltaStart);				

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwResultReadClauseLen %d\n"), cs->dwResultReadClauseLen);
				if (cs->dwResultReadClauseLen)
				{				
					//StringCchPrintf(szBuf, _countof(szBuf), _T("CompStr:"));
					//LPCTSTR pszStr = (LPCTSTR)((LPBYTE)cs + cs->dwCompStrOffset);					
					//RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), pszStr);
				}
				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwResultReadStrLen %d\n"), cs->dwResultReadStrLen);
				if (cs->dwResultReadStrLen)
				{
					LPCTSTR pszStr = (LPCTSTR)((LPBYTE)cs + cs->dwResultReadStrOffset);					
					RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), pszStr);
				}
				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwResultClauseLen %d\n"), cs->dwResultClauseLen);

				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwResultStrLen %d\n"), cs->dwResultStrLen);
				if (cs->dwResultStrLen)
				{
					LPCTSTR pszStr = (LPCTSTR)((LPBYTE)cs + cs->dwResultStrOffset);					
					RETAILMSG(MSG_LEVEL_DEBUG, _T("%s\n"), pszStr);
				}
				RETAILMSG(MSG_LEVEL_DEBUG, _T("dwPrivateSize %d\n"), cs->dwPrivateSize);
			}
		}
	}
	if (lParam & GCS_COMPSTR)
	{
		dwCompStrLen = ImmGetCompositionString(hIMC, GCS_COMPSTR, szCompStr, sizeof(szCompStr));
		dwCompStrLen /= sizeof(TCHAR);
		szCompStr[dwCompStrLen] = TEXT('\0');
		fRedraw = TRUE;
	}
	else
	{
		dwCompStrLen = 0;
		szCompStr[0] = TEXT('\0');
	}

	if (lParam & GCS_COMPATTR)
	{
		dwCompAttrLen = ImmGetCompositionString(hIMC, GCS_COMPATTR, bCompAttr, sizeof(bCompAttr));
		fRedraw = TRUE;
	}
	else
	{
		dwCompAttrLen = 0;
		bCompAttr[0] = 0;
	}

	if (lParam & GCS_COMPCLAUSE)
	{
		dwCompClsLen = ImmGetCompositionString(hIMC, GCS_COMPCLAUSE, dwCompCls, sizeof(dwCompCls));
		fRedraw = TRUE;
	}
	else
	{
		dwCompClsLen = 0;
		dwCompCls[0] = 0;
		dwCompCls[1] = 0;
	}


	if (lParam & GCS_COMPREADSTR)
	{
		dwCompReadStrLen = ImmGetCompositionString(hIMC, GCS_COMPREADSTR, szCompReadStr, sizeof(szCompReadStr));
		dwCompReadStrLen /= sizeof(TCHAR);
		szCompReadStr[dwCompReadStrLen] = TEXT('\0');
		fRedraw = TRUE;
	}
	else
	{
		dwCompReadStrLen = 0;
		szCompReadStr[0] = TEXT('\0');
	}

	if (lParam & GCS_COMPREADATTR)
	{
		dwCompReadAttrLen = ImmGetCompositionString(hIMC, GCS_COMPREADATTR, bCompReadAttr, sizeof(bCompReadAttr));
		fRedraw = TRUE;
	}
	else
	{
		dwCompReadAttrLen = 0;
		bCompReadAttr[0] = 0;
	}

	if (lParam & GCS_COMPREADCLAUSE)
	{
		dwCompReadClsLen = ImmGetCompositionString(hIMC, GCS_COMPREADCLAUSE, dwCompReadCls, sizeof(dwCompReadCls));
		fRedraw = TRUE;
	}
	else
	{
		dwCompReadClsLen = 0;
		dwCompReadCls[0] = 0;
		dwCompReadCls[1] = 0;
	}


	if (lParam & GCS_RESULTSTR)
	{
		RECT rc = {0};
		HDC hIC = NULL;
		SIZE sz0, sz1;
		HFONT hOldFont = NULL;

		if (lParam & GCS_RESULTCLAUSE)
		{
			dwResultClsLen = ImmGetCompositionString(hIMC, GCS_RESULTCLAUSE, dwResultCls, sizeof(dwResultCls));
		}
		else
		{
			dwResultClsLen = 0;
			dwResultCls[0] = 0;
			dwResultCls[1] = 0;
		}

		dwResultStrLen = ImmGetCompositionString(hIMC, GCS_RESULTSTR, szResultStr, sizeof(szResultStr));
		dwResultStrLen /= sizeof(TCHAR);
		szResultStr[dwResultStrLen] = TEXT('\0');


		// szPaintResult may overflow..
		GetClientRect(hWnd,&rc);
		hIC = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
		hOldFont = (HFONT)SelectObject(hIC,hFont);

		GetTextExtentPoint(hIC, szPaintResult, lstrlen(szPaintResult), &sz0);
		GetTextExtentPoint(hIC, szResultStr, lstrlen(szResultStr), &sz1);


		if (sz0.cx + sz1.cx >= rc.right)
		{
			szPaintResult[0] = TEXT('\0');
			szPaintResultRead[0] = TEXT('\0');
		}
		SelectObject(hIC,hOldFont);
		DeleteDC(hIC);

		MakePaintString(hWnd,szResultStr,dwResultStrLen,dwResultCls,dwResultClsLen,szPaintResult, ARRAYSIZE(szPaintResult));

		fRedraw = TRUE;
	}
	else
	{
		dwResultStrLen = 0;
		szResultStr[0] = TEXT('\0');
		dwResultClsLen = 0;
		dwResultCls[0] = 0;
		dwResultCls[1] = 0;
	}



	if (lParam & GCS_RESULTREADSTR)
	{
		if (lParam & GCS_RESULTREADCLAUSE)
		{
			dwResultReadClsLen = ImmGetCompositionString(hIMC,GCS_RESULTREADCLAUSE,dwResultReadCls,sizeof(dwResultReadCls));
			fRedraw = TRUE;
		}
		else
		{
			dwResultReadClsLen = 0;
			dwResultReadCls[0] = 0;
			dwResultReadCls[1] = 0;
		}
		dwResultReadStrLen = ImmGetCompositionString(hIMC,GCS_RESULTREADSTR,szResultReadStr,sizeof(szResultReadStr));
		dwResultReadStrLen /= sizeof(TCHAR);
		szResultReadStr[dwResultReadStrLen] = TEXT('\0');
		MakePaintString(hWnd,szResultReadStr,dwResultReadStrLen,dwResultReadCls,dwResultReadClsLen,szPaintResultRead, ARRAYSIZE(szPaintResultRead));
		fRedraw = TRUE;
	}
	else
	{
		dwResultReadStrLen = 0;
		szResultReadStr[0] = TEXT('\0');
		dwResultReadClsLen = 0;
		dwResultReadCls[0] = 0;
		dwResultReadCls[1] = 0;
	}

	if (lParam & GCS_CURSORPOS)
	{		
		DWORD dwPos = ImmGetCompositionString(hIMC, GCS_CURSORPOS, NULL, 0);
		fRedraw = TRUE;		
		StringCchPrintf(szCursor, _countof(szCursor), _T("%d"), dwPos);
	}
	else
	{
		szCursor[0] = 0;
	}
	if (lParam & GCS_DELTASTART)
	{		
		DWORD dwDelta = ImmGetCompositionString(hIMC, GCS_DELTASTART, NULL, 0);
		fRedraw = TRUE;		
		StringCchPrintf(szDelta, _countof(szDelta), _T("%d"), dwDelta);
	}
	else
	{
		szDelta[0] = 0;
	}

	if (fRedraw)
	{
		CGlobalData::GetInstance().NotifyUpdateUI();
	}
	return 1;
}


LRESULT HandleChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc = {0};
	HDC hIC = NULL;
	SIZE sz0, sz1;
	HFONT hOldFont = NULL;
	GetClientRect(hWnd,&rc);
	hIC = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
	hOldFont = (HFONT)SelectObject(hIC,hFont);

	GetTextExtentPoint(hIC,szPaintResult,lstrlen(szPaintResult),&sz0);
	GetTextExtentPoint(hIC,szResultStr,lstrlen(szResultStr),&sz1);

	if (sz0.cx + sz1.cx >= rc.right)
	{
		szPaintResult[0] = TEXT('\0');
		szPaintResultRead[0] = TEXT('\0');
	}
	SelectObject(hIC,hOldFont);
	DeleteDC(hIC);

	TCHAR szCode[16] = {0};
	StringCchPrintf(szCode, _countof(szCode), _T("{%c},"), wParam);	
	StringCchCat(szPaintResult, _countof(szPaintResult), szCode);

	CGlobalData::GetInstance().NotifyUpdateUI();
	return 1;
}

LRESULT HandleIMEChar(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	RECT rc = {0};
	HDC hIC = NULL;
	SIZE sz0, sz1;
	HFONT hOldFont = NULL;
	GetClientRect(hWnd,&rc);
	hIC = CreateIC(TEXT("DISPLAY"), NULL, NULL, NULL);
	hOldFont = (HFONT)SelectObject(hIC,hFont);

	GetTextExtentPoint(hIC,szPaintResult,lstrlen(szPaintResult),&sz0);
	GetTextExtentPoint(hIC,szResultStr,lstrlen(szResultStr),&sz1);

	if (sz0.cx + sz1.cx >= rc.right)
	{
		szPaintResult[0] = TEXT('\0');
		szPaintResultRead[0] = TEXT('\0');
	}
	SelectObject(hIC,hOldFont);
	DeleteDC(hIC);

	TCHAR szCode[16] = {0};
	StringCchPrintf(szCode, _countof(szCode), _T("[%c],"), wParam);	
	StringCchCat(szPaintResult, _countof(szPaintResult), szCode);

	CGlobalData::GetInstance().NotifyUpdateUI();
	return 1;
}

LRESULT HandleNotify(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HIMC hIMC = NULL;
	BOOL fOpen = FALSE;
	hIMC = ImmGetContext(hWnd);
	ON_LEAVE_2(ImmReleaseContext(hWnd,hIMC), HWND, hWnd, HIMC, hIMC);
	switch (wParam)
	{
	case IMN_SETCANDIDATEPOS:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_SETCANDIDATEPOS[%08X][%08X]"), wParam, lParam);
		}
		break;
	case IMN_SETCOMPOSITIONWINDOW:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_SETCOMPOSITIONWINDOW[%08X][%08X]"), wParam, lParam);
		}
		break;
	case IMN_OPENSTATUSWINDOW: /* fall-through */
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_OPENSTATUSWINDOW[%08X][%08X]"), wParam, lParam);
		}
		break;
	case IMN_CLOSESTATUSWINDOW:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_CLOSESTATUSWINDOW[%08X][%08X]"), wParam, lParam);
		}
		break;
	case IMN_SETOPENSTATUS:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_SETOPENSTATUS[%08X][%08X]"), wParam, lParam);
			if (hIMC)
			{
				SetStatusItems(hWnd);
				fOpen = ImmGetOpenStatus(hIMC);
				UpdateShowOpenStatusButton(fOpen);	
			}				
		}
		break;
	case IMN_SETCONVERSIONMODE:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_SETCONVERSIONMODE[%08X][%08X]"), wParam, lParam);
			if (hIMC)
			{
				fOpen = ImmGetOpenStatus(hIMC);
				ImmGetConversionStatus(hIMC,&dwConversionMode,&dwSentenceMode);
				if (fOpen)
				{
					SetConvModeParts(dwConversionMode);
					UpdateModeButton(dwConversionMode);
				}
				else
				{
					ClearConvModeParts();
				}
			}						
		}
		break;
	case IMN_OPENCANDIDATE:
		{		
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_OPENCANDIDATE[%08X][%08X]"), wParam, lParam);
			if ((lParam != 0x01))
			{
				if (fdwProperty & IME_PROP_SPECIAL_UI)
				{
					// Normally, we only need to set the composition window
					// position for a special UI IME
				}
				else if (fdwProperty & IME_PROP_AT_CARET)
				{
					CANDIDATEFORM cdf;			
					cdf.dwIndex = 0;
					cdf.dwStyle = CFS_CANDIDATEPOS;
					cdf.ptCurrentPos.x = 500;//ptImeUIPos.x;
					cdf.ptCurrentPos.y = ptImeUIPos.y;
					ImmSetCandidateWindow(hIMC, &cdf);					
				}
				else
				{
					// Normally, we only need to set the composition window
					// position for a near caret IME
				}
				return (DefWindowProc(hWnd, message, wParam, lParam));
			}
		}
		break;
	case IMN_CHANGECANDIDATE:
		{
			if(hIMC)
			{
				RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_CHANGECANDIDATE[%08X][%08X]"), wParam, lParam);
				DWORD dwSize;
				LPCANDIDATELIST lpC;
				UINT uIdx = 0;	
				if (lParam)
				{							
					for (int i = 0; i < 32; i ++)
					{
						if (lParam & (0x1 << i))
						{
							break;
						}
						else
						{
							uIdx ++;
						}
					}
				}
				if (dwSize = ImmGetCandidateList(hIMC, uIdx, NULL, 0))
				{
					lpC = (LPCANDIDATELIST)GlobalAlloc(GPTR, dwSize);
					ImmGetCandidateList(hIMC, uIdx, lpC, dwSize);
					RETAILMSG(MSG_LEVEL_DEBUG, TEXT("DumpCandList[%d]"), uIdx);				
					RETAILMSG(MSG_LEVEL_DEBUG, TEXT("dwSize[%08X] dwCount [%d] dwSelection [%d] dwPageStart [%d] dwPageSize [%d] dwStype[%d]"), lpC->dwSize, lpC->dwCount, lpC->dwSelection, lpC->dwPageStart, lpC->dwPageSize, lpC->dwStyle);				
					GlobalFree((HANDLE)lpC);
				}				
				if (lParam == 0x01)
				{
					DWORD dwSize;
					if (!lpCandList)
					{
						lpCandList = (LPCANDIDATELIST)GlobalAlloc(GPTR,sizeof(CANDIDATELIST));
					}				
					if (dwSize = ImmGetCandidateList(hIMC, 0x0, NULL, 0))
					{
						GlobalFree((HANDLE)lpCandList);
						lpCandList = (LPCANDIDATELIST)GlobalAlloc(GPTR,dwSize);
						ImmGetCandidateList(hIMC, 0x0, lpCandList, dwSize);
					}
					else
					{
						memset(lpCandList, 0, sizeof(CANDIDATELIST));
					}
					CGlobalData::GetInstance().NotifyUpdateUI();
				}
				else
				{
					return (DefWindowProc(hWnd, message, wParam, lParam));
				}
			}
		}
		break;
	case IMN_CLOSECANDIDATE:
		{
			if (hIMC)
			{
				RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_CLOSECANDIDATE[%08X][%08X]"), wParam, lParam);
				if (lParam == 0x01)
				{
					if (!lpCandList)
					{
						lpCandList = (LPCANDIDATELIST)GlobalAlloc(GPTR,sizeof(CANDIDATELIST));
					}
					memset(lpCandList, 0, sizeof(CANDIDATELIST));
					CGlobalData::GetInstance().NotifyUpdateUI();
				}
				else
				{
					return (DefWindowProc(hWnd, message, wParam, lParam));
				}
			}
		}
		break;
	case IMN_GUIDELINE:
		{
			if(hIMC)
			{
				RETAILMSG(MSG_LEVEL_DEBUG, TEXT("IMN_GUIDELINE[%08X][%08X]"), wParam, lParam);
				DWORD dwLevel = ImmGetGuideLine(hIMC, GGL_LEVEL, NULL, 0);
				INPUTCONTEXT* pInput = NULL;
				pInput = (INPUTCONTEXT*)ImmLockIMC(hIMC);
				if (pInput)
				{
					GUIDELINE* pGuidLine = (GUIDELINE*)ImmLockIMCC(pInput->hGuideLine);
					if (pGuidLine)
					{
						StringCchPrintf(szGuideLine, _countof(szGuideLine), _T("%08X-%08X-%08X-%08X-%08X-%08X-%08X\n"), pGuidLine->dwSize, pGuidLine->dwLevel, pGuidLine->dwIndex, pGuidLine->dwStrLen, pGuidLine->dwStrOffset, pGuidLine->dwPrivateSize, pGuidLine->dwPrivateOffset);					
					}
				}
				return (DefWindowProc(hWnd, message, wParam, lParam));
			}
		}
		break;
	case NI_CHANGECANDIDATELIST:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("NI_CHANGECANDIDATELIST[%08X][%08X]"), wParam, lParam);
		}
		break;
	case NI_SELECTCANDIDATESTR:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("NI_SELECTCANDIDATESTR[%08X][%08X]"), wParam, lParam);
		}
		break;	
	case NI_FINALIZECONVERSIONRESULT:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("NI_FINALIZECONVERSIONRESULT[%08X][%08X]"), wParam, lParam);
		}
		break;  
	case NI_SETCANDIDATE_PAGESTART:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("NI_SETCANDIDATE_PAGESTART[%08X][%08X]"), wParam, lParam);
		}
		break; 
	case NI_SETCANDIDATE_PAGESIZE:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("NI_SETCANDIDATE_PAGESIZE[%08X][%08X]"), wParam, lParam);
		}
		break;
	case NI_IMEMENUSELECTED:
		{
			RETAILMSG(MSG_LEVEL_DEBUG, TEXT("NI_IMEMENUSELECTED[%08X][%08X]"), wParam, lParam);
		}
		break;
	default:
		return (DefWindowProc(hWnd, message, wParam, lParam));
	}

	return 0;
}
