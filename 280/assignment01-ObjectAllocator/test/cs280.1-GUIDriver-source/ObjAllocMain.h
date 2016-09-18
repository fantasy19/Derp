//---------------------------------------------------------------------------

#ifndef ObjAllocMainH
#define ObjAllocMainH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ComCtrls.hpp>
#include "ObjectAllocator.h"
#include <ExtCtrls.hpp>

/******************************************************************************
TStudent is the client's "object" that is being managed:
******************************************************************************/
struct TStudent
{
	int Age;   // these 4 bytes get "hijacked" by the allocator for the Next link
	int Year;
	float GPA;
	int ID;
};

typedef TStudent* PStudent;

//---------------------------------------------------------------------------
class TfrmObjAllocMain : public TForm
{
__published:	// IDE-managed Components
	TStatusBar *barStatus;
	TMemo *mmoStatus;
	TSplitter *splitBottom;
	TPageControl *pgeMain;
	TTabSheet *shtDemo;
	TPanel *pnlMain;
	TScrollBox *sboxButtons;
	TButton *btnCreate;
	TButton *btnCreateThreeStudents;
	TButton *btnDeleteFirst;
	TButton *btnCreateOneStudent;
	TButton *btnCreate100Students;
	TButton *btnDestroy;
	TButton *btnError;
	TButton *btnFreeStudents;
	TButton *btnCreateXStudents;
	TEdit *edtCreateXStudents;
	TUpDown *spnCreateXStudents;
	TTabSheet *shtStress;
	TButton *btnStress;
	TGroupBox *grpStress;
	TRadioButton *radUseObjectAllocator;
	TRadioButton *radUseCPPHeapMgr;
	TLabel *lblElapsedTime_;
	TLabel *lblElapsedTime;
	TPageControl *pgeMemory;
	TTabSheet *shtFreeList;
	TLabel *lblAllocated_;
	TLabel *lblAllocated;
	TLabel *lblFreeList_;
	TLabel *lblFreeList;
	TCheckBox *chkInitializeData;
	TCheckBox *chkMemoryDump;
	TListBox *lstStudents;
	TListBox *lstFreeList;
	TButton *btnPopulate;
	TButton *btnDumpMemoryInUse;
	TTabSheet *shtLeaks;
	TMemo *mmoLeaks;
	TButton *btnFreeEmptyPages;
	TPanel *pnlTop;
	TLabel *lblFreeObjects_;
	TLabel *lblFreeObjects;
	TLabel *lblPagesAllocated_;
	TLabel *lblPagesAllocated;
	TLabel *lblObjectsInUse_;
	TLabel *lblObjectsInUse;
	TLabel *lblManagerActive;
	TLabel *lblObjectsAllocated_;
	TLabel *lblObjectsAllocated;
	TLabel *lblObjectsPerPage_;
	TLabel *lblObjectsPerPage;
	TLabel *lblMaxPages_;
	TLabel *lblObjectSize_;
	TLabel *lblSizeOfObject;
	TLabel *lblObjectsPerPage2_;
	TLabel *lblPaddingSize_;
	TLabel *lblPageSize_;
	TLabel *lblPageSize;
	TEdit *edtMaxPages;
	TEdit *edtObjectsPerPage;
	TUpDown *spnMaxPages;
	TUpDown *spnObjectsPerPage;
	TMemo *mmoDisclaimer;
	TCheckBox *chkShowAddresses;
	TCheckBox *chkDebugState;
	TEdit *edtPaddingSize;
	TUpDown *spnPaddingSize;
	TRadioGroup *grpErrors;
	TLabel *Label16;
	TEdit *edtAlignment;
	TUpDown *spnAlignment;
	TLabel *Label1;
	TEdit *edtHeaderSize;
	TUpDown *spnHeaderSize;
	TTabSheet *shtPageList;
	TPanel *pnlMemDumpControls;
	TLabel *lblColumns;
	TEdit *edtColumns;
	TUpDown *spnColumns;
	TRichEdit *mmoPageLists;
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall btnCreateOneStudentClick(TObject *Sender);
	void __fastcall FormClose(TObject *Sender, TCloseAction &Action);
	void __fastcall btnCreateClick(TObject *Sender);
	void __fastcall btnCreateThreeStudentsClick(TObject *Sender);
	void __fastcall btnCreate100StudentsClick(TObject *Sender);
	void __fastcall btnDeleteFirstClick(TObject *Sender);
	void __fastcall btnFreeStudentsClick(TObject *Sender);
	void __fastcall btnErrorClick(TObject *Sender);
	void __fastcall btnDestroyClick(TObject *Sender);
	void __fastcall btnCreateXStudentsClick(TObject *Sender);
	void __fastcall FormActivate(TObject *Sender);
	void __fastcall chkShowAddressesClick(TObject *Sender);
	void __fastcall lstStudentsKeyDown(TObject *Sender, WORD &Key, TShiftState Shift);
	void __fastcall chkDebugStateClick(TObject *Sender);
	void __fastcall chkMemoryDumpClick(TObject *Sender);
	void __fastcall btnStressClick(TObject *Sender);
	void __fastcall btnPopulateClick(TObject *Sender);
	void __fastcall btnDumpMemoryInUseClick(TObject *Sender);
	void __fastcall btnFreeEmptyPagesClick(TObject *Sender);
	void __fastcall edtColumnsChange(TObject *Sender);

private:
	TList *FStudents; // The client's container of students
	unsigned FObjectsPerPage;
	unsigned FMaxPages;
	unsigned FPadBytes;
	bool FIsProcessing;
	unsigned FHeaderSize;
	unsigned FAlignment;

	ObjectAllocator *FStudentObjectMgr; // The object allocator
	long FStudentCount;                 // To generate sequential IDs
	unsigned FDumpColumns;

	void UpdateUI(void);
	void DumpMemory(void);
	bool AddStudent(bool dump = true);
	void FillStudent(PStudent Student);
	void DumpStudents(void);
	void DumpFreeList(void);
	void DeleteSelectedStudent(void);
	int GetMaxItemWidth(TListBox *list);
	void AdjustStudentsScrollbar(void);
	void Stress(void);
	void DumpMemoryInUse(void);
	void CheckAndDumpLeaks(const ObjectAllocator* oa);

	void __fastcall CreateObjectAllocator(void);
	void __fastcall DestroyObjectAllocator(void);
	void __fastcall CreateStudents(int count);
	void __fastcall DeleteFirstStudent(void);
	void __fastcall DeleteAllStudents(void);
	void __fastcall GenerateError(void);

	unsigned GetDumpColumns(void);
	void SetDumpColumns(unsigned value);

	__property unsigned DumpColumns = {read=GetDumpColumns, write=SetDumpColumns};

public:
	__fastcall TfrmObjAllocMain(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfrmObjAllocMain *frmObjAllocMain;
//---------------------------------------------------------------------------
#endif
