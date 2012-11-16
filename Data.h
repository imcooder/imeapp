

#pragma once

#define MSG_NOTIFY_UPDATEUI					(WM_USER + 0x321)
class CGlobalData
{
private:
	CGlobalData()
	{
		m_isShowCand = false;
	}
public:
	static CGlobalData&GetInstance()
	{
		static CGlobalData gd;
		return gd;
	}
	virtual ~CGlobalData()
	{

	}
	void NotifyUpdateUI()
	{
		if (!IsWindow(m_hHost))
		{
			return;
		}
		PostMessage(m_hHost, MSG_NOTIFY_UPDATEUI, 0, 0);
		return;
	}
public:
	tstring m_strCompositionText;
	bool m_isShowCand;
	HWND m_hHost;
};