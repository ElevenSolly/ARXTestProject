
#include "pch.h"
#include "framework.h"
#include "ArxGlobalFuncs.h"

#define INITGUID
#include "guiddef.h"
#include <opmext.h>
#include <dynprops.h>
#include <acadi.h>
#include <axboiler.h>
#include "acpi.h"
#include "acpi_i.c"

#include "TestCom.h"

using namespace ARXTest;

class CoInitializeAutoHandler
{
public:
	CoInitializeAutoHandler()
	{
		if (FAILED(CoInitialize(NULL)))
		{
			throw;
		}
	}

	~CoInitializeAutoHandler()
	{
		CoUninitialize();
	}
};

//获取选中对象类型PropertySource上的所有动态属性
void TestCom1()
{
	AcDbObjectIdArray selIds;
	int nRet = ssgetObjects(selIds);
	if (RTNORM != nRet || selIds.length() == 0)
		return;

	CLSID clsId;
	AcRxClass* pRxClass = NULL;
	AcDbEntity* pEnt = NULL;
	AcDbObjectId selId = selIds.at(0);
	acdbOpenObject(pEnt, selId, AcDb::kForRead);
	if (pEnt)
	{
		if (pEnt->isKindOf(AcDbDimension::desc()))
		{
			pRxClass = AcDbDimension::desc();
		}
		else
		{
			pRxClass = pEnt->isA();
		}
		pEnt->getClassID(&clsId);
		pEnt->close();
	}

	if (!pRxClass)
		return;

	//初始化COM环境
	try
	{
		CoInitializeAutoHandler coAutoInit;
	}
	catch (...)
	{
		return;
	}

	IUnknown* pIUnknown = NULL;
	if (FAILED(::CoCreateInstance(clsId, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown)))
		return;

	IAcadBaseObject* pBaseObj = NULL;
	if (FAILED(pIUnknown->QueryInterface(IID_IAcadBaseObject, (void**)&pBaseObj)))
	{
		pIUnknown->Release();
		return;
	}

	pBaseObj->SetObjectId(selId);

	do
	{
		VARIANT names;
		::VariantInit(&names);
		bool bRet = GET_OPM_PERINSTANCE_EXTENSION_PROTOCOL(pRxClass)->GetObjectPropertySourceNames(&names);
		if (bRet)
		{
			long nCount = 0;
			long llow = 0;
			long lupper = 0;
			SafeArrayGetLBound(names.parray, 1, &llow);
			SafeArrayGetUBound(names.parray, 1, &lupper);
			nCount = lupper - llow + 1;
			if (nCount <= 0)
			{
				pRxClass = pRxClass->myParent();
				if (pRxClass)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			acutPrintf(_T("\n%s Class PropertySource Properties: "), pRxClass->name());

			for (long i = 0; i < nCount; ++i)
			{
				BSTR srcName;
				SafeArrayGetElement(names.parray, &i, &srcName);

				CComPtr<IPropertySource> pSrc;
				pSrc = GET_OPM_PERINSTANCE_PROPERTY_SOURCES()->GetPropertySourceAt(&srcName);
				if (NULL == pSrc)
					continue;

				acutPrintf(_T("\nPropertySource Name: %s."), (CString)srcName);

				VARIANT props;
				::VariantInit(&props);
				if (FAILED(pSrc->GetProperties(pIUnknown, &props)))
					continue;

				long nCount2 = 0;
				long llow2 = 0;
				long lupper2 = 0;
				SafeArrayGetLBound(props.parray, 1, &llow2);
				SafeArrayGetUBound(props.parray, 1, &lupper2);
				nCount2 = lupper2 - llow2 + 1;

				for (long j = 0; j < nCount2; ++j)
				{
					CComPtr<IUnknown> pIUnknownProp;
					SafeArrayGetElement(props.parray, &j, &pIUnknownProp);
					if (!pIUnknownProp)
						continue;

					IDynamicProperty2* pIProp = NULL;
					pIUnknownProp->QueryInterface(IID_IDynamicProperty2, (void**)&pIProp);
					if (pIProp)
					{
						BSTR propName;
						pIProp->GetDisplayName(&propName);
						acutPrintf(_T("\nDisplayName: %s."), (CString)propName);

						BSTR propDesc;
						pIProp->GetDescription(&propDesc);
						acutPrintf(_T("\tDescription: %s."), (CString)propDesc);

						BOOL bEnable = FALSE;
						pIProp->IsPropertyEnabled(pIUnknown, &bEnable);
						acutPrintf(_T("\tPropertyEnable: %s."), bEnable ? _T("Enable") : _T("Disable"));
					}
				}
			}

			break;
		}
		else
		{
			break;
		}
	}
	while (true);
	

	//释放COM环境
	CoUninitialize();
}

//获取选中对象类型PropertyManager上的所有动态属性
void TestCom2()
{
	AcDbObjectIdArray selIds;
	int nRet = ssgetObjects(selIds);
	if (RTNORM != nRet || selIds.length() == 0)
		return;

	CLSID clsId;
	AcRxClass* pRxClass = NULL;
	AcDbEntity* pEnt = NULL;
	AcDbObjectId selId = selIds.at(0);
	acdbOpenObject(pEnt, selId, AcDb::kForRead);
	if (pEnt)
	{
		if (pEnt->isKindOf(AcDbDimension::desc()))
		{
			pRxClass = AcDbDimension::desc();
		}
		else
		{
			pRxClass = pEnt->isA();
		}
		pEnt->getClassID(&clsId);
		pEnt->close();
	}

	if (!pRxClass)
		return;

	//初始化COM环境
	try
	{
		CoInitializeAutoHandler coAutoInit;
	}
	catch (...)
	{
		return;
	}

	IUnknown* pIUnknown = NULL;
	if (FAILED(::CoCreateInstance(clsId, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown)))
		return;

	IAcadBaseObject* pBaseObj = NULL;
	if (FAILED(pIUnknown->QueryInterface(IID_IAcadBaseObject, (void**)&pBaseObj)))
	{
		pIUnknown->Release();
		return;
	}

	pBaseObj->SetObjectId(selId);

	do
	{
		IPropertyManager* pPropertyManager = GET_OPMPROPERTY_MANAGER(pRxClass);
		if (pPropertyManager)
		{
			LONG lPropertyCount = 0;
			if (FAILED(pPropertyManager->GetDynamicPropertyCount(&lPropertyCount)))
				return;

			if (lPropertyCount <= 0)
			{
				pRxClass = pRxClass->myParent();
				if (pRxClass)
				{
					continue;
				}
				else
				{
					break;
				}
			}

			for (LONG i = 0; i < lPropertyCount; ++i)
			{
				IDynamicProperty* pProperty = NULL;
				if (FAILED(pPropertyManager->GetDynamicProperty(i, &pProperty)))
					return;

				if (pProperty)
				{
					BSTR propName;
					pProperty->GetDisplayName(&propName);
					acutPrintf(_T("\nDisplayName: %s."), (CString)propName);

					BSTR propDesc;
					pProperty->GetDescription(&propDesc);
					acutPrintf(_T("\tDescription: %s"), (CString)propDesc);

					//BOOL bEnable = FALSE;
					//pProperty->IsPropertyEnabled(selId, &bEnable);
					//acutPrintf(_T("\tPropertyEnable: %s."), bEnable ? _T("Enable") : _T("Disable"));
				}
			}
			
		}
		else
		{
			pRxClass = pRxClass->myParent();
			if (pRxClass)
			{
				continue;
			}
			else
			{
				break;
			}
		}
	} while (true);


	//释放COM环境
	CoUninitialize();
}

//获取输入命令关联的PropertyManager上的所有动态属性
void TestCom3()
{

}

//获取选中对象类型对应的COM对象的所有方法
void TestCom4()
{
	AcDbObjectIdArray selIds;
	int nRet = ssgetObjects(selIds);
	if (RTNORM != nRet || selIds.length() == 0)
		return;

	CLSID clsId = CLSID_NULL;
	AcRxClass* pRxClass = NULL;
	AcDbEntity* pEnt = NULL;
	Acad::ErrorStatus es = acdbOpenObject(pEnt, selIds[0], AcDb::kForRead);
	if (Acad::eOk != es)
		return;

	pEnt->getClassID(&clsId);
	pEnt->close();

	//初始化COM环境
	try
	{
		CoInitializeAutoHandler coAutoInit;
	}
	catch (...)
	{
		return;
	}

	CComPtr<IUnknown> pIUnknown;
	if (FAILED(::CoCreateInstance(clsId, NULL, CLSCTX_INPROC_SERVER, IID_IUnknown, (void**)&pIUnknown)))
		return;

	CComPtr<IDispatch> pDispatch;
	if (FAILED(pIUnknown->QueryInterface(IID_IDispatch, (void**)&pDispatch)))
		return;

	CComPtr<ITypeInfo> pTypeInfo;
	if (FAILED(pDispatch->GetTypeInfo(0, ::GetSystemDefaultLCID(), &pTypeInfo)))
		return;

	LPTYPEATTR pTypeAttr = NULL;
	if (FAILED(pTypeInfo->GetTypeAttr(&pTypeAttr)))
		return;

	BSTR strName;
	BSTR strDocString;
	DWORD dwHelpContext;
	BSTR strHelpFile;
	LPVARDESC pVarDesc = NULL;
	for (WORD i = 0; i < pTypeAttr->cVars; ++i)
	{
		if (FAILED(pTypeInfo->GetVarDesc(i, &pVarDesc)))
			return;

		if (FAILED(pTypeInfo->GetDocumentation(pVarDesc->memid, &strName, &strDocString, &dwHelpContext, &strHelpFile)))
			return;
	}

	BSTR strFuncName;
	BSTR strFuncDocString;
	DWORD dwFuncHelpContext;
	BSTR strFuncHelpFile;
	LPFUNCDESC pFuncDesc = NULL;
	for (WORD i = 0; i < pTypeAttr->cFuncs; ++i)
	{
		if (FAILED(pTypeInfo->GetFuncDesc(i, &pFuncDesc)))
			return;

		if (FAILED(pTypeInfo->GetDocumentation(pFuncDesc->memid, &strFuncName, &strFuncDocString, &dwFuncHelpContext, &strFuncHelpFile)))
			return;

		CString strFuncIvkKind;
		switch (pFuncDesc->invkind)
		{
		case INVOKE_FUNC:
			strFuncIvkKind = "INVOKE_FUNC";
			break;
		case INVOKE_PROPERTYGET:
			strFuncIvkKind = "INVOKE_PROPERTYGET";
			break;
		case INVOKE_PROPERTYPUT:
			strFuncIvkKind = "INVOKE_PROPERTYPUT";
			break;
		case INVOKE_PROPERTYPUTREF:
			strFuncIvkKind = "INVOKE_PROPERTYPUTREF";
			break;
		default:
			break;
		}
		acutPrintf(_T("\nFunctionName: %s,\t\t\t\t\t\tFunctionInvokeKind: %s"), (CString)strFuncName, strFuncIvkKind);
	}

	//释放COM环境
	CoUninitialize();
}

void TestCom5()
{
}

void TestCom6()
{
}

void TestCom7()
{
}

void TestCom8()
{
}

void TestCom9()
{
}

void TestCom10()
{
}
