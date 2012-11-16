/**********************************************************************/
/*                                                                    */
/*      PAINT.C                                                       */
/*                                                                    */
/*      Copyright (c) 1995 - 2000  Microsoft Corporation                */
/*                                                                    */
/**********************************************************************/
#include "stdafx.h"
#include <windows.h>
#include "imm.h"
#include "resource.h"
#include "imeapps.h"
#include <strsafe.h>
#include <string>
#define DEBUG 1



void SetAttrColor(HDC hDC, BYTE bAttr)
{
	switch (bAttr)
	{
	case ATTR_INPUT:
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		break;

	case ATTR_TARGET_CONVERTED:
		SetTextColor(hDC,RGB(255,255,255));
		SetBkMode(hDC,OPAQUE);
		SetBkColor(hDC,RGB(0,0,255));
		break;

	case ATTR_CONVERTED:
		SetTextColor(hDC,RGB(0,0,255));
		SetBkMode(hDC,TRANSPARENT);
		break;

	case ATTR_TARGET_NOTCONVERTED:
		SetTextColor(hDC,RGB(255,255,255));
		SetBkMode(hDC,OPAQUE);
		SetBkColor(hDC,RGB(0,255,0));
		break;

	case ATTR_INPUT_ERROR:
		SetTextColor(hDC,RGB(255,255,0));
		SetBkMode(hDC,TRANSPARENT);
		break;

	case ATTR_FIXEDCONVERTED:
		SetTextColor(hDC,RGB(255,0,0));
		SetBkMode(hDC,TRANSPARENT);
		break;

	default:
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		break;
	}
}

LRESULT HandlePaint(HWND hWnd, WPARAM wParam, LPARAM lParam)
{
	HDC hDC = NULL;
	PAINTSTRUCT ps = {0};
	UINT i;
	int x = ORG_X;
	int y = ORG_Y;
	const int nOffsetX = 200;
	const int nOffsetY = 10;
	SIZE sz;
	HFONT hOldFont = NULL;
	HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	int height,defheight;
	const TCHAR  szResult[] = TEXT("Result String");
	const TCHAR  szComp[] = TEXT("Composition String");
	RECT rc = {0};
	TCHAR szBuf[64] = {0};
	GetClientRect(hWnd,&rc);

	hDC = BeginPaint(hWnd, &ps);

	if (hFont)
	{
		hOldFont = (HFONT)SelectObject(hDC,hDefFont);
	}


	// Get the height of the default gui font.
	GetTextExtentPoint(hDC,TEXT("A"),1,&sz);
	defheight = sz.cy + 1;

	// Get the height of the font.
	SelectObject(hDC,hFont);
	GetTextExtentPoint(hDC,TEXT("A"),1,&sz);
	height = sz.cy + 1;

	SelectObject(hDC,hDefFont);
	SetTextColor(hDC,RGB(0,0,0));
	SetBkMode(hDC,TRANSPARENT);
	TextOut(hDC,ORG_X,y,szResult,lstrlen(szResult));
	y += defheight;

	if (lstrlen(szPaintResult))
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(255,0,0));
		SetBkMode(hDC,TRANSPARENT);
		TextOut(hDC,x,y,szPaintResult,lstrlen(szPaintResult));

	}

	y += height;

	if (lstrlen(szPaintResultRead))
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(255,0,0));
		SetBkMode(hDC,TRANSPARENT);
		TextOut(hDC,x,y,szPaintResultRead,lstrlen(szPaintResultRead));
	}

	if (dwResultReadStrLen)
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);

		if (dwResultReadClsLen)
		{
			dwResultReadCls[127] = 0;
			i = 1;

			SetTextColor(hDC,RGB(255,0,0));
			while (dwResultReadCls[i] && dwResultReadCls[i-1] < dwResultReadStrLen)
			{
				DWORD dwTextLen = dwResultReadCls[i] - dwResultReadCls[i-1];
				LPCTSTR lpStart = szResultReadStr + dwResultReadCls[i-1];

				TextOut(hDC,x,y,lpStart,dwTextLen);
				GetTextExtentPoint(hDC,lpStart,dwTextLen,&sz);
				x += sz.cx;

				TextOut(hDC, x, y, TEXT(","), 1);
				GetTextExtentPoint(hDC, TEXT(","),1,&sz);
				x += (sz.cx + 2);

				i++;
			}
		}
		else
		{
			SetTextColor(hDC,RGB(255,0,0));
			SetBkMode(hDC,TRANSPARENT);
			TextOut(hDC,x,y,szResultReadStr,dwResultReadStrLen);
		}
	}

	y += height;

	SelectObject(hDC,hDefFont);
	SetTextColor(hDC, RGB(0,0,0));
	SetBkMode(hDC, TRANSPARENT);
	TextOut(hDC, ORG_X, y, szComp, lstrlen(szComp));
	y += defheight;

	if (dwCompStrLen)
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));

		if (dwCompClsLen && dwCompAttrLen)
		{
			dwCompCls[127] = 0;
			i = 1;

			while (dwCompCls[i] && dwCompCls[i-1] < dwCompStrLen)
			{
				DWORD dwTextLen = dwCompCls[i] - dwCompCls[i-1];
				LPTSTR lpStart = szCompStr + dwCompCls[i-1];

				SetAttrColor(hDC, bCompAttr[dwCompCls[i - 1]]);

				TextOut(hDC, x, y, lpStart, dwTextLen);
				GetTextExtentPoint(hDC, lpStart, dwTextLen, &sz);
				x += sz.cx;

				SetTextColor(hDC,RGB(0,0,0));
				SetBkMode(hDC,TRANSPARENT);
				TextOut(hDC,x,y,TEXT(","),1);
				GetTextExtentPoint(hDC,TEXT(","),1,&sz);
				x += (sz.cx + 2);

				i++;
			}
		}
		else
		{
			SetBkMode(hDC,TRANSPARENT);
			TextOut(hDC,x,y,szCompStr,dwCompStrLen);
		}

	}
	y += height;

	//////////////////////////////////////////////////////////////
	/// CompReadStr
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompReadStr:%04d"), dwCompReadStrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		if (dwCompReadStrLen)
		{			
			DWORD dwPos = 0;					
			TextOut(hDC, x + nOffsetX, y, szCompReadStr,_tcslen(szCompReadStr));
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// CompReadAttr
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompReadAttr:%04d"), dwCompReadAttrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		if (dwCompReadAttrLen)
		{
			tstring strText;
			DWORD dwPos = 0;
			while (dwPos < dwCompReadAttrLen && dwPos < _countof(bCompReadAttr))
			{
				StringCchPrintf(szBuf, _countof(szBuf), TEXT("%02X-"), bCompReadAttr[dwPos]);
				strText += szBuf;	

				dwPos ++;		
			}				
			TextOut(hDC, x + nOffsetX, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// CompReadCls
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompReadCls:%04d"), dwCompReadClsLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		if (dwCompReadClsLen)
		{
			tstring strText;
			DWORD dwPos = 0;
			while (dwPos < dwCompReadClsLen && dwPos < _countof(dwCompReadCls))
			{
				StringCchPrintf(szBuf, _countof(szBuf), TEXT("%08X-"), dwCompReadCls[dwPos]);
				strText += szBuf;	

				dwPos ++;		
			}				
			TextOut(hDC, x + nOffsetX, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// CompAttr
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompAttr:%04d"), dwCompAttrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		if (dwCompAttrLen)
		{
			tstring strText;
			DWORD dwPos = 0;
			while (dwPos < dwCompAttrLen && dwPos < _countof(bCompAttr))
			{
				StringCchPrintf(szBuf, _countof(szBuf), TEXT("%02X-"), bCompAttr[dwPos]);
				strText += szBuf;	

				dwPos ++;		
			}				
			TextOut(hDC, x + nOffsetX, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// CompCls
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompCls:%04d"), dwCompClsLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		if (dwCompClsLen)
		{
			tstring strText;
			DWORD dwPos = 0;
			while (dwPos < dwCompClsLen && dwPos < _countof(dwCompCls))
			{
				StringCchPrintf(szBuf, _countof(szBuf), TEXT("%08X-"), dwCompCls[dwPos]);
				strText += szBuf;	

				dwPos ++;		
			}				
			TextOut(hDC, x + nOffsetX, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// CompAttr Format
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC, RGB(0, 0, 0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompAttr Format:%04d"), dwCompAttrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));	
		x += nOffsetX;
		if (dwCompAttrLen)
		{
			if (dwCompClsLen && dwCompAttrLen)
			{
				dwCompCls[127] = 0;
				i = 1;				
				while (dwCompCls[i] && dwCompCls[i-1] < dwCompStrLen)
				{
					DWORD dwTextLen = dwCompCls[i] - dwCompCls[i-1];
					LPCTSTR lpStart = szCompStr + dwCompCls[i-1];

					SetAttrColor(hDC, bCompAttr[dwCompCls[i-1]]);
					TextOut(hDC,x,y,lpStart,dwTextLen);
					GetTextExtentPoint(hDC,lpStart,dwTextLen,&sz);
					x += sz.cx;

					SetTextColor(hDC,RGB(0,0,0));
					SetBkMode(hDC,TRANSPARENT);
					TextOut(hDC,x,y,TEXT(","),1);
					GetTextExtentPoint(hDC,TEXT(","),1,&sz);
					x += (sz.cx + 2);

					i++;
				}
			}			
		}
		y += height;
	}	
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompReadAttr Format:%04d"), dwCompReadStrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (dwCompReadStrLen)
		{
			if (dwCompReadClsLen && dwCompReadAttrLen)
			{
				dwCompReadCls[127] = 0;
				i = 1;				
				while (dwCompReadCls[i] && dwCompReadCls[i-1] < dwCompReadStrLen)
				{
					DWORD dwTextLen = dwCompReadCls[i] - dwCompReadCls[i-1];
					LPCTSTR lpStart = szCompReadStr + dwCompReadCls[i-1];

					SetAttrColor(hDC, bCompReadAttr[dwCompReadCls[i-1]]);
					TextOut(hDC,x,y,lpStart,dwTextLen);
					GetTextExtentPoint(hDC,lpStart,dwTextLen,&sz);
					x += sz.cx;

					SetTextColor(hDC, RGB(0,0,0));
					SetBkMode(hDC, TRANSPARENT);
					TextOut(hDC, x, y, TEXT(","), 1);
					GetTextExtentPoint(hDC, TEXT(","), 1, &sz);
					x += (sz.cx + 2);
					i++;
				}
			}
			else
			{
				SetBkMode(hDC, TRANSPARENT);
				TextOut(hDC, x, y, szCompReadStr, dwCompReadStrLen);
			}
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// CompStr
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("CompStr:%04d"), dwCompStrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (dwCompStrLen)
		{
			tstring strText;
			strText.assign(szCompStr);		
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// ResultReadCls
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("ResultReadCls:%04d"), dwResultReadClsLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (dwResultReadClsLen)
		{
			tstring strText;
			DWORD dwPos = 0;
			while (dwPos < dwResultReadClsLen && dwPos < _countof(dwResultReadCls))
			{
				StringCchPrintf(szBuf, _countof(szBuf), TEXT("%08X-"), dwResultReadCls[dwPos]);
				strText += szBuf;	

				dwPos ++;		
			}				
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// ResultReadStr
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("ResultReadStr:%04d"), dwResultReadStrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (dwResultReadStrLen)
		{
			tstring strText;
			strText.assign(szResultStr);		
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// ResultCls
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("ResultCls:%04d"), dwResultClsLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (dwResultClsLen)
		{
			tstring strText;
			DWORD dwPos = 0;
			while (dwPos < dwResultClsLen && dwPos < _countof(dwResultCls))
			{
				StringCchPrintf(szBuf, _countof(szBuf), TEXT("%08X-"), dwResultCls[dwPos]);
				strText += szBuf;	

				dwPos ++;		
			}				
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// ResultStr
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("ResultStr:%04d"), dwResultStrLen);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (dwResultStrLen)
		{
			tstring strText;
			strText.assign(szResultStr);		
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// Cursor
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("Cursor:"));	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (szCursor[0])
		{
			tstring strText;
			strText.assign(szCursor);		
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// Delta
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("Delta:"));	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (szCursor[0])
		{
			tstring strText;
			strText.assign(szDelta);		
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// GuideLine
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("GuideLine:"));	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		if (szGuideLine[0])
		{
			tstring strText;
			strText.assign(szGuideLine);		
			TextOut(hDC, x, y, strText.c_str(),strText.length());
		}
		y += height;
	}
	//////////////////////////////////////////////////////////////
	/// Property
	{
		x = ORG_X;
		SelectObject(hDC,hFont);
		SetTextColor(hDC,RGB(0,0,0));
		SetBkMode(hDC,TRANSPARENT);
		StringCchPrintf(szBuf, _countof(szBuf), _T("Property:%08X"), fdwProperty);	
		TextOut(hDC, x, y, szBuf, _tcslen(szBuf));
		x += nOffsetX;
		tstring strText;
		ParseProperty(fdwProperty, strText);
		TextOut(hDC, x, y, strText.c_str(), strText.length());	

		y += height;
	}	
	ptImeUIPos.y = y;

	SelectObject(hDC,hOldFont);
	EndPaint(hWnd,&ps);
	return 1;
}

LRESULT HandleCandPaint(HWND hWnd,WPARAM wParam, LPARAM lParam)
{
	HDC hDC = NULL;
	PAINTSTRUCT ps = {0};
	UINT i;
	int x = ORG_X;
	int y = ORG_Y;
	SIZE sz;
	HFONT hOldFont = NULL;
	HFONT hDefFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
	int height,defheight;
	LPDWORD lpdwOffset = NULL;
	RECT rect = {0};
	const TCHAR  szCand[] = TEXT("Candidate List");
	const TCHAR  szCandNull[] = TEXT("");
	const TCHAR  szCandRead[] = TEXT("Reading");
	const TCHAR  szCandCode[] = TEXT("Code");
	const TCHAR  szCandMean[] = TEXT("Meaning");
	const TCHAR  szCandRadi[] = TEXT("Radical");
	const TCHAR  szCandStrk[] = TEXT("Stroke");


	GetClientRect(hWnd,&rect);

	hDC = BeginPaint(hWnd, &ps);

	if (!lpCandList)
	{
		goto pt_cand_10;
	}

	if (hFont)
	{
		hOldFont = (HFONT)SelectObject(hDC,hDefFont);
	}


	// Get the height of the default gui font.
	GetTextExtentPoint(hDC,TEXT("A"),1,&sz);
	defheight = sz.cy + 1;

	// Get the height of the font.
	SelectObject(hDC,hFont);
	GetTextExtentPoint(hDC,TEXT("A"),1,&sz);
	height = sz.cy + 1;

	SelectObject(hDC,hDefFont);
	SetTextColor(hDC,RGB(0,0,0));
	SetBkMode(hDC,TRANSPARENT);
	TextOut(hDC,ORG_X,y,szCand,lstrlen(szCand));
	y += defheight;

	switch (lpCandList->dwStyle)
	{
	case IME_CAND_READ:
		TextOut(hDC,ORG_X,y,szCandRead,lstrlen(szCandRead));
		break;

	case IME_CAND_CODE:
		TextOut(hDC,ORG_X,y,szCandCode,lstrlen(szCandCode));
		break;

	case IME_CAND_MEANING:
		TextOut(hDC,ORG_X,y,szCandMean,lstrlen(szCandMean));
		break;

	case IME_CAND_RADICAL:
		TextOut(hDC,ORG_X,y,szCandRadi,lstrlen(szCandRadi));
		break;

	case IME_CAND_STROKE:
		TextOut(hDC,ORG_X,y,szCandStrk,lstrlen(szCandStrk));
		break;

	default:
		break;

	}
	y += defheight;

	if (!lpCandList->dwCount)
	{
		goto pt_cand_10;
	}

	lpdwOffset = &lpCandList->dwOffset[0];

	lpdwOffset += lpCandList->dwPageStart;

	for (i = lpCandList->dwPageStart;
		(i < lpCandList->dwCount) && 
		(i < lpCandList->dwPageStart + lpCandList->dwPageSize) &&
		(y <= rect.bottom + height); i++)
	{
		LPTSTR lpstr = (LPTSTR)((BYTE *)lpCandList + *lpdwOffset++);

		x = ORG_X;

		SelectObject(hDC,hFont);
		if (i != lpCandList->dwSelection)
		{
			SetTextColor(hDC,RGB(0,0,0));
			SetBkMode(hDC,TRANSPARENT);
		}
		else
		{
			SetTextColor(hDC,RGB(255,255,255));
			SetBkColor(hDC,RGB(0,0,255));
			SetBkMode(hDC,OPAQUE);
		}

		TextOut(hDC,x,y,lpstr,lstrlen(lpstr));
		y += height;
	}


	SelectObject(hDC,hOldFont);

pt_cand_10:
	EndPaint(hWnd,&ps);
	return 1;
}

