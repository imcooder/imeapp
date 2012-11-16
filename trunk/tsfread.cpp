
#include "stdafx.h"
#include "tsfread.h"




CTSFRead::CTSFRead()
{
	m_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
	m_dwTextEditSinkCookie = TF_INVALID_COOKIE;
	m_pTextEditSinkContext = NULL;
	m_pThreadMgr = NULL;
	m_nRefCount = 1;
}

//+---------------------------------------------------------------------------
//
// dtor
//
//----------------------------------------------------------------------------

CTSFRead::~CTSFRead()
{
}

//+---------------------------------------------------------------------------
//
// QueryInterface
//
//----------------------------------------------------------------------------

STDAPI CTSFRead::QueryInterface(REFIID riid, void **ppvObj)
{
	if (ppvObj == NULL)
		return E_INVALIDARG;

	*ppvObj = NULL;

	if (IsEqualIID(riid, IID_IUnknown) ||	IsEqualIID(riid, IID_ITfTextEditSink))
	{
		*ppvObj = (ITfTextEditSink *)this;
	}
	else if (IsEqualIID(riid, IID_ITfThreadMgrEventSink))
	{
		*ppvObj = (ITfThreadMgrEventSink *)this;
	}

	if (*ppvObj)
	{
		AddRef();
		return S_OK;
	}
	return E_NOINTERFACE;
}


//+---------------------------------------------------------------------------
//
// AddRef
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) CTSFRead::AddRef()
{
	return ++m_nRefCount;
}

//+---------------------------------------------------------------------------
//
// Release
//
//----------------------------------------------------------------------------

STDAPI_(ULONG) CTSFRead::Release()
{
	LONG cr = --m_nRefCount;

	assert(m_nRefCount >= 0);

	if (m_nRefCount == 0)
	{
		delete this;
	}

	return cr;
}

HRESULT CTSFRead::Initialize()
{
	CComPtr<ITfThreadMgr> pTM;
	HRESULT hr;	
	if (FAILED(hr = CoCreateInstance(CLSID_TF_ThreadMgr, NULL, CLSCTX_INPROC_SERVER, IID_ITfThreadMgr, (void**)&pTM)))
	{
		RETAILMSG(MSG_LEVEL_WARNING, L"[CTSFRead::Initialize] CoCreateInstance Failed[%08X]", hr);
		return hr;
	}
	m_pThreadMgr = pTM;	
	InitThreadMgrSink();
	CComPtr<ITfDocumentMgr>pDocMgr;
	if (SUCCEEDED(hr = pTM->GetFocus(&pDocMgr)))
	{
		if (pDocMgr)
		{
			InitTextEditSink(pDocMgr);			
		}
	}
	return S_OK;
}

HRESULT CTSFRead::Uninitialize()
{
	if (!m_pThreadMgr)
	{
		return S_OK;
	}
	InitTextEditSink(NULL);
	UninitThreadMgrSink();
	m_pThreadMgr.Release();
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnInitDocumentMgr
//
// Sink called by the framework just before the first context is pushed onto
// a document.
//----------------------------------------------------------------------------

STDAPI CTSFRead::OnInitDocumentMgr(ITfDocumentMgr *pDocMgr)
{
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnUninitDocumentMgr
//
// Sink called by the framework just after the last context is popped off a
// document.
//----------------------------------------------------------------------------

STDAPI CTSFRead::OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr)
{
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnSetFocus
//
// Sink called by the framework when focus changes from one document to
// another.  Either document may be NULL, meaning previously there was no
// focus document, or now no document holds the input focus.
//----------------------------------------------------------------------------

STDAPI CTSFRead::OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus)
{	
	InitTextEditSink(pDocMgrFocus);
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnPushContext
//
// Sink called by the framework when a context is pushed.
//----------------------------------------------------------------------------

STDAPI CTSFRead::OnPushContext(ITfContext *pContext)
{
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// OnPopContext
//
// Sink called by the framework when a context is popped.
//----------------------------------------------------------------------------

STDAPI CTSFRead::OnPopContext(ITfContext *pContext)
{
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// InitThreadMgrSink
//
// Advise our sink.
//----------------------------------------------------------------------------

BOOL CTSFRead::InitThreadMgrSink()
{
	if (!m_pThreadMgr)
	{
		return FALSE;
	}
	HRESULT hr = S_OK;
	CComPtr<ITfSource>pSource;
	if (FAILED(hr = m_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource)))
	{
		RETAILMSG(MSG_LEVEL_WARNING, L"[CTSFRead::InitThreadMgrSink] QueryInterface Failed[%08X]", hr);
		return FALSE;
	}
	if (FAILED(hr = pSource->AdviseSink(IID_ITfThreadMgrEventSink, (ITfThreadMgrEventSink *)this, &m_dwThreadMgrEventSinkCookie)))
	{
		RETAILMSG(MSG_LEVEL_WARNING, L"[CTSFRead::InitThreadMgrSink] AdviseSink Failed[%08X]", hr);
		// make sure we don't try to Unadvise m_dwThreadMgrEventSinkCookie later
		m_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
		return FALSE;
	}
	return TRUE;
}

//+---------------------------------------------------------------------------
//
// UninitThreadMgrSink
//
// Unadvise our sink.
//----------------------------------------------------------------------------

void CTSFRead::UninitThreadMgrSink()
{
	if (m_dwThreadMgrEventSinkCookie == TF_INVALID_COOKIE)
		return; // never Advised

	CComPtr<ITfSource>pSource;
	if (m_pThreadMgr->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
	{
		pSource->UnadviseSink(m_dwThreadMgrEventSinkCookie);		
	}
	m_dwThreadMgrEventSinkCookie = TF_INVALID_COOKIE;
}

//+---------------------------------------------------------------------------
//
// OnEndEdit
//
// Called by the system whenever anyone releases a write-access document lock.
//----------------------------------------------------------------------------

STDAPI CTSFRead::OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord)
{
	CheckComposition(pContext, ecReadOnly);
	return S_OK;
}

//+---------------------------------------------------------------------------
//
// InitTextEditSink
//
// Init a text edit sink on the topmost context of the document.
// Always release any previous sink.
//----------------------------------------------------------------------------

BOOL CTSFRead::InitTextEditSink(ITfDocumentMgr *pDocMgr)
{
	CComPtr<ITfSource>pSource;
	BOOL fRet;
	// clear out any previous sink first
	if (m_dwTextEditSinkCookie != TF_INVALID_COOKIE)
	{
		if (m_pTextEditSinkContext->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
		{
			pSource->UnadviseSink(m_dwTextEditSinkCookie);
			pSource.Release();
		}

		m_pTextEditSinkContext.Release();		
		m_dwTextEditSinkCookie = TF_INVALID_COOKIE;
	}

	if (pDocMgr == NULL)
		return TRUE; // caller just wanted to clear the previous sink

	// setup a new sink advised to the topmost context of the document

	if (pDocMgr->GetTop(&m_pTextEditSinkContext) != S_OK)
		return FALSE;

	if (m_pTextEditSinkContext == NULL)
		return TRUE; // empty document, no sink possible

	fRet = FALSE;

	if (m_pTextEditSinkContext->QueryInterface(IID_ITfSource, (void **)&pSource) == S_OK)
	{
		if (pSource->AdviseSink(IID_ITfTextEditSink, (ITfTextEditSink *)this, &m_dwTextEditSinkCookie) == S_OK)
		{
			fRet = TRUE;
		}
		else
		{
			m_dwTextEditSinkCookie = TF_INVALID_COOKIE;
		}
		pSource.Release();
	}

	if (fRet == FALSE)
	{
		m_pTextEditSinkContext.Release();		
	}
	return fRet;
}
//+---------------------------------------------------------------------------
//
// ClearCompositionText
//
//----------------------------------------------------------------------------

void CTSFRead::ClearCompositionText()
{
	m_strCompositionText.clear();
}

//+---------------------------------------------------------------------------
//
// CheckComposition
//
//----------------------------------------------------------------------------

BOOL CTSFRead::CheckComposition(ITfContext *pContext, TfEditCookie ecReadOnly)
{
	HRESULT hr;
	CComPtr<ITfContextComposition>pContextComposition;	
	ClearCompositionText();	
	{
		CComPtr<IEnumTfDisplayAttributeInfo> pEnumDA;
		if (SUCCEEDED(hr = pContext->QueryInterface(IID_IEnumTfDisplayAttributeInfo,	(void **)&pEnumDA)))
		{
			pEnumDA->Reset();
			CComPtr<ITfDisplayAttributeInfo> pInfo;
			while (pEnumDA->Next(1, &pInfo, NULL) == S_OK)
			{
				TF_DISPLAYATTRIBUTE info = {};					
				if (SUCCEEDED(hr = pInfo->GetAttributeInfo(&info)))
				{
					BSTR pDsp = NULL;
					pInfo->GetDescription(&pDsp);
					if (pDsp)
					{
						RETAILMSG(MSG_LEVEL_INFO, L"DisaplyAttr[%s]", pDsp);
						SysFreeString(pDsp);
					}					
				}
				pInfo.Release();
			}
		}
	}
	WCHAR szBuf[1024] = {0};
	if (SUCCEEDED(hr = pContext->QueryInterface(IID_ITfContextComposition,	(void **)&pContextComposition)))
	{
		CComPtr<IEnumITfCompositionView>pEnumCompositionView;		
		if (SUCCEEDED(hr = pContextComposition->EnumCompositions(&pEnumCompositionView)))
		{
			CComPtr<ITfCompositionView>pCompositionView;
			while (pEnumCompositionView->Next(1, &pCompositionView, NULL) == S_OK)
			{
				CComPtr<ITfRange> pRange;				
				if (SUCCEEDED(hr = pCompositionView->GetRange(&pRange)))
				{
					BOOL fEmpty;
					while (pRange->IsEmpty(ecReadOnly, &fEmpty) == S_OK && !fEmpty)
					{
						ULONG uRead = 0;
						pRange->GetText(ecReadOnly, TF_TF_MOVESTART, szBuf, _countof(szBuf) - 1, &uRead);
						m_strCompositionText += szBuf;
					}
					TF_DISPLAYATTRIBUTE info = {
						{ TF_CT_NONE, 0 },			// text color
						{ TF_CT_NONE, 0 },			// background color (TF_CT_NONE => app default)
						TF_LS_NONE,                             // underline style
						FALSE,                                  // underline boldness
						{ TF_CT_NONE, 0 },                      // underline color
						TF_ATTR_OTHER									// attribute info};
					};
					if (SUCCEEDED(GetDispAttrFromRange(pContext, pRange, ecReadOnly, &info)))
					{
						RETAILMSG(MSG_LEVEL_INFO, TEXT("TSF Range Attr: textColor[%d-%08X] bgColor[%d-%08X] underlineStype[%d] underlineBold[%d] underlineColor[%d-%08X] attr[%d]"), info.crText.nIndex, info.crText.cr, info.crBk.nIndex, info.crBk.cr, info.crLine, info.fBoldLine, info.crLine.nIndex, info.crLine.cr, info.bAttr);
					}
				}
				pCompositionView.Release();
			}
		}		
	}
	CGlobalData::GetInstance().m_strCompositionText = m_strCompositionText;
	CGlobalData::GetInstance().NotifyUpdateUI();
	RETAILMSG(MSG_LEVEL_INFO, TEXT("TSF CompStr[%X][%s]"), m_strCompositionText.length(), m_strCompositionText.c_str());
	return TRUE;
}


HRESULT CTSFRead::GetDispAttrFromRange(ITfContext *pContext, ITfRange *pRange, TfEditCookie ec, TF_DISPLAYATTRIBUTE *pDispAttr)
{
	HRESULT     hr;
	//Create the category manager. 
	CComPtr<ITfCategoryMgr>pCategoryMgr;
	if(FAILED(hr = CoCreateInstance(  CLSID_TF_CategoryMgr,	NULL, CLSCTX_INPROC_SERVER, IID_ITfCategoryMgr, (LPVOID*)&pCategoryMgr)))
	{
		RETAILMSG(MSG_LEVEL_WARNING, TEXT("[GetDispAttrFromRange::CTSFRead] CoCreateInstance CLSID_TF_CategoryMgr Failed[%08X]"), hr);
		return hr;
	}

	//Create the display attribute manager. 
	ITfDisplayAttributeMgr  *pDispMgr;
	if(FAILED(hr = CoCreateInstance( CLSID_TF_DisplayAttributeMgr,	NULL, CLSCTX_INPROC_SERVER, IID_ITfDisplayAttributeMgr, (LPVOID*)&pDispMgr)))
	{
		RETAILMSG(MSG_LEVEL_WARNING, TEXT("[GetDispAttrFromRange::CTSFRead] CoCreateInstance CLSID_TF_DisplayAttributeMgr Failed[%08X]"), hr);		
		return hr;
	}

	//Get the display attribute property. 
	CComPtr<ITfProperty> pProp;	
	if(SUCCEEDED(hr = pContext->GetProperty(GUID_PROP_ATTRIBUTE, &pProp)))
	{
		VARIANT var;
		VariantInit(&var);		
		if(S_OK == (hr = pProp->GetValue(ec, pRange, &var)))  //Returns S_FALSE if the range is not completely covered by the property.  
		{
			if(VT_I4 == var.vt)
			{
				//The property is a guidatom. 
				GUID    guid;
				//Convert the guidatom into a GUID. 				
				if(SUCCEEDED(hr = pCategoryMgr->GetGUID((TfGuidAtom)var.lVal, &guid)))
				{
					CComPtr<ITfDisplayAttributeInfo>pDispInfo;
					//Get the display attribute info object for this attribute.
					if(SUCCEEDED(hr = pDispMgr->GetDisplayAttributeInfo(guid, &pDispInfo, NULL)))
					{
						//Get the display attribute info. 
						hr = pDispInfo->GetAttributeInfo(pDispAttr);						
					}
				}
			}
			else
			{
				//An error occurred; GUID_PROP_ATTRIBUTE must always be VT_I4. 
				hr = E_FAIL;
			}
			VariantClear(&var);
		}		
	}
	return hr;
}