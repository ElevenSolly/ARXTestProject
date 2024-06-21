
#include "pch.h"
#include "framework.h"
#include "MapTest1.h"
#include "dbGeoData.h"
#include "ArxGlobalFuncs.h"


void MapTest1()
{
	AcDbDatabase* pDb = acdbCurDwg();
	if (!pDb)
		return;

	AcDbBlockTableRecord* pModelSpace = NULL;
	Acad::ErrorStatus res = acdbOpenObject(pModelSpace, acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForRead);
	if (Acad::eOk != res)
		return;

	AcDbObjectId geoDataObjId;
	AcDbDictionary* pDict = NULL;
	res = acdbOpenObject(pDict, pModelSpace->extensionDictionary(), AcDb::kForRead);
	if (Acad::eOk == res)
	{
		pDict->getAt(_T("ACAD_GEOGRAPHICDATA"), geoDataObjId);
		pDict->close();
	}

	pModelSpace->close();

	if (geoDataObjId.isNull())
		return;

	AcDbGeoData* pGeoData = NULL;
	res = acdbOpenObject(pGeoData, geoDataObjId, AcDb::kForRead);
	if (Acad::eOk != res)
		return;

	AcGePoint2dArray sourcePts, destPts;
	res = pGeoData->getMeshPointMaps(sourcePts, destPts);
	if (Acad::eOk == res)
	{
		//sourcePts
		AcDbPolyline* pSrcPline = new AcDbPolyline();
		pSrcPline->setNormal(AcGeVector3d(0.0, 0.0, 1.0));

		AcCmColor clr;
		clr.setColorIndex(1);
		pSrcPline->setColor(clr);

		int iLen = sourcePts.length();
		for (int i = 0; i < iLen; ++i)
		{
			pSrcPline->setPointAt(i, sourcePts.at(i));
			pSrcPline->addVertexAt();
		}

		if (!ARXTest::addEntityToModelSpace(pSrcPline).isNull())
		{
			pSrcPline->close();
		}
		else
		{
			delete pSrcPline;
			pSrcPline = NULL;
		}

		//destPts
		AcDbPolyline* pDstPline = new AcDbPolyline();
		pDstPline->setNormal(AcGeVector3d(0.0, 0.0, 1.0));

		clr.setColorIndex(2);
		pDstPline->setColor(clr);

		iLen = destPts.length();
		for (int i = 0; i < iLen; ++i)
		{
			pDstPline->setPointAt(i, destPts.at(i));
		}

		if (!ARXTest::addEntityToModelSpace(pDstPline).isNull())
		{
			pDstPline->close();
		}
		else
		{
			delete pDstPline;
			pDstPline = NULL;
		}
	}

	pGeoData->close();
}


