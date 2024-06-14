
#include "pch.h"
#include "framework.h"
#include "resource.h"

#include "MyEntity.h"
#include <atlimage.h>


ACRX_CONS_DEFINE_MEMBERS(MyEntity, AcDbEntity, AcDb::kMReleaseCurrent)


MyEntity* g_pEntity = new MyEntity();


MyEntity::MyEntity()
	: m_bColorByObject(false)
	, m_bTexture(true)
{
}

MyEntity::~MyEntity()
{
}

Acad::ErrorStatus MyEntity::subGetGripPoints(AcGePoint3dArray& gripPoints, AcDbIntArray& osnapModes, AcDbIntArray& geomIds) const
{

	return Acad::ErrorStatus();
}

Adesk::Boolean MyEntity::subWorldDraw(AcGiWorldDraw* wd)
{
	assertReadEnabled();

	if (!wd)
		return Adesk::kTrue;

	if (!(wd->context()))
		return Adesk::kTrue;

	AcDbDatabase* pDb = wd->context()->database();
	if (!pDb)
		return Adesk::kTrue;

	//获取并设置材质
	AcDbObjectId materialId = getMaterialId(pDb);
	if (materialId.isNull())
		return Adesk::kTrue;

	wd->subEntityTraits().setMaterial(materialId);
	//wd->subEntityTraits().setColor(1);

	//绘制
	AcGePoint3dArray vertexArray;
	vertexArray.append(AcGePoint3d(0.0, 0.0, 0.0));
	vertexArray.append(AcGePoint3d(100.0, 0.0, 0.0));
	vertexArray.append(AcGePoint3d(100.0, 100.0, 0.0));
	vertexArray.append(AcGePoint3d(0.0, 100.0, 0.0));
	//vertexArray.append(AcGePoint3d(0.0, 0.0, 0.0));

	Adesk::UInt32 face_list_size = 5;
	Adesk::Int32* face_list = new Adesk::Int32[face_list_size];
	face_list[0] = 4;
	for (int i = 0; i < 4; ++i)
	{
		face_list[i + 1] = i;
	}

	wd->geometry().shell(4, vertexArray.asArrayPtr(), face_list_size, face_list);
	delete[] face_list;

	return Adesk::kTrue;
}

AcDbObjectId MyEntity::getMaterialId(AcDbDatabase* pDb)
{
	AcDbObjectId materialId;
	AcDbDictionary* pDict = NULL;
	Acad::ErrorStatus es = pDb->getMaterialDictionary(pDict, AcDb::kForRead);	//material object open.
	if (Acad::eOk != es || !pDict)
		return materialId;

	AcString sMaterialName = _T("MyMaterial");
	if (!pDict->has(sMaterialName.kTCharPtr()))
	{//new create a material object.
		AcDbMaterial* pMaterialObj = new AcDbMaterial();
		pMaterialObj->setName(sMaterialName.kTCharPtr());

		AcGiMaterialColor diffuseColor;
		if (false)	//随材质
		{
			int iRed(0), iGreen(0), iBlue(0);
			AcCmEntityColor cColor(iRed, iGreen, iBlue);
			//cColor.setColorMethod(AcCmEntityColor::kByColor);

			diffuseColor.setMethod(AcGiMaterialColor::Method::kOverride);
			diffuseColor.setColor(cColor);

			AcGiMaterialMap map;
			pMaterialObj->setDiffuse(diffuseColor, map);
			pMaterialObj->setAmbient(diffuseColor);
		}

		if (true)		//启用纹理
		{
			AcGiMaterialMap map;
			map.setSource(AcGiMaterialMap::Source::kFile);

			double bBlendFactor = 1.0;
			map.setBlendFactor(bBlendFactor);

			AcString sSrcFile = _T("C:\\Users\\ElevenSolly\\Pictures\\test.png");
			AcGiImageFileTexture* pMaterialTexture = new AcGiImageFileTexture();
			pMaterialTexture->setSourceFileName(sSrcFile.kTCharPtr());
			map.setTexture(pMaterialTexture);

			/*int iWidth(256), iHeight(256);
			CImage img;
			HRESULT hRet = img.Load(sSrcFile.kTCharPtr());
			if (SUCCEEDED(hRet))
			{
				iWidth = img.GetWidth();
				iHeight = img.GetHeight();
			}
			img.Destroy();*/

			AcGiMapper mapper;
			/*AcGeMatrix3d matrix;
			matrix = matrix.setToScaling(1.0 / iWidth);*/
			mapper.setProjection(AcGiMapper::kBox);
			//mapper.setAutoTransform(AcGiMapper::kModel);
			map.setMapper(mapper);

			pMaterialObj->setDiffuse(diffuseColor, map);
		}

		//光泽度
		double dGloss = 0.55;
		AcGiMaterialMap specularMap;
		pMaterialObj->setSpecular(diffuseColor, specularMap, dGloss);
		pMaterialObj->setIlluminationModel(AcGiMaterialTraits::IlluminationModel::kBlinnShader);

		//反射率
		double dReflectivity = 0.01;
		pMaterialObj->setReflectivity(dReflectivity);

		//透明度
		double dPercentage = 1;
		AcGiMaterialMap opacityMap;
		pMaterialObj->setOpacity(dPercentage, opacityMap);

		//半透明度
		double dTranslucence = 0.0;
		pMaterialObj->setTranslucence(dTranslucence);

		AcGiMaterialMap refractionMap;
		double dRefraction = 1.08;
		pMaterialObj->setRefraction(dRefraction, refractionMap);

		es = pDict->upgradeOpen();
		if (Acad::eOk == es)
		{
			pDict->setAt(sMaterialName.kTCharPtr(), pMaterialObj, materialId);
			pMaterialObj->close();
		}
		else
		{
			delete pMaterialObj;
		}
	}
	else
	{
		pDict->getAt(sMaterialName.kTCharPtr(), materialId);
	}

	pDict->close();	//material object close.

	return materialId;
}
