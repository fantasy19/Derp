//---------------------------------------------------------------------------

#include <vcl.h>
#pragma hdrstop

USEFORM("ObjAllocMain.cpp", frmObjAllocMain);
//---------------------------------------------------------------------------
WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	try
	{
		Application->Initialize();
		Application->Title = "ObjectAllocator Driver";
		Application->CreateForm(__classid(TfrmObjAllocMain), &frmObjAllocMain);
		Application->Run();
	}
	catch (Exception &exception)
	{
		Application->ShowException(&exception);
	}
	return 0;
}
//---------------------------------------------------------------------------
