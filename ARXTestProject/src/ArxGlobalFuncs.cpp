
#include "pch.h"
#include "framework.h"
#include "ArxGlobalFuncs.h"

AcDbObjectId ARXTest::selectSingleObject()
{
	AcDbObjectId objId;

	ads_name ssname;
	if (RTNORM != acedSSGet(_T(":S"), NULL, NULL, NULL, ssname))
		return objId;

	ads_name ename;
	acedSSName(ssname, 0, ename);

	acdbGetObjectId(objId, ename);

	acedSSFree(ssname);

	return objId;
}

int ARXTest::ssgetObjects(AcDbObjectIdArray& ids)
{
	int nRet = RTNORM;

	ads_name ssname;
	if (RTNORM != (nRet = acedSSGet(NULL, NULL, NULL, NULL, ssname)))
		return nRet;

	Adesk::Int32 nLen = 0;
	acedSSLength(ssname, &nLen);

	for (int i = 0; i < nLen; ++i)
	{
		ads_name ename;
		if (RTNORM != acedSSName(ssname, i, ename))
			continue;

		AcDbObjectId objId;
		if (Acad::eOk == acdbGetObjectId(objId, ename))
			ids.append(objId);
	}

	acedSSFree(ssname);

	return nRet;
}

AcDbObjectId ARXTest::addEntityToModelSpace(AcDbEntity* pEnt)
{
	AcDbObjectId entId;
	if (!pEnt)
		return entId;

	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	AcDbBlockTableRecord* pModelSpace = NULL;
	acdbOpenObject(pModelSpace, acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForWrite);
	if (pModelSpace)
	{
		if (Acad::eOk == pModelSpace->appendAcDbEntity(entId, pEnt))
			pEnt->close();
		pModelSpace->close();
	}

	return entId;
}

Acad::ErrorStatus ARXTest::getViewportBorder(AcDbDatabase* pDb, AcDbObjectId& vbId)
{
    //get paperspace
    AcDbBlockTable* pBlockTable;
    Acad::ErrorStatus es = pDb->getBlockTable(pBlockTable, AcDb::kForRead);
    if (es != Acad::eOk) 
    {
        return es;
    }
    AcDbBlockTableRecord* pBlockTableRecord;
    es = pBlockTable->getAt(ACDB_PAPER_SPACE, pBlockTableRecord, AcDb::kForRead);
    if (es != Acad::eOk) 
    {
        return es;
    }
    pBlockTable->close();

    //iterate through all entities in paperspace
    AcDbBlockTableRecordIterator* pBlockTableRecordIterator;
    es = pBlockTableRecord->newIterator(pBlockTableRecordIterator);
    if (es != Acad::eOk) 
    {
        return es;
    }
    pBlockTableRecord->close();

    //access the entity form the iterator
    CLSID Clsid;
    AcDbEntity* pEntity;
    for (pBlockTableRecordIterator->start(); !pBlockTableRecordIterator->done(); pBlockTableRecordIterator->step()) 
    {
        es = pBlockTableRecordIterator->getEntity(pEntity, AcDb::kForRead);
        if (es != Acad::eOk) 
        {
            delete pBlockTableRecordIterator;
            return es;
        }

        //do something with the entity
        if (pEntity->isA() != AcDbViewport::desc())
        {
            es = pEntity->getClassID(&Clsid);
            vbId = pEntity->objectId();
            pEntity->close();
            break;
        }

        //close the entity
        pEntity->close();
    }

    delete pBlockTableRecordIterator;

    return Acad::eOk;
}
