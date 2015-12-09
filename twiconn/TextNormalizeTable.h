#pragma once

using namespace std;

#pragma warning(push)
#pragma warning( disable : 4592)
const map<CString, CString> NormalizeTable = 
{
	{ L"&lt;", L"<" },
	{ L"&gt;", L">" },
	{ L"&amp;", L"&" },
	{ L"&quot;", L"\"" },
};
#pragma warning(pop)