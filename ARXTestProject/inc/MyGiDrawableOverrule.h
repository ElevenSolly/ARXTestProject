#pragma once


class MyGiDrawableOverrule : public AcGiDrawableOverrule
{
public:
	ACRX_DECLARE_MEMBERS(MyGiDrawableOverrule);

	bool isApplicable(const AcRxObject* pOverruledSubject) const;

	virtual Adesk::Boolean  worldDraw(AcGiDrawable* pSubject, AcGiWorldDraw* wd);
	virtual void            viewportDraw(AcGiDrawable* pSubject, AcGiViewportDraw* vd);
};

