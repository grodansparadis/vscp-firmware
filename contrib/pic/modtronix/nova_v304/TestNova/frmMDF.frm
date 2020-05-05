VERSION 5.00
Object = "{EAB22AC0-30C1-11CF-A7EB-0000C05BAE0B}#1.1#0"; "shdocvw.dll"
Begin VB.Form frmMDF 
   Caption         =   "Simple Nova Test Application -- http://www.eurosource.se"
   ClientHeight    =   7485
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   10650
   Icon            =   "frmMDF.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   7485
   ScaleWidth      =   10650
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton Command1 
      Caption         =   "Exit"
      Height          =   495
      Left            =   4680
      TabIndex        =   1
      Top             =   6840
      Width           =   1215
   End
   Begin SHDocVwCtl.WebBrowser WebBrowser1 
      Height          =   6135
      Left            =   120
      TabIndex        =   0
      Top             =   480
      Width           =   10455
      ExtentX         =   18441
      ExtentY         =   10821
      ViewMode        =   0
      Offline         =   0
      Silent          =   0
      RegisterAsBrowser=   0
      RegisterAsDropTarget=   1
      AutoArrange     =   0   'False
      NoClientEdge    =   0   'False
      AlignLeft       =   0   'False
      NoWebView       =   0   'False
      HideFileNames   =   0   'False
      SingleClick     =   0   'False
      SingleSelection =   0   'False
      NoFolders       =   0   'False
      Transparent     =   0   'False
      ViewID          =   "{0057D0E0-3573-11CF-AE69-08002B2E1262}"
      Location        =   ""
   End
   Begin VB.Label Label1 
      Alignment       =   2  'Center
      Caption         =   "Module Description File"
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
      TabIndex        =   2
      Top             =   240
      Width           =   10335
   End
End
Attribute VB_Name = "frmMDF"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub Command1_Click()
    Unload Me
End Sub



Private Sub Form_Load()
    WebBrowser1.Navigate "http://www.eurosource.se/sht_001.xml"
End Sub
