
#include "pch.h"
#include "framework.h"
#include "MyDoubleClickEdit.h"

ACRX_NO_CONS_DEFINE_MEMBERS(MyDoubleClickEdit, AcDbDoubleClickEdit)

MyDoubleClickEdit::MyDoubleClickEdit(void)
{

}

MyDoubleClickEdit::~MyDoubleClickEdit(void)
{

}

void MyDoubleClickEdit::startEdit(AcDbEntity *pEnt, AcGePoint3d pt)
{
	if (!pEnt)
		return;

	Acad::ErrorStatus res = acDocManager->lockDocument(curDoc(), AcAp::kWrite);
	if (Acad::eOk != res)
		return;
}

void MyDoubleClickEdit::finishEdit(void)
{

}

