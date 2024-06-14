#pragma once


class MyEntity : public AcDbEntity
{
public:
	ACRX_DECLARE_MEMBERS(MyEntity);

	MyEntity();
	virtual ~MyEntity();

protected:
    virtual Acad::ErrorStatus subGetGripPoints(
		AcGePoint3dArray& gripPoints,
		AcDbIntArray& osnapModes,
		AcDbIntArray& geomIds) const;

	virtual Adesk::Boolean subWorldDraw(AcGiWorldDraw* wd);

	AcDbObjectId getMaterialId(AcDbDatabase* pDb);

private:
	bool		m_bColorByObject;	//颜色是随对象还是随材质，true：随对象，false：随材质
	bool		m_bTexture;			//是否启用纹理，true：启用纹理，false：不启用纹理
};

