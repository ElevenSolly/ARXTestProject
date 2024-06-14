
#include "pch.h"
#include "framework.h"
#include "MyGiDrawableOverrule.h"

ACRX_NO_CONS_DEFINE_MEMBERS(MyGiDrawableOverrule, AcGiDrawableOverrule)

bool MyGiDrawableOverrule::isApplicable(const AcRxObject* pOverruledSubject) const
{
	return true;
}

Adesk::Boolean MyGiDrawableOverrule::worldDraw(AcGiDrawable* pSubject, AcGiWorldDraw* wd)
{
	Adesk::Boolean bRet = AcGiDrawableOverrule::worldDraw(pSubject, wd);
	return bRet;
}

void MyGiDrawableOverrule::viewportDraw(AcGiDrawable* pSubject, AcGiViewportDraw* vd)
{
	AcGiDrawableOverrule::viewportDraw(pSubject, vd);
}
