
#include "pch.h"
#include "framework.h"
#include "ArxGlobalFuncs.h"
#include <dbobjptr.h>
#include "acedads.h"
#include "dbmleader.h"

using namespace ARXTest;

void Test1()
{
	AcDbDatabase* pDb = acdbCurDwg();
	if (!pDb)
		return;

	AcDbObjectId objId = selectSingleObject();
	if (objId.isNull())
		return;

	AcDbObjectPointer<AcDbObject> pObj(objId, AcDb::kForRead);
	if (Acad::eOk != pObj.openStatus() || pObj == NULL)
		return;

	if (pObj->isA() != AcDbBlockReference::desc())
		return;

	AcDbBlockReference* pBlockRef = AcDbBlockReference::cast(pObj);
	if (!pBlockRef)
		return;

	AcDbObjectPointer<AcDbBlockTableRecord> pRec(pBlockRef->blockTableRecord(), AcDb::kForRead);
	if (Acad::eOk != pRec.openStatus() || pRec == NULL)
		return;

	AcDbObjectIdArray ids;
	AcDbBlockTableRecordIterator* pIter = NULL;
	pRec->newIterator(pIter);
	if (pIter)
	{
		for (pIter->start(); !pIter->done(); pIter->step())
		{
			AcDbObjectId entId;
			pIter->getEntityId(entId);

			if (!entId.isNull())
				ids.append(entId);
		}
		delete pIter;
	}

	AcDbIdMapping idMapping;
	Acad::ErrorStatus res = pDb->deepCloneObjects(ids, pDb->currentSpaceId(), idMapping);
	if (res != Acad::eOk)
		return;
}

void Test1_2()
{
	AcDbDatabase* pDb = acdbCurDwg();
	if (!pDb)
		return;

	AcDbObjectId objId = selectSingleObject();
	if (objId.isNull())
		return;

	AcDbObjectPointer<AcDbObject> pObj(objId, AcDb::kForRead);
	if (Acad::eOk != pObj.openStatus() || pObj == NULL)
		return;

	AcDbDimension* pDim = AcDbDimension::cast(pObj);
	if (!pDim)
		return;

	AcDbObjectPointer<AcDbBlockTableRecord> pDimBlock(pDim->dimBlockId(), AcDb::kForRead);
	if (Acad::eOk != pDimBlock.openStatus() || pDimBlock == NULL)
		return;

	AcDbBlockTableRecordIterator* iter;
	pDimBlock->newIterator(iter);
	if (iter)
	{
		for (iter->start(); !iter->done(); iter->step())
		{
			AcDbObjectId entId;
			iter->getEntityId(entId);

			AcDbObjectPointer<AcDbObject> pObj(entId, AcDb::kForRead);
			if (Acad::eOk != pObj.openStatus())
				continue;

			if (pObj->isKindOf(AcDbMText::desc()))
			{
				AcDbMText* pMText = AcDbMText::cast(pObj);
				if (pMText)
				{
					if (Acad::eOk == pMText->upgradeOpen())
					{
						int nRet = pMText->setContents(_T("TEST"));
						pMText->downgradeOpen();
					}
				}
			}
		}
		delete iter;
	}
}



void Test2()
{
	resbuf* rb = acutBuildList(-4, _T("<AND"), RTDXF0, _T("INSERT"), -4, _T("AND>"), 0);
	ads_name ssname;
	int nRet = acedSSGet(_T("_I"), NULL, NULL, rb, ssname);
	if (RTNORM == nRet)
	{
		Adesk::Int32 nLen = 0;
		acedSSLength(ssname, &nLen);
	}
}

void Test3()
{
	const ACHAR* pClipFormat = acedClipFormatName();
	if (!pClipFormat)
		return;
}

void Test4()
{
	CRgn rgn1, rgn2, rgn3;
	rgn1.SetRectRgn(0, 0, 10, 10);
	rgn2.SetRectRgn(10, 10, 20, 20);
	rgn3.CombineRgn(&rgn1, &rgn2, RGN_OR);

	int nDataSize = rgn3.GetRegionData(NULL, 0);
	LPRGNDATA lpRgnData = new RGNDATA[nDataSize];
	rgn3.GetRegionData(lpRgnData, nDataSize);
}

class Base
{
public:
	Base() {}
	virtual ~Base() { acutPrintf(_T("\nBase::Destructor")); }

	virtual void Fun() = 0;
};

class Derive1 : public Base
{
public:
	Derive1() {}
	~Derive1() { acutPrintf(_T("\nDerive1::Destructor")); }

	void Fun() {}
};

class Derive2 : public Derive1
{
public:
	Derive2() {}
	~Derive2() { acutPrintf(_T("\nDerive2::Destructor")); }

	void Fun() {}
};

void Test5()
{
	Derive1* pDerive2 = new Derive2;
	delete pDerive2;
}

class AcGiMyDrawableTraits : public AcGiDrawableTraits
{
public:

	virtual void              setColor(const Adesk::UInt16 color) = 0;
	virtual void              setTrueColor(const AcCmEntityColor& color) = 0;
	virtual void              setLayer(const AcDbObjectId layerId) = 0;
	virtual void              setLineType(const AcDbObjectId linetypeId) = 0;
	virtual void              setSelectionMarker(const Adesk::LongPtr markerId) = 0;
	virtual void              setFillType(const AcGiFillType) = 0;
	virtual void              setLineWeight(const AcDb::LineWeight lw) = 0;
	virtual void              setLineTypeScale(double dScale = 1.0) = 0;
	virtual void              setThickness(double dThickness) = 0;

	virtual Adesk::UInt16           color(void) const = 0;
	virtual AcCmEntityColor         trueColor(void) const = 0;
	virtual AcDbObjectId            layerId(void) const = 0;
	virtual AcDbObjectId            lineTypeId(void) const = 0;
	virtual AcGiFillType            fillType(void) const = 0;
	virtual AcDb::LineWeight        lineWeight(void) const = 0;
	virtual double                  lineTypeScale(void) const = 0;
	virtual double                  thickness(void) const = 0;
};

class AcGiTestDrawableOverrule : public AcGiDrawableOverrule
{
public:
	virtual bool isApplicable(const AcRxObject* pOverruledSubject) const
	{
		return true;
	}

	virtual Adesk::UInt32 setAttributes(AcGiDrawable* pSubject, AcGiDrawableTraits * traits)
	{
		Adesk::UInt32 nRet = AcGiDrawableOverrule::setAttributes(pSubject, traits);
		traits->setColor(1);
		return nRet;
	}

	virtual Adesk::Boolean  worldDraw(AcGiDrawable* pSubject, AcGiWorldDraw * wd)
	{
		AcGeMatrix3d mat;
		wd->geometry().getModelToWorldTransform(mat);
		wd->subEntityTraits().setColor(1);
		
		return AcGiDrawableOverrule::worldDraw(pSubject, wd);
	}
};

AcGiDrawableOverrule* pOverrule = new AcGiTestDrawableOverrule;
void Test6()
{
	AcRxOverrule::addOverrule(AcDbEntity::desc(), pOverrule);
	//AcDbObjectId objId = selectSingleObject();
	//if (objId.isNull())
	//	return;

	//AcDbEntity* pEnt = NULL;
	//acdbOpenObject(pEnt, objId, AcDb::kForRead);
	//if (pEnt)
	//{
	//	bool bRet = AcRxOverrule::hasOverrule(pEnt, pOverrule->isA());
	//	pEnt->close();
	//}
}

void Test7()
{
	AcDbObjectId objId = selectSingleObject();
	if (objId.isNull())
		return;

	AcDbBlockReference* pRef = NULL;
	acdbOpenObject(pRef, objId, AcDb::kForWrite);
	if (pRef)
	{
		AcDbBlockTableRecord* pRec = NULL;
		acdbOpenObject(pRec, pRef->blockTableRecord(), AcDb::kForRead);
		if (pRec)
		{
			AcDbBlockTableRecordIterator* itr = NULL;
			pRec->newIterator(itr);
			if (itr)
			{
				for (; !itr->done(); itr->step())
				{
					AcDbEntity* pEnt = NULL;
					itr->getEntity(pEnt, AcDb::kForWrite);
					if (pEnt)
					{
						pEnt->setVisibility(AcDb::kInvisible);
						pEnt->close();
					}
				}
				
				delete itr;
			}
			pRec->close();
		}
		pRef->recordGraphicsModified();
		pRef->close();
	}

	ads_point pt;
	acedGetPoint(NULL, NULL, pt);

	acdbOpenObject(pRef, objId, AcDb::kForWrite);
	if (pRef)
	{
		AcDb::Visibility vb = pRef->visibility();
		pRef->close();
	}
}

void Test8()
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if (!pDb)
		return;

	AcGsManager* pGsManager = acgsGetGsManager();
	if (!pGsManager)
		return;

	//AcGsModel* pGsModel = pGsManager->gsModel(pDb);
	//if (!pGsModel)
	//	return;

	AcDbHandle hdl(_T("1B0ECE"));
	AcDbObjectId objId;
	if (Acad::eOk != pDb->getAcDbObjectId(objId, false, hdl))
		return;

	AcDbExtents ext;
	AcDbMText* pMText = NULL;
	acdbOpenObject(pMText, objId, AcDb::kForRead);
	if (pMText)
	{
		pMText->getGeomExtents(ext);

		AcString strContents;
		pMText->contents(strContents);

		AcString strContentsRTF;
		pMText->contentsRTF(strContentsRTF);

		AcString strText;
		pMText->text(strText);

		AcGiDrawable* pDrawable = pMText->drawable();
		if (pDrawable)
		{
			AcDbExtents bds;
			pDrawable->bounds(bds);

			AcGiDrawable::DrawableType type = pDrawable->drawableType();

			pDrawable->desc();
		}

		pMText->close();
	}
}

void Test9()
{
	AcDbDatabase* pDb = acdbCurDwg();
	if (!pDb)
		return;

	AcDbLayerTable* pLayerTable = NULL;
	pDb->getLayerTable(pLayerTable, AcDb::kForRead);
	if (pLayerTable)
	{
		AcDbLayerTableIterator* iter;
		pLayerTable->newIterator(iter);
		if (iter)
		{
			for (; !iter->done(); iter->step())
			{
				const ACHAR* pName = NULL;
				AcDbLayerTableRecord* pLayerRec = NULL;
				iter->getRecord(pLayerRec);
				if (pLayerRec)
				{
					pLayerRec->getName(pName);
					pLayerRec->close();
				}
			}
			delete iter;
		}
		pLayerTable->close();
	}

	ads_name ename;
	ads_point pt;
	int nRet = acedEntSel(_T(""), ename, pt);
	if (RTNORM != nRet)
		return;

	AcDbObjectId objId;
	acdbGetObjectId(objId, ename);
	if (objId.isNull())
		return;

	AcDbMLeader* pMleader = NULL;
	acdbOpenObject(pMleader, objId, AcDb::kForRead);
	if (pMleader)
	{
		AcDbLayerTableRecord* pLayerRec = NULL;
		Acad::ErrorStatus res = acdbOpenObject(pLayerRec, pMleader->layerId(), AcDb::kForRead);
		if (pLayerRec)
		{
			ACHAR* pName = NULL;
			pLayerRec->getName(pName);
			pLayerRec->close();
		}
		ACHAR* pLayerName = pMleader->layer();
		pMleader->close();
	}
}

void Test10()
{
	AcDbDatabase* pDb = acdbHostApplicationServices()->workingDatabase();
	if (!pDb)
		return;

	int vpNum = 2;
	resbuf rbCVPORT;
	if (RTNORM == acedGetVar(_T(""), &rbCVPORT))
		vpNum = rbCVPORT.resval.rint;
	
	AcGsView* pGsView = acgsGetCurrentAcGsView(vpNum);
	if (!pGsView)
		return;

	AcGePoint2d pixelArea;
	pGsView->getNumPixelsInUnitSquare(AcGePoint3d(0.0, 0.0, 0.0), pixelArea);

	double pX = pixelArea.x;
	double pY = pixelArea.y;
}
