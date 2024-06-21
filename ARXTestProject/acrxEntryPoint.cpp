// (C) Copyright 2002-2012 by Autodesk, Inc. 
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted, 
// provided that the above copyright notice appears in all copies and 
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting 
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM "AS IS" AND WITH ALL FAULTS. 
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC. 
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to 
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//

//-----------------------------------------------------------------------------
//----- acrxEntryPoint.cpp
//-----------------------------------------------------------------------------
#include "pch.h"
#include "framework.h"
#include "resource.h"

#include "Test1.h"
#include "Test2.h"
#include "Test3.h"
#include "Test4.h"
#include "TestCom.h"

#include "AcExtensionModule.h"

#include "MyDoubleClickEdit.h"
#include "MyGiDrawableOverrule.h"

#include "MyEntity.h"

#include "MapTest1.h"

//-----------------------------------------------------------------------------
#define szRDS _RXST("")

extern CAcExtensionModule ArxTestProjectDLL;


//-----------------------------------------------------------------------------
//----- ObjectARX EntryPoint
class CArxTestProjectApp : public AcRxArxApp 
{

public:
	CArxTestProjectApp () : AcRxArxApp () {}

	virtual AcRx::AppRetCode On_kInitAppMsg (void *pkt) 
	{
		// TODO: Load dependencies here

		// You *must* call On_kInitAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kInitAppMsg (pkt) ;

		MyDoubleClickEdit::rxInit();
		MyGiDrawableOverrule::rxInit();
		MyEntity::rxInit();
		acrxBuildClassHierarchy();
		
		// TODO: Add your initialization code here
		ADD_COMMAND_1;
		ADD_COMMAND_2;
		ADD_COMMAND_3;
		ADD_COMMAND_4;
		ADD_COMMAND_COM;

		acedRegCmds->addCommand(_T("TestProject"), _T("convert_All"), _T("convert_All"), ACRX_CMD_MODAL | ACRX_CMD_NOPAPERSPACE, Test15, NULL, -1, ArxTestProjectDLL.ModuleResourceInstance());
		acedRegCmds->addCommand(_T("TestProject"), _T("convert_LayerList"), _T("convert_LayerList"), ACRX_CMD_MODAL, Test16, NULL, -1, ArxTestProjectDLL.ModuleResourceInstance());
		acedRegCmds->addCommand(_T("TestProject"), _T("convert_Selected"), _T("convert_Selected"), ACRX_CMD_MODAL, Test17, NULL, -1, ArxTestProjectDLL.ModuleResourceInstance());
		acedRegCmds->addCommand(_T("TestProject"), _T("MapTest1"), _T("MapTest1"), ACRX_CMD_MODAL, MapTest1, NULL, -1, ArxTestProjectDLL.ModuleResourceInstance());

		return (retCode) ;
	}

	virtual AcRx::AppRetCode On_kUnloadAppMsg (void *pkt) 
	{
		// TODO: Add your code here
		acedRegCmds->removeGroup(_T("TestProject"));

		deleteAcRxClass(MyDoubleClickEdit::desc());
		deleteAcRxClass(MyGiDrawableOverrule::desc());
		deleteAcRxClass(MyEntity::desc());

		// You *must* call On_kUnloadAppMsg here
		AcRx::AppRetCode retCode =AcRxArxApp::On_kUnloadAppMsg (pkt) ;

		// TODO: Unload dependencies here

		return (retCode) ;
	}

	virtual void RegisterServerComponents () {
	}
	
} ;

//-----------------------------------------------------------------------------
IMPLEMENT_ARX_ENTRYPOINT(CArxTestProjectApp)




