#pragma once

#include "twitter_i.h"

class UpdateScope
{
public:
	UpdateScope(ITimelineControl* pTimelineControl);
	~UpdateScope();

private:
	ITimelineControl* m_pTimelineControl;
};

UpdateScope::UpdateScope(ITimelineControl* pTimelineControl)
{
	m_pTimelineControl = pTimelineControl;
	m_pTimelineControl->BeginUpdate();
}

UpdateScope::~UpdateScope()
{
	m_pTimelineControl->EndUpdate();
}