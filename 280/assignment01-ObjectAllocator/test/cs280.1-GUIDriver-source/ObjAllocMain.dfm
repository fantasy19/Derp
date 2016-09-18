object frmObjAllocMain: TfrmObjAllocMain
  Left = 177
  Top = 140
  Anchors = [akLeft, akTop, akRight]
  Caption = 'Object Allocator Driver'
  ClientHeight = 784
  ClientWidth = 909
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001002020100000000000E80200001600000028000000200000004000
    0000010004000000000080020000000000000000000000000000000000000000
    000000008000008000000080800080000000800080008080000080808000C0C0
    C0000000FF0000FF000000FFFF00FF000000FF00FF00FFFF0000FFFFFF000000
    000000000000330077000000000000000000000000003B077070000000000000
    000000000000BB807007000000000000000000000300B0007000700000000000
    00000000330070070700070000000000000000003B0700700070007000000000
    00000000BB800700000700070000000000000300B00070000000700070000000
    0000330070070000000007000700000000003B07007000000000007007000000
    0000BB800700000000000007070000000300B000700000000070000077000000
    330070070000000007000000803300003B070070000000000000000800330000
    BB8007000000000000000080BBBB0300B000700000000070000008000BB03300
    70070000000707000000803300003B070070000000707000000800330000BB80
    07000000070700000080BBBB0000B000700000000070000008000BB000007007
    0000000007000000803300000000707000007770000000080033000000008700
    0007070700000080BBBB00000000080000077777000008000BB0000000000080
    0007070700008033000000000000000800007770000800330000000000000000
    800000000080BBBB00000000000000000800000008000BB00000000000000000
    0080000080330000000000000000000000080008003300000000000000000000
    00008080BBBB00000000000000000000000008000BB00000000000000000FFFF
    33FFFFFF21FFFFFF00FFFFFB007FFFF3003FFFF2001FFFF0000FFFB00007FF30
    0003FF200003FF000003FB000003F3000000F2000000F0000010B00000393000
    000F2000000F0000010F0000039F000000FF000000FF000010FF800039FFC000
    0FFFE0000FFFF0010FFFF8039FFFFC00FFFFFE00FFFFFF10FFFFFFB9FFFF}
  OldCreateOrder = False
  Position = poScreenCenter
  ShowHint = True
  OnActivate = FormActivate
  OnClose = FormClose
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object splitBottom: TSplitter
    Left = 0
    Top = 708
    Width = 909
    Height = 3
    Cursor = crVSplit
    Align = alBottom
    AutoSnap = False
    MinSize = 1
    ResizeStyle = rsUpdate
    ExplicitTop = 497
    ExplicitWidth = 892
  end
  object barStatus: TStatusBar
    Left = 0
    Top = 765
    Width = 909
    Height = 19
    Panels = <>
  end
  object mmoStatus: TMemo
    Left = 0
    Top = 711
    Width = 909
    Height = 54
    Align = alBottom
    ScrollBars = ssVertical
    TabOrder = 1
  end
  object pgeMain: TPageControl
    Left = 8
    Top = 116
    Width = 833
    Height = 569
    ActivePage = shtDemo
    TabOrder = 2
    object shtDemo: TTabSheet
      Caption = 'Demo'
      object pnlMain: TPanel
        Left = 16
        Top = 12
        Width = 789
        Height = 521
        BevelOuter = bvLowered
        TabOrder = 0
        object sboxButtons: TScrollBox
          Left = 1
          Top = 1
          Width = 144
          Height = 519
          Align = alLeft
          BorderStyle = bsNone
          TabOrder = 0
          object btnCreate: TButton
            Left = 8
            Top = 8
            Width = 129
            Height = 25
            Caption = 'Create object allocator'
            TabOrder = 0
            OnClick = btnCreateClick
          end
          object btnCreateThreeStudents: TButton
            Left = 8
            Top = 104
            Width = 129
            Height = 25
            Caption = 'Create 3 students'
            TabOrder = 3
            OnClick = btnCreateThreeStudentsClick
          end
          object btnDeleteFirst: TButton
            Left = 8
            Top = 211
            Width = 129
            Height = 25
            Caption = 'Delete first student'
            TabOrder = 8
            OnClick = btnDeleteFirstClick
          end
          object btnCreateOneStudent: TButton
            Left = 8
            Top = 72
            Width = 129
            Height = 25
            Caption = 'Create 1 student'
            TabOrder = 2
            OnClick = btnCreateOneStudentClick
          end
          object btnCreate100Students: TButton
            Left = 8
            Top = 136
            Width = 129
            Height = 25
            Caption = 'Create 100 students'
            TabOrder = 4
            OnClick = btnCreate100StudentsClick
          end
          object btnDestroy: TButton
            Left = 8
            Top = 40
            Width = 129
            Height = 25
            Caption = 'Destroy object allocator'
            TabOrder = 1
            OnClick = btnDestroyClick
          end
          object btnError: TButton
            Left = 8
            Top = 337
            Width = 129
            Height = 25
            Caption = 'Generate error (debug)'
            TabOrder = 10
            OnClick = btnErrorClick
          end
          object btnFreeStudents: TButton
            Left = 8
            Top = 243
            Width = 129
            Height = 25
            Caption = 'Delete all students'
            TabOrder = 9
            OnClick = btnFreeStudentsClick
          end
          object btnCreateXStudents: TButton
            Left = 8
            Top = 168
            Width = 57
            Height = 25
            Caption = 'Create'
            TabOrder = 5
            OnClick = btnCreateXStudentsClick
          end
          object edtCreateXStudents: TEdit
            Left = 72
            Top = 172
            Width = 45
            Height = 21
            TabStop = False
            TabOrder = 6
            Text = '0'
          end
          object spnCreateXStudents: TUpDown
            Left = 117
            Top = 172
            Width = 15
            Height = 21
            Associate = edtCreateXStudents
            TabOrder = 7
          end
          object btnDumpMemoryInUse: TButton
            Left = 8
            Top = 274
            Width = 129
            Height = 25
            Caption = 'Dump Memory In Use'
            TabOrder = 11
            OnClick = btnDumpMemoryInUseClick
          end
          object btnFreeEmptyPages: TButton
            Left = 8
            Top = 306
            Width = 129
            Height = 25
            Caption = 'Free Empty Pages'
            Enabled = False
            TabOrder = 12
            OnClick = btnFreeEmptyPagesClick
          end
          object grpErrors: TRadioGroup
            Left = 8
            Top = 368
            Width = 129
            Height = 137
            Caption = 'Error type to generate'
            ItemIndex = 1
            Items.Strings = (
              'No memory'
              'No pages'
              'Bad address'
              'Bad boundary'
              'Multiple free'
              'Corrupted block')
            TabOrder = 13
          end
        end
        object pgeMemory: TPageControl
          Left = 160
          Top = 9
          Width = 605
          Height = 297
          ActivePage = shtPageList
          MultiLine = True
          TabOrder = 1
          TabPosition = tpBottom
          object shtFreeList: TTabSheet
            Caption = 'Free list'
            ExplicitLeft = 0
            ExplicitTop = 0
            ExplicitWidth = 0
            ExplicitHeight = 0
            DesignSize = (
              597
              271)
            object lblAllocated_: TLabel
              Left = 8
              Top = 4
              Width = 47
              Height = 13
              Caption = 'Allocated:'
            end
            object lblAllocated: TLabel
              Left = 60
              Top = 4
              Width = 54
              Height = 13
              Caption = 'lblAllocated'
            end
            object lblFreeList_: TLabel
              Left = 380
              Top = 4
              Width = 39
              Height = 13
              Caption = 'Free list:'
            end
            object lblFreeList: TLabel
              Left = 424
              Top = 4
              Width = 47
              Height = 13
              Caption = 'lblFreeList'
            end
            object chkInitializeData: TCheckBox
              Left = 156
              Top = 2
              Width = 89
              Height = 17
              Caption = 'Initialize data'
              Checked = True
              State = cbChecked
              TabOrder = 0
            end
            object chkMemoryDump: TCheckBox
              Left = 260
              Top = 2
              Width = 89
              Height = 17
              Caption = 'Memory dump'
              TabOrder = 1
              OnClick = chkMemoryDumpClick
            end
            object lstStudents: TListBox
              Left = 8
              Top = 20
              Width = 353
              Height = 241
              Hint = 'Press delete key to free the selected Student'
              Anchors = [akLeft, akTop, akBottom]
              Font.Charset = GREEK_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'Courier New'
              Font.Style = []
              ItemHeight = 14
              ParentFont = False
              TabOrder = 2
              OnKeyDown = lstStudentsKeyDown
            end
            object lstFreeList: TListBox
              Left = 368
              Top = 20
              Width = 221
              Height = 241
              Anchors = [akLeft, akTop, akRight, akBottom]
              Font.Charset = GREEK_CHARSET
              Font.Color = clWindowText
              Font.Height = -11
              Font.Name = 'Courier New'
              Font.Style = []
              ItemHeight = 14
              ParentFont = False
              TabOrder = 3
            end
          end
          object shtPageList: TTabSheet
            Caption = 'Page list'
            ImageIndex = 2
            object pnlMemDumpControls: TPanel
              Left = 0
              Top = 232
              Width = 597
              Height = 39
              Align = alBottom
              BevelOuter = bvNone
              TabOrder = 0
              object lblColumns: TLabel
                Left = 8
                Top = 12
                Width = 43
                Height = 13
                Caption = 'Columns:'
              end
              object edtColumns: TEdit
                Left = 57
                Top = 8
                Width = 29
                Height = 21
                TabOrder = 0
                Text = '16'
                OnChange = edtColumnsChange
              end
              object spnColumns: TUpDown
                Left = 86
                Top = 8
                Width = 15
                Height = 21
                Associate = edtColumns
                Min = 4
                Max = 256
                Position = 16
                TabOrder = 1
              end
            end
            object mmoPageLists: TRichEdit
              Left = 31
              Top = 30
              Width = 470
              Height = 163
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -15
              Font.Name = 'Courier New'
              Font.Style = []
              ParentFont = False
              ScrollBars = ssBoth
              TabOrder = 1
              WordWrap = False
            end
          end
          object shtLeaks: TTabSheet
            Caption = 'Leaks'
            ImageIndex = 1
            ExplicitLeft = 0
            ExplicitTop = 0
            ExplicitWidth = 0
            ExplicitHeight = 0
            object mmoLeaks: TMemo
              Left = 56
              Top = 56
              Width = 441
              Height = 161
              Font.Charset = ANSI_CHARSET
              Font.Color = clWindowText
              Font.Height = -13
              Font.Name = 'Courier New'
              Font.Style = []
              ParentFont = False
              TabOrder = 0
            end
          end
        end
      end
    end
    object shtStress: TTabSheet
      Caption = 'Stress'
      ImageIndex = 2
      ExplicitLeft = 0
      ExplicitTop = 0
      ExplicitWidth = 0
      ExplicitHeight = 0
      object lblElapsedTime_: TLabel
        Left = 124
        Top = 116
        Width = 66
        Height = 13
        Caption = 'Elapsed time: '
      end
      object lblElapsedTime: TLabel
        Left = 196
        Top = 116
        Width = 71
        Height = 13
        Caption = 'lblElapsedTime'
      end
      object btnStress: TButton
        Left = 20
        Top = 108
        Width = 75
        Height = 25
        Caption = 'Stress'
        TabOrder = 0
        OnClick = btnStressClick
      end
      object grpStress: TGroupBox
        Left = 16
        Top = 20
        Width = 157
        Height = 77
        Caption = 'Memory manager'
        TabOrder = 1
        object radUseObjectAllocator: TRadioButton
          Left = 12
          Top = 24
          Width = 129
          Height = 17
          Caption = 'Use object allocator'
          Checked = True
          TabOrder = 0
          TabStop = True
        end
        object radUseCPPHeapMgr: TRadioButton
          Left = 12
          Top = 48
          Width = 141
          Height = 17
          Caption = 'Use C++ heap manager'
          TabOrder = 1
        end
      end
      object btnPopulate: TButton
        Left = 20
        Top = 144
        Width = 75
        Height = 25
        Caption = '2048 x 2048'
        TabOrder = 2
        OnClick = btnPopulateClick
      end
    end
  end
  object pnlTop: TPanel
    Left = 0
    Top = 0
    Width = 909
    Height = 109
    Align = alTop
    BevelOuter = bvNone
    TabOrder = 3
    object lblFreeObjects_: TLabel
      Left = 156
      Top = 36
      Width = 61
      Height = 13
      Caption = 'Free objects:'
    end
    object lblFreeObjects: TLabel
      Left = 228
      Top = 36
      Width = 42
      Height = 13
      Caption = 'FreeObjs'
    end
    object lblPagesAllocated_: TLabel
      Left = 16
      Top = 36
      Width = 79
      Height = 13
      Caption = 'Pages allocated:'
    end
    object lblPagesAllocated: TLabel
      Left = 104
      Top = 36
      Width = 14
      Height = 13
      Caption = 'PA'
    end
    object lblObjectsInUse_: TLabel
      Left = 147
      Top = 52
      Width = 70
      Height = 13
      Caption = 'Objects in use:'
    end
    object lblObjectsInUse: TLabel
      Left = 228
      Top = 52
      Width = 19
      Height = 13
      Caption = 'OIU'
    end
    object lblManagerActive: TLabel
      Left = 16
      Top = 12
      Width = 137
      Height = 13
      Caption = 'Object Allocator is INACTIVE'
    end
    object lblObjectsAllocated_: TLabel
      Left = 10
      Top = 68
      Width = 85
      Height = 13
      Caption = 'Objects allocated:'
    end
    object lblObjectsAllocated: TLabel
      Left = 104
      Top = 68
      Width = 15
      Height = 13
      Caption = 'OA'
    end
    object lblObjectsPerPage_: TLabel
      Left = 11
      Top = 52
      Width = 84
      Height = 13
      Caption = 'Objects per page:'
    end
    object lblObjectsPerPage: TLabel
      Left = 104
      Top = 52
      Width = 22
      Height = 13
      Caption = 'OPP'
    end
    object lblMaxPages_: TLabel
      Left = 357
      Top = 12
      Width = 55
      Height = 13
      Caption = 'Max pages:'
    end
    object lblObjectSize_: TLabel
      Left = 28
      Top = 84
      Width = 67
      Height = 13
      Caption = 'sizeof(Object):'
    end
    object lblSizeOfObject: TLabel
      Left = 104
      Top = 84
      Width = 23
      Height = 13
      Caption = 'SOO'
    end
    object lblObjectsPerPage2_: TLabel
      Left = 196
      Top = 12
      Width = 84
      Height = 13
      Caption = 'Objects per page:'
    end
    object lblPaddingSize_: TLabel
      Left = 349
      Top = 36
      Width = 63
      Height = 13
      Caption = 'Padding size:'
    end
    object lblPageSize_: TLabel
      Left = 168
      Top = 68
      Width = 49
      Height = 13
      Caption = 'Page size:'
    end
    object lblPageSize: TLabel
      Left = 228
      Top = 68
      Width = 14
      Height = 13
      Caption = 'PS'
    end
    object Label16: TLabel
      Left = 363
      Top = 84
      Width = 49
      Height = 13
      Caption = 'Alignment:'
    end
    object Label1: TLabel
      Left = 353
      Top = 60
      Width = 59
      Height = 13
      Caption = 'Header size:'
    end
    object edtMaxPages: TEdit
      Left = 417
      Top = 9
      Width = 37
      Height = 21
      TabOrder = 0
      Text = '2'
    end
    object edtObjectsPerPage: TEdit
      Left = 285
      Top = 9
      Width = 37
      Height = 21
      TabOrder = 1
      Text = '4'
    end
    object spnMaxPages: TUpDown
      Left = 454
      Top = 9
      Width = 15
      Height = 21
      Associate = edtMaxPages
      Max = 1000
      Position = 2
      TabOrder = 2
    end
    object spnObjectsPerPage: TUpDown
      Left = 322
      Top = 9
      Width = 15
      Height = 21
      Associate = edtObjectsPerPage
      Position = 4
      TabOrder = 3
    end
    object mmoDisclaimer: TMemo
      Left = 528
      Top = 9
      Width = 357
      Height = 56
      BorderStyle = bsNone
      Color = clBtnFace
      TabOrder = 4
    end
    object chkShowAddresses: TCheckBox
      Left = 487
      Top = 83
      Width = 137
      Height = 17
      Caption = 'Show object addresses'
      Checked = True
      State = cbChecked
      TabOrder = 5
      OnClick = chkShowAddressesClick
    end
    object chkDebugState: TCheckBox
      Left = 652
      Top = 83
      Width = 121
      Height = 17
      Caption = 'Enable debug code'
      Checked = True
      State = cbChecked
      TabOrder = 6
      OnClick = chkDebugStateClick
    end
    object edtPaddingSize: TEdit
      Left = 417
      Top = 33
      Width = 37
      Height = 21
      TabOrder = 7
      Text = '0'
    end
    object spnPaddingSize: TUpDown
      Left = 454
      Top = 33
      Width = 15
      Height = 21
      Associate = edtPaddingSize
      Max = 64
      TabOrder = 8
    end
    object edtAlignment: TEdit
      Left = 417
      Top = 81
      Width = 37
      Height = 21
      TabOrder = 9
      Text = '1'
    end
    object spnAlignment: TUpDown
      Left = 454
      Top = 81
      Width = 15
      Height = 21
      Associate = edtAlignment
      Min = 1
      Max = 128
      Position = 1
      TabOrder = 10
    end
    object edtHeaderSize: TEdit
      Left = 417
      Top = 57
      Width = 37
      Height = 21
      TabOrder = 11
      Text = '0'
    end
    object spnHeaderSize: TUpDown
      Left = 454
      Top = 57
      Width = 15
      Height = 21
      Associate = edtHeaderSize
      Max = 64
      TabOrder = 12
    end
  end
end
