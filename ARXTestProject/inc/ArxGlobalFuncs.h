#pragma once

#include <dbid.h>
#include <dbidar.h>


namespace ARXTest {

	AcDbObjectId selectSingleObject();
	int ssgetObjects(AcDbObjectIdArray& ids);
	AcDbObjectId addEntityToModelSpace(AcDbEntity* pEnt);

	Acad::ErrorStatus getViewportBorder(AcDbDatabase* pDb, AcDbObjectId& vbId);
}

