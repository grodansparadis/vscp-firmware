VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{248DD890-BB45-11CF-9ABC-0080C7E7B78D}#1.0#0"; "MSWINSCK.OCX"
Begin VB.Form frmMain 
   Caption         =   "Simple Nova Test Application -- http://www.eurosource.se"
   ClientHeight    =   4965
   ClientLeft      =   165
   ClientTop       =   735
   ClientWidth     =   9765
   Icon            =   "FrmMain.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   4965
   ScaleWidth      =   9765
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnExit 
      Caption         =   "Exit"
      Height          =   375
      Left            =   8280
      TabIndex        =   20
      Top             =   2640
      Width           =   1335
   End
   Begin VB.CommandButton btnMDF 
      Caption         =   "MDF"
      Height          =   375
      Left            =   8280
      TabIndex        =   19
      Top             =   1320
      Width           =   1335
      Visible         =   0   'False
   End
   Begin VB.CommandButton btnDM 
      Caption         =   "Decision Matrix"
      Height          =   375
      Left            =   6840
      TabIndex        =   18
      Top             =   1320
      Width           =   1335
      Visible         =   0   'False
   End
   Begin VB.CommandButton btnControl 
      Caption         =   "Control"
      Height          =   375
      Left            =   8280
      TabIndex        =   17
      Top             =   840
      Width           =   1335
      Visible         =   0   'False
   End
   Begin VB.CommandButton btnStdRegisters 
      Caption         =   "Std. Regs."
      Height          =   375
      Left            =   6840
      TabIndex        =   16
      Top             =   840
      Width           =   1335
      Visible         =   0   'False
   End
   Begin VB.TextBox editGUID 
      Height          =   285
      Left            =   2640
      TabIndex        =   14
      Text            =   "255:255:255:255:255:255:255:254:0:4:163:0:0:0:0:0"
      Top             =   2640
      Width           =   5415
   End
   Begin VB.Timer Timer1 
      Interval        =   200
      Left            =   720
      Top             =   2280
   End
   Begin VB.ListBox ListGUID 
      Height          =   1230
      Left            =   2640
      TabIndex        =   10
      Top             =   3240
      Width           =   5415
   End
   Begin VB.CommandButton btnConnect 
      Caption         =   "Connect"
      Height          =   495
      Left            =   6840
      TabIndex        =   9
      Top             =   0
      Width           =   2775
   End
   Begin VB.TextBox editVSCPPassword 
      Height          =   285
      IMEMode         =   3  'DISABLE
      Left            =   2040
      PasswordChar    =   "*"
      TabIndex        =   8
      Text            =   "secret"
      Top             =   1560
      Width           =   1575
   End
   Begin VB.TextBox editVSCPUser 
      Height          =   285
      Left            =   2040
      TabIndex        =   7
      Text            =   "admin"
      Top             =   1200
      Width           =   1575
   End
   Begin VB.TextBox editVSCPServerPort 
      Height          =   285
      Left            =   2040
      TabIndex        =   4
      Text            =   "9598"
      Top             =   600
      Width           =   1575
   End
   Begin VB.TextBox editVSCPServer 
      Height          =   285
      Left            =   2040
      TabIndex        =   3
      Text            =   "localhost"
      Top             =   240
      Width           =   3255
   End
   Begin MSComctlLib.ProgressBar ProgressBar1 
      Height          =   255
      Left            =   0
      TabIndex        =   0
      Top             =   4680
      Width           =   9735
      _ExtentX        =   17171
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin MSWinsockLib.Winsock tcpsocket1 
      Left            =   240
      Top             =   2280
      _ExtentX        =   741
      _ExtentY        =   741
      _Version        =   393216
      RemoteHost      =   "localhost"
      RemotePort      =   9598
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
      Left            =   2640
      TabIndex        =   15
      Top             =   2400
      Width           =   5415
   End
   Begin VB.Label Label1 
      Alignment       =   2  'Center
      Caption         =   $"FrmMain.frx":0442
      Height          =   1335
      Index           =   4
      Left            =   120
      TabIndex        =   13
      Top             =   3000
      Width           =   2415
   End
   Begin VB.Label lblCntMsg 
      Alignment       =   2  'Center
      BackColor       =   &H00FFC0C0&
      Caption         =   "0"
      Height          =   255
      Left            =   8160
      TabIndex        =   12
      Top             =   4200
      Width           =   1575
   End
   Begin VB.Label Label2 
      Caption         =   "GUID's found from heartbeat events"
      Height          =   255
      Left            =   2640
      TabIndex        =   11
      Top             =   3000
      Width           =   5295
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "Password :"
      Height          =   255
      Index           =   3
      Left            =   960
      TabIndex        =   6
      Top             =   1560
      Width           =   975
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "User :"
      Height          =   255
      Index           =   2
      Left            =   1320
      TabIndex        =   5
      Top             =   1200
      Width           =   615
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "VSCP Server Port :"
      Height          =   255
      Index           =   1
      Left            =   360
      TabIndex        =   2
      Top             =   600
      Width           =   1575
   End
   Begin VB.Label Label1 
      Alignment       =   1  'Right Justify
      Caption         =   "VSCP TCP Server :"
      Height          =   255
      Index           =   0
      Left            =   360
      TabIndex        =   1
      Top             =   240
      Width           =   1575
   End
   Begin VB.Menu mnuFile 
      Caption         =   "File"
      Index           =   10
      Begin VB.Menu mnuExit 
         Caption         =   "Exit"
         Index           =   11
      End
   End
   Begin VB.Menu mnuNova 
      Caption         =   "Nova"
      Index           =   20
      Begin VB.Menu mnuStandardRegs 
         Caption         =   "Standard Registers"
         Enabled         =   0   'False
         Index           =   21
      End
      Begin VB.Menu mnuControl 
         Caption         =   "Control"
         Enabled         =   0   'False
         Index           =   22
      End
      Begin VB.Menu mnuDM 
         Caption         =   "Decision Matrix"
         Enabled         =   0   'False
         Index           =   23
      End
      Begin VB.Menu mnuMDF 
         Caption         =   "MDF"
         Enabled         =   0   'False
         Index           =   24
      End
   End
   Begin VB.Menu mnuHelp 
      Caption         =   "Help"
      Index           =   90
   End
End
Attribute VB_Name = "frmMain"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub mnuEdit_Click(Index As Integer)

End Sub

Private Sub btnControl_Click()
    ' Must be connected
    If connectionState <> CONNECTION_STATE_CONNECTED Then
        MsgBox "Must be connected to perform this operation."
        Exit Sub
    End If
    
    If Len(editGUID) = 0 Then
            MsgBox "No GUID is set for the node you want to examine."
            Exit Sub
    End If
    
    ' Load the form
    Load frmControl1
    
    ' Get GUID if available
    If -1 <> ListGUID.ListIndex Then
        frmControl1.editGUID = ListGUID.List(ListGUID.ListIndex)
    Else
        frmControl1.editGUID = editGUID
    End If
    
    frmControl1.Vscptcpctrl1.VSCPServer = editVSCPServer.Text
    frmControl1.Vscptcpctrl1.VSCPServerPort = editVSCPServerPort.Text
    frmControl1.Vscptcpctrl1.VSCPUsername = editVSCPUser.Text
    frmControl1.Vscptcpctrl1.VSCPPassword = editVSCPPassword.Text
    frmControl1.Vscptcpctrl1.Connect
    
    ' Show it
    frmControl1.Show vbModal, Me
    
End Sub

Private Sub btnDM_Click()
    ' Must be connected
    If connectionState <> CONNECTION_STATE_CONNECTED Then
        MsgBox "Must be connected to perform this operation."
        Exit Sub
    End If
    
    If Len(editGUID) = 0 Then
            MsgBox "No GUID is set for the node you want to examine."
            Exit Sub
    End If
    
    ' Load the form
    Load frmDM
    
    ' Get GUID if available
    If -1 <> ListGUID.ListIndex Then
        frmDM.editGUID = ListGUID.List(ListGUID.ListIndex)
    Else
        frmDM.editGUID = editGUID
    End If
    
    frmDM.Vscptcpctrl1.VSCPServer = editVSCPServer.Text
    frmDM.Vscptcpctrl1.VSCPServerPort = editVSCPServerPort.Text
    frmDM.Vscptcpctrl1.VSCPUsername = editVSCPUser.Text
    frmDM.Vscptcpctrl1.VSCPPassword = editVSCPPassword.Text
    frmDM.Vscptcpctrl1.Connect
    
    ' Show it
    frmDM.Show vbModal, Me
    

End Sub



Private Sub btnMDF_Click()
    ' Must be connected
    If connectionState <> CONNECTION_STATE_CONNECTED Then
        MsgBox "Must be connected to perform this operation."
        Exit Sub
    End If
    
    If Len(editGUID) = 0 Then
            MsgBox "No GUID is set for the node you want to examine."
            Exit Sub
    End If
    
    ' Load the form
    Load frmMDF
       
    ' Show it
    frmMDF.Show vbModal, Me
    
End Sub

Private Sub btnStdRegisters_Click()

    ' Must be connected
    If connectionState <> CONNECTION_STATE_CONNECTED Then
        MsgBox "Must be connected to perform this operation."
        Exit Sub
    End If
    
    If Len(editGUID) = 0 Then
            MsgBox "No GUID is set for the node you want to examine."
            Exit Sub
    End If
    
    ' Load the form
    Load frmStandardRegs
    
    ' Get GUID if available
    If -1 <> ListGUID.ListIndex Then
        frmStandardRegs.editGUID = ListGUID.List(ListGUID.ListIndex)
    Else
        frmStandardRegs.editGUID = editGUID
    End If
    
    frmStandardRegs.Vscptcpctrl1.VSCPServer = editVSCPServer.Text
    frmStandardRegs.Vscptcpctrl1.VSCPServerPort = editVSCPServerPort.Text
    frmStandardRegs.Vscptcpctrl1.VSCPUsername = editVSCPUser.Text
    frmStandardRegs.Vscptcpctrl1.VSCPPassword = editVSCPPassword.Text
    frmStandardRegs.Vscptcpctrl1.Connect
    
    ' Show it
    frmStandardRegs.Show vbModal, Me
End Sub

Private Sub btnExit_Click()
    If connectionState = CONNECTION_STATE_CONNECTED Then
        btnConnect.value = False
    End If
    Unload Me
End Sub

Private Sub ListGUID_Click()
    If ListGUID.ListIndex <> -1 Then
        editGUID.Text = ListGUID.List(ListGUID.ListIndex)
    End If
End Sub

Private Sub mnuControl_Click(Index As Integer)
    btnControl.value = True
End Sub

Private Sub mnuDM_Click(Index As Integer)
    btnDM.value = True
End Sub

Private Sub mnuExit_Click(Index As Integer)
    End
End Sub

Private Sub Winsock1_Error(ByVal Number As Integer, Description As String, ByVal Scode As Long, ByVal Source As String, ByVal HelpFile As String, ByVal HelpContext As Long, CancelDisplay As Boolean)
    bDataAvailable = True   ' Signal that data is available
End Sub

Private Sub btnConnect_Click()
    Dim wrkstr As String
    Dim buf As String
  
    On Error Resume Next
    
    ProgressBar1.value = 0
  
    If connectionState = CONNECTION_STATE_CONNECTED Then
    
        ProgressBar1.value = 0
        
        ' Close Channel to daemon
        wrkstr = "QUIT "
        wrkstr = wrkstr + Chr(13) + Chr(10)
        tcpsocket1.SendData wrkstr
        connectionState = CONNECTION_STATE_QUIT
        ProgressBar1.value = 20
        
        btnStdRegisters.Visible = False
        mnuStandardRegs.Item(21).Enabled = False
        btnControl.Visible = False
        mnuControl.Item(22).Enabled = False
        btnDM.Visible = False
        mnuDM.Item(23).Enabled = False
        btnMDF.Visible = False
        mnuMDF.Item(24).Enabled = False
        
    Else
        ' Make sure connection is closed
        tcpsocket1.Close
        
        ' Open Channel to daemon
        ProgressBar1.value = 50
        connectionState = CONNECTION_STATE_WAIT_CONNECT
        tcpsocket1.RemoteHost = editVSCPServer.Text
        tcpsocket1.RemotePort = Val(editVSCPServerPort.Text)
        tcpsocket1.Connect
    
  End If
  
End Sub

Private Sub mnuMDF_Click(Index As Integer)
    btnMDF.value = True
End Sub

Private Sub mnuStandardRegs_Click(Index As Integer)
    
    btnStdRegisters.value = True
    
    
End Sub

Private Sub tcpsocket1_DataArrival(ByVal bytesTotal As Long)
    
    Dim wrkstr As String
    Dim buf As String

    bDataAvailable = True
    tcpsocket1.GetData buf, vbString
    
    
    Select Case connectionState
    
        Case CONNECTION_STATE_NONE
            ' We do nothing here
        
        Case CONNECTION_STATE_WAIT_CONNECT
            If 0 <> InStr(buf, "+OK") Then
                connectionState = CONNECTION_STATE_USERNAME
                wrkstr = "USER "
                wrkstr = wrkstr + editVSCPUser.Text + Chr(13) + Chr(10)
                tcpsocket1.SendData wrkstr
                ProgressBar1.value = 50
            Else
                connectionState = CONNECTION_STATE_NONE
                tcpsocket1.Close
                MsgBox "Failed to connect to server."
            End If
        
        Case CONNECTION_STATE_USERNAME
            If 0 <> InStr(buf, "+OK") Then
                connectionState = CONNECTION_STATE_PASSWORD
                wrkstr = "PASS "
                wrkstr = wrkstr + editVSCPPassword.Text + Chr(13) + Chr(10)
                tcpsocket1.SendData wrkstr
                ProgressBar1.value = 70
            Else
                connectionState = CONNECTION_STATE_NONE
                tcpsocket1.Close
                MsgBox "Failed to login to server (username)."
            End If
        
        Case CONNECTION_STATE_PASSWORD
            If 0 <> InStr(buf, "+OK") Then
                connectionState = CONNECTION_STATE_CONNECTED
                bConnected = True
                ProgressBar1.value = 100
                btnConnect.Caption = "Disconnect"
            Else
                connectionState = CONNECTION_STATE_NONE
                tcpsocket1.Close
                MsgBox "Failed to login to server (password)."
            End If
        
        Case CONNECTION_STATE_CONNECTED
        
            btnStdRegisters.Visible = True
            mnuStandardRegs.Item(21).Enabled = True
            btnControl.Visible = True
            mnuControl.Item(22).Enabled = True
            btnDM.Visible = True
            mnuDM.Item(23).Enabled = True
            btnMDF.Visible = True
            mnuMDF.Item(24).Enabled = True
            
            Select Case connectionSubState
            
                Case CONNECTION_SUB_STATE_NONE
                    ' Do nothing
            
                Case CONNECTION_SUB_STATE_COMMAND_CDTA
                    If 0 <> InStr(buf, "+OK") Then
                    
                    End If
                    connectionSubState = CONNECTION_SUB_STATE_NONE
                
                Case CONNECTION_SUB_STATE_COMMAND_RETR
                    If 0 <> InStr(buf, "+OK") Then
                        lblCntMsg = str(Val(lblCntMsg) + 1)
                        receiveEvent buf
                    End If
                    connectionSubState = CONNECTION_SUB_STATE_NONE
            
                Case Else
                    connectionSubState = CONNECTION_SUB_STATE_NONE
                    
            End Select
    
        Case CONNECTION_STATE_QUIT
            tcpsocket1.Close
            ProgressBar1.value = 0
            btnConnect.Caption = "Connect"
            
        Case Else
            connectionState = CONNECTION_STATE_NONE
    
    End Select
 

End Sub

 

Private Sub Timer1_Timer()
    Dim wrkstr As String
    
    On Error Resume Next
    systemtick = systemtick + 1
    
    If (connectionState = CONNECTION_STATE_CONNECTED) And (connectionSubState = CONNECTION_SUB_STATE_NONE) Then
        wrkstr = "RETR"
        wrkstr = wrkstr + editVSCPUser.Text + Chr(13) + Chr(10)
        tcpsocket1.SendData wrkstr
        connectionSubState = CONNECTION_SUB_STATE_COMMAND_RETR
    End If
    
End Sub
