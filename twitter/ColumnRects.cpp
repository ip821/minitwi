// ColumnRects.cpp : Implementation of CColumnRects

#include "stdafx.h"
#include "ColumnRects.h"


// CColumnRects

STDMETHODIMP CColumnRects::AddRect(RECT rect)
{
	m_rects.push_back(rect);
	return S_OK;
}

STDMETHODIMP CColumnRects::GetRect(UINT uiIndex, RECT* rect)
{
	*rect = m_rects[uiIndex];
	return S_OK;
}

STDMETHODIMP CColumnRects::GetCount(UINT* puiCount)
{
	*puiCount = m_rects.size();
	return S_OK;
}

STDMETHODIMP CColumnRects::Clear()
{
	m_rects.clear();
	return S_OK;
}