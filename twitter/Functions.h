#pragma once

#include <comdef.h>

#include "twconn_contract_i.h"
#include "twmdl_contract_i.h"
#include "..\model-libs\objmdl\Macros.h"
#include "..\model-libs\objmdl\Functions.h"
#include "..\twiconn\Metadata.h"
#include "..\twiconn\Plugins.h"

using namespace IP;

static HRESULT HrOpenConnection(ISettings* pSettings, ITwitterConnection** ppConnection)
{
    CComPtr<ISettings> pSettingsTwitter;
    RETURN_IF_FAILED(pSettings->OpenSubSettings(Twitter::Metadata::Settings::PathRoot, &pSettingsTwitter));

    CComBSTR bstrKey;
    RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterKey, &bstrKey));

    CComBSTR bstrSecret;
    RETURN_IF_FAILED(HrSettingsGetBSTR(pSettingsTwitter, Twitter::Metadata::Settings::Twitter::TwitterSecret, &bstrSecret));

    CComPtr<ITwitterConnection> pConnection;
    RETURN_IF_FAILED(HrCoCreateInstance(CLSID_TwitterConnection, &pConnection));
    RETURN_IF_FAILED(pConnection->OpenConnection(bstrKey, bstrSecret));
    RETURN_IF_FAILED(pConnection->QueryInterface(ppConnection));
    return S_OK;
}