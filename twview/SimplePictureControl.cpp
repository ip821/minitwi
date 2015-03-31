#include "stdafx.h"
#include "SimplePictureControl.h"

CSimplePictureControl::CSimplePictureControl()
{

}

CSimplePictureControl::~CSimplePictureControl()
{
	if (IsWindow())
		DestroyWindow();
}

STDMETHODIMP CSimplePictureControl::GetHWND(HWND* phWnd)
{
	CHECK_E_POINTER(phWnd);
	*phWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSimplePictureControl::CreateEx(HWND hWndParent, HWND *hWnd)
{
	CRect rectParent;
	::GetClientRect(&rectParent);
	__super::Create(hWndParent, rectParent, 0);
	if (hWnd)
		*hWnd = m_hWnd;
	return S_OK;
}

STDMETHODIMP CSimplePictureControl::PreTranslateMessage(MSG *pMsg, BOOL *bResult)
{
	return S_OK;
}

STDMETHODIMP CSimplePictureControl::SetVariantObject(IVariantObject *pVariantObject)
{
	CHECK_E_POINTER(pVariantObject);
	m_pVariantObject = pVariantObject;
	RETURN_IF_FAILED(m_pAnimationService->SetParams(0, 255, STEPS, TARGET_INTERVAL));
	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	return S_OK;
}

STDMETHODIMP CSimplePictureControl::OnInitialized(IServiceProvider *pServiceProvider)
{
	CHECK_E_POINTER(pServiceProvider);
	m_pServiceProvider = pServiceProvider;
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_ImageManagerService, &m_pImageManagerService));
	RETURN_IF_FAILED(m_pServiceProvider->QueryService(CLSID_AnimationService, &m_pAnimationService));
	RETURN_IF_FAILED(AtlAdvise(m_pAnimationService, pUnk, __uuidof(IAnimationServiceEventSink), &m_dwAdviceAnimationService));
	return S_OK;
}
STDMETHODIMP CSimplePictureControl::OnShutdown()
{
	RETURN_IF_FAILED(AtlUnadvise(m_pAnimationService, __uuidof(IAnimationServiceEventSink), m_dwAdviceAnimationService));
	IInitializeWithControlImpl::OnShutdown();
	m_pAnimationService.Release();
	m_pVariantObject.Release();
	m_pImageManagerService.Release();
	m_pServiceProvider.Release();
	return S_OK;
}

STDMETHODIMP CSimplePictureControl::OnAnimationStep(IAnimationService *pAnimationService, DWORD dwValue, DWORD dwStep)
{
	Invalidate();

	if (dwStep == STEPS)
	{
		return S_OK;
	}

	RETURN_IF_FAILED(m_pAnimationService->StartAnimationTimer());
	return 0;
}
