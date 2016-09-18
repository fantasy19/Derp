//  ---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"

#include <assert.h>
#include "ObjAllocMain.h"
#include <stdio.h>  // sprintf (Best. Function. Ever.)
#include <algorithm>

TfrmObjAllocMain *frmObjAllocMain;

int GetRandom(int Low, int High)
{
	return rand() % (High - Low + 1) + Low;
}

/*
From CS120 lab!

000000 52 6F 73 65 73 20 61 72  65 20 72 65 64 2E 0A 56   Roses are red..V
000010 69 6F 6C 65 74 73 20 61  72 65 20 62 6C 75 65 2E   iolets are blue.
000020 0A 53 6F 6D 65 20 70 6F  65 6D 73 20 72 68 79 6D   .Some poems rhym
000030 65 2E 0A 42 75 74 20 6E  6F 74 20 74 68 69 73 20   e..But not this
000040 6F 6E 65 2E 0A                                     one..
*/
static void format(const /*unsigned*/ char* inbuf, /*unsigned*/ char *outbuf, int count, int start, int width)
{
	int i;
	int len;

		/* |000000| */
	sprintf(outbuf, " 0x%04X", start);

		/* | 52 6F 73 65 73 20 61 72  65 20 72 65 64 2E 0A 56| */
	for (i = 0; i < count; i++)
	{
		char buf[4];
		sprintf(buf, " %02X", (unsigned char)inbuf[i]);
		strcat(outbuf, buf);

			/* Midway separator */
		//if (i == (width - 1) / 2)
			//strcat(outbuf, " ");
	}

		/* Pad with spaces if  not full line */
	if (count < width)
	{
		int i;
		for (i = 0; i < width - count; i++)
			strcat(outbuf, "   ");

			/* Midway separator */
		//if (count < width / 2)
			//strcat(outbuf, " ");
	}

		/* Separating hex numbers from ASCII text */
	strcat(outbuf, "  ");

	len = (int)strlen(outbuf);
	for (i = 0; i < count; i++)
	{
		/* These do the same thing */
		if ( (inbuf[i] >= 32) && (inbuf[i] < 127) )
		/*if (isprint(inbuf[i]))*/
		{
			outbuf[i + len] = inbuf[i];
			/*printf("%i >= 32\n", inbuf[i]);*/
		}
		else
			outbuf[i + len] = '.';
	}
		/* Terminate the string */
	outbuf[i + len] = 0;
}



//********************************************************************************
//********************************************************************************
//********************************************************************************

__fastcall TfrmObjAllocMain::TfrmObjAllocMain(TComponent* Owner) : TForm(Owner)
{
	FStudents = 0;
	FStudentObjectMgr = 0;
	FStudentCount = 0;
	srand(time(0));
	FIsProcessing = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::FormCreate(TObject *Sender)
{
	lblSizeOfObject->Caption = IntToStr(sizeof(TStudent));
	pnlMain->Align = alClient;
	pgeMain->Align = alClient;
	pgeMemory->Align = alClient;
	pgeMain->ActivePageIndex = 0;
	pgeMemory->ActivePageIndex = 0;
	mmoPageLists->Align = alClient;
	mmoPageLists->PlainText = true;
	mmoLeaks->Align = alClient;
	UpdateUI();

	mmoDisclaimer->Lines->Add("This GUI driver has not been thoroughly tested to handle all possible cases. It is very "
														"likely that there are bugs. It is meant to demonstrate creating graphical driver programs "
														"that can be used to test your projects.");
	mmoDisclaimer->Lines->Add("                       -- The Management");

	this->Caption = "Object Allocator Driver (v 1.03s)";
	mmoStatus->Visible = false;
	splitBottom->Visible = false;
	chkDebugStateClick(this);
	chkDebugStateClick(this);

	lblElapsedTime->Caption = "";
	Width = 900;
	Height = 700;

	FDumpColumns = StrToInt(edtColumns->Text);
}
//---------------------------------------------------------------------------



//********************************************************************************
//********************************************************************************
//********************************************************************************
// Enables/Disable UI components based on the state of the allocator
void TfrmObjAllocMain::UpdateUI(void)
{
	if (FStudentObjectMgr) // enable
	{
		int freeObjects = FStudentObjectMgr->GetStats().FreeObjects_;
		lblFreeObjects->Caption = IntToStr(freeObjects);
		lblFreeList->Caption = IntToStr(freeObjects);

		int pages = FStudentObjectMgr->GetStats().PagesInUse_;
		lblPagesAllocated->Caption = IntToStr(pages);

		int objectsUsed = FStudentObjectMgr->GetStats().ObjectsInUse_;
		lblObjectsInUse->Caption = IntToStr(objectsUsed);

		int pagesize = FStudentObjectMgr->GetStats().PageSize_;
		lblPageSize->Caption = IntToStr(pagesize);

		lblObjectsPerPage->Caption = IntToStr(FObjectsPerPage);
		lblObjectsAllocated->Caption = IntToStr(pages * FObjectsPerPage);
		lblManagerActive->Caption = "Object Manager is ACTIVE";
		lblManagerActive->Font->Style = TFontStyles()<< fsBold;
		btnCreate->Enabled = false;
		btnDestroy->Enabled = true;
		btnCreateOneStudent->Enabled = true;
		btnCreateThreeStudents->Enabled = true;
		btnCreate100Students->Enabled = true;
		btnFreeStudents->Enabled = true;
		btnDeleteFirst->Enabled = true;
		btnError->Enabled = true;
		btnCreateXStudents->Enabled = true;
		btnDumpMemoryInUse->Enabled = true;
		btnFreeEmptyPages->Enabled = true;
	}
	else // disable or set to 0
	{
		for (int i = 0; i < sboxButtons->ControlCount; i++)
		{
			TControl *control = sboxButtons->Controls[i];
			if (control->InheritsFrom(__classid(TButton)))
				control->Enabled = false;
			if (dynamic_cast<TButton *>(control) != 0)
				control->Enabled = false;
		}
		btnCreate->Enabled = true;

		lblAllocated->Caption = "0";
		lblPagesAllocated->Caption = "0";
		lblObjectsPerPage->Caption = "0";
		lblObjectsAllocated->Caption = "0";
		lblFreeObjects->Caption = "0";
		lblFreeList->Caption = "0";
		lblObjectsInUse->Caption = "0";
		lblPageSize->Caption = "0";
		lblManagerActive->Caption = "Object Manager is INACTIVE";
		lblManagerActive->Font->Style = TFontStyles();
	}
}

/*
	Deletes the selected student and returns the memory to
	the object allocator
*/
void TfrmObjAllocMain::DeleteSelectedStudent(void)
{
	if (!FStudentObjectMgr)
		return;

		// Get selected student
	int index = lstStudents->ItemIndex;
	if ( (FStudents->Count > 0) && (index > -1) )
	{
		PStudent student = reinterpret_cast<PStudent>(FStudents->Items[index]);
		FStudents->Delete(index);
		FStudentObjectMgr->Free(student);
		UpdateUI();
		DumpStudents();
		DumpFreeList();
	  DumpMemory();

			// If there are students remaining in the list, highlight the next one
		if (FStudents->Count > 0)
		{
			index = (index >= FStudents->Count) ? FStudents->Count - 1 : index;
			lstStudents->ItemIndex = index;
			lstStudents->Selected[index] = true;
		}
	}
}

/*
	Add another student to the list (allocate from object allocator).
	If dump is true, redisplay the lists. Returns true, if successful.
*/
bool TfrmObjAllocMain::AddStudent(bool dump)
{
	PStudent ps;
	try
	{
		ps = (PStudent)reinterpret_cast<PStudent>(FStudentObjectMgr->Allocate());
	}
	catch (const OAException& e)
	{
		MessageDlg(AnsiString("Exception caught!\n\n") +  e.what(), mtError, TMsgDlgButtons() << mbOK, 0);
		return false;
	}

	FStudentCount++;

		// Initializing data?
	if (chkInitializeData->Checked)
		FillStudent(ps);

		// Add the student and adjust UI counters
	FStudents->Add(ps);
	UpdateUI();

		// Redisplay memory
	if (dump)
	{
		DumpStudents();
		DumpFreeList();
		DumpMemory();
	}
	return true;
}

/*
	Fill Student record with random data.
*/
void TfrmObjAllocMain::FillStudent(PStudent Student)
{
	Student->ID = FStudentCount;
	Student->Age = GetRandom(18, 28);
	Student->GPA = GetRandom(300, 400) / 100.0F;
	Student->Year = GetRandom(1, 4);
}

/*
	Format and display all of the students in the student list.
*/
void TfrmObjAllocMain::DumpStudents(void)
{
	lstStudents->Items->BeginUpdate();

		lstStudents->Clear(); // empty the list

			// Format each student record and display it in the list
		for (int i = 0; i < FStudents->Count; i++)
		{
			PStudent student = reinterpret_cast<PStudent>( FStudents->Items[i]);
			AnsiString s;
			if (chkShowAddresses->Checked)
			{
				if (chkMemoryDump->Checked)
					s = Format("(%p) %.8X %.8X %.8X %.8X", ARRAYOFCONST((student, student->Age, student->Year, *(int *)(&student->GPA), student->ID)));
				else
					s = Format("(%p) Age: %d, Year=%d, GPA=%1.2f, ID: %.3d", ARRAYOFCONST((student, student->Age, student->Year, student->GPA, student->ID)));
			}
			else
			{
				if (chkMemoryDump->Checked)
					s = Format("%.8X %.8X %.8X %.8X", ARRAYOFCONST((student->Age, student->Year, *(int *)(&student->GPA), student->ID)));
				else
					s = Format("Age: %d, Year=%d, GPA=%1.2f, ID: %.3d", ARRAYOFCONST((student->Age, student->Year, student->GPA, student->ID)));
			}
			lstStudents->Items->Add(s);
		}
		lblAllocated->Caption = IntToStr(FStudents->Count);

	lstStudents->Items->EndUpdate();

	AdjustStudentsScrollbar();
	Application->ProcessMessages();
}

/*
	Format and display all of the free blocks on the free list.
*/
void TfrmObjAllocMain::DumpFreeList(void)
{
	PStudent student;

	lstFreeList->Items->BeginUpdate();
		lstFreeList->Clear();
	lstFreeList->Items->EndUpdate();

	if (!FStudentObjectMgr)
		return;

	PStudent head = const_cast<PStudent>(reinterpret_cast<const TStudent *>(FStudentObjectMgr->GetFreeList()));
	lstFreeList->Items->BeginUpdate();

			// Format each node on the free list and display its contents
		while (head)
		{
			student = head;
			AnsiString s;
			if (chkShowAddresses->Checked)
				s = Format("(%p) %.8x %.8x %.8x %.8x", ARRAYOFCONST((student, student->Age, student->Year, *(int *)&student->GPA, student->ID)));
			else
				s = Format("%.8x %.8x %.8x %.8x", ARRAYOFCONST((student->Age, student->Year, *(int *)&student->GPA, student->ID)));
			lstFreeList->Items->Add(s);
			head = reinterpret_cast<PStudent>(head->Age);
		}

	lstFreeList->Items->EndUpdate();
	Application->ProcessMessages();
}

/*
	Creates an object allocator from the user's configuration.
*/
void  __fastcall TfrmObjAllocMain::CreateObjectAllocator(void)
{
	FStudents->Free();
	FStudents = new TList();
	FStudentCount = 0;

	int size = sizeof(TStudent);
	FMaxPages = StrToInt(edtMaxPages->Text);
	FObjectsPerPage = StrToInt(edtObjectsPerPage->Text);
	FPadBytes = StrToInt(edtPaddingSize->Text);
	FHeaderSize = StrToInt(edtHeaderSize->Text);
	FAlignment = StrToInt(edtAlignment->Text);

	OAConfig config(false,
									FObjectsPerPage,
									FMaxPages,
									chkDebugState->Checked,
									FPadBytes,
									FHeaderSize,
									FAlignment
								 );

	FStudentObjectMgr = new ObjectAllocator(size, config);
	assert(FStudentObjectMgr);
	btnCreate->Enabled = false;
	UpdateUI();
	DumpFreeList();
	DumpMemory();
}

/*
	Deletes the object allocator and reinitializes the UI
*/
void  __fastcall TfrmObjAllocMain::DestroyObjectAllocator(void)
{
	assert(FStudentObjectMgr);
	delete FStudentObjectMgr;
	FStudentObjectMgr = 0;

	btnCreate->Enabled = true;
	FStudents->Clear();
	DumpStudents();
	DumpFreeList();
	DumpMemory();
	UpdateUI();
}

/*
	Creates 'count' random students.
*/
void  __fastcall TfrmObjAllocMain::CreateStudents(int count)
{
	for (int i = 0; i < count; i++)
		if (!AddStudent(false))
			break;

	DumpStudents();
	DumpFreeList();
	DumpMemory();
}

/*
	Frees the first student in the list.
*/
void  __fastcall TfrmObjAllocMain::DeleteFirstStudent(void)
{
	assert(FStudentObjectMgr);
	if (FStudents->Count > 0)
	{
		PStudent student = reinterpret_cast<PStudent>(FStudents->Items[0]);
		FStudents->Delete(0);
		FStudentObjectMgr->Free(student);
		UpdateUI();
		DumpStudents();
		DumpFreeList();
		DumpMemory();
	}
}

/*
	Frees all students in the list.
*/
void  __fastcall TfrmObjAllocMain::DeleteAllStudents(void)
{
	PStudent student;

	int count = FStudents->Count;
	for (int i = 0; i < count; i++)
	{
		student = reinterpret_cast<PStudent>(FStudents->Items[0]);
		FStudents->Delete(0);
		FStudentObjectMgr->Free(student);
	}
	UpdateUI();
	DumpStudents();
	DumpFreeList();
	DumpMemory();
}

/* From text driver */
void TestCorruption(void)
{
	unsigned char *p;
	unsigned i, padbytes = 8;
	OAConfig config(false, 4, 2, true, padbytes);
	ObjectAllocator oa(sizeof(TStudent), config);
	TStudent *pStudent1 = reinterpret_cast<TStudent *>( oa.Allocate() );

		// corrupt left pad bytes
	p = const_cast<unsigned char*>(reinterpret_cast<const unsigned char *>(pStudent1)) - padbytes;
	for (i = 0; i < padbytes - 2; i++)
		*p++ = 0xFF;

	oa.Free(pStudent1);
}

void TestNoPages(void)
{
		OAConfig config(false, 4, 2, false, true, 0, false);
		ObjectAllocator oa(sizeof(TStudent), config);

		for (int i = 0; i < 9; i++)
			oa.Allocate();
}

/*
	Generates an OAException.
*/
//{E_NO_MEMORY, E_NO_PAGES, E_BAD_ADDRESS, E_BAD_BOUNDARY, E_MULTIPLE_FREE, E_CORRUPTED_BLOCK};
void  __fastcall TfrmObjAllocMain::GenerateError(void)
{
	OAException::OA_EXCEPTION ex = static_cast<OAException::OA_EXCEPTION>(grpErrors->ItemIndex);
	try
	{
		PStudent free = const_cast<PStudent>(reinterpret_cast<const TStudent *>(FStudentObjectMgr->GetFreeList()));
		PStudent student = 0;
		if (FStudents->Count)
			student = reinterpret_cast<PStudent>(FStudents->Items[0]);
		switch (ex)
		{
			case OAException::E_NO_MEMORY:
				MessageDlg(AnsiString("No exception thrown.\n\nNot implemented in driver."), mtInformation, TMsgDlgButtons() << mbOK, 0);
			break;

			case OAException::E_NO_PAGES:
				TestNoPages();
			break;

			case OAException::E_BAD_BOUNDARY:
				if (chkDebugState->Checked)
					if (student)
						FStudentObjectMgr->Free(reinterpret_cast<char *>(student) + 3);
					else
						MessageDlg(AnsiString("No exception thrown.\n\nNo blocks allocated."), mtInformation, TMsgDlgButtons() << mbOK, 0);
				else
					MessageDlg(AnsiString("No exception thrown.\n\nDebug information is disabled."), mtInformation, TMsgDlgButtons() << mbOK, 0);
			break;

			case OAException::E_BAD_ADDRESS:
				if (chkDebugState->Checked)
					FStudentObjectMgr->Free(reinterpret_cast<void *>(100));
				else
					MessageDlg(AnsiString("No exception thrown.\n\nDebug information is disabled."), mtInformation, TMsgDlgButtons() << mbOK, 0);
			break;

			case OAException::E_MULTIPLE_FREE:
				if (chkDebugState->Checked)
					if (free)
						FStudentObjectMgr->Free(free);
					else
						MessageDlg(AnsiString("No exception thrown.\n\nNo blocks on the free list."), mtInformation, TMsgDlgButtons() << mbOK, 0);
				else
					MessageDlg(AnsiString("No exception thrown.\n\nDebug information is disabled."), mtInformation, TMsgDlgButtons() << mbOK, 0);
			break;

			case OAException::E_CORRUPTED_BLOCK:
				if (chkDebugState->Checked)
					TestCorruption();
				else
					MessageDlg(AnsiString("No exception thrown.\n\nDebug information is disabled."), mtInformation, TMsgDlgButtons() << mbOK, 0);
			break;
		}
	}
	catch (const OAException& e)
	{
		MessageDlg(AnsiString("Exception caught!\n\n") +  e.what(), mtError, TMsgDlgButtons() << mbOK, 0);
		//ShowMessage(e.what());
	}
}


//********************************************************************************
//********************************************************************************
//********************************************************************************
// Event Handlers
//
//********************************************************************************
void __fastcall TfrmObjAllocMain::FormClose(TObject *Sender, TCloseAction &Action)
{
	FStudents->Free();
	if (FStudentObjectMgr)
		delete FStudentObjectMgr;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnCreateOneStudentClick(TObject *Sender)
{
	AddStudent();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnCreateClick(TObject *Sender)
{
	CreateObjectAllocator();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnCreateThreeStudentsClick(TObject *Sender)
{
	CreateStudents(3);
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnCreate100StudentsClick(TObject *Sender)
{
  CreateStudents(100);
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnDeleteFirstClick(TObject *Sender)
{
  DeleteFirstStudent();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnFreeStudentsClick(TObject *Sender)
{
  DeleteAllStudents();
}
//---------------------------------------------------------------------------

 void __fastcall TfrmObjAllocMain::btnErrorClick(TObject *Sender)
{
	GenerateError();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnDestroyClick(TObject *Sender)
{
  DestroyObjectAllocator();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnCreateXStudentsClick(TObject *Sender)
{
  CreateStudents(StrToInt(edtCreateXStudents->Text));
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::FormActivate(TObject *Sender)
{
  btnCreate->SetFocus();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::chkShowAddressesClick(TObject *Sender)
{
	if (FStudentObjectMgr)
	{
		DumpStudents();
		DumpFreeList();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::lstStudentsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift)
{
  if (Key == VK_DELETE)
    DeleteSelectedStudent();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::chkDebugStateClick(TObject *Sender)
{
	if (FStudentObjectMgr)
		FStudentObjectMgr->SetDebugState(chkDebugState->Checked);

	/*
	for (int i = 2; i < 6; i++)
	{
		grpErrors->Buttons[i]->Enabled = chkDebugState->Checked;
		grpErrors->Buttons[i]->Checked = 0;
	}
	*/
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::chkMemoryDumpClick(TObject *Sender)
{
	if (FStudents)
		DumpStudents();
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnStressClick(TObject *Sender)
{
	if (FIsProcessing)
	{
		FIsProcessing = false;
		return;
	}

	FIsProcessing = true;
	btnStress->Caption = "Stop";
	Screen->Cursor = crAppStart;
		Stress();
	btnStress->Caption = "Stress";
	Screen->Cursor = crDefault;
	FIsProcessing = false;
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnPopulateClick(TObject *Sender)
{
	edtMaxPages->Text = "2048";
	edtObjectsPerPage->Text = "2048";
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnDumpMemoryInUseClick(TObject *Sender)
{
	mmoLeaks->Clear();
	pgeMemory->ActivePageIndex = 2;
	CheckAndDumpLeaks(FStudentObjectMgr);
}
//---------------------------------------------------------------------------

void __fastcall TfrmObjAllocMain::btnFreeEmptyPagesClick(TObject *Sender)
{
	if (!FStudentObjectMgr)
		return;

	FStudentObjectMgr->FreeEmptyPages();
	DumpFreeList();
	DumpMemory();
	UpdateUI();
}
//---------------------------------------------------------------------------

void TfrmObjAllocMain::AdjustStudentsScrollbar(void)
{
	int width = GetMaxItemWidth(lstStudents);
	if (width > lstStudents->Width)
		lstStudents->ScrollWidth = width;
	else
		lstStudents->ScrollWidth = 0;
}

int TfrmObjAllocMain::GetMaxItemWidth(TListBox *list)
{
	AnsiString s;
	int max = 0;
	list->Canvas->Font = list->Font;
	for (int i = 0; i < list->Count; i++)
	{
		s = list->Items->Strings[i];
		int width = list->Canvas->TextWidth(s);
		if (width > max)
			max = width;
	}
	return max + 10;
}


//******************************************************************************
// Most of the rest is from the text driver.
//
//******************************************************************************

#include <time.h>
int RandomInt(int low, int high)
{
	return rand() % (high - low + 1) + low;
}

template <typename T>
void SwapT(T &a, T &b)
{
	T temp = a;
	a = b;
	b = temp;
}

template <typename T>
void Shuffle(T *array, unsigned count)
{
	for (unsigned int i = 0; i < count; i++)
	{
		int r = RandomInt(i, (int)count - 1);
		SwapT(array[i], array[r]);
	}
}

void TfrmObjAllocMain::Stress(void)
{
	clock_t start, end;

	int size = sizeof(TStudent);
	int MaxPages = StrToInt(edtMaxPages->Text);
	int ObjectsPerPage = StrToInt(edtObjectsPerPage->Text);
	int padbytes = StrToInt(edtPaddingSize->Text);
	int Align = StrToInt(edtAlignment->Text);
	int headers = StrToInt(edtHeaderSize->Text);
	int total = MaxPages * ObjectsPerPage;
	void **ptrs = new void *[total];

	OAConfig config(radUseCPPHeapMgr->Checked,
									ObjectsPerPage,
									MaxPages,
									chkDebugState->Checked,
									padbytes,
									headers,
									Align
								 );
	ObjectAllocator *oa = new ObjectAllocator(size, config);

	start = clock();
	for (int i = 0; i < total; i++)
	{
		void *p = oa->Allocate();
		ptrs[i] = p;
		Application->ProcessMessages();
	}

	if (FIsProcessing)
	{
		Shuffle(ptrs, total);
		for (int i = 0; i < total; i++)
		{
			oa->Free(ptrs[i]);
			Application->ProcessMessages();
			if (!FIsProcessing)
				break;
		}
	}

	delete oa;
	end = clock();
	lblElapsedTime->Caption = Format("%4.3f secs", ARRAYOFCONST(( ((double)end - start) / 1000)) );
	delete [] ptrs;
}

void DumpCallback(const void *block, unsigned int actual_size);

void TfrmObjAllocMain::CheckAndDumpLeaks(const ObjectAllocator* oa)
{
	if (oa->GetStats().ObjectsInUse_)
	{
		frmObjAllocMain->mmoLeaks->Lines->Add("Detected memory leaks!");
		frmObjAllocMain->mmoLeaks->Lines->Add("Dumping objects ->");
		int leaks = oa->DumpMemoryInUse(DumpCallback);

		char buf[100];
		sprintf(buf, "Object dump complete. [%i]", leaks);
		frmObjAllocMain->mmoLeaks->Lines->Add(buf);
	}
	else
		frmObjAllocMain->mmoLeaks->Lines->Add("No leaks detected.");
}

void TfrmObjAllocMain::DumpMemoryInUse(void)
{
	if (!FStudentObjectMgr)
		return;

	FStudentObjectMgr->DumpMemoryInUse(DumpCallback);
}

void DumpCallback(const void *block, unsigned int actual_size)
{
		// If we were passed a NULL pointer, do nothing
	if (!block)
		return;

	unsigned int size = actual_size;

		// limit to 16 bytes
	if (actual_size > 16)
		size = 16;

	char *buf = new char[size * 10 + 100];
	unsigned char *data = const_cast<unsigned char*>(reinterpret_cast<const unsigned char *>(block));

#define SHOWADDRESS
#ifdef SHOWADDRESS
	sprintf(buf, "Block at 0x%p, %i bytes long.", block, actual_size);
	frmObjAllocMain->mmoLeaks->Lines->Add(buf);

#else
	sprintf(buf, "Block at 0x00000000, %i bytes long.", actual_size);
	frmObjAllocMain->mmoLeaks->Lines->Add(buf);
#endif

	sprintf(buf, " Data: <");
	for (unsigned int i = 0; i < size; i++)
	{
		unsigned char c = *data++;
		if (c > 31 && c < 128) // printable range
			sprintf(buf + strlen(buf), "%c", c);
		else
			sprintf(buf + strlen(buf), " ");
	}
	sprintf(buf + strlen(buf), ">");

	data = const_cast<unsigned char*>(reinterpret_cast<const unsigned char *>(block));
	for (unsigned int i = 0; i < size; i++)
		sprintf(buf + strlen(buf), " %02X", (int)*data++);

	frmObjAllocMain->mmoLeaks->Lines->Add(buf);

	delete [] buf;
}

static const int MINCOLUMNS = 4;
unsigned TfrmObjAllocMain::GetDumpColumns(void)
{
	return (FDumpColumns < MINCOLUMNS) ? MINCOLUMNS : FDumpColumns;
}

void TfrmObjAllocMain::SetDumpColumns(unsigned value)
{
	FDumpColumns = (value < MINCOLUMNS) ? MINCOLUMNS : value;
}

void TfrmObjAllocMain::DumpMemory(void)
{
	if (!FStudentObjectMgr)
		return;

	mmoPageLists->Clear();

	OAStats stats = FStudentObjectMgr->GetStats();
	unsigned size = stats.PageSize_;
	unsigned pagecount = stats.PagesInUse_;
	int width = DumpColumns;
	if (!pagecount)
		return;

	char *inbuf = new char[width * 10];
	char *outbuf = new char[width * 10];

		// Limit the number of pages to dump
	const int MAXPAGES = 20;
	unsigned actual = (pagecount >= MAXPAGES) ? MAXPAGES : pagecount;

	const char *pages = static_cast<const char *>(FStudentObjectMgr->GetPageList());
	unsigned count = 0;
	while (pages)
	{
		const char *realpage = pages;
		char *ptr = outbuf;

			/* Header line */
		sprintf(ptr, "%7s", "");
		ptr += 7;
		for (int i = 0; i < width; i++)
		{
			//if (i == (width) / 2)
				//sprintf(ptr++, " ");

			sprintf(ptr, " %02X", i);
			ptr += 3;
		}

			// ASCII side
		sprintf(ptr, "  ");
		ptr += 2;
		for (int i = 0; i < width; i++)
		{
			sprintf(ptr, "%X", i % 16);
			ptr++;
		}

		//mmoPageLists->Lines->BeginUpdate();
		mmoPageLists->Lines->Add(outbuf);

		int bytes = size;
		int start = 0;
		while (bytes >= width)
		{
			format(realpage + start, outbuf, width, start, width);
			mmoPageLists->Lines->Add(outbuf);
			bytes -= width;
			start += width;
		}

		if (bytes)
			format(realpage + start, outbuf, bytes, start, width);

		mmoPageLists->Lines->Add(outbuf);
		mmoPageLists->Lines->Add("");

		count++;
		if (count > actual)
			break;

		pages = reinterpret_cast<const char *>((reinterpret_cast<const GenericObject *>(realpage))->Next);
	}
	//mmoPageLists->Lines->EndUpdate();

	delete [] inbuf;
	delete [] outbuf;
}

void __fastcall TfrmObjAllocMain::edtColumnsChange(TObject *Sender)
{
	try
	{
		FDumpColumns = StrToInt(edtColumns->Text);
		if (FDumpColumns > 256)
			FDumpColumns = 256;
		if (FDumpColumns < 1)
			FDumpColumns = 1;
		DumpMemory();
	}
	catch (...) // hack to deal with invalid input
	{
	}
}
//---------------------------------------------------------------------------



