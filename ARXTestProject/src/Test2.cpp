
#include "pch.h"
#include "framework.h"
#include "ArxGlobalFuncs.h"
#include <fstream>
#include <iostream>
#include <AcTcUiScheme.h>
#include "AcTcUiManager.h"

using namespace ARXTest;

void Test11()
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();

	AcDbBlockTableRecord* pModelSpace = NULL;
	acdbOpenObject(pModelSpace, acdbSymUtil()->blockModelSpaceId(pDb), AcDb::kForRead);
	if (pModelSpace)
	{
		AcDbBlockTableRecordIterator* pIterator = NULL;
		pModelSpace->newIterator(pIterator);
		if (pIterator)
		{
			for (; !pIterator->done(); pIterator->step())
			{
				AcDbObjectId entId;
				pIterator->getEntityId(entId);
				if (entId.isNull())
					continue;

				AcDbExtents ext;
				AcDbText* pText = NULL;
				acdbOpenObject(pText, entId, AcDb::kForRead);
				if (pText)
				{
					pText->getGeomExtents(ext);
					pText->close();
				}
			}
			delete pIterator;
		}
		pModelSpace->close();
	}

	AcDbText* pText = new AcDbText;
	pText->setDatabaseDefaults(pDb);
	pText->setTextString(_T(" "));

	AcDbObjectId txtId = addEntityToModelSpace(pText);
	if (txtId.isNull())
	{
		delete pText;
		return;
	}

	AcDbExtents ext;
	acdbOpenObject(pText, txtId, AcDb::kForRead);
	if (pText)
	{
		pText->getGeomExtents(ext);
		pText->close();
	}
}

void Test12()
{
	AcEdCommand* pCommand = acedRegCmds->lookupGlobalCmd(_T("zoom"));
	if (pCommand)
	{
		Adesk::Int32 nFlag = pCommand->commandFlags();
		bool bVal = !!(nFlag & ACRX_CMD_REDRAW);
		bVal = !!(nFlag & ACRX_CMD_USEPICKSET);
		bVal = !!(nFlag & ACRX_CMD_MODAL);
		bVal = !!(nFlag & ACRX_CMD_UNDEFINED);
		bVal = !!(nFlag & ACRX_CMD_DEFUN);
	}
}

void Test13()
{
	ads_name ssname;
	if (RTNORM != acedSSGet(NULL, NULL, NULL, NULL, ssname))
		return;

	acedSSSetFirst(ssname, NULL);

	ads_ssfree(ssname);
}

void Test14()
{
	resbuf *pRbSZ = NULL;
	if (RTNORM != acedGetFileNavDialog(_T("选择深圳坐标图纸"), _T(""), _T("dwg"), _T("坐标转换"), 0, &pRbSZ) || !pRbSZ)
		return;

	resbuf *pRbGS2000 = NULL;
	if (RTNORM != acedGetFileNavDialog(_T("选择大地2000坐标图纸"), _T(""), _T("dwg"), _T("坐标转换"), 0, &pRbGS2000) || !pRbGS2000)
		return;

	AcString path1 = pRbSZ->resval.rstring;
	acutRelRb(pRbSZ);

	AcString path2 = pRbGS2000->resval.rstring;
	acutRelRb(pRbGS2000);

	AcDbDatabase* pShenZhenDb = new AcDbDatabase;
	pShenZhenDb->readDwgFile(path1.kTCharPtr());

	AcDbDatabase* pGS2000Db = new AcDbDatabase;
	pGS2000Db->readDwgFile(path2.kTCharPtr());

	AcGePoint3d ptGS2000;
	AcGeMatrix3d szGS2000Mat;
	AcDbBlockTableRecord* pGS2000ModelSpace = NULL;
	acdbOpenObject(pGS2000ModelSpace, acdbSymUtil()->blockModelSpaceId(pGS2000Db), AcDb::kForRead);
	if (pGS2000ModelSpace)
	{
		AcDbBlockTableRecordIterator* pIter = NULL;
		pGS2000ModelSpace->newIterator(pIter);
		if (pIter)
		{
			AcDbEntity* pEnt = NULL;
			pIter->getEntity(pEnt, AcDb::kForRead);
			if (pEnt)
			{
				if (pEnt->isKindOf(AcDbPolyline::desc()))
				{
					AcDbPolyline* pPline = AcDbPolyline::cast(pEnt);

					AcGePoint3d pt1, pt2;
					pPline->getPointAt(0, pt1);
					pPline->getPointAt(1, pt2);

					ptGS2000 = pt1;

					AcGeVector3d vX = pt2 - pt1;
					vX.normalize();

					AcGeVector3d vY = vX.perpVector();
					vY.normalize();

					AcGeVector3d vZ = vX.crossProduct(vY);

					szGS2000Mat.setCoordSystem(AcGePoint3d::kOrigin, vX, vY, vZ);
				}
				pEnt->close();
			}
			delete pIter;
		}
		pGS2000ModelSpace->close();
	}

	AcDbBlockTableRecord* pShenZhenModelSpace = NULL;
	acdbOpenObject(pShenZhenModelSpace, acdbSymUtil()->blockModelSpaceId(pShenZhenDb), AcDb::kForRead);
	if (pShenZhenModelSpace)
	{
		AcDbBlockTableRecordIterator* pIter = NULL;
		pShenZhenModelSpace->newIterator(pIter);
		if (pIter)
		{
			AcDbEntity* pEnt = NULL;
			pIter->getEntity(pEnt, AcDb::kForWrite);
			if (pEnt)
			{
				if (pEnt->isKindOf(AcDbPolyline::desc()))
				{
					AcDbPolyline* pPline = AcDbPolyline::cast(pEnt);

					AcGePoint3d pt1, pt2;
					pPline->getPointAt(0, pt1);
					pPline->getPointAt(1, pt2);

					AcGeVector3d vX = pt2 - pt1;
					vX.normalize();

					AcGeVector3d vY = vX.perpVector();
					vY.normalize();

					AcGeVector3d vZ = vX.crossProduct(vY);

					AcGeMatrix3d szMat;
					szMat.setCoordSystem(AcGePoint3d::kOrigin, vX, vY, vZ);

					AcGeMatrix3d szOffsetMat;
					szOffsetMat.setTranslation(ptGS2000 - pt1);

					AcGeMatrix3d transMat1;
					transMat1.setToTranslation(-pt1.asVector());
					AcGeMatrix3d transMat2;
					transMat2.setToTranslation(pt1.asVector());

					AcGeMatrix3d szTranMat = szOffsetMat * transMat2 * szMat.inverse() * szGS2000Mat * transMat1;
					pPline->transformBy(szTranMat);
				}
				pEnt->close();
			}
			delete pIter;
		}
		pShenZhenModelSpace->close();
	}

	AcString path3;
	if (RTNORM != acedGetString(0, _T("\n输入另存为文件路径："), path3))
		return;

	pShenZhenDb->saveAs(path3.kTCharPtr());
}

int readLayerList(AcStringArray& strArrValues)
{
	AcString strFilePath;
	if (Acad::eOk != acdbHostApplicationServices()->findFile(strFilePath, _T("layerlist.ini")))
		return -1;

	int nRetCode = 0;
	CString strKeyIndex;
	AcString strValue;
	TCHAR Buf[1024] = { 0 };
	DWORD dwRtCode = 0;
	INT nCountTotal = 0;
	dwRtCode = GetPrivateProfileInt(_T("NeedTransformLayerList"), _T("LayerlistCount"), nCountTotal, strFilePath.kTCharPtr());
	if (dwRtCode > 0)
		nCountTotal = dwRtCode;

	for (int i = 0; i < nCountTotal; ++i)
	{
		CString strIndex;
		strIndex.Format(_T("%d"), i);
		strKeyIndex = _T("Layer") + strIndex;
		dwRtCode = GetPrivateProfileString(_T("NeedTransformLayerList"), strKeyIndex, NULL, Buf, 1024, strFilePath.kTCharPtr());
		if (dwRtCode == 0)
			continue;

		strValue = Buf;
		strValue.trim();
		if (!strValue.isEmpty())
		{
			strArrValues.append(strValue);
			nRetCode = 1;
		}
	}

	return nRetCode;
}

void covertSZToEarth(bool bCheckLayerList)
{
	AcGeMatrix3d mat;
	//mat.entry[0][0] = 0.99985447554714357;
	//mat.entry[0][1] = -0.017059534822107925;
	//mat.entry[0][2] = 0.00000000000000000;
	//mat.entry[0][3] = 391090.34252006118;
	//mat.entry[1][0] = 0.017059534822108147;
	//mat.entry[1][1] = 0.99985447554714346;
	//mat.entry[1][2] = 0.00000000000000000;
	//mat.entry[1][3] = 2472660.5165160755;
	//mat.entry[2][0] = 0.00000000000000000;
	//mat.entry[2][1] = 0.00000000000000000;
	//mat.entry[2][2] = 0.99999999999999989;
	//mat.entry[2][3] = 0.00000000000000000;
	//mat.entry[3][0] = 0.00000000000000000;
	//mat.entry[3][1] = 0.00000000000000000;
	//mat.entry[3][2] = 0.00000000000000000;
	//mat.entry[3][3] = 1.0000000000000000;

	mat.entry[0][0] = 0.99985447554819507;
	mat.entry[0][1] = -0.017059534760457032;
	mat.entry[0][2] = 0.00000000000000000;
	mat.entry[0][3] = 391090.33528273401;
	mat.entry[1][0] = 0.017059534760457032;
	mat.entry[1][1] = 0.99985447554819507;
	mat.entry[1][2] = 0.00000000000000000;
	mat.entry[1][3] = 2472660.4789712704;
	mat.entry[2][0] = 0.00000000000000000;
	mat.entry[2][1] = 0.00000000000000000;
	mat.entry[2][2] = 0.99999999999999944;
	mat.entry[2][3] = 0.00000000000000000;
	mat.entry[3][0] = 0.00000000000000000;
	mat.entry[3][1] = 0.00000000000000000;
	mat.entry[3][2] = 0.00000000000000000;
	mat.entry[3][3] = 1.0000000000000000;

	resbuf *pRbSZ = NULL;
	if (RTNORM != acedGetFileNavDialog(_T("选择深圳坐标图纸"), _T(""), _T("dwg"), _T("坐标转换"), 4096, &pRbSZ) || !pRbSZ)
		return;

	AcStringArray strLayerList;
	if (bCheckLayerList)
		readLayerList(strLayerList);

	resbuf *pRbHead = pRbSZ;
	while (pRbSZ)
	{
		AcString path1 = pRbSZ->resval.rstring;

		AcDbDatabase* pShenZhenDb = new AcDbDatabase;
		pShenZhenDb->readDwgFile(path1.kTCharPtr());

		AcDbObjectIdArray frozenLayers;
		AcDbObjectIdArray offLayers;
		AcDbObjectIdArray lockedLayers;

		AcDbLayerTable* pLayerTable = NULL;
		pShenZhenDb->getLayerTable(pLayerTable, AcDb::kForRead);
		if (pLayerTable)
		{
			AcDbLayerTableIterator* pIterator = NULL;
			pLayerTable->newIterator(pIterator);
			if (pIterator)
			{
				AcDbLayerTableRecord* pLayer = NULL;
				for (; !pIterator->done(); pIterator->step())
				{
					pIterator->getRecord(pLayer, AcDb::kForRead);
					if (pLayer)
					{
						if (pLayer->isFrozen())
						{
							if (Acad::eOk == pLayer->upgradeOpen())
							{
								pLayer->setIsFrozen(false);
								frozenLayers.append(pLayer->objectId());
							}
						}

						if (pLayer->isOff())
						{
							if (Acad::eOk == pLayer->upgradeOpen())
							{
								pLayer->setIsOff(false);
								offLayers.append(pLayer->objectId());
							}
						}

						if (pLayer->isLocked())
						{
							if (Acad::eOk == pLayer->upgradeOpen())
							{
								pLayer->setIsLocked(false);
								lockedLayers.append(pLayer->objectId());
							}
						}

						pLayer->close();
					}
				}
				delete pIterator;
			}
			pLayerTable->close();
		}

		AcDbBlockTableRecord* pShenZhenModelSpace = NULL;
		acdbOpenObject(pShenZhenModelSpace, acdbSymUtil()->blockModelSpaceId(pShenZhenDb), AcDb::kForRead);
		if (pShenZhenModelSpace)
		{
			AcDbBlockTableRecordIterator* pIter = NULL;
			pShenZhenModelSpace->newIterator(pIter);
			if (pIter)
			{
				for (; !pIter->done(); pIter->step())
				{
					AcDbEntity* pEnt = NULL;
					pIter->getEntity(pEnt, AcDb::kForRead);
					if (pEnt)
					{
						const ACHAR* pLayerName = pEnt->layer();
						if (bCheckLayerList && !strLayerList.isEmpty() && strLayerList.find(pLayerName) == -1)
						{
							pEnt->close();
							continue;
						}

						if (Acad::eOk == pEnt->upgradeOpen())
						{
							pEnt->transformBy(mat);
						}
						pEnt->close();
					}
				}
				delete pIter;
			}

			pShenZhenModelSpace->close();
		}

		for (int i = 0; i < frozenLayers.length(); ++i)
		{
			AcDbLayerTableRecord* pLayer = NULL;
			acdbOpenObject(pLayer, frozenLayers.at(i), AcDb::kForWrite);
			if (pLayer)
			{
				pLayer->setIsFrozen(true);
				pLayer->close();
			}
		}

		for (int i = 0; i < offLayers.length(); ++i)
		{
			AcDbLayerTableRecord* pLayer = NULL;
			acdbOpenObject(pLayer, offLayers.at(i), AcDb::kForWrite);
			if (pLayer)
			{
				pLayer->setIsOff(true);
				pLayer->close();
			}
		}

		for (int i = 0; i < lockedLayers.length(); ++i)
		{
			AcDbLayerTableRecord* pLayer = NULL;
			acdbOpenObject(pLayer, lockedLayers.at(i), AcDb::kForWrite);
			if (pLayer)
			{
				pLayer->setIsLocked(true);
				pLayer->close();
			}
		}

		ACHAR drive[256] = { 0 };
		ACHAR dir[256] = { 0 };
		ACHAR filename[256] = { 0 };
		ACHAR ext[256] = { 0 };
		_tsplitpath_s(path1.kTCharPtr(), drive, dir, filename, ext);

		AcString strTransFile;
		strTransFile.format(_T("%s%s%s-2.%s"), drive, dir, filename, ext);
		pShenZhenDb->saveAs(strTransFile.kTCharPtr());

		pRbSZ = pRbSZ->rbnext;
	}

	acutRelRb(pRbHead);
}

void Test15()
{
	covertSZToEarth(false);
}

void Test16()
{
	covertSZToEarth(true);
}

void Test17()
{
	AcDbDatabase* pCurDb = acdbHostApplicationServices()->workingDatabase();
	if (!pCurDb)
		return;

	const ACHAR* strPromp[2] = {_T("\n选择需要转换的对象： "), _T("")};
	ads_name ssname;
	if (RTNORM != acedSSGet(_T(":$"), strPromp, NULL, NULL, ssname))
		return;

	Adesk::Int32 nLen = 0;
	ads_sslength(ssname, &nLen);
	if (nLen <= 0)
		return;

	AcGeMatrix3d mat;
	//mat.entry[0][0] = 0.99985447554714357;
	//mat.entry[0][1] = -0.017059534822107925;
	//mat.entry[0][2] = 0.00000000000000000;
	//mat.entry[0][3] = 391090.34252006118;
	//mat.entry[1][0] = 0.017059534822108147;
	//mat.entry[1][1] = 0.99985447554714346;
	//mat.entry[1][2] = 0.00000000000000000;
	//mat.entry[1][3] = 2472660.5165160755;
	//mat.entry[2][0] = 0.00000000000000000;
	//mat.entry[2][1] = 0.00000000000000000;
	//mat.entry[2][2] = 0.99999999999999989;
	//mat.entry[2][3] = 0.00000000000000000;
	//mat.entry[3][0] = 0.00000000000000000;
	//mat.entry[3][1] = 0.00000000000000000;
	//mat.entry[3][2] = 0.00000000000000000;
	//mat.entry[3][3] = 1.0000000000000000;

	mat.entry[0][0] = 0.99985447554819507;
	mat.entry[0][1] = -0.017059534760457032;
	mat.entry[0][2] = 0.00000000000000000;
	mat.entry[0][3] = 391090.33528273401;
	mat.entry[1][0] = 0.017059534760457032;
	mat.entry[1][1] = 0.99985447554819507;
	mat.entry[1][2] = 0.00000000000000000;
	mat.entry[1][3] = 2472660.4789712704;
	mat.entry[2][0] = 0.00000000000000000;
	mat.entry[2][1] = 0.00000000000000000;
	mat.entry[2][2] = 0.99999999999999944;
	mat.entry[2][3] = 0.00000000000000000;
	mat.entry[3][0] = 0.00000000000000000;
	mat.entry[3][1] = 0.00000000000000000;
	mat.entry[3][2] = 0.00000000000000000;
	mat.entry[3][3] = 1.0000000000000000;

	AcDbObjectIdArray frozenLayers;
	AcDbObjectIdArray offLayers;
	AcDbObjectIdArray lockedLayers;

	AcDbLayerTable* pLayerTable = NULL;
	pCurDb->getLayerTable(pLayerTable, AcDb::kForRead);
	if (pLayerTable)
	{
		AcDbLayerTableIterator* pIterator = NULL;
		pLayerTable->newIterator(pIterator);
		if (pIterator)
		{
			AcDbLayerTableRecord* pLayer = NULL;
			for (; !pIterator->done(); pIterator->step())
			{
				pIterator->getRecord(pLayer, AcDb::kForRead);
				if (pLayer)
				{
					if (pLayer->isFrozen())
					{
						if (Acad::eOk == pLayer->upgradeOpen())
						{
							pLayer->setIsFrozen(false);
							frozenLayers.append(pLayer->objectId());
						}
					}

					if (pLayer->isOff())
					{
						if (Acad::eOk == pLayer->upgradeOpen())
						{
							pLayer->setIsOff(false);
							offLayers.append(pLayer->objectId());
						}
					}

					if (pLayer->isLocked())
					{
						if (Acad::eOk == pLayer->upgradeOpen())
						{
							pLayer->setIsLocked(false);
							lockedLayers.append(pLayer->objectId());
						}
					}

					pLayer->close();
				}
			}
			delete pIterator;
		}
		pLayerTable->close();
	}

	for (int i = 0; i < nLen; ++i)
	{
		ads_name ename;
		ads_ssname(ssname, i, ename);

		AcDbObjectId endId;
		acdbGetObjectId(endId, ename);
		if (endId.isNull())
			continue;

		AcDbEntity* pEnt = NULL;
		acdbOpenObject(pEnt, endId, AcDb::kForWrite);
		if (pEnt)
		{
			pEnt->transformBy(mat);
			pEnt->close();
		}
	}

	for (int i = 0; i < frozenLayers.length(); ++i)
	{
		AcDbLayerTableRecord* pLayer = NULL;
		acdbOpenObject(pLayer, frozenLayers.at(i), AcDb::kForWrite);
		if (pLayer)
		{
			pLayer->setIsFrozen(true);
			pLayer->close();
		}
	}

	for (int i = 0; i < offLayers.length(); ++i)
	{
		AcDbLayerTableRecord* pLayer = NULL;
		acdbOpenObject(pLayer, offLayers.at(i), AcDb::kForWrite);
		if (pLayer)
		{
			pLayer->setIsOff(true);
			pLayer->close();
		}
	}

	for (int i = 0; i < lockedLayers.length(); ++i)
	{
		AcDbLayerTableRecord* pLayer = NULL;
		acdbOpenObject(pLayer, lockedLayers.at(i), AcDb::kForWrite);
		if (pLayer)
		{
			pLayer->setIsLocked(true);
			pLayer->close();
		}
	}
}

void Test18()
{
	CAcTcUiManager* pAcTcUiManager = AcTcUiGetManager();
	if (!pAcTcUiManager)
		return;

	//如果已存在ESRI_ArcGIS计划，则直接显示
	CAcTcUiScheme* pArcGISUiScheme = pAcTcUiManager->GetScheme(_T("ESRI_ArcGIS"));
	if (pArcGISUiScheme)
	{
		pArcGISUiScheme->Show(CAcTcUiScheme::kShow, 0);
	}

	try
	{
		AcTcScheme* pAcTcScheme = new AcTcScheme(_T("ESRI_ArcGIS"));
		pArcGISUiScheme = new CAcTcUiScheme(pAcTcScheme);
		CAcTcUiToolPaletteSet* pToolPaletteSet = pArcGISUiScheme->CreatePaletteSet(_T("ESRI_ArcGIS"));
		if (pToolPaletteSet)
		{
			CAcTcUiToolPalette* pToolPalette = new CAcTcUiToolPalette;
			pToolPalette->Create(pToolPaletteSet);
			pToolPalette->SetName(_T("Contents"));
			pToolPaletteSet->AddPalette(pToolPalette);

			pToolPalette = new CAcTcUiToolPalette;
			pToolPalette->Create(pToolPaletteSet);
			pToolPalette->SetName(_T("Locate"));
			pToolPaletteSet->AddPalette(pToolPalette);

			pArcGISUiScheme->Show(CAcTcUiScheme::kShow, 0);
		}
	}
	catch (...)
	{

	}
}

void Test19()
{
	//AcDbObjectIdArray ids;
	//int iRet = ssgetObjects(ids);
	//if (RTNORM != iRet)
	//	return;

	//if (ids.length() != 1)
	//	return;

	//AcDbEntity* pEnt = NULL;
	//acdbOpenObject(pEnt, ids[0], AcDb::kForRead);
	//if (pEnt)
	//{
	//	AcDb3dSolid* p3dSolid = new AcDb3dSolid;
	//	p3dSolid->copyFrom(pEnt);

	//	AcDbGripDataPtrArray grips;
	//	double curViewUnitSize = 0.0;
	//	int gripSize = 1;
	//	AcGeVector3d curViewDir;
	//	int bitflags = 0;
	//	Acad::ErrorStatus res = p3dSolid->getGripPoints(grips, curViewUnitSize, gripSize, curViewDir, bitflags);

	//	AcGePoint3dArray gripPoints;
	//	AcDbIntArray osnapModes;
	//	AcDbIntArray geomIds;
	//	res = p3dSolid->getGripPoints(gripPoints, osnapModes, geomIds);

	//	pEnt->close();
	//}
}

void Test20()
{
	AcDbDatabase* pCurDb = acdbCurDwg();

	AcDbDatabase* pDb = new AcDbDatabase(true);
	pDb->insert(AcGeMatrix3d::kIdentity, pCurDb);
	pDb->saveAs(_T("D:\\Drawing1.dwg"), false, AcDb::kDHL_1027);
	delete pDb;
}
