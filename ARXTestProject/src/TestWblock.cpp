
#include "pch.h"
#include "framework.h"
#include <vector>
#include "axlock.h"

bool IsBlkHasAttrib(AcDbBlockReference* blkPtr)
{
	bool bRet = false;
	AcDbObjectIterator * pIter = NULL;
	pIter = blkPtr->attributeIterator();
	if (pIter == NULL)
	{
		return bRet;
	}

	AcDbAttribute* pCurAttr;
	AcDbObjectId uId;
	AcDbEntity* pIterEnt;
	for (pIter->start(); !pIter->done(); pIter->step())
	{
		uId = pIter->objectId();
		if (acdbOpenAcDbEntity(pIterEnt, uId, AcDb::kForRead) != Acad::eOk)
		{
			continue;
		}

		pCurAttr = AcDbAttribute::cast(pIterEnt);
		if (pCurAttr == NULL)
		{
			pIterEnt->close();
			continue;
		}

		if (pCurAttr->isInvisible() == Adesk::kTrue)
		{
			pIterEnt->close();
			continue;
		}

		TCHAR* pTextstr = pCurAttr->textString();

		if (_tcslen(pTextstr) > 0)
		{
			bRet = true;
			break;
		}

		pIterEnt->close();
	}
	delete pIter;

	return bRet;
}

void GetBlkIdArr(AcDbDatabase* pWorkDb, AcDbObjectIdArray& idArr)
{
	if (pWorkDb == NULL)
	{
		return;
	}

	AcDbBlockTable *pBlkTbl = NULL;
	Acad::ErrorStatus esGet = pWorkDb->getBlockTable(pBlkTbl, AcDb::kForRead);
	if (esGet != Acad::eOk)
	{
		return;
	}
	pBlkTbl->close();

	AcDbBlockTableRecord *pBlkTblRcd = NULL;
	esGet = pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	if (esGet != Acad::eOk || pBlkTblRcd == NULL)
	{
		return;
	}

	//遍历模型空间的实体
	AcDbBlockTableRecordIterator *it(NULL);
	esGet = pBlkTblRcd->newIterator(it);
	if (esGet != Acad::eOk || it == NULL)
	{
		return;
	}

	for (it->start(); !it->done(); it->step())
	{
		AcDbEntity *pEntity = NULL;
		if (it->getEntity(pEntity, AcDb::kForRead) != Acad::eOk)
		{
			continue;
		}
		pEntity->close();

		if (!pEntity->isKindOf(AcDbBlockReference::desc()))
		{
			continue;
		}

		idArr.append(pEntity->objectId());
	}

	delete it;
	it = NULL;
	pBlkTblRcd->close();
}

void OpenBlkEntForRead(std::vector<AcDbBlockReference*>& ptrArr, const AcDbObjectIdArray& ids)
{
	AcDbEntity *pEnt;
	for (int i = 0; i < ids.length(); i++)
	{
		if (!ids[i].isValid())
		{
			continue;
		}

		if (acdbOpenAcDbEntity(pEnt, ids[i], AcDb::kForRead) != Acad::eOk)
		{
			continue;
		}

		AcDbBlockReference* pBlkEnt = AcDbBlockReference::cast(pEnt);
		if (pBlkEnt == NULL)
		{
			continue;
		}

		ptrArr.push_back(pBlkEnt);
	}
}

void CloseMyEntsPtr(std::vector<AcDbBlockReference*>& ptrArr)
{
	for (int i = 0; i < ptrArr.size(); i++)
	{
		ptrArr[i]->close();
	}
}

AcDbBlockTableRecord* openCurrentSpaceBlock(AcDb::OpenMode mode, AcDbDatabase* db)
{
	ASSERT(db != NULL);

	AcDbBlockTableRecord* blkRec;

	Acad::ErrorStatus es = acdbOpenObject(blkRec, db->currentSpaceId(), mode);
	if (es != Acad::eOk)
		return NULL;
	else
		return blkRec;
}

Acad::ErrorStatus addToCurrentSpace(AcDbEntity* newEnt, AcDbDatabase* db, AcDbObjectId &retId)
{
	ASSERT(newEnt != NULL);
	ASSERT(db != NULL);

	AcDbBlockTableRecord* blkRec = openCurrentSpaceBlock(AcDb::kForWrite, db);

	ASSERT(blkRec != NULL);

	if (blkRec == NULL)
		return Acad::eInvalidInput;

	// append new entity to current space block
	Acad::ErrorStatus es = blkRec->appendAcDbEntity(retId, newEnt);
	if (es != Acad::eOk)
	{
		//PmAcutPrintfErrMsg(_T("\nERROR: could not add entity to current space "), ArxDbgUtils::rxErrorStr(es));
	}

	blkRec->close();
	return es;
}

BOOL AddToDb(AcDbEntity * pEntity, AcDbObjectId& id)
{
	try
	{
		BOOL function_return = FALSE;
		//parameter	
		if (pEntity == NULL)
		{
			return FALSE;
		}
		//end parameter
		AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
		if (pDb == NULL) {
			acedAlert(_T("No database!"));
			return Adesk::kFalse;
		}
		Acad::ErrorStatus es = addToCurrentSpace(pEntity, pDb, id);
		if (es == Acad::eOk)
		{
			pEntity->close();
			pEntity = NULL;

			function_return = TRUE;
		}
		else
		{
			//			delete pEntity;
			delete pEntity;
			pEntity = NULL;
		}
		return function_return;
	}
	catch (...)
	{
		return FALSE;
	}
}

void ExplodeNoAttributeBlock(AcDbDatabase* pWorkDb)
{
	AcDbBlockTable* pTable;
	if (pWorkDb->getBlockTable(pTable, AcDb::kForRead) != Acad::eOk)
	{
		return;
	}

	AcDbBlockTableIterator* pIterator = NULL;
	pTable->newIterator(pIterator);
	pTable->close();

	AcDbObjectIdArray hasAttribIds;

	AcDbBlockTableRecord* pRecord;
	for (; !pIterator->done(); pIterator->step())
	{
		if (pIterator->getRecord(pRecord, AcDb::kForRead) != Acad::eOk)
		{
			continue;
		}

		if (pRecord->hasAttributeDefinitions())
		{
			hasAttribIds.append(pRecord->id());
		}

		pRecord->close();
	}
	delete pIterator;

	AcDbObjectIdArray blkIds;
	GetBlkIdArr(pWorkDb, blkIds);

	std::vector<AcDbBlockReference*> blkPtrs;
	OpenBlkEntForRead(blkPtrs, blkIds);

	AcDbObjectId id;
	AcDbEntity* pEnt = NULL;

	AcDbExtents exts;

	static double s_dMinLen = 2500.0;
	for (int i = 0; i < blkPtrs.size(); i++)
	{
		if (blkPtrs[i]->visibility() == AcDb::kInvisible)
		{
			continue;
		}

		AcDbFilter* pFilter = NULL;
		Acad::ErrorStatus esGet = AcDbIndexFilterManager::getFilter(blkPtrs.at(i), AcDbSpatialFilter::desc(), AcDb::kForRead, pFilter);
		if (esGet == Acad::eOk && pFilter != NULL)
		{
			///此块被裁剪，若炸开会得到很多裁剪边框之外的实体
			pFilter->close();
			continue;
		}

		if (hasAttribIds.contains(blkPtrs[i]->blockTableRecord()))
		{
			continue;
		}

		if (blkPtrs[i]->getGeomExtents(exts) == Acad::eOk)
		{
			if (((exts.maxPoint().x - exts.minPoint().x) < s_dMinLen)
				&& ((exts.maxPoint().y - exts.minPoint().y) < s_dMinLen))
			{
				continue;
			}
		}

		// QC：28528 针对门窗等炸开会丢失属性的那种块，pRecord->hasAttributeDefinitions获取到的是不含属性的结果
		// 因为属性要靠AcDbAttribute获取
		if (IsBlkHasAttrib(blkPtrs[i]))
		{
			continue;
		}

		AcDbBlockReference* pCurBlk = blkPtrs.at(i);
		AcGePoint3d ptPos = pCurBlk->position();
		AcDbExtents extCurBlk;
		pCurBlk->getGeomExtents(extCurBlk);
		Acad::ErrorStatus esExplode = pCurBlk->explodeToOwnerSpace();
		if (esExplode == Acad::eOk)
		{
			int iTest = 0;
			++iTest;
		}

		//AcDbVoidPtrArray subs;
		//bool bExploeSuccess = false;
		//Acad::ErrorStatus esExplode = blkPtrs[i]->explode(subs);
		//if(esExplode == Acad::eOk)
		//{
		//	bExploeSuccess = true;
		//	for(int j=0; j<subs.length(); j++)
		//	{
		//		pEnt = (AcDbEntity*)subs[j];

		//		if (!AddToDb(pEnt, id))
		//		{
		//			acutPrintf(_T("AddToDb Not Successfully.\n"));
		//		}
		//		pEnt->close();
		//	}
		//}

		//if(bExploeSuccess && blkPtrs[i]->upgradeOpen() == Acad::eOk)
		//{
		//	blkPtrs[i]->erase();
		//}
	}

	CloseMyEntsPtr(blkPtrs);
	acutPrintf(_T("块炸开成功!\n"));
}

void getMsEnts(AcDbDatabase* pDb, AcDbObjectIdArray& ids)
{
	AcDbBlockTable* pTbl = NULL;
	pDb->getBlockTable(pTbl, AcDb::kForRead);
	if (pTbl)
	{
		AcDbBlockTableRecord* pBtr = NULL;
		pTbl->getAt(ACDB_MODEL_SPACE, pBtr, AcDb::kForRead);
		if (pBtr)
		{
			AcDbBlockTableRecordIterator* pItr = NULL;
			pBtr->newIterator(pItr);
			if (pItr)
			{
				for (pItr->start(); !pItr->done(); pItr->step())
				{
					AcDbObjectId id;
					if (Acad::eOk == pItr->getEntityId(id))
					{
						ids.append(id);
					}
				}
				delete pItr;
				pItr = NULL;
			}
			pBtr->close();
		}
		pTbl->close();
	}
}

void MoveEntInModelSpace(AcDbDatabase* pWorkDb, const AcGeMatrix3d& transForm)
{
	if (pWorkDb == NULL)
	{
		return;
	}

	AcAxDocLock DbLock(pWorkDb);
	AcDbBlockTable *pBlkTbl = NULL;
	Acad::ErrorStatus esGet = pWorkDb->getBlockTable(pBlkTbl, AcDb::kForRead);
	if (esGet != Acad::eOk)
	{
		return;
	}
	pBlkTbl->close();

	AcDbBlockTableRecord *pBlkTblRcd = NULL;
	esGet = pBlkTbl->getAt(ACDB_MODEL_SPACE, pBlkTblRcd, AcDb::kForRead);
	if (esGet != Acad::eOk)
	{
		return;
	}

	//遍历模型空间的实体
	AcDbBlockTableRecordIterator *it(NULL);
	pBlkTblRcd->newIterator(it);
	for (it->start(); !it->done(); it->step())
	{
		AcDbEntity *pEntity = NULL;
		if (it->getEntity(pEntity, AcDb::kForWrite) != Acad::eOk)
		{
			continue;
		}

		if (pEntity->isAProxy())
		{
			pEntity->close();
		}
		else
		{
			pEntity->transformBy(transForm);
		}
		
		pEntity->close();
	}

	delete it;
	pBlkTblRcd->close();
}

BOOL CheckFile(const CString & strFileName)
{
	CFileStatus cStatus;
	if (CFile::GetStatus(strFileName, cStatus))
		if (!(cStatus.m_attribute & CFile::directory))
			return TRUE;
	return FALSE;

}

CString GetFileAP(const CString& strFileDir, const CString& strPathName)
{
	CString strRet(strPathName);

	if (!CheckFile(strPathName))
	{
		///相对路径
		if (strRet.GetAt(0) == _T('.'))
		{
			strRet.TrimLeft(_T('.'));
		}

		strRet = strFileDir + strRet;
	}

	if (!CheckFile(strRet))
	{
		strRet.Empty();
	}

	return strRet;
}

CString GetFileDir(const CString& strFilePath)
{
	CString strRet;
	int iFindPos = strFilePath.ReverseFind(_T('\\'));
	if (iFindPos >= 0)
	{
		strRet = strFilePath.Left(iFindPos);
	}

	return strRet;
}

bool MotifyExtBlkPath(AcDbBlockTableRecord* pBlkTabRcdXRef, const CString& strFileDir, const CString& strProDir)
{
	bool bRet = false;
	if (pBlkTabRcdXRef == NULL)
	{
		return bRet;
	}

	///获取参照图纸的目录
	const ACHAR* pPathName = NULL;
	Acad::ErrorStatus esGetName = pBlkTabRcdXRef->pathName(pPathName);
	if (esGetName != Acad::eOk || pPathName == NULL)
	{
		return bRet;
	}

	///检查参照图纸目录是否存在
	CString strSrcFilePath = GetFileAP(strFileDir, CString(pPathName));
	if (strSrcFilePath.IsEmpty())
	{
		return bRet;
	}

	//
	///获取文件名称
	int iFind = strSrcFilePath.ReverseFind(_T('\\'));
	CString strFileName;
	if (iFind != 0)
	{
		strFileName = strSrcFilePath.Right(strSrcFilePath.GetLength() - iFind - 1);
	}

	///拷贝文件
	CString strNewFilePath = strProDir + _T('\\') + strFileName;
	CopyFile(strSrcFilePath, strNewFilePath, FALSE);

	Acad::ErrorStatus esUp = pBlkTabRcdXRef->upgradeOpen();
	if (esUp == Acad::eOk)
	{
		pBlkTabRcdXRef->setPathName(strNewFilePath.GetBuffer());

		AcDb::XrefStatus xsXref = pBlkTabRcdXRef->xrefStatus();
		if (xsXref != AcDb::kXrfResolved)
		{
			
		}

		bRet = true;
	}

	return bRet;
}

CString GetProXRefDir()
{
	CString strRet = _T("E:");
	strRet.AppendFormat(_T("\\参照图纸"));

	if (!PathIsDirectory(strRet))
	{
		::CreateDirectory(strRet, NULL);
	}

	return strRet;
}

bool DelaXRefV2(AcDbDatabase* pWorkDb, const CString& strFilePath)
{
	bool bRet = false;
	if (pWorkDb == NULL)
	{
		return bRet;
	}

	///图纸加锁
	AcAxDocLock DBLock(pWorkDb);

	AcDbBlockTable* pBlkTab = NULL;
	Acad::ErrorStatus esGet = pWorkDb->getBlockTable(pBlkTab, AcDb::kForRead);
	if (esGet != Acad::eOk || pBlkTab == NULL)
	{
		return bRet;
	}
	pBlkTab->close();

	AcDbBlockTableIterator* pBlkTabIter = NULL;
	esGet = pBlkTab->newIterator(pBlkTabIter);
	if (esGet != Acad::eOk || pBlkTabIter == NULL)
	{
		return bRet;
	}

	//选择图纸所在的文件夹
	CString strFileDir = GetFileDir(strFilePath);
	//工程文件夹
	CString strProDir = GetProXRefDir();

	for (pBlkTabIter->start(); !pBlkTabIter->done(); pBlkTabIter->step())
	{
		AcDbBlockTableRecord* pBlkTabRcd = NULL;
		esGet = pBlkTabIter->getRecord(pBlkTabRcd, AcDb::kForRead);
		if (esGet != Acad::eOk || pBlkTabRcd == NULL)
		{
			continue;
		}

		///检查是否是外部参照的块表记录
		if (!pBlkTabRcd->isFromExternalReference())
		{
			pBlkTabRcd->close();
			continue;
		}

		//
		bRet |= MotifyExtBlkPath(pBlkTabRcd, strFileDir, strProDir);

		pBlkTabRcd->close();
	}
	delete pBlkTabIter;
	pBlkTabIter = NULL;


	return bRet;
}

void WblockTest()
{
	static int iWBlockCount = 0;

	AcAxDocLock DbLock(curDoc()->database());
	//设置新变量

	AcDbBlockTable* pTbl = NULL;
	curDoc()->database()->getBlockTable(pTbl, AcDb::kForRead);
	if (pTbl)
	{
		AcDbObjectId msId;
		pTbl->getAt(ACDB_MODEL_SPACE, msId);
		pTbl->close();

		resbuf* rb = acutNewRb(RTSTR);

		if (RTNORM != acedGetFileNavDialog(_T("选择DWG文件"), NULL, _T("dwg"), NULL, 0, &rb))
			//if (RTNORM != acedGetFileD(_T("选择DWG文件"), NULL, _T("dwg"), 0, rb))
		{
			return;
		}
		ACHAR result[MAX_PATH];
		if (RTNORM == acedFindFile(rb->resval.rstring, result))
		{
			AcDbDatabase* pDb = new AcDbDatabase(Adesk::kFalse, Adesk::kTrue);
			pDb->readDwgFile(result);
			AcDbObjectIdArray ids;

			AcGePoint3d ptCurDbMin = curDoc()->database()->extmin();
			AcGePoint3d ptCurDbMax = curDoc()->database()->extmax();

			AcGePoint3d ptOrigin(AcGePoint3d::kOrigin);
			if (iWBlockCount != 0)
			{
				double dXMove = 9999;
				ptOrigin = AcGePoint3d(acdbCurDwg()->extmax().x + dXMove, acdbCurDwg()->extmin().y, 0.0);
			}

			AcGePoint3d ptExtMin = pDb->extmin();
			AcGeVector3d vecMove = ptOrigin - ptExtMin;

			AcGeMatrix3d xform;
			xform.setToTranslation(vecMove);
			MoveEntInModelSpace(pDb, xform);

			getMsEnts(pDb, ids);

			//AcDbObjectId msId;
			//BOOL bOk = CGUtilDb::GetRecordOwnerId(acdbCurDwg(),msId);

			AcDbIdMapping idMap;

			DWORD dStart = ::GetTickCount();
			Acad::ErrorStatus es = curDoc()->database()->wblockCloneObjects(ids, msId, idMap, AcDb::kDrcIgnore/*AcDb::kDrcReplace*/);
			DWORD dEnd = ::GetTickCount();
			DWORD dCount = dEnd - dStart;

			if (es == Acad::eOk)
			{
				acutPrintf(_T("wblockCloneObjects用时：%d.\n"), dCount);
				acutPrintf(_T("\n%d entities are imported."), ids.length());
				++iWBlockCount;
			}
			else
			{
				acutPrintf(_T("\nError: %s"), acadErrorStatusText(es));
			}

			delete pDb;
			pDb = NULL;
		}
		
		DelaXRefV2(curDoc()->database(), CString(result));
		
		acutRelRb(rb);
	}

	//SaveDb(acdbHostApplicationServices()->workingDatabase()/*curDoc()->database()*/);
	acDocManager->sendStringToExecute(curDoc(), _T("zoom e "));

	//ExplodeNoAttributeBlock(curDoc()->database());

	return;
}