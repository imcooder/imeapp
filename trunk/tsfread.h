
#ifndef _TSFREAD_H_
#define _TSFREAD_H_
#include <msctf.h>
class CCompositionPopup;

class CTSFRead : public ITfThreadMgrEventSink, 
	public ITfTextEditSink
{
public:
	CTSFRead();
	~CTSFRead();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObj);
	STDMETHODIMP_(ULONG) AddRef(void);
	STDMETHODIMP_(ULONG) Release(void);

	// ITfThreadMgrEventSink
	STDMETHODIMP OnInitDocumentMgr(ITfDocumentMgr *pDocMgr);
	STDMETHODIMP OnUninitDocumentMgr(ITfDocumentMgr *pDocMgr);
	STDMETHODIMP OnSetFocus(ITfDocumentMgr *pDocMgrFocus, ITfDocumentMgr *pDocMgrPrevFocus);
	STDMETHODIMP OnPushContext(ITfContext *pContext);
	STDMETHODIMP OnPopContext(ITfContext *pContext);

	// ITfTextEditSink
	STDMETHODIMP OnEndEdit(ITfContext *pContext, TfEditCookie ecReadOnly, ITfEditRecord *pEditRecord);

	BOOL InitThreadMgrSink();
	void UninitThreadMgrSink();
	BOOL InitTextEditSink(ITfDocumentMgr *pDocMgr);

	void AppendCompositionText(ITfRange *pRange, TfEditCookie ecReadOnly);
	void ClearCompositionText();
	BOOL CheckComposition(ITfContext *pContext, TfEditCookie ecReadOnly);
	HRESULT Initialize();
	HRESULT Uninitialize();
private:
	DWORD m_dwThreadMgrEventSinkCookie;
	DWORD m_dwTextEditSinkCookie;
	CComPtr<ITfContext>m_pTextEditSinkContext;
	CComPtr<ITfThreadMgr> m_pThreadMgr;

	std::wstring m_strCompositionText;
	LONG m_nRefCount;
};


#endif _TSFREAD_H_
