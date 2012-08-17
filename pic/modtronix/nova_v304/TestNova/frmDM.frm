VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{88A90C47-ABC4-4FB0-A125-7EA7E2E40181}#1.0#0"; "vscptcpctrl.ocx"
Begin VB.Form frmDM 
   Caption         =   "Simple Nova Test Application -- http://www.eurosource.se"
   ClientHeight    =   4050
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   10305
   Icon            =   "frmDM.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   4050
   ScaleWidth      =   10305
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ProgressBar ProgressBar1 
      Height          =   255
      Left            =   120
      TabIndex        =   33
      Top             =   3720
      Width           =   5655
      _ExtentX        =   9975
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin VSCPTCPCTRLLib.Vscptcpctrl Vscptcpctrl1 
      Left            =   9360
      Top             =   3120
      _Version        =   65536
      _ExtentX        =   1296
      _ExtentY        =   873
      _StockProps     =   0
   End
   Begin VB.TextBox editGUID 
      Height          =   285
      Left            =   120
      TabIndex        =   23
      Top             =   3360
      Width           =   5655
   End
   Begin VB.CommandButton btnClear 
      Caption         =   "Clear"
      Height          =   375
      Left            =   8520
      TabIndex        =   20
      Top             =   1560
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "Decision Matrix Row"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   2895
      Left            =   120
      TabIndex        =   2
      Top             =   120
      Width           =   9975
      Begin VB.TextBox editFilter 
         Height          =   375
         Index           =   3
         Left            =   3720
         TabIndex        =   32
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editFilter 
         Height          =   375
         Index           =   2
         Left            =   3240
         TabIndex        =   31
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editFilter 
         Height          =   375
         Index           =   1
         Left            =   2760
         TabIndex        =   30
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editFilter 
         Height          =   375
         Index           =   0
         Left            =   2280
         TabIndex        =   29
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editMask 
         Height          =   375
         Index           =   3
         Left            =   1560
         TabIndex        =   28
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editMask 
         Height          =   375
         Index           =   2
         Left            =   1080
         TabIndex        =   27
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editMask 
         Height          =   375
         Index           =   1
         Left            =   600
         TabIndex        =   26
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editAction 
         Height          =   375
         Left            =   4440
         TabIndex        =   25
         Text            =   "0"
         Top             =   840
         Width           =   735
      End
      Begin VB.CommandButton btnRead 
         Caption         =   "Read"
         Height          =   375
         Left            =   7080
         TabIndex        =   22
         Top             =   2160
         Width           =   1215
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   375
         Left            =   8400
         TabIndex        =   21
         Top             =   2160
         Width           =   1215
      End
      Begin VB.CommandButton btnLast 
         Caption         =   ">>"
         Height          =   255
         Left            =   4440
         TabIndex        =   19
         Top             =   2280
         Width           =   495
      End
      Begin VB.CommandButton btnFirst 
         Caption         =   "<<"
         Height          =   255
         Left            =   2640
         TabIndex        =   18
         Top             =   2280
         Width           =   495
      End
      Begin VB.CommandButton btnPrev 
         Caption         =   "<"
         Height          =   255
         Left            =   3240
         TabIndex        =   17
         Top             =   2280
         Width           =   495
      End
      Begin VB.TextBox editMask 
         Height          =   375
         Index           =   0
         Left            =   120
         TabIndex        =   10
         Text            =   "0"
         Top             =   840
         Width           =   375
      End
      Begin VB.TextBox editActionParam 
         Height          =   375
         Left            =   5280
         TabIndex        =   9
         Top             =   840
         Width           =   4455
      End
      Begin VB.CheckBox CheckEnable 
         Caption         =   "Enabled"
         ForeColor       =   &H00C00000&
         Height          =   255
         Left            =   240
         TabIndex        =   8
         Top             =   1560
         Width           =   975
      End
      Begin VB.CheckBox CheckGUID 
         Caption         =   "Check GUID"
         ForeColor       =   &H00C00000&
         Height          =   255
         Left            =   1320
         TabIndex        =   7
         Top             =   1560
         Width           =   1215
      End
      Begin VB.CheckBox CheckEOM 
         Caption         =   "End Of Matrix"
         ForeColor       =   &H00C00000&
         Height          =   255
         Left            =   2640
         TabIndex        =   6
         Top             =   1560
         Width           =   1455
      End
      Begin VB.CheckBox CheckMatchZone 
         Caption         =   "Match Zone"
         ForeColor       =   &H00C00000&
         Height          =   255
         Left            =   4200
         TabIndex        =   5
         Top             =   1560
         Width           =   1215
      End
      Begin VB.CheckBox CheckMatchSubZone 
         Caption         =   "Match subzone"
         ForeColor       =   &H00C00000&
         Height          =   255
         Left            =   5520
         TabIndex        =   4
         Top             =   1560
         Width           =   1455
      End
      Begin VB.CommandButton btnNext 
         Caption         =   ">"
         Height          =   255
         Left            =   3840
         TabIndex        =   3
         Top             =   2280
         Width           =   495
      End
      Begin VB.Label Label1 
         Alignment       =   2  'Center
         Caption         =   "Mask"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   120
         TabIndex        =   16
         Top             =   600
         Width           =   1815
      End
      Begin VB.Label Label2 
         Alignment       =   2  'Center
         Caption         =   "Filter"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   2280
         TabIndex        =   15
         Top             =   600
         Width           =   1815
      End
      Begin VB.Label Label3 
         Alignment       =   2  'Center
         Caption         =   "Action"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   4440
         TabIndex        =   14
         Top             =   600
         Width           =   735
      End
      Begin VB.Label Label4 
         Caption         =   "Action Parameters"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   5280
         TabIndex        =   13
         Top             =   600
         Width           =   3975
      End
      Begin VB.Label Label5 
         Alignment       =   1  'Right Justify
         Caption         =   "Row :"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         ForeColor       =   &H00000000&
         Height          =   255
         Left            =   240
         TabIndex        =   12
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label lblRow 
         Alignment       =   2  'Center
         BackColor       =   &H0080FFFF&
         Caption         =   "1"
         Height          =   255
         Left            =   1320
         TabIndex        =   11
         Top             =   2280
         Width           =   1215
      End
   End
   Begin VB.CommandButton btnPrevious 
      Caption         =   "<"
      Height          =   255
      Left            =   3360
      TabIndex        =   1
      Top             =   2400
      Width           =   495
   End
   Begin VB.CommandButton btnExit 
      Caption         =   "Exit"
      Height          =   495
      Left            =   7200
      TabIndex        =   0
      Top             =   3120
      Width           =   1215
   End
   Begin VB.Label Label12 
      Caption         =   "GUID for node "
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   9.75
         Charset         =   0
         Weight          =   400
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   24
      Top             =   3120
      Width           =   5655
   End
End
Attribute VB_Name = "frmDM"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub btnExit_Click()
    Unload Me
End Sub

Private Sub btnFirst_Click()
    lblRow.Caption = str(1)
End Sub

Private Sub btnLast_Click()
    lblRow.Caption = str(16)
End Sub

Private Sub btnNext_Click()
    If Val(lblRow) < 16 Then
        lblRow.Caption = str(Val(lblRow) + 1)
    Else
        lblRow.Caption = str(1)
    End If
End Sub

Private Sub btnPrev_Click()
    If Val(lblRow) > 1 Then
        lblRow.Caption = str(Val(lblRow) - 1)
    Else
        lblRow.Caption = str(16)
    End If
End Sub

Private Sub btnRead_Click()
    Screen.MousePointer = vbHourglass
    readDMRow (Val(lblRow))
    Screen.MousePointer = vbDefault
End Sub

'
' Read a DM row and fill in form information
'
Sub readDMRow(nRow As Byte)
    
    Dim i As Integer
    Dim dm(32) As Byte
    Dim addr As Long
    Dim wrk1 As Long
    
    addr = 256 + (32 * (nRow - 1))
    
    ' Read first half
    If (Not ReadRegisterMany(Me, 0, 0, Int(addr / 256), addr And 255, 16)) Then
        MsgBox "Failed to read DM registers"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        For i = 0 To 15
            dm(i) = Vscptcpctrl1.data(8 + i)
        Next i
    End If
    
    
    ' Read second half
    If (Not ReadRegisterMany(Me, 0, 0, Int(addr / 256), (addr And 255) + 16, 16)) Then
        MsgBox "Failed to read DM registers"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        For i = 0 To 15
            dm(16 + i) = Vscptcpctrl1.data(8 + i)
        Next i
    End If
    
    ' Mask
    For i = 0 To 3
        editMask(i).Text = Trim(str(dm(i)))
    Next
    
    ' Filter
    For i = 0 To 3
        editFilter(i).Text = Trim(str(dm(4 + i)))
    Next
    
    ' Switch - Enable
    If dm(8) And 128 Then
        CheckEnable.value = 1
    Else
        CheckEnable.value = 0
    End If
    
    ' Switch - Check GUID
    If dm(8) And 64 Then
        CheckGUID.value = 1
    Else
        CheckGUID.value = 0
    End If
    
    ' Switch - End of Matrix
    If dm(8) And 32 Then
        CheckEOM.value = 1
    Else
        CheckEOM.value = 0
    End If
    
    ' Switch - Match zone
    If dm(11) And 16 Then
        CheckMatchZone.value = 1
    Else
        CheckMatchZone.value = 0
    End If
    
    ' Switch - Match subzone
    If dm(11) And 8 Then
        CheckMatchSubZone.value = 1
    Else
        CheckMatchSubZone.value = 0
    End If
    
    ' Action
   wrk1 = dm(12)
   wrk1 = 256 * wrk1
   wrk1 = wrk1 + dm(13)
   editAction.Text = Trim(str(wrk1))
   
   ' Action Parameter
   editActionParam = ""
   For i = 0 To 17
        editActionParam.Text = editActionParam.Text + Trim(str(dm(14 + i))) + ","
    Next
End Sub

Private Sub btnWrite_Click()

    Dim i As Integer
    Dim dm(32) As Byte
    Dim retval As Byte
    Dim addr As Long
    Dim wrk1 As Long
    Dim str As String
    Dim pos As Integer
    
    'on error resume next
    
    Screen.MousePointer = vbHourglass
    ProgressBar1.value = 100
    
    ' null the matrix row
    For i = 0 To 31
        dm(i) = 0
    Next
    
    ' mask
    For i = 0 To 3
        dm(i) = Val(editMask(i).Text)
    Next i
    
    ' filter
    For i = 0 To 3
        dm(4 + i) = Val(editMask(i).Text)
    Next i
       
    ' Switch - Enable
    If CheckEnable.value = 1 Then
        dm(8) = dm(8) + 128
    End If
    
    ' Switch - Check GUID
    If CheckGUID.value = 1 Then
        dm(8) = dm(8) + 64
    End If
    
    ' Switch - End of Matrix
    If CheckEOM.value = 1 Then
        dm(8) = dm(8) + 32
    End If
       
    ' Switch - Match zone
    If CheckMatchZone.value = 1 Then
        dm(11) = dm(11) + 16
    End If
    
    ' Switch - Match subzone
    If CheckMatchSubZone.value = 1 Then
        dm(11) = dm(11) + 8
    End If
    
    
    ' Action
   dm(12) = Int(Val(editAction.Text) / 256)
   dm(13) = Int(Val(editAction.Text) And 255)
    
    ' Action Parameters
    i = 0
    str = editActionParam.Text
    pos = InStr(1, str, ",")
    While Not 0 = pos
        dm(14 + i) = Val(str)
        str = Right(str, Len(str) - pos)
        pos = InStr(1, str, ",")
        i = i + 1
        If i > 17 Then pos = 0
    Wend
    
    addr = 256 + (32 * (Val(lblRow) - 1))
    
    For i = 0 To 32
        If (Not WriteRegister2(Me, addr + i, dm(i), retval)) Then
            MsgBox "Failed to write value"
            Exit For
        End If
        ProgressBar1.value = ProgressBar1.value - 3
    Next
    
    btnRead_Click
    
    ProgressBar1.value = 0
    Screen.MousePointer = vbDefault
    
End Sub

Private Sub Form_Activate()
    btnRead_Click
End Sub

 

Private Sub lblRow_Change()
    btnRead_Click
End Sub

