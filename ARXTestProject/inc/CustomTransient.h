#pragma once

#include <drawable.h>

class CustomTransient : public AcGiDrawable
{
public:

	ACRX_DECLARE_MEMBERS(CustomTransient);

	CustomTransient() {}
	~CustomTransient() {}

	virtual Adesk::Boolean  isPersistent(void) const;
	virtual AcDbObjectId    id(void) const;

	virtual Adesk::UInt32   subSetAttributes(AcGiDrawableTraits * traits);
	virtual Adesk::Boolean  subWorldDraw(AcGiWorldDraw * wd);
	virtual void            subViewportDraw(AcGiViewportDraw * vd);
};
