#pragma once

#include "twmdl_i.h"

class CUpdateScope
{
public:
	CUpdateScope(ITimelineControl* pTimelineControl)
	{
		m_pTimelineControl = pTimelineControl;
		m_pTimelineControl->BeginUpdate();
	}

	~CUpdateScope()
	{
		m_pTimelineControl->EndUpdate();
	}

private:
	ITimelineControl* m_pTimelineControl;
};

class CDownloadSuspendScope
{
public:
	CDownloadSuspendScope(IDownloadService* pDownloadService)
	{
		m_pDownloadService = pDownloadService;
		m_pDownloadService->SuspendDownloads();
	}

	~CDownloadSuspendScope()
	{
		m_pDownloadService->ResumeDownloads();
	}
private:
	IDownloadService* m_pDownloadService;
};
