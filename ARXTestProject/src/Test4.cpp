
#include "pch.h"
#include "framework.h"
#include "ArxGlobalFuncs.h"
#include "Test4.h"
#include "rxmfcapi.h"
#include "MyGiDrawableOverrule.h"
#include "AcDbGeoMap.h"
#include "MyEntity.h"


void Test31()
{
	AcDbDatabase* pDb1 = new AcDbDatabase(false, true);
	Acad::ErrorStatus res = pDb1->readDwgFile(_T("D:\\test-1.dwg"));
	if (Acad::eOk != res)
	{
		delete pDb1;
		return;
	}

	AcDbDatabase* pDb2 = new AcDbDatabase(false, true);
	res = pDb2->readDwgFile(_T("D:\\test-1.dwg"));
	if (Acad::eOk != res)
	{
		delete pDb1;
		delete pDb2;
		return;
	}

	AcDbObjectId entId;
	AcDbBlockTableRecord* pModelSpace = NULL;
	res = acdbOpenObject(pModelSpace, acdbSymUtil()->blockModelSpaceId(pDb1));
	if (pModelSpace)
	{
		AcDbBlockTableRecordIterator* pItr = NULL;
		pModelSpace->newIterator(pItr);
		if (pItr)
		{
			pItr->getEntityId(entId);
			delete pItr;
		}

		pModelSpace->close();
	}

	if (entId.isNull())
	{
		delete pDb1;
		delete pDb2;
		return;
	}

	AcDbObjectIdArray objIds;
	objIds.append(entId);

	AcDbIdPair idPair;
	//idPair.setKey();

	AcDbIdMapping idMap;
	idMap.assign(idPair);
	
	res = pDb2->wblockCloneObjects(objIds, acdbSymUtil()->blockModelSpaceId(pDb2), idMap, AcDb::kDrcReplace);
	if (Acad::eOk == res)
	{
		AcDbIdMappingIter itr(idMap);
	}

	delete pDb1;
	delete pDb2;
}

void Test32()
{
	AcDbDatabase* pDb = acdbCurDwg();
	if (!pDb)
		return;

	AcDbObjectId objId;
	AcDbHandle hl(_T("12B"));
	Acad::ErrorStatus es = pDb->getAcDbObjectId(objId, false, hl);
	if (Acad::eOk != es)
		return;

	AcDbXrecord* pXrec = NULL;
	es = acdbOpenObject(pXrec, objId, AcDb::kForRead);
	if (Acad::eOk != es)
		return;

	resbuf* pRb = NULL;
	es = pXrec->rbChain(&pRb, pDb);
	pXrec->close();
	if (Acad::eOk != es)
		return;

	while (pRb)
	{
		switch (pRb->restype)
		{
		case RTSHORT:
			acutPrintf(_T("\nRTSHORT: %d"), pRb->resval.rint);
			break;
		case RTREAL:
			acutPrintf(_T("\nRTREAL: %f"), pRb->resval.rreal);
			break;
		case RTPOINT:
			acutPrintf(_T("\nRTPOINT: (%f, %f, %f)"), pRb->resval.rpoint[0], pRb->resval.rpoint[1], pRb->resval.rpoint[2]);
			break;
		case RTSTR:
			acutPrintf(_T("\nRTSTR: %s"), pRb->resval.rstring);
			break;
		case RTENAME:
			acutPrintf(_T("\nRTENAME: %d"), pRb->resval.rlname);
			break;
		default:
			break;
		}

		pRb = pRb->rbnext;
	}
}

void Test33()
{
	AcDbObjectId objId = ARXTest::selectSingleObject();
	AcDbPolyline* pRevcloud = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pRevcloud, objId, AcDb::kForWrite);
	if (Acad::eOk != es)
		return;

	AcDbPolyline* pPline = new AcDbPolyline();
	pPline->setDatabaseDefaults(objId.database());
	pPline->setPropertiesFrom(pRevcloud);

	for (unsigned int i = 0; i < pRevcloud->numVerts(); ++i)
	{
		AcGePoint2d pt;
		pRevcloud->getPointAt(i, pt);

		double dBulge = 0.0;
		pRevcloud->getBulgeAt(i, dBulge);

		pPline->addVertexAt(i, pt, dBulge);
	}

	pRevcloud->close();

	AcDbObjectId id = ARXTest::addEntityToModelSpace(pPline);
	if (!id.isNull())
	{
		pPline->close();
	}

}

static MyGiDrawableOverrule* m_psMyGiDrawableOverrule = NULL;
void Test34()
{
	if (!m_psMyGiDrawableOverrule)
	{
		m_psMyGiDrawableOverrule = new MyGiDrawableOverrule();
		AcRxOverrule::addOverrule(AcDbGeoMap::desc(), m_psMyGiDrawableOverrule);
	}
}

void Test35()
{
	if (m_psMyGiDrawableOverrule)
	{
		AcRxOverrule::removeOverrule(AcDbGeoMap::desc(), m_psMyGiDrawableOverrule);
		delete m_psMyGiDrawableOverrule;
		m_psMyGiDrawableOverrule = NULL;
	}
}

void Test36()
{
	MyEntity* pMyEntity = new MyEntity();
	ARXTest::addEntityToModelSpace(pMyEntity);
	pMyEntity->close();
}

void Test37()
{

}

void Test38()
{

}

void Test39()
{

}

void Test40()
{

}
