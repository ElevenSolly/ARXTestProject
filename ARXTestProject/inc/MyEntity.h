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
	bool		m_bColorByObject;	//��ɫ�������������ʣ�true�������false�������
	bool		m_bTexture;			//�Ƿ���������true����������false������������
};

