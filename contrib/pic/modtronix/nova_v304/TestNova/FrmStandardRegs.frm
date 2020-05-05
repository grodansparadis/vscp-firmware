VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{88A90C47-ABC4-4FB0-A125-7EA7E2E40181}#1.0#0"; "vscptcpctrl.ocx"
Begin VB.Form frmStandardRegs 
   Caption         =   "Nova Standard registers"
   ClientHeight    =   5745
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   10290
   Icon            =   "FrmStandardRegs.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   5745
   ScaleWidth      =   10290
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnReadLevel2 
      Caption         =   "Read Level II"
      Height          =   375
      Left            =   8640
      TabIndex        =   36
      Top             =   1800
      Width           =   1455
   End
   Begin MSComctlLib.ProgressBar ProgressBar1 
      Height          =   375
      Left            =   3000
      TabIndex        =   35
      Top             =   720
      Width           =   5655
      _ExtentX        =   9975
      _ExtentY        =   661
      _Version        =   393216
      Appearance      =   1
   End
   Begin VB.CommandButton btnWrite 
      Caption         =   "Write"
      Height          =   375
      Left            =   8640
      TabIndex        =   34
      Top             =   2400
      Width           =   1455
   End
   Begin VB.CommandButton cmdGetEvent 
      Caption         =   "Get Event"
      Height          =   375
      Left            =   8640
      TabIndex        =   33
      Top             =   3840
      Width           =   1455
   End
   Begin VB.CommandButton btnSendEvent 
      Caption         =   "Send Event"
      Height          =   375
      Left            =   8640
      TabIndex        =   32
      Top             =   4320
      Width           =   1455
   End
   Begin VB.CommandButton btnGetCount 
      Caption         =   "Get Event Count"
      Height          =   375
      Left            =   8640
      TabIndex        =   31
      Top             =   3360
      Width           =   1455
   End
   Begin VSCPTCPCTRLLib.Vscptcpctrl Vscptcpctrl1 
      Left            =   8880
      Top             =   360
      _Version        =   65536
      _ExtentX        =   873
      _ExtentY        =   873
      _StockProps     =   0
   End
   Begin VB.TextBox editGUID 
      Height          =   285
      Left            =   3000
      TabIndex        =   29
      Top             =   360
      Width           =   5655
   End
   Begin VB.TextBox editPageSelect 
      Height          =   285
      Left            =   3000
      TabIndex        =   18
      Text            =   "0"
      Top             =   3000
      Width           =   1215
   End
   Begin VB.TextBox editUserID4 
      Height          =   285
      Left            =   5400
      TabIndex        =   14
      Text            =   "0"
      Top             =   2280
      Width           =   495
   End
   Begin VB.TextBox editUserID3 
      Height          =   285
      Left            =   4800
      TabIndex        =   13
      Text            =   "0"
      Top             =   2280
      Width           =   495
   End
   Begin VB.TextBox editUserID2 
      Height          =   285
      Left            =   4200
      TabIndex        =   12
      Text            =   "0"
      Top             =   2280
      Width           =   495
   End
   Begin VB.TextBox editUserID1 
      Height          =   285
      Left            =   3600
      TabIndex        =   11
      Text            =   "0"
      Top             =   2280
      Width           =   495
   End
   Begin VB.CheckBox checkReadWrite 
      Caption         =   "R/W"
      Height          =   255
      Left            =   3000
      TabIndex        =   9
      Top             =   1920
      Width           =   1215
   End
   Begin VB.CheckBox checkAlarmLowHigh 
      Caption         =   "A/D   Low / High"
      Enabled         =   0   'False
      Height          =   255
      Left            =   3960
      TabIndex        =   5
      Top             =   1200
      Width           =   1575
   End
   Begin VB.CheckBox checkAlarm 
      Caption         =   "Alarm"
      Enabled         =   0   'False
      Height          =   255
      Left            =   3000
      TabIndex        =   4
      Top             =   1200
      Width           =   735
   End
   Begin VB.TextBox editUserID0 
      Height          =   285
      Left            =   3000
      TabIndex        =   3
      Text            =   "0"
      Top             =   2280
      Width           =   495
   End
   Begin VB.CommandButton btnUpdate 
      Caption         =   "Read Level I"
      Height          =   375
      Left            =   8640
      TabIndex        =   2
      Top             =   1320
      Width           =   1455
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "E&xit"
      Height          =   495
      Left            =   8640
      TabIndex        =   1
      Top             =   5160
      Width           =   1455
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
      Left            =   3000
      TabIndex        =   30
      Top             =   120
      Width           =   5655
   End
   Begin VB.Label lblMdfUrl 
      Caption         =   "---"
      Height          =   615
      Left            =   3000
      TabIndex        =   28
      Top             =   5040
      Width           =   4935
   End
   Begin VB.Label Label11 
      Alignment       =   1  'Right Justify
      Caption         =   "Module Description File URL :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   120
      TabIndex        =   27
      Top             =   5040
      Width           =   2775
   End
   Begin VB.Label lblGUID 
      Caption         =   "---"
      Height          =   495
      Left            =   3000
      TabIndex        =   26
      Top             =   4440
      Width           =   5415
   End
   Begin VB.Label Label10 
      Alignment       =   1  'Right Justify
      Caption         =   "GUID :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   25
      Top             =   4440
      Width           =   2055
   End
   Begin VB.Label lblBufferSize 
      Caption         =   "---"
      Height          =   255
      Left            =   3000
      TabIndex        =   24
      Top             =   4080
      Width           =   2175
   End
   Begin VB.Label Label9 
      Alignment       =   1  'Right Justify
      Caption         =   "Buffer Size :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   23
      Top             =   4080
      Width           =   2055
   End
   Begin VB.Label lblBootLoaderAlgorithm 
      Caption         =   "---"
      Height          =   255
      Left            =   3000
      TabIndex        =   22
      Top             =   3720
      Width           =   2175
   End
   Begin VB.Label Label8 
      Alignment       =   1  'Right Justify
      Caption         =   "Boot Loader Algorithm :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   21
      Top             =   3720
      Width           =   2055
   End
   Begin VB.Label lblFirmwareVersion 
      Caption         =   "---"
      Height          =   255
      Left            =   3000
      TabIndex        =   20
      Top             =   3360
      Width           =   2175
   End
   Begin VB.Label Label7 
      Alignment       =   1  'Right Justify
      Caption         =   "Firmware Version :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   19
      Top             =   3360
      Width           =   2055
   End
   Begin VB.Label Label6 
      Alignment       =   1  'Right Justify
      Caption         =   "Page Select :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   17
      Top             =   3000
      Width           =   2055
   End
   Begin VB.Label lblManufacturerID 
      Caption         =   "---"
      Height          =   255
      Left            =   3000
      TabIndex        =   16
      Top             =   2640
      Width           =   4095
   End
   Begin VB.Label Label5 
      Alignment       =   1  'Right Justify
      Caption         =   "Manufacturer ID :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   15
      Top             =   2640
      Width           =   2055
   End
   Begin VB.Label Label4 
      Alignment       =   1  'Right Justify
      Caption         =   "Module user ID :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   10
      Top             =   2280
      Width           =   2055
   End
   Begin VB.Label lblVersion 
      Caption         =   "---"
      Height          =   255
      Left            =   3000
      TabIndex        =   8
      Top             =   1560
      Width           =   2175
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Caption         =   "Node Control Flags :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   7
      Top             =   1920
      Width           =   2055
   End
   Begin VB.Label Label2 
      Alignment       =   1  'Right Justify
      Caption         =   "VSCP Version support :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   6
      Top             =   1560
      Width           =   2055
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "Alarm Status :"
      BeginProperty Font 
         Name            =   "MS Sans Serif"
         Size            =   8.25
         Charset         =   0
         Weight          =   700
         Underline       =   0   'False
         Italic          =   0   'False
         Strikethrough   =   0   'False
      EndProperty
      Height          =   255
      Left            =   840
      TabIndex        =   0
      Top             =   1200
      Width           =   2055
   End
End
Attribute VB_Name = "frmStandardRegs"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False

Option Explicit


Private Sub btnGetCount_Click()
    Dim cnt As Integer
    
    cnt = Vscptcpctrl1.getEventCount
    editPageSelect = str(cnt)
    ReadRegisterMany Me, &HFF, &HFF, &HFF, &HE0, 12
End Sub

Private Sub btnOK_Click()
    Unload Me
End Sub



Private Sub btnSendEvent_Click()
    Vscptcpctrl1.head = 0
    Vscptcpctrl1.vscpclass = 10
    Vscptcpctrl1.vscptype = 33
    
    Vscptcpctrl1.GUID(0) = 0
    Vscptcpctrl1.GUID(1) = 1
    Vscptcpctrl1.GUID(2) = 2
    Vscptcpctrl1.GUID(3) = 3
    Vscptcpctrl1.GUID(4) = 4
    Vscptcpctrl1.GUID(5) = 5
    Vscptcpctrl1.GUID(6) = 6
    Vscptcpctrl1.GUID(7) = 7
    Vscptcpctrl1.GUID(8) = 8
    Vscptcpctrl1.GUID(9) = 9
    Vscptcpctrl1.GUID(10) = 10
    Vscptcpctrl1.GUID(11) = 11
    Vscptcpctrl1.GUID(12) = 12
    Vscptcpctrl1.GUID(13) = 13
    Vscptcpctrl1.GUID(14) = 14
    Vscptcpctrl1.GUID(15) = 15
    
    Vscptcpctrl1.cntData = 5
    Vscptcpctrl1.data(0) = 0
    Vscptcpctrl1.data(1) = 1
    Vscptcpctrl1.data(2) = 2
    Vscptcpctrl1.data(3) = 3
    Vscptcpctrl1.data(4) = 4
    If (Vscptcpctrl1.sendEvent()) Then
    End If
End Sub

Private Sub btnUpdate_Click()
    Dim i As Integer
    Dim content As Byte
    Dim readarray(32) As Byte
    Dim vscpmajor As Byte
    Dim vscpminor As Byte
    Dim pageselect As Long
    
    ' Change mouse pointer to hourglass.
    Screen.MousePointer = vbHourglass
    
    ProgressBar1.value = 100
    
    ' Read alarm register
    If (Not ReadRegister(Me, 128, content)) Then
        MsgBox "Failed to read register"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        If (content And 128) Then
            checkAlarm.value = 1
        Else
            checkAlarm.value = 0
        End If
        If (content And 1) Then
            checkAlarmLowHigh.value = 1
        Else
            checkAlarmLowHigh.value = 0
        End If
    End If
    
    ProgressBar1.value = 95
    
    ' Read VSCP Major version
    If (Not ReadRegister(Me, 129, content)) Then
        MsgBox "Failed to read VSCP Major version register"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        vscpmajor = content
    End If
    
    ProgressBar1.value = 90
    
    ' Read VSCP Minor version
    If (Not ReadRegister(Me, 130, content)) Then
        MsgBox "Failed to read VSCP Minor version register"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        vscpminor = content
    End If
    
    lblVersion.Caption = str(vscpmajor) + "." + str(vscpminor)
    
    ProgressBar1.value = 85
    
    ' Read VSCP Node Control flags
    If (Not ReadRegister(Me, 131, content)) Then
        MsgBox "Failed to read VSCP Node Control flags"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        If (content And 32) Then
            checkReadWrite.value = 1
        Else
            checkReadWrite.value = 0
        End If
    End If
    
    ProgressBar1.value = 80
    
    ' Read VSCP Node User id 0
    If (Not ReadRegister(Me, 132, content)) Then
        MsgBox "Failed to read VSCP Node User id 1"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID0.Text = str(content)
    End If
    
    ProgressBar1.value = 75
    
    ' Read VSCP Node User id 1
    If (Not ReadRegister(Me, 133, content)) Then
        MsgBox "Failed to read VSCP Node User id 1"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID1.Text = str(content)
    End If
    
    ProgressBar1.value = 70
    
    ' Read VSCP Node User id 2
    If (Not ReadRegister(Me, 134, content)) Then
        MsgBox "Failed to read VSCP Node User id 2"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID2.Text = str(content)
    End If
    
    ProgressBar1.value = 65
    
    ' Read VSCP Node User id 3
    If (Not ReadRegister(Me, 135, content)) Then
        MsgBox "Failed to read VSCP Node User id 3"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID3.Text = str(content)
    End If
    
    ProgressBar1.value = 60
    
    ' Read VSCP Node User id 4
    If (Not ReadRegister(Me, 136, content)) Then
        MsgBox "Failed to read VSCP Node User id 4"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID4.Text = str(content)
    End If
    
    ProgressBar1.value = 55
    
    
    For i = 0 To 7
        ' Read VSCP Node Manufacturer ID's
        If (Not ReadRegister(Me, 137 + i, content)) Then
            MsgBox "Failed to read VSCP Node Manufacturer id"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            readarray(i) = content
        End If
    Next
    
    lblManufacturerID.Caption = Trim(str(readarray(0))) + "." + Trim(str(readarray(1))) + "." + Trim(str(readarray(2))) + "." + Trim(str(readarray(3))) + "    " + Trim(str(readarray(4))) + "." + Trim(str(readarray(5))) + "." + Trim(str(readarray(6))) + "." + Trim(str(readarray(7)))
    
    ProgressBar1.value = 50
    
    ' Read VSCP Node page select MSB
    If (Not ReadRegister(Me, 146, content)) Then
        MsgBox "Failed to read VSCP Node page select MSB"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        readarray(0) = content
    End If
    
    ProgressBar1.value = 48
    
    ' Read VSCP Node page selct LSB
    If (Not ReadRegister(Me, 147, content)) Then
        MsgBox "Failed to read VSCP Node page select LSB"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        readarray(1) = content
    End If
    
    pageselect = Vscptcpctrl1.data(26)
    pageselect = pageselect * 256
    pageselect = pageselect + Vscptcpctrl1.data(27)
    editPageSelect.Text = Trim(str(pageselect))
    
    ProgressBar1.value = 46
    
    For i = 0 To 2
        ' Read VSCP Node Firmware version
        If (Not ReadRegister(Me, 148 + i, content)) Then
            MsgBox "Failed to read VSCP Node Firmware version"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            readarray(i) = content
        End If
    Next
    
    lblFirmwareVersion.Caption = Trim(str(readarray(0))) + "." + Trim(str(readarray(1))) + "." + Trim(str(readarray(2))) + "."
    
    ' Read VSCP Node Boot loader algorithm
    If (Not ReadRegister(Me, 151, content)) Then
        MsgBox "Failed to read VSCP Node Boot loader algorithm"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        lblBootLoaderAlgorithm.Caption = Trim(str(content))
        If 255 = content Then
            lblBootLoaderAlgorithm.Caption = lblBootLoaderAlgorithm.Caption + " = No bootloader support"
        End If
    End If
    
    ProgressBar1.value = 44
    
    ' Read VSCP Node buffer size
    If (Not ReadRegister(Me, 152, content)) Then
        MsgBox "Failed to read VSCP Node Boot buffer size"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        lblBufferSize.Caption = Trim(str(content))
        If 32 = content Then
            lblBufferSize.Caption = lblBufferSize.Caption + "  = Limited Level II device"
        End If
    End If
    
    For i = 0 To 15
        ' Read VSCP Node GUID
        If (Not ReadRegister(Me, 208 + i, content)) Then
            MsgBox "Failed to read VSCP Node GUID"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            readarray(i) = content
            ProgressBar1.value = ProgressBar1.value - 1
        End If
    Next
    
    ProgressBar1.value = 35
    
    lblGUID.Caption = ""
    For i = 0 To 15
        lblGUID.Caption = lblGUID.Caption + Trim(str(readarray(i))) + " "
    Next
    
    lblMdfUrl.Caption = ""
    For i = 0 To 31
        ' Read VSCP Node MDF
        If (Not ReadRegister(Me, 224 + i, content)) Then
            MsgBox "Failed to read VSCP Node MDF"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            ProgressBar1.value = ProgressBar1.value - 1
            If (0 = content) Then
                Exit For
            Else
                lblMdfUrl.Caption = lblMdfUrl.Caption + Chr(content)
            End If
        End If
    Next
    
    ProgressBar1.value = 0
    
    ' Return mouse pointer to normal.
    Screen.MousePointer = vbDefault
    
End Sub

Private Sub btnWrite_Click()
    
    Dim retval As Byte
    Dim retval2 As Byte
    Dim pageselect As Long
    
    ' Change mouse pointer to hourglass.
    Screen.MousePointer = vbHourglass
    
    
    If (Not WriteRegister(Me, 132, Val(editUserID0.Text), retval)) Then
        MsgBox "Failed to write VSCP Node User ID1"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID0.Text = str(retval)
    End If
    
    
    If (Not WriteRegister(Me, 133, Val(editUserID1.Text), retval)) Then
        MsgBox "Failed to write VSCP Node User ID2"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID1.Text = str(retval)
    End If
    
    
    If (Not WriteRegister(Me, 134, Val(editUserID2.Text), retval)) Then
        MsgBox "Failed to write VSCP Node User ID3"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID2.Text = str(retval)
    End If
    
    
    If (Not WriteRegister(Me, 135, Val(editUserID3.Text), retval)) Then
        MsgBox "Failed to write VSCP Node User ID4"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID3.Text = str(retval)
    End If
    
    
    If (Not WriteRegister(Me, 136, Val(editUserID4.Text), retval)) Then
        MsgBox "Failed to write VSCP Node User ID5"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        editUserID4.Text = str(retval)
    End If
    
    ' Page select
    If (Not WriteRegister(Me, 146, (Val((editPageSelect.Text) / 256) And 255), retval)) Then
        MsgBox "Failed to write VSCP Node Page Select MSB"
        Screen.MousePointer = vbDefault
        Exit Sub
    End If
    
    
    If (Not WriteRegister(Me, 147, (Val(editPageSelect.Text) And 255), retval2)) Then
        MsgBox "Failed to write VSCP Node Page Select LSB"
        Screen.MousePointer = vbDefault
        Exit Sub
    End If
    
    pageselect = retval
    pageselect = pageselect * 256
    pageselect = pageselect + retval2
    editPageSelect.Text = Trim(str(pageselect))
    
    ' Return mouse pointer to normal.
    Screen.MousePointer = vbDefault
    
End Sub

Private Sub btnReadLevel2_Click()

    Dim i As Integer
    Dim content As Byte
    Dim readarray(32) As Byte
    Dim vscpmajor As Byte
    Dim vscpminor As Byte
    Dim pageselect As Long
    
    ' Change mouse pointer to hourglass.
    Screen.MousePointer = vbHourglass
    
    ProgressBar1.value = 100
    
    ' Read first register set
    If (Not ReadRegisterMany(Me, 255, 255, 255, 128, 24)) Then
        MsgBox "Failed to read registers"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        
        ' Alarm
        If (Vscptcpctrl1.data(8) And 128) Then
            checkAlarm.value = 1
        Else
            checkAlarm.value = 0
        End If
        If (Vscptcpctrl1.data(8) And 1) Then
            checkAlarmLowHigh.value = 1
        Else
            checkAlarmLowHigh.value = 0
        End If
        
        ' VSCP version
        lblVersion.Caption = str(Vscptcpctrl1.data(9)) + "." + str(Vscptcpctrl1.data(10))
        
        ' Node Control Flags
        If (Vscptcpctrl1.data(11) And 32) Then
            checkReadWrite.value = 1
        Else
            checkReadWrite.value = 0
        End If
        
        ' User ID
        editUserID0.Text = str(Vscptcpctrl1.data(12))
        editUserID1.Text = str(Vscptcpctrl1.data(13))
        editUserID2.Text = str(Vscptcpctrl1.data(14))
        editUserID3.Text = str(Vscptcpctrl1.data(15))
        editUserID4.Text = str(Vscptcpctrl1.data(16))
        
        ' Manufacturer ID
        lblManufacturerID.Caption = Trim(str(Vscptcpctrl1.data(17))) + "." + Trim(str(Vscptcpctrl1.data(18))) + "." + Trim(str(Vscptcpctrl1.data(19))) + "." + Trim(str(Vscptcpctrl1.data(20))) + "    " + Trim(str(Vscptcpctrl1.data(21))) + "." + Trim(str(Vscptcpctrl1.data(22))) + "." + Trim(str(Vscptcpctrl1.data(23))) + "." + Trim(str(Vscptcpctrl1.data(24)))
    
        ' Page select
        pageselect = Vscptcpctrl1.data(26)
        pageselect = pageselect * 256
        pageselect = pageselect + Vscptcpctrl1.data(27)
        editPageSelect.Text = Trim(str(pageselect))
        
        ' Firmware version
        lblFirmwareVersion.Caption = Trim(str(Vscptcpctrl1.data(28))) + "." + Trim(str(Vscptcpctrl1.data(29))) + "." + Trim(str(Vscptcpctrl1.data(30)))
    
        ' Boot loader algorithm
        lblBootLoaderAlgorithm.Caption = Trim(str(Vscptcpctrl1.data(31)))
        If 255 = Vscptcpctrl1.data(31) Then
            lblBootLoaderAlgorithm.Caption = lblBootLoaderAlgorithm.Caption + " = No bootloader support"
        End If
        
    End If
    
    ProgressBar1.value = 70
    
     ' Read VSCP Node buffer size
    If (Not ReadRegister(Me, 152, content)) Then
        MsgBox "Failed to read VSCP Node Boot buffer size"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        lblBufferSize.Caption = Trim(str(content))
        If 32 = content Then
            lblBufferSize.Caption = lblBufferSize.Caption + "  = Limited Level II device"
        End If
    End If
    
    ProgressBar1.value = 50
    
    ' GUID
    If (Not ReadRegisterMany(Me, 255, 255, 255, 208, 16)) Then
        MsgBox "Failed to read registers"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        lblGUID.Caption = ""
        For i = 0 To 15
            lblGUID.Caption = lblGUID.Caption + Trim(str(Vscptcpctrl1.data(8 + i))) + " "
        Next
    End If
    
    ProgressBar1.value = 40
    
    lblMdfUrl.Caption = ""
    
    ' MDF part 1
    If (Not ReadRegisterMany(Me, 255, 255, 255, 224, 16)) Then
        MsgBox "Failed to read registers"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        For i = 0 To 15
            lblMdfUrl.Caption = lblMdfUrl.Caption + Chr(Vscptcpctrl1.data(8 + i))
        Next
    End If
    
    ProgressBar1.value = 20
    
    ' MDF part 2
    If (Not ReadRegisterMany(Me, 255, 255, 255, 240, 16)) Then
        MsgBox "Failed to read registers"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        For i = 0 To 15
             lblMdfUrl.Caption = lblMdfUrl.Caption + Chr(Vscptcpctrl1.data(8 + i))
        Next
    End If

    ProgressBar1.value = 0
    
    ' Return mouse pointer to normal.
    Screen.MousePointer = vbDefault
End Sub


Private Sub cmdGetEvent_Click()
    If (Vscptcpctrl1.getEvent()) Then
    End If
End Sub

Private Sub Form_Activate()
    btnReadLevel2_Click
End Sub

Private Sub Form_Load()
    On Error Resume Next
    
    Dim buf As String
    Dim bRun As Boolean
    
End Sub



