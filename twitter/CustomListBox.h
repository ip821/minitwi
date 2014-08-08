#pragma once

#include "atlctrls.h"

class CCustomListBox : public CWindowImpl<CCustomListBox, CListBox>
{
public:
	DECLARE_WND_CLASS(_T("WTL_CCustomListBox"))

	BEGIN_MSG_MAP(CCustomListBox)
		
	END_MSG_MAP()
};
