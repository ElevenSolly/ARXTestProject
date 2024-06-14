#pragma once
#include "AcDblClkEdit.h"

class MyDoubleClickEdit : public AcDbDoubleClickEdit
{
public:
	ACRX_DECLARE_MEMBERS(MyDoubleClickEdit);

	MyDoubleClickEdit(void);
	~MyDoubleClickEdit(void);
	virtual void startEdit(AcDbEntity *pEnt, AcGePoint3d pt);
	virtual void finishEdit(void);
};
