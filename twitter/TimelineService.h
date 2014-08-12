// TimelineService.h : Declaration of the CTimelineService

#pragma once
#include "resource.h"       // main symbols
#include "twitter_i.h"

using namespace ATL;

// CTimelineService

class ATL_NO_VTABLE CTimelineService :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CTimelineService, &CLSID_TimelineService>,
	public ITimelineService
{
public:
	CTimelineService()
	{
	}

	DECLARE_REGISTRY_RESOURCEID(IDR_TIMELINESERVICE)

	BEGIN_COM_MAP(CTimelineService)
		COM_INTERFACE_ENTRY(ITimelineService)
	END_COM_MAP()

public:

	STDMETHOD(Start)();
	STDMETHOD(Stop)();
};

OBJECT_ENTRY_AUTO(__uuidof(TimelineService), CTimelineService)
