#pragma once

using namespace std;

const map<CString, CString> NormalizeTable = 
{
	{ L"&lt;", L"<" },
	{ L"&gt;", L">" },
	{ L"&amp;", L"&" },
	{ L"&quot;", L"\"" },
};