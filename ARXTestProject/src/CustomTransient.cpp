
#include "pch.h"
#include "framework.h"
#include "dbmleader.h"
#include "CustomTransient.h"

ACRX_NO_CONS_DEFINE_MEMBERS(CustomTransient, AcGiDrawable)

Adesk::Boolean CustomTransient::isPersistent(void) const
{
	return Adesk::kFalse;
}

AcDbObjectId CustomTransient::id(void) const
{
	return AcDbObjectId::kNull;
}

Adesk::UInt32 CustomTransient::subSetAttributes(AcGiDrawableTraits * traits)
{
	//return AcGiDrawable::subSetAttributes(traits);
	return 0;
}

Adesk::Boolean CustomTransient::subWorldDraw(AcGiWorldDraw * wd)
{
	AcGePoint3d pts[4] = { {0, 0, 0}, {100, 0, 0}, {100, 100, 0}, {0, 100, 0} };
	wd->geometry().polygon(4, pts);

	return true;
}

void CustomTransient::subViewportDraw(AcGiViewportDraw * vd)
{

}

