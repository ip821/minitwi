#include "stdafx.h"
#include "ListsService.h"

STDMETHODIMP CListsService::OnInitialized(IServiceProvider *pServiceProvider)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnShutdown() 
{
    return S_OK;
}

STDMETHODIMP CListsService::GetListMemebers(IObjArray** ppArrayMembers)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnStart(IVariantObject* pObject)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnRun(IVariantObject* pObject)
{
    return S_OK;
}

STDMETHODIMP CListsService::OnFinish(IVariantObject* pObject)
{
    return S_OK;
}
