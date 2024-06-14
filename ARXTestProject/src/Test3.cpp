
#include "pch.h"
#include "framework.h"
#include "ArxGlobalFuncs.h"
#include "CustomTransient.h"
#include "rxmfcapi.h"
#include "dbmleader.h"
#include <atlcom.h>
#include <dynprops.h>
#include "acgitransient.h"


void Test21()
{
	AcDbObjectId objId1 = ARXTest::selectSingleObject();
	if (objId1.isNull())
		return;

	AcDbObjectId objId2 = ARXTest::selectSingleObject();
	if (objId2.isNull())
		return;

	ads_point pt1 = { 2693.5, 1385.05, 0 };
	ads_point pt2 = { 3113.5, 1682.05, 0 };

	resbuf* pRb = acutBuildList(1001, _T("GENIUS_GENOREF_13"), 1005, _T("40A"),
		1001, _T("GENIUS_GENOBJ-N-SDF_13"), 1002, _T("{"), 1000, _T("OWNER"), 1000, _T("GEN"), 1002, _T("}"),
		1001, _T("GENIUS_GENODEF_13"), 1002, _T("{"), 1000, _T("HIER"), 1070, 0, 1002, _T("}"),
		1002, _T("{"), 1000, _T("MANEDIT"), 1070, 0, 1002, _T("}"),
		1002, _T("{"), 1000, _T("NOSEL"), 1070, 0, 1002, _T("}"),
		1002, _T("{"), 1000, _T("BEGIND"), 1070, 0, 1002, _T("}"),
		1002, _T("{"), 1000, _T("OTHER"), 1070, 0, 1002, _T("}"),
		1002, _T("{"), 1000, _T("REACTOR"), 1002, _T("}"),
		1001, _T("GENIUS_GENSCREG_15"), 1040, 1.0, 1040, 1.0, 1011, pt1, 1011, pt2, 1070, 3,
		RTNONE);

	AcDbObject* pObj1 = NULL;
	acdbOpenObject(pObj1, objId1, AcDb::kForRead);
	if (pObj1)
	{
		resbuf* pRb = pObj1->xData();
		pObj1->close();
		if (pRb)
		{
			AcDbObject* pObj2 = NULL;
			acdbOpenObject(pObj2, objId2, AcDb::kForWrite);
			if (pObj2)
			{
				pObj2->setXData(pRb);
				pObj2->close();
			}
		}
	}
	
	AcDbObject* pObj3 = NULL;
	acdbOpenObject(pObj3, objId1, AcDb::kForWrite);
	if (pObj3)
	{
		pObj3->setXData(pRb);
		pObj3->close();
	}
}

void Test22()
{
	AcGiTransientManager* pManager = acgiGetTransientManager();
	if (!pManager)
		return;

	//CustomTransient* pCustomTransient = new CustomTransient;
	//AcDbIntArray arr;
	//pManager->addTransient(pCustomTransient, kAcGiHighlight, 0x80, arr);

	AcDbPolyline* pPline = new AcDbPolyline;
	pPline->setNormal(AcGeVector3d(0, 0, 1));
	pPline->addVertexAt(0, AcGePoint2d(0, 0));
	pPline->addVertexAt(1, AcGePoint2d(100, 0));
	pPline->addVertexAt(2, AcGePoint2d(100, 100));
	pPline->addVertexAt(3, AcGePoint2d(0, 100));
	pPline->setClosed(true);
	AcDbIntArray arr;
	pManager->addTransient(pPline, kAcGiHighlight, 0x80, arr);
}

void Test23()
{
	AcDbDatabase* pDb = new AcDbDatabase();
	if (!pDb)
		return;

	if (Acad::eOk != pDb->readDwgFile(_T("C:\\Users\\localuser\\Desktop\\Drawing1.dwg")))
	{
		delete pDb;
		return;
	}

	AcDbDictionary* pNamedDic = NULL;
	pDb->getNamedObjectsDictionary(pNamedDic);
	if (!pNamedDic)
		return;

	AcDbDictionary* pFeatureLayersDic = NULL;
	pNamedDic->getAt(_T("ESRI_WEBFEATURELAYERS"), pFeatureLayersDic);
	pNamedDic->close();

	if (!pFeatureLayersDic)
		return;

	AcDbDictionary* pFeatureLayerDic = NULL;
	pFeatureLayersDic->getAt(_T("USA_States_Generalized"), pFeatureLayerDic, AcDb::kForWrite);
	pFeatureLayersDic->close();

	if (!pFeatureLayerDic)
		return;

	AcDbObjectId dicId;
	AcDbDictionary* pNewDic = new AcDbDictionary;
	if (Acad::eOk == pFeatureLayerDic->setAt(_T("ESRI_ATTRIBUTES"), pNewDic, dicId))
	{
		pNewDic->close();
	}
	else
	{
		delete pNewDic;
	}

	pDb->saveAs(_T("C:\\Users\\localuser\\Desktop\\Drawing1_save.dwg"));
	delete pDb;
}

void Test24()
{

}

void Test25()
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if (!pDb)
		return;

	ACHAR chInput[128] = { 0 };
	if (RTNORM != acedGetString(0, _T("\nInput Handle: "), chInput))
		return;

	AcDbHandle hdl(chInput);
	AcDbObjectId objId;
	if (Acad::eOk != pDb->getAcDbObjectId(objId, false, hdl))
		return;

	AcString sName, sDxfName;
	AcDbObject* pObj = NULL;
	acdbOpenObject(pObj, objId, AcDb::kForRead, true);
	if (pObj)
	{
		acutPrintf(_T("\nName: %s"), sName = pObj->isA()->name());
		sDxfName = pObj->isA()->dxfName();
		acutPrintf(_T("\nDXFName: %s"), sDxfName.kTCharPtr());
		pObj->ownerId().handle().print();
		acutPrintf(_T("\nHandle: %d"), (Adesk::UInt64)(pObj->ownerId().handle()));
		pObj->close();
	}
}

extern void WblockTest();
void Test26()
{
	WblockTest();
}

void Test27()
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if (!pDb)
		return;

	AcDbLine* pLine = new AcDbLine;
	pLine->setDatabaseDefaults(pDb);
	pLine->setColorIndex(2);
	pLine->setStartPoint(AcGePoint3d(0.0, 0.0, 0.0));
	pLine->setEndPoint(AcGePoint3d(100.0, 0.0, 0.0));
	pLine->setNormal(AcGeVector3d(0.0, 0.0, 1.0));

	pDb->disableUndoRecording(true);

	AcDbBlockTableRecord* pModelSpace = NULL;
	acdbOpenObject(pModelSpace, acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForWrite);
	if (pModelSpace)
	{
		if (Acad::eOk == pModelSpace->appendAcDbEntity(pLine))
		{
			pLine->close();
		}
		else
		{
			delete pLine;
		}
		pModelSpace->close();
		pLine = NULL;
	}

	pDb->disableUndoRecording(false);

	if (pLine)
		delete pLine;
}

#include "MyDoubleClickEdit.h"
void Test28()
{
	AcAp::DocLockMode mode = AcAp::kXWrite;
	Acad::ErrorStatus res = acDocManager->lockDocument(curDoc(), mode);
	if (Acad::eOk != res)
		return;

	mode = curDoc()->lockMode(true);

	mode = AcAp::kWrite;
	res = acDocManager->lockDocument(curDoc(), mode);
	if (Acad::eOk != res)
		return;

	mode = curDoc()->lockMode(true);
}

void Test29()
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr))
	{
		acutPrintf(_T("\n初始化COM环境失败！"));
		return;
	}

	IPropertyManager* pManager = nullptr, *pManager2 = nullptr;
	OPMPropertyExtensionFactory* pOPMPropertyExtensionFactory = GET_OPMEXTENSION_CREATE_PROTOCOL();
	//OPMPropertyExtension* pExtension = pOPMPropertyExtensionFactory->CreateOPMObjectProtocol(AcDbEntity::desc() , 2);
	//OPMPropertyExtension* pExtension = pOPMPropertyExtensionFactory->CreateOPMObjectProtocol(AcDbDatabase::desc() , 2);

	AcRxClass* pRxClass = NULL;
	LONG lResered = 2;
	OPMPropertyExtension* pExtension = pOPMPropertyExtensionFactory->CreateOPMObjectProtocol(pRxClass, lResered);
	if (pExtension)
		pManager = pExtension->GetPropertyManager();
	else
	{
		acutPrintf(_T("\npExtension is NULL!"));
	}

	// 释放COM环境
	CoUninitialize();
}

void Test30()
{
	AcDbDatabase* pDb = acdbCurDwg();
	if (!pDb)
		return;

	AcGiTransientManager* pTransientManager = acgiGetTransientManager();
	if (!pTransientManager)
		return;

	AcDbDictionary* pNamedDict = NULL;
	Acad::ErrorStatus res = pDb->getNamedObjectsDictionary(pNamedDict, AcDb::kForWrite);
	if (Acad::eOk != res || !pNamedDict)
		return;

	AcDbDictionary* pImageDict = NULL;
	if (pNamedDict->has(_T("ACAD_IMAGE_DICT")))
	{
		res = pNamedDict->getAt(_T("ACAD_IMAGE_DICT"), pImageDict, AcDb::kForWrite);
		pNamedDict->close();
		if (Acad::eOk != res || pImageDict)
			return;
	}
	else
	{
		AcDbObjectId dictid;
		pImageDict = new AcDbDictionary;
		res = pNamedDict->setAt(_T("ACAD_IMAGE_DICT"), pImageDict, dictid);
		pNamedDict->close();
		if (Acad::eOk != res)
		{
			delete pImageDict;
			return;
		}
	}

	AcDbRasterImageDef* pImageDef = new AcDbRasterImageDef;
	pImageDef->setSourceFileName(_T("C:\\Users\\localuser\\Pictures\\top_blue_selected.png"));
	pImageDef->load();

	AcDbObjectId imgDef;
	res = pImageDict->setAt(_T("esriImage"), pImageDef, imgDef);
	pImageDict->close();
	if (Acad::eOk != res)
	{
		delete pImageDef;
		return;
	}
	else
	{
		pImageDef->close();
	}

	AcDbRasterImage* pImage = new AcDbRasterImage;
	pImage->setDatabaseDefaults();
	pImage->setImageDefId(imgDef);
	pImage->setDisplayOpt(AcDbRasterImage::kTransparent, false);

	AcArray<int> vpArray;
	vpArray.append(0);
	vpArray.append(1);
	vpArray.append(2);

	pTransientManager->addTransient(pImage, AcGiTransientDrawingMode::kAcGiMain, 0, vpArray);
}
