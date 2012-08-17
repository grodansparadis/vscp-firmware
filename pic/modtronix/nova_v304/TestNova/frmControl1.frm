VERSION 5.00
Object = "{831FDD16-0C5C-11D2-A9FC-0000F8754DA1}#2.0#0"; "MSCOMCTL.OCX"
Object = "{88A90C47-ABC4-4FB0-A125-7EA7E2E40181}#1.0#0"; "VSCPTC~1.OCX"
Begin VB.Form frmControl1 
   Caption         =   "Simple Nova Test Application -- http://www.eurosource.se"
   ClientHeight    =   9630
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   15240
   Icon            =   "frmControl1.frx":0000
   LinkTopic       =   "Form1"
   ScaleHeight     =   9630
   ScaleWidth      =   15240
   StartUpPosition =   3  'Windows Default
   Begin MSComctlLib.ProgressBar ProgressBar1 
      Height          =   255
      Left            =   240
      TabIndex        =   292
      Top             =   7320
      Width           =   5655
      _ExtentX        =   9975
      _ExtentY        =   450
      _Version        =   393216
      Appearance      =   1
   End
   Begin VB.TextBox editGUID 
      Height          =   285
      Left            =   240
      TabIndex        =   286
      Top             =   6960
      Width           =   5655
   End
   Begin VSCPTCPCTRLLib.Vscptcpctrl Vscptcpctrl1 
      Left            =   14280
      Top             =   6720
      _Version        =   65536
      _ExtentX        =   873
      _ExtentY        =   873
      _StockProps     =   0
   End
   Begin VB.CommandButton btnWrite 
      Caption         =   "&Write"
      Height          =   495
      Left            =   8520
      TabIndex        =   285
      Top             =   6960
      Width           =   1455
   End
   Begin VB.CommandButton btnRead 
      Caption         =   "&Read"
      Height          =   495
      Left            =   6960
      TabIndex        =   284
      Top             =   6960
      Width           =   1455
   End
   Begin VB.Frame framePWM 
      BorderStyle     =   0  'None
      Height          =   5895
      Left            =   10560
      TabIndex        =   281
      Top             =   2280
      Width           =   14295
      Begin VB.TextBox editPWM 
         Height          =   375
         Left            =   2760
         TabIndex        =   282
         Text            =   "0"
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "PWM :"
         Height          =   255
         Index           =   25
         Left            =   1440
         TabIndex        =   283
         Top             =   960
         Width           =   1215
      End
   End
   Begin VB.Frame frameAD 
      BorderStyle     =   0  'None
      Height          =   5895
      Left            =   600
      TabIndex        =   27
      Top             =   5160
      Width           =   14175
      Begin VB.TextBox editMinAlarm 
         Height          =   375
         Index           =   0
         Left            =   10200
         TabIndex        =   243
         Text            =   "0"
         Top             =   600
         Width           =   1215
      End
      Begin VB.TextBox editHysteresis 
         Height          =   375
         Left            =   10200
         TabIndex        =   242
         Text            =   "0"
         Top             =   4560
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         BackColor       =   &H80000000&
         Enabled         =   0   'False
         Height          =   375
         Index           =   7
         Left            =   10200
         TabIndex        =   240
         Text            =   "0"
         Top             =   3960
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         BackColor       =   &H80000000&
         Enabled         =   0   'False
         Height          =   375
         Index           =   7
         Left            =   11760
         TabIndex        =   238
         Text            =   "0"
         Top             =   3960
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         BackColor       =   &H80000000&
         Enabled         =   0   'False
         Height          =   375
         Index           =   6
         Left            =   10200
         TabIndex        =   237
         Text            =   "0"
         Top             =   3480
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         BackColor       =   &H80000000&
         Enabled         =   0   'False
         Height          =   375
         Index           =   6
         Left            =   11760
         TabIndex        =   235
         Text            =   "0"
         Top             =   3480
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         Height          =   375
         Index           =   5
         Left            =   10200
         TabIndex        =   234
         Text            =   "0"
         Top             =   3000
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         Height          =   375
         Index           =   5
         Left            =   11760
         TabIndex        =   232
         Text            =   "0"
         Top             =   3000
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         Height          =   375
         Index           =   4
         Left            =   10200
         TabIndex        =   231
         Text            =   "0"
         Top             =   2520
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         Height          =   375
         Index           =   4
         Left            =   11760
         TabIndex        =   229
         Text            =   "0"
         Top             =   2520
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         Height          =   375
         Index           =   3
         Left            =   10200
         TabIndex        =   228
         Text            =   "0"
         Top             =   2040
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         Height          =   375
         Index           =   3
         Left            =   11760
         TabIndex        =   226
         Text            =   "0"
         Top             =   2040
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         Height          =   375
         Index           =   2
         Left            =   10200
         TabIndex        =   225
         Text            =   "0"
         Top             =   1560
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         Height          =   375
         Index           =   2
         Left            =   11760
         TabIndex        =   223
         Text            =   "0"
         Top             =   1560
         Width           =   1215
      End
      Begin VB.TextBox editMinAlarm 
         Height          =   375
         Index           =   1
         Left            =   10200
         TabIndex        =   222
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         Height          =   375
         Index           =   1
         Left            =   11760
         TabIndex        =   220
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.TextBox editMaxAlarm 
         Height          =   375
         Index           =   0
         Left            =   11760
         TabIndex        =   216
         Text            =   "0"
         Top             =   600
         Width           =   1215
      End
      Begin VB.Frame Frame1 
         Caption         =   "Periodic events"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Left            =   240
         TabIndex        =   61
         Top             =   2280
         Width           =   6975
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "ch 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   69
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "ch 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   68
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "ch 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   67
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "Ch 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   66
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "Ch 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   65
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "Ch 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   64
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "Ch 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   63
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckPeriodicEvents 
            Caption         =   "Ch 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   62
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame Frame3 
         Caption         =   "Low Alarm"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Left            =   240
         TabIndex        =   52
         Top             =   4200
         Width           =   6975
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "ch 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   60
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "ch 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   59
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "ch 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   58
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "Ch 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   57
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "Ch 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   56
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "Ch 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   55
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "Ch 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   54
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckLowAlarm 
            Caption         =   "Ch 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   53
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame Frame2 
         Caption         =   "High Alarm"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Left            =   240
         TabIndex        =   43
         Top             =   3240
         Width           =   6975
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "ch 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   51
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "ch 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   50
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "ch 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   49
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "Ch 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   48
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "Ch 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   47
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "Ch 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   46
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "Ch 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   45
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckHighAlarm 
            Caption         =   "Ch 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   44
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 7  :"
         Enabled         =   0   'False
         Height          =   255
         Index           =   50
         Left            =   3480
         TabIndex        =   321
         Top             =   1440
         Width           =   855
      End
      Begin VB.Label Label17 
         Alignment       =   1  'Right Justify
         Caption         =   "Hysteresis :"
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
         Index           =   30
         Left            =   8880
         TabIndex        =   241
         Top             =   4680
         Width           =   1215
      End
      Begin VB.Label Label17 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 7 :"
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
         Index           =   25
         Left            =   8880
         TabIndex        =   239
         Top             =   4080
         Width           =   1215
      End
      Begin VB.Label Label17 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 6 :"
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
         Index           =   1
         Left            =   8880
         TabIndex        =   236
         Top             =   3600
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 5 :"
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
         Index           =   8
         Left            =   8880
         TabIndex        =   233
         Top             =   3120
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 4 :"
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
         Index           =   7
         Left            =   8880
         TabIndex        =   230
         Top             =   2640
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 3 :"
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
         Index           =   6
         Left            =   8880
         TabIndex        =   227
         Top             =   2160
         Width           =   1215
      End
      Begin VB.Label Label18 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 2 :"
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
         Index           =   0
         Left            =   8880
         TabIndex        =   224
         Top             =   1680
         Width           =   1215
      End
      Begin VB.Label Label17 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 1 :"
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
         Index           =   0
         Left            =   8880
         TabIndex        =   221
         Top             =   1200
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 0 :"
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
         Index           =   5
         Left            =   8880
         TabIndex        =   219
         Top             =   720
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   2  'Center
         Caption         =   "Max alarm"
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
         Index           =   10
         Left            =   11760
         TabIndex        =   218
         Top             =   360
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   2  'Center
         Caption         =   "Min alarm"
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
         Index           =   9
         Left            =   10200
         TabIndex        =   217
         Top             =   360
         Width           =   1215
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 0  :"
         Height          =   255
         Index           =   17
         Left            =   240
         TabIndex        =   42
         Top             =   360
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 1  :"
         Height          =   255
         Index           =   18
         Left            =   240
         TabIndex        =   41
         Top             =   720
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 2  :"
         Height          =   255
         Index           =   19
         Left            =   240
         TabIndex        =   40
         Top             =   1080
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 3  :"
         Height          =   255
         Index           =   20
         Left            =   240
         TabIndex        =   39
         Top             =   1440
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 4  :"
         Height          =   255
         Index           =   21
         Left            =   3480
         TabIndex        =   38
         Top             =   360
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 5  :"
         Height          =   255
         Index           =   22
         Left            =   3480
         TabIndex        =   37
         Top             =   720
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Channel 6  :"
         Enabled         =   0   'False
         Height          =   255
         Index           =   23
         Left            =   3480
         TabIndex        =   36
         Top             =   1080
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
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
         Index           =   0
         Left            =   1200
         TabIndex        =   35
         Top             =   360
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
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
         Index           =   1
         Left            =   1200
         TabIndex        =   34
         Top             =   720
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
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
         Index           =   2
         Left            =   1200
         TabIndex        =   33
         Top             =   1080
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
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
         Index           =   3
         Left            =   1200
         TabIndex        =   32
         Top             =   1440
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
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
         Index           =   4
         Left            =   4440
         TabIndex        =   31
         Top             =   360
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
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
         Index           =   5
         Left            =   4440
         TabIndex        =   30
         Top             =   720
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
         Enabled         =   0   'False
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
         Index           =   6
         Left            =   4440
         TabIndex        =   29
         Top             =   1080
         Width           =   855
      End
      Begin VB.Label lblad 
         Alignment       =   1  'Right Justify
         BackColor       =   &H00FFC0C0&
         Caption         =   "0"
         Enabled         =   0   'False
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
         Index           =   7
         Left            =   4440
         TabIndex        =   28
         Top             =   1440
         Width           =   855
      End
   End
   Begin VB.Frame frameOutput 
      BorderStyle     =   0  'None
      Height          =   5895
      Left            =   360
      TabIndex        =   18
      Top             =   5280
      Width           =   14175
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   0
         Left            =   2040
         TabIndex        =   279
         Text            =   "0"
         Top             =   4800
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   7
         Left            =   12840
         TabIndex        =   278
         Text            =   "0"
         Top             =   5280
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   6
         Left            =   12840
         TabIndex        =   276
         Text            =   "0"
         Top             =   4800
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   5
         Left            =   9240
         TabIndex        =   274
         Text            =   "0"
         Top             =   5280
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   4
         Left            =   9240
         TabIndex        =   272
         Text            =   "0"
         Top             =   4800
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   3
         Left            =   5640
         TabIndex        =   270
         Text            =   "0"
         Top             =   5280
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   2
         Left            =   5640
         TabIndex        =   268
         Text            =   "0"
         Top             =   4800
         Width           =   1215
      End
      Begin VB.TextBox editProtectionTimer 
         Height          =   375
         Index           =   1
         Left            =   2040
         TabIndex        =   266
         Text            =   "0"
         Top             =   5280
         Width           =   1215
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 7"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   18
         Left            =   7200
         TabIndex        =   207
         Top             =   3840
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   215
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   214
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   213
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   212
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   211
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   210
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   209
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg7 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   208
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 6"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   17
         Left            =   7200
         TabIndex        =   198
         Top             =   3000
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   206
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   205
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   204
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   203
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   202
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   201
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   200
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg6 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   199
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 5"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   16
         Left            =   7200
         TabIndex        =   189
         Top             =   2160
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   197
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   196
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   195
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   194
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   193
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   192
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   191
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg5 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   190
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 4"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   15
         Left            =   7200
         TabIndex        =   180
         Top             =   1320
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   188
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   187
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   186
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   185
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   184
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   183
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   182
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg4 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   181
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 3"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   14
         Left            =   120
         TabIndex        =   171
         Top             =   3840
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   179
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   178
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   177
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   176
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   175
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   174
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   173
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg3 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   172
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 2"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   13
         Left            =   120
         TabIndex        =   162
         Top             =   3000
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   170
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   169
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   168
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   167
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   166
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   165
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   164
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg2 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   163
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 1"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   12
         Left            =   120
         TabIndex        =   153
         Top             =   2160
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   161
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   160
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   159
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   158
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   157
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   156
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   155
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg1 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   154
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Output Control Register Bit 0"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   11
         Left            =   120
         TabIndex        =   144
         Top             =   1320
         Width           =   6975
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   152
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   151
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   150
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   149
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   148
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   147
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   146
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckOutoutCtrlReg0 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   145
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 7"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   0
         Left            =   240
         TabIndex        =   26
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 6"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   1
         Left            =   1080
         TabIndex        =   25
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 5"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   2
         Left            =   1920
         TabIndex        =   24
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 4"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   3
         Left            =   2760
         TabIndex        =   23
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 3"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   4
         Left            =   3600
         TabIndex        =   22
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 2"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   5
         Left            =   4440
         TabIndex        =   21
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 1"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   6
         Left            =   5280
         TabIndex        =   20
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckOutputStatus 
         Caption         =   "Bit 0"
         ForeColor       =   &H00FF0000&
         Height          =   375
         Index           =   7
         Left            =   6120
         TabIndex        =   19
         Top             =   360
         Width           =   735
      End
      Begin VB.Label Label 
         Caption         =   "Bit 7:  If set to one the output is enabled."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   40
         Left            =   10800
         TabIndex        =   311
         Top             =   960
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 6:  Reserved."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   39
         Left            =   10800
         TabIndex        =   310
         Top             =   720
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 5:  Reserved."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   38
         Left            =   10800
         TabIndex        =   309
         Top             =   480
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 4:  Send Off event on high to low."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   37
         Left            =   10800
         TabIndex        =   308
         Top             =   240
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 3: Send On event on low to high. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   36
         Left            =   7920
         TabIndex        =   307
         Top             =   960
         Width           =   2655
      End
      Begin VB.Label Label 
         Caption         =   "Bit 2: Protection timer enable. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   35
         Left            =   7920
         TabIndex        =   306
         Top             =   720
         Width           =   2415
      End
      Begin VB.Label Label 
         Caption         =   "Bit 1: Reserved. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   34
         Left            =   7920
         TabIndex        =   305
         Top             =   480
         Width           =   2415
      End
      Begin VB.Label Label 
         Caption         =   "Bit 0: Initial startup output state. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   33
         Left            =   7920
         TabIndex        =   304
         Top             =   240
         Width           =   2415
      End
      Begin VB.Label Label 
         Alignment       =   2  'Center
         Caption         =   "Bit definitions"
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
         Index           =   32
         Left            =   7920
         TabIndex        =   303
         Top             =   0
         Width           =   5775
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 7 :"
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
         Index           =   16
         Left            =   11040
         TabIndex        =   277
         Top             =   5400
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 6 :"
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
         Index           =   15
         Left            =   11040
         TabIndex        =   275
         Top             =   4920
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 5 :"
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
         Index           =   14
         Left            =   7440
         TabIndex        =   273
         Top             =   5400
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 4 :"
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
         Index           =   13
         Left            =   7440
         TabIndex        =   271
         Top             =   4920
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 3 :"
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
         Index           =   12
         Left            =   3840
         TabIndex        =   269
         Top             =   5400
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 2 :"
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
         Index           =   11
         Left            =   3840
         TabIndex        =   267
         Top             =   4920
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 1 :"
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
         Index           =   103
         Left            =   240
         TabIndex        =   265
         Top             =   5400
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Protection timer 0 :"
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
         Index           =   101
         Left            =   240
         TabIndex        =   264
         Top             =   4920
         Width           =   1695
      End
      Begin VB.Label Label 
         Caption         =   "Output status"
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
         Index           =   100
         Left            =   240
         TabIndex        =   143
         Top             =   120
         Width           =   1215
      End
   End
   Begin VB.Frame frameInput 
      BorderStyle     =   0  'None
      Height          =   5895
      Left            =   600
      TabIndex        =   9
      Top             =   9240
      Width           =   14175
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 7"
         Enabled         =   0   'False
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   10
         Left            =   7200
         TabIndex        =   134
         Top             =   3600
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 0"
            Enabled         =   0   'False
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   142
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 1"
            Enabled         =   0   'False
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   141
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 2"
            Enabled         =   0   'False
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   140
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 3"
            Enabled         =   0   'False
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   139
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 4"
            Enabled         =   0   'False
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   138
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 5"
            Enabled         =   0   'False
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   137
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   136
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg7 
            Caption         =   "Bit 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   135
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 6"
         Enabled         =   0   'False
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   9
         Left            =   7200
         TabIndex        =   125
         Top             =   2760
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 0"
            Enabled         =   0   'False
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   133
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 1"
            Enabled         =   0   'False
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   132
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 2"
            Enabled         =   0   'False
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   131
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 3"
            Enabled         =   0   'False
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   130
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 4"
            Enabled         =   0   'False
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   129
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 5"
            Enabled         =   0   'False
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   128
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   127
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg6 
            Caption         =   "Bit 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   126
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 5"
         Enabled         =   0   'False
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   8
         Left            =   7200
         TabIndex        =   116
         Top             =   1920
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 0"
            Enabled         =   0   'False
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   124
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 1"
            Enabled         =   0   'False
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   123
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 2"
            Enabled         =   0   'False
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   122
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 3"
            Enabled         =   0   'False
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   121
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 4"
            Enabled         =   0   'False
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   120
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 5"
            Enabled         =   0   'False
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   119
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   118
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg5 
            Caption         =   "Bit 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   117
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 4"
         Enabled         =   0   'False
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   7
         Left            =   7200
         TabIndex        =   107
         Top             =   1080
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 7"
            Enabled         =   0   'False
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   115
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 6"
            Enabled         =   0   'False
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   114
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 5"
            Enabled         =   0   'False
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   113
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 4"
            Enabled         =   0   'False
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   112
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 3"
            Enabled         =   0   'False
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   111
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 2"
            Enabled         =   0   'False
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   110
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 1"
            Enabled         =   0   'False
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   109
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg4 
            Caption         =   "Bit 0"
            Enabled         =   0   'False
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   108
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 3"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   6
         Left            =   120
         TabIndex        =   98
         Top             =   3600
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   106
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   105
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   104
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   103
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   102
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   101
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   100
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg3 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   99
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 2"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   5
         Left            =   120
         TabIndex        =   89
         Top             =   2760
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   97
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   960
            TabIndex        =   96
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   95
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   94
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   93
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   92
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   91
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg2 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   90
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 1"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   4
         Left            =   120
         TabIndex        =   80
         Top             =   1920
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   88
            Top             =   240
            Width           =   615
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   87
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   86
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   85
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   84
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   83
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   82
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg1 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   81
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Input Control Register Bit 0"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   735
         Index           =   3
         Left            =   120
         TabIndex        =   71
         Top             =   1080
         Width           =   6975
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 0"
            Height          =   375
            Index           =   7
            Left            =   6120
            TabIndex        =   79
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 1"
            Height          =   375
            Index           =   6
            Left            =   5280
            TabIndex        =   78
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 2"
            Height          =   375
            Index           =   5
            Left            =   4440
            TabIndex        =   77
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 3"
            Height          =   375
            Index           =   4
            Left            =   3600
            TabIndex        =   76
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 4"
            Height          =   375
            Index           =   3
            Left            =   2760
            TabIndex        =   75
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 5"
            Height          =   375
            Index           =   2
            Left            =   1920
            TabIndex        =   74
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 6"
            Height          =   375
            Index           =   1
            Left            =   1080
            TabIndex        =   73
            Top             =   240
            Width           =   735
         End
         Begin VB.CheckBox CheckInputCtrlReg0 
            Caption         =   "Bit 7"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   72
            Top             =   240
            Width           =   735
         End
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 0"
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   7
         Left            =   6120
         TabIndex        =   17
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 1"
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   6
         Left            =   5280
         TabIndex        =   16
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 2"
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   5
         Left            =   4440
         TabIndex        =   15
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 3"
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   4
         Left            =   3600
         TabIndex        =   14
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 4"
         Enabled         =   0   'False
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   3
         Left            =   2760
         TabIndex        =   13
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 5"
         Enabled         =   0   'False
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   2
         Left            =   1920
         TabIndex        =   12
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 6"
         Enabled         =   0   'False
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   1
         Left            =   1080
         TabIndex        =   11
         Top             =   360
         Width           =   735
      End
      Begin VB.CheckBox CheckInputStatus 
         Caption         =   "Bit 7"
         Enabled         =   0   'False
         ForeColor       =   &H00008000&
         Height          =   375
         Index           =   0
         Left            =   240
         TabIndex        =   10
         Top             =   360
         Width           =   735
      End
      Begin VB.Label Label 
         Alignment       =   2  'Center
         Caption         =   "Bit definitions"
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
         Index           =   49
         Left            =   4560
         TabIndex        =   320
         Top             =   4560
         Width           =   4815
      End
      Begin VB.Label Label 
         Caption         =   "Bit 0: Reserved. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   48
         Left            =   4560
         TabIndex        =   319
         Top             =   4800
         Width           =   2415
      End
      Begin VB.Label Label 
         Caption         =   "Bit 1: Alarm event on high. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   47
         Left            =   4560
         TabIndex        =   318
         Top             =   5040
         Width           =   2415
      End
      Begin VB.Label Label 
         Caption         =   "Bit 2: Off event on low. "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   46
         Left            =   4560
         TabIndex        =   317
         Top             =   5280
         Width           =   2415
      End
      Begin VB.Label Label 
         Caption         =   "Bit 3: On event on high.  "
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   45
         Left            =   4560
         TabIndex        =   316
         Top             =   5520
         Width           =   2655
      End
      Begin VB.Label Label 
         Caption         =   "Bit 4:  Reserved."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   44
         Left            =   7440
         TabIndex        =   315
         Top             =   4800
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 5:  Reserved."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   43
         Left            =   7440
         TabIndex        =   314
         Top             =   5040
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 6:  Reserved."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   42
         Left            =   7440
         TabIndex        =   313
         Top             =   5280
         Width           =   2895
      End
      Begin VB.Label Label 
         Caption         =   "Bit 7:  If set to one the input is enabled."
         ForeColor       =   &H00008000&
         Height          =   255
         Index           =   41
         Left            =   7440
         TabIndex        =   312
         Top             =   5520
         Width           =   2895
      End
      Begin VB.Label lbl 
         Caption         =   "Input status"
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
         Left            =   240
         TabIndex        =   70
         Top             =   120
         Width           =   1215
      End
   End
   Begin VB.Frame frameGeneral 
      BorderStyle     =   0  'None
      Height          =   5895
      Left            =   10080
      TabIndex        =   2
      Top             =   1680
      Width           =   14295
      Begin VB.Frame Frame4 
         Caption         =   "CAN GUID"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   5535
         Left            =   11760
         TabIndex        =   322
         Top             =   360
         Width           =   2535
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   0
            Left            =   600
            TabIndex        =   337
            Text            =   "0"
            Top             =   720
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   1
            Left            =   600
            TabIndex        =   336
            Text            =   "0"
            Top             =   1200
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   2
            Left            =   600
            TabIndex        =   335
            Text            =   "0"
            Top             =   1680
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   3
            Left            =   600
            TabIndex        =   334
            Text            =   "0"
            Top             =   2160
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   4
            Left            =   600
            TabIndex        =   333
            Text            =   "0"
            Top             =   2640
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   5
            Left            =   600
            TabIndex        =   332
            Text            =   "0"
            Top             =   3120
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   6
            Left            =   600
            TabIndex        =   331
            Text            =   "0"
            Top             =   3600
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   7
            Left            =   600
            TabIndex        =   330
            Text            =   "0"
            Top             =   4080
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   8
            Left            =   1320
            TabIndex        =   329
            Text            =   "0"
            Top             =   720
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   9
            Left            =   1320
            TabIndex        =   328
            Text            =   "0"
            Top             =   1200
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   10
            Left            =   1320
            TabIndex        =   327
            Text            =   "0"
            Top             =   1680
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   11
            Left            =   1320
            TabIndex        =   326
            Text            =   "0"
            Top             =   2160
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   12
            Left            =   1320
            TabIndex        =   325
            Text            =   "0"
            Top             =   2640
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   13
            Left            =   1320
            TabIndex        =   324
            Text            =   "0"
            Top             =   3120
            Width           =   615
         End
         Begin VB.TextBox editCanGuid 
            Height          =   375
            Index           =   14
            Left            =   1320
            TabIndex        =   323
            Text            =   "0"
            Top             =   3600
            Width           =   615
         End
         Begin VB.Label Label 
            Alignment       =   2  'Center
            Caption         =   "15-8"
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
            Index           =   24
            Left            =   600
            TabIndex        =   339
            Top             =   480
            Width           =   615
         End
         Begin VB.Label Label 
            Alignment       =   2  'Center
            Caption         =   "8-1"
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
            Index           =   51
            Left            =   1320
            TabIndex        =   338
            Top             =   480
            Width           =   615
         End
      End
      Begin VB.Frame frame 
         Caption         =   "CAN Channel"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   2175
         Index           =   2
         Left            =   3840
         TabIndex        =   253
         Top             =   3720
         Width           =   7815
         Begin VB.TextBox editMask 
            Height          =   375
            Index           =   3
            Left            =   6720
            TabIndex        =   298
            Text            =   "0"
            Top             =   1440
            Width           =   615
         End
         Begin VB.TextBox editMask 
            Height          =   375
            Index           =   2
            Left            =   6000
            TabIndex        =   297
            Text            =   "0"
            Top             =   1440
            Width           =   615
         End
         Begin VB.TextBox editMask 
            Height          =   375
            Index           =   1
            Left            =   5280
            TabIndex        =   296
            Text            =   "0"
            Top             =   1440
            Width           =   615
         End
         Begin VB.TextBox editFilter 
            Height          =   375
            Index           =   3
            Left            =   6720
            TabIndex        =   295
            Text            =   "0"
            Top             =   720
            Width           =   615
         End
         Begin VB.TextBox editFilter 
            Height          =   375
            Index           =   2
            Left            =   6000
            TabIndex        =   294
            Text            =   "0"
            Top             =   720
            Width           =   615
         End
         Begin VB.TextBox editFilter 
            Height          =   375
            Index           =   1
            Left            =   5280
            TabIndex        =   293
            Text            =   "0"
            Top             =   720
            Width           =   615
         End
         Begin VB.TextBox editMask 
            Height          =   375
            Index           =   0
            Left            =   4560
            TabIndex        =   289
            Text            =   "0"
            Top             =   1440
            Width           =   615
         End
         Begin VB.TextBox editFilter 
            Height          =   375
            Index           =   0
            Left            =   4560
            TabIndex        =   288
            Text            =   "0"
            Top             =   720
            Width           =   615
         End
         Begin VB.CheckBox CheckCAN 
            Caption         =   "Persistent Open/Close"
            Height          =   375
            Index           =   7
            Left            =   1080
            TabIndex        =   263
            Top             =   1560
            Width           =   2295
         End
         Begin VB.CheckBox CheckCAN 
            Caption         =   "Port Open Status"
            Height          =   375
            Index           =   6
            Left            =   1080
            TabIndex        =   262
            Top             =   1200
            Width           =   2295
         End
         Begin VB.ComboBox ComboCANBitrate 
            Height          =   315
            ItemData        =   "frmControl1.frx":0442
            Left            =   240
            List            =   "frmControl1.frx":0461
            TabIndex        =   254
            Text            =   "500K"
            Top             =   720
            Width           =   1215
         End
         Begin VB.Label Label 
            Alignment       =   2  'Center
            Caption         =   "TYPE"
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
            Index           =   31
            Left            =   6000
            TabIndex        =   302
            Top             =   1200
            Width           =   1335
         End
         Begin VB.Label Label 
            Alignment       =   2  'Center
            Caption         =   "CLASS"
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
            Index           =   30
            Left            =   4560
            TabIndex        =   301
            Top             =   1200
            Width           =   1335
         End
         Begin VB.Label Label 
            Alignment       =   2  'Center
            Caption         =   "TYPE"
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
            Index           =   29
            Left            =   6000
            TabIndex        =   300
            Top             =   480
            Width           =   1335
         End
         Begin VB.Label Label 
            Alignment       =   2  'Center
            Caption         =   "CLASS"
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
            Index           =   28
            Left            =   4560
            TabIndex        =   299
            Top             =   480
            Width           =   1335
         End
         Begin VB.Label Label 
            Alignment       =   1  'Right Justify
            Caption         =   "Mask :"
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
            Index           =   27
            Left            =   3240
            TabIndex        =   291
            Top             =   1440
            Width           =   1215
         End
         Begin VB.Label Label 
            Alignment       =   1  'Right Justify
            Caption         =   "Filter :"
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
            Index           =   26
            Left            =   3240
            TabIndex        =   290
            Top             =   720
            Width           =   1215
         End
         Begin VB.Label Label 
            Caption         =   "Bitrate"
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
            Index           =   4
            Left            =   240
            TabIndex        =   256
            Top             =   480
            Width           =   1215
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Serial Channel"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   3255
         Index           =   0
         Left            =   7320
         TabIndex        =   251
         Top             =   360
         Width           =   4335
         Begin VB.CheckBox CheckSerial 
            Caption         =   "Persistent Open/Close"
            Height          =   375
            Index           =   7
            Left            =   1080
            TabIndex        =   261
            Top             =   2640
            Width           =   2295
         End
         Begin VB.CheckBox CheckSerial 
            Caption         =   "Port Open Status"
            Height          =   375
            Index           =   6
            Left            =   1080
            TabIndex        =   260
            Top             =   2280
            Width           =   2295
         End
         Begin VB.CheckBox CheckSerial 
            Caption         =   "Enable Serial Stream Input"
            Height          =   375
            Index           =   5
            Left            =   1080
            TabIndex        =   259
            Top             =   1920
            Width           =   2295
         End
         Begin VB.CheckBox CheckSerial 
            Caption         =   "Enable Serial Stream Output"
            Height          =   375
            Index           =   4
            Left            =   1080
            TabIndex        =   258
            Top             =   1560
            Width           =   2295
         End
         Begin VB.CheckBox CheckSerial 
            Caption         =   "Enable Confirm Events"
            Height          =   375
            Index           =   0
            Left            =   1080
            TabIndex        =   257
            Top             =   1200
            Width           =   2295
         End
         Begin VB.ComboBox ComboBaudrate 
            Height          =   315
            ItemData        =   "frmControl1.frx":0496
            Left            =   360
            List            =   "frmControl1.frx":04BE
            TabIndex        =   252
            Text            =   "57600"
            Top             =   720
            Width           =   1215
         End
         Begin VB.Label Label 
            Caption         =   "Baudrate"
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
            Index           =   0
            Left            =   360
            TabIndex        =   255
            Top             =   480
            Width           =   1215
         End
      End
      Begin VB.Frame frame 
         Caption         =   "Module Control Register"
         BeginProperty Font 
            Name            =   "MS Sans Serif"
            Size            =   8.25
            Charset         =   0
            Weight          =   700
            Underline       =   0   'False
            Italic          =   0   'False
            Strikethrough   =   0   'False
         EndProperty
         Height          =   2775
         Index           =   1
         Left            =   3840
         TabIndex        =   244
         Top             =   360
         Width           =   3375
         Begin VB.CheckBox CheckCtrlReg 
            Caption         =   "Enable external Vref- on RA2"
            Height          =   375
            Index           =   0
            Left            =   240
            TabIndex        =   250
            Top             =   360
            Width           =   2535
         End
         Begin VB.CheckBox CheckCtrlReg 
            Caption         =   "Enable external Vref+ on RA3"
            Height          =   375
            Index           =   1
            Left            =   240
            TabIndex        =   249
            Top             =   720
            Width           =   2535
         End
         Begin VB.CheckBox CheckCtrlReg 
            Caption         =   "Make  PWM value persistent"
            Height          =   375
            Index           =   4
            Left            =   240
            TabIndex        =   248
            Top             =   1080
            Width           =   2535
         End
         Begin VB.CheckBox CheckCtrlReg 
            Caption         =   "Enable Input Change events"
            Height          =   375
            Index           =   5
            Left            =   240
            TabIndex        =   247
            Top             =   1440
            Width           =   2535
         End
         Begin VB.CheckBox CheckCtrlReg 
            Caption         =   "Enable Output periodic events"
            Height          =   375
            Index           =   6
            Left            =   240
            TabIndex        =   246
            Top             =   1800
            Width           =   2535
         End
         Begin VB.CheckBox CheckCtrlReg 
            Caption         =   "Enable Input periodic events"
            Height          =   375
            Index           =   7
            Left            =   240
            TabIndex        =   245
            Top             =   2160
            Width           =   2535
         End
      End
      Begin VB.TextBox editZone 
         Height          =   285
         Left            =   2520
         TabIndex        =   5
         Text            =   "0"
         Top             =   600
         Width           =   975
      End
      Begin VB.TextBox editSubzoneBase 
         Height          =   285
         Left            =   2520
         TabIndex        =   4
         Text            =   "0"
         Top             =   960
         Width           =   975
      End
      Begin VB.TextBox editEventInterval 
         Height          =   285
         Left            =   2520
         TabIndex        =   3
         Text            =   "0"
         Top             =   1560
         Width           =   975
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Zone  :"
         Height          =   255
         Index           =   1
         Left            =   1560
         TabIndex        =   8
         Top             =   600
         Width           =   855
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Subzone  base :"
         Height          =   255
         Index           =   2
         Left            =   960
         TabIndex        =   7
         Top             =   960
         Width           =   1455
      End
      Begin VB.Label Label 
         Alignment       =   1  'Right Justify
         Caption         =   "Event interval in seconds  :"
         Height          =   255
         Index           =   3
         Left            =   360
         TabIndex        =   6
         Top             =   1560
         Width           =   2055
      End
   End
   Begin MSComctlLib.TabStrip TabStrip1 
      Height          =   6495
      Left            =   120
      TabIndex        =   1
      Top             =   120
      Width           =   14655
      _ExtentX        =   25850
      _ExtentY        =   11456
      _Version        =   393216
      BeginProperty Tabs {1EFB6598-857C-11D1-B16A-00C0F0283628} 
         NumTabs         =   5
         BeginProperty Tab1 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "General"
            Key             =   "general"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab2 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Outputs"
            Key             =   "outputs1"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab3 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "Inputs"
            Key             =   "inputs"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab4 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "A/D Conversion"
            Key             =   "ad"
            ImageVarType    =   2
         EndProperty
         BeginProperty Tab5 {1EFB659A-857C-11D1-B16A-00C0F0283628} 
            Caption         =   "PWM"
            Key             =   "pwm"
            ImageVarType    =   2
         EndProperty
      EndProperty
   End
   Begin VB.CommandButton btnOK 
      Caption         =   "E&xit"
      Height          =   495
      Left            =   11280
      TabIndex        =   0
      Top             =   6960
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
      Left            =   240
      TabIndex        =   287
      Top             =   6720
      Width           =   5655
   End
   Begin VB.Label Label1 
      Caption         =   "Label1"
      Height          =   495
      Left            =   7080
      TabIndex        =   280
      Top             =   4560
      Width           =   1215
   End
End
Attribute VB_Name = "frmControl1"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Option Explicit

Private Sub btnOK_Click()
    Unload Me
End Sub



Private Sub btnRead_Click()

    Dim filter As Long
    Dim mask As Long
    Dim content As Byte
    Dim wrk1, wrk2, wrk3 As Long
    Dim i As Integer
    
    ' Change mouse pointer to hourglass.
    Screen.MousePointer = vbHourglass
    
    ProgressBar1.value = 100
    
    ' =================================================================
    ' General
    ' =================================================================
    If TabStrip1.SelectedItem.Index = 1 Then
    
        ' Read first register set
        If (Not ReadRegisterMany(Me, 0, 0, 0, 0, 5)) Then
            MsgBox "Failed to read general registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
        
            ' Zone
            editZone = Trim(str(Vscptcpctrl1.data(8)))
        
            ' Subzonebase
            editSubzoneBase = Trim(str(Vscptcpctrl1.data(9)))
        
            ' Event interval
            editEventInterval = Trim(str(Vscptcpctrl1.data(12)))
       
            setControlRegister Vscptcpctrl1.data(11)
       
        End If
            
    
        ProgressBar1.value = 70
    
        ' Read serial channel related information
        If (Not ReadRegisterMany(Me, 0, 0, 0, 68, 3)) Then
            MsgBox "Failed to read serial registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
        
            ' Baudrate
            If (Vscptcpctrl1.data(8) And 15) <= 11 Then
                ComboBaudrate.ListIndex = (Vscptcpctrl1.data(8) And 15)
            Else
                ComboBaudrate.ListIndex = 7 ' 57600
            End If
        
            ' Serial Channel control register.
            SetSerialChannelControl Vscptcpctrl1.data(10)
            
        
        End If
    
        ProgressBar1.value = 30
    
        ' Read CAN channel related information
        If (Not ReadRegisterMany(Me, 0, 0, 0, 104, 10)) Then
            MsgBox "Failed to read CAN registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            ' CAN Bitrate
            If (Vscptcpctrl1.data(8) And 15) <= 8 Then
                ComboCANBitrate.ListIndex = (Vscptcpctrl1.data(8) And 15)
            Else
                ComboCANBitrate.ListIndex = 6 ' 500K
            End If
        
            ' CAN channel open status. Set to one to open zero to close channel. Read to see if open/closed.
            If Vscptcpctrl1.data(8) And 64 Then
                CheckCAN(6).value = 1
            Else
                CheckCAN(6).value = 0
            End If
        
            ' Persistent open/close. Set state for channel when board is started.
            If Vscptcpctrl1.data(8) And 128 Then
                CheckCAN(7).value = 1
            Else
                CheckCAN(7).value = 0
            End If
        
            ' Filter
            editFilter(0).Text = Trim(str(Vscptcpctrl1.data(13)))
            editFilter(1).Text = Trim(str(Vscptcpctrl1.data(14)))
            editFilter(2).Text = Trim(str(Vscptcpctrl1.data(15)))
            editFilter(3).Text = Trim(str(Vscptcpctrl1.data(16)))
        
            ' Mask
            editMask(0).Text = Trim(str(Vscptcpctrl1.data(9)))
            editMask(1).Text = Trim(str(Vscptcpctrl1.data(10)))
            editMask(2).Text = Trim(str(Vscptcpctrl1.data(11)))
            editMask(3).Text = Trim(str(Vscptcpctrl1.data(12)))
        
        End If
        
        
        ProgressBar1.value = 20
    
        ' Read CAN GUID
        If (Not ReadRegisterMany(Me, 0, 0, 0, 113, 15)) Then
            MsgBox "Failed to read CAN GUID registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            For i = 0 To 14
                editCanGuid(i) = Trim(str(Vscptcpctrl1.data(8 + i)))
            Next
        End If
    
    ' =================================================================
    ' Outputs
    ' =================================================================
    ElseIf TabStrip1.SelectedItem.Index = 2 Then
           
        ' Read Output Control bytes
        If (Not ReadRegisterMany(Me, 0, 0, 0, 6, 12)) Then
            MsgBox "Failed to read output control registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            
            ' Control register 0
            setCheckBoxBits CheckOutoutCtrlReg0, Vscptcpctrl1.data(12)
            
            ' Control register 1
            setCheckBoxBits CheckOutoutCtrlReg1, Vscptcpctrl1.data(13)
            
            ' Control register 2
            setCheckBoxBits CheckOutoutCtrlReg2, Vscptcpctrl1.data(14)
            
            ' Control register 3
            setCheckBoxBits CheckOutoutCtrlReg3, Vscptcpctrl1.data(15)
            
            ' Control register 4
            setCheckBoxBits CheckOutoutCtrlReg4, Vscptcpctrl1.data(16)

            ' Control register 5
            setCheckBoxBits CheckOutoutCtrlReg5, Vscptcpctrl1.data(17)

            ' Control register 6
            setCheckBoxBits CheckOutoutCtrlReg6, Vscptcpctrl1.data(18)
            
            ' Control register 7
            setCheckBoxBits CheckOutoutCtrlReg7, Vscptcpctrl1.data(19)
            
            ' Set serial status
            setOutputStatus Vscptcpctrl1.data(8)
            
        End If
        
        ProgressBar1.value = 90
        
        ' Read protection timers
        If (Not ReadRegisterMany(Me, 0, 0, 0, 26, 16)) Then
            MsgBox "Failed to read protection timer registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            For i = 0 To 7
                wrk1 = Vscptcpctrl1.data(8 + i * 2)
                wrk1 = wrk1 * 256
                wrk1 = wrk1 + Vscptcpctrl1.data(9 + i * 2)
                editProtectionTimer(i).Text = Trim(str(wrk1))
                ProgressBar1.value = ProgressBar1.value - 10
            Next
        End If
    
    ' =================================================================
    ' Inputs
    ' =================================================================
    ElseIf TabStrip1.SelectedItem.Index = 3 Then
    
        ' Read Output Control bytes
        If (Not ReadRegisterMany(Me, 0, 0, 0, 18, 8)) Then
            MsgBox "Failed to read output control registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            
            ' Control register 0
            setCheckBoxBits CheckInputCtrlReg0, Vscptcpctrl1.data(8)
            
            ' Control register 1
            setCheckBoxBits CheckInputCtrlReg1, Vscptcpctrl1.data(9)
            
            ' Control register 2
            setCheckBoxBits CheckInputCtrlReg2, Vscptcpctrl1.data(10)
            
            ' Control register 3
            setCheckBoxBits CheckInputCtrlReg3, Vscptcpctrl1.data(11)
            
            ' Control register 4
            setCheckBoxBits CheckInputCtrlReg4, Vscptcpctrl1.data(12)

            ' Control register 5
            setCheckBoxBits CheckInputCtrlReg5, Vscptcpctrl1.data(13)

            ' Control register 6
            setCheckBoxBits CheckInputCtrlReg6, Vscptcpctrl1.data(14)
            
            ' Control register 7
            setCheckBoxBits CheckInputCtrlReg7, Vscptcpctrl1.data(15)
            
        End If
    
        ' Read inputs status
        If (Not ReadRegister(Me, 5, content)) Then
            MsgBox "Failed to read input status register"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputStatus, content
        End If
    
    ' =================================================================
    ' A/D
    ' =================================================================
    ElseIf TabStrip1.SelectedItem.Index = 4 Then
    
        ' Read A/D values
        If (Not ReadRegisterMany(Me, 0, 0, 0, 42, 8)) Then
            MsgBox "Failed to read A/D registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            
            For i = 0 To 5
                wrk1 = Vscptcpctrl1.data(8 + i * 2)
                wrk1 = wrk1 * 256
                wrk1 = wrk1 + Vscptcpctrl1.data(9 + i * 2)
                lblad(i).Caption = Trim(str(wrk1))
                ProgressBar1.value = ProgressBar1.value - 10
            Next
            
        End If
        
        
        ' Read A/D control registers
        If (Not ReadRegisterMany(Me, 0, 0, 0, 7, 3)) Then
            MsgBox "Failed to read A/D control registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
        
            ' Periodic Events
            setCheckBoxBits CheckPeriodicEvents, Vscptcpctrl1.data(8)
            
            ' High Alarm
            setCheckBoxBits CheckHighAlarm, Vscptcpctrl1.data(9)
            
            ' Low Alarm
            setCheckBoxBits CheckLowAlarm, Vscptcpctrl1.data(10)
            
        End If
        
        
        ' Read Min A/D control registers
        If (Not ReadRegisterMany(Me, 0, 0, 0, 71, 16)) Then
            MsgBox "Failed to read min A/D control registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            For i = 0 To 7
                wrk1 = Vscptcpctrl1.data(8 + i * 2)
                wrk1 = wrk1 * 256
                wrk1 = wrk1 + Vscptcpctrl1.data(9 + i * 2)
                editMinAlarm(i).Text = Trim(str(wrk1))
                ProgressBar1.value = ProgressBar1.value - 2
            Next
        End If
        
        ' Read Max A/D control registers
        If (Not ReadRegisterMany(Me, 0, 0, 0, 87, 17)) Then
            MsgBox "Failed to read max A/D control registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            For i = 0 To 7
                wrk1 = Vscptcpctrl1.data(8 + i * 2)
                wrk1 = wrk1 * 256
                wrk1 = wrk1 + Vscptcpctrl1.data(9 + i * 2)
                editMaxAlarm(i).Text = Trim(str(wrk1))
                ProgressBar1.value = ProgressBar1.value - 2
            Next
            
            ' Hysteresis
            editHysteresis.Text = Trim(str(Vscptcpctrl1.data(24)))
        End If
        
    
    ' =================================================================
    ' PWM
    ' =================================================================
    ElseIf TabStrip1.SelectedItem.Index = 5 Then
    
        ' Read PWM registers
        If (Not ReadRegisterMany(Me, 0, 0, 0, 66, 2)) Then
            MsgBox "Failed to read PWM registers"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            
            wrk1 = Val(Vscptcpctrl1.data(8))
            wrk1 = 256 * wrk1
            wrk1 = wrk1 + Val(Vscptcpctrl1.data(9))
            editPWM.Text = Trim(str(wrk1))
        
        End If
    
    End If
    
    ProgressBar1.value = 0
    
    Screen.MousePointer = vbDefault
    
End Sub




Private Sub btnWrite_Click()
    Dim value As Byte
    Dim retval As Byte
    Dim i As Integer
    Dim wrk1, wrk2, wrk3 As Long
    
    ' Change mouse pointer to hourglass.
    Screen.MousePointer = vbHourglass
    
    ProgressBar1.value = 100

    ' =======================================================================
    ' General
    ' =======================================================================
    If TabStrip1.SelectedItem.Index = 1 Then
    
        ' Zone
        If (Not WriteRegister(Me, 0, Val(editZone.Text), retval)) Then
            MsgBox "Failed to write VSCP Node Zone"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            editZone.Text = Trim(str(retval))
        End If
        
        ProgressBar1.value = 70
        
        ' Subzonebase
        WriteRegister Me, 0, Val(editSubzoneBase.Text), retval
        editSubzoneBase.Text = Trim(str(retval))
        
        ' Event Interval
        If (Not WriteRegister(Me, 4, Val(editEventInterval.Text), retval)) Then
            MsgBox "Failed to write VSCP Node event period"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            editEventInterval.Text = Trim(str(retval))
        End If
        
        ProgressBar1.value = 60
        
        ' Module Control Register
        value = 0
        If CheckCtrlReg(0) Then value = value + 1
        If CheckCtrlReg(1) Then value = value + 2
        'If CheckCtrlReg(2) Then value = value + 4
        'If CheckCtrlReg(3) Then value = value + 8
        If CheckCtrlReg(4) Then value = value + 16
        If CheckCtrlReg(5) Then value = value + 32
        If CheckCtrlReg(6) Then value = value + 64
        If CheckCtrlReg(7) Then value = value + 128
        If (Not WriteRegister(Me, 3, value, retval)) Then
            MsgBox "Failed to write VSCP Node Module Control Register"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setControlRegister retval
        End If
        
        ProgressBar1.value = 65
        
        ' Serial Channel
        If ComboBaudrate.ListIndex <= 11 Then
            value = ComboBaudrate.ListIndex
        Else
            value = 7
        End If
        
        If (Not WriteRegister(Me, 68, value, retval)) Then
            MsgBox "Failed to write VSCP Node Serial Baudrate register."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            If (retval And 15) <= 11 Then
                ComboBaudrate.ListIndex = retval And 15
            Else
                ComboBaudrate.ListIndex = 7
            End If
        End If
        
        ProgressBar1.value = 60
        
        ' - Serial Channel control register.
        value = 0
        If CheckSerial(0) Then value = value + 1
        'If CheckSerial(1) Then value = value + 2
        'If CheckSerial(2) Then value = value + 3
        'If CheckSerial(3) Then value = value + 4
        If CheckSerial(4) Then value = value + 16
        If CheckSerial(5) Then value = value + 32
        If CheckSerial(6) Then value = value + 64
        If CheckSerial(7) Then value = value + 128
        If (Not WriteRegister(Me, 70, value, retval)) Then
            MsgBox "Failed to write VSCP Node Serial Channel control register."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            SetSerialChannelControl retval
        End If
        
        ' CAN
        If ComboCANBitrate.ListIndex <= 8 Then
            value = ComboCANBitrate.ListIndex
        Else
            value = 6
        End If
        
        If CheckCAN(6) Then value = value + 64
        If CheckCAN(7) Then value = value + 128
        
        ProgressBar1.value = 50
        
        If (Not WriteRegister(Me, 104, value, retval)) Then
            MsgBox "Failed to write VSCP Node CAN bitrate register."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            If (retval And 15) <= 8 Then
                ComboCANBitrate.ListIndex = retval And 15
            Else
                ComboCANBitrate.ListIndex = 6
            End If
        End If
        
        If retval And 64 Then
            CheckCAN(6) = 1
        Else
            CheckCAN(6) = 0
        End If
        
        If retval And 128 Then
            CheckCAN(7) = 1
        Else
            CheckCAN(7) = 0
        End If
        
        ProgressBar1.value = 40
        
        ' Filter
        For i = 0 To 3
            If (Not WriteRegister(Me, 109 + i, editFilter(i).Text, retval)) Then
                MsgBox "Failed to write VSCP Node CAN filter register."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                editFilter(i).Text = Trim(str(retval))
            End If
        Next i
        
        ProgressBar1.value = 30
        
        ' Mask
        For i = 0 To 3
            If (Not WriteRegister(Me, 105 + i, editMask(i).Text, retval)) Then
                MsgBox "Failed to write VSCP Node CAN mask register."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                editMask(i).Text = Trim(str(retval))
            End If
        Next i
        
        ProgressBar1.value = 20
        
        ' CAN GUID
        For i = 0 To 14
            If (Not WriteRegister(Me, 113 + i, editCanGuid(i).Text, retval)) Then
                MsgBox "Failed to write VSCP Node CAN GUID register."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                editCanGuid(i).Text = Trim(str(retval))
            End If
        Next i
        
    ' =======================================================================
    ' Outputs
    ' =======================================================================
    ElseIf TabStrip1.SelectedItem.Index = 2 Then
    
        ' Control reg 0
        If (Not WriteRegister(Me, 10, GetCheckBoxBits(CheckOutoutCtrlReg0), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 0."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg0, retval
        End If
        
        ProgressBar1.value = 95
        
        
        ' Control reg 1
        If (Not WriteRegister(Me, 11, GetCheckBoxBits(CheckOutoutCtrlReg1), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 1."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg1, retval
        End If
        
        ProgressBar1.value = 90
        
        ' Control reg 2
        If (Not WriteRegister(Me, 12, GetCheckBoxBits(CheckOutoutCtrlReg2), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 2."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg2, retval
        End If
        
        ProgressBar1.value = 85
        
        ' Control reg 3
        If (Not WriteRegister(Me, 13, GetCheckBoxBits(CheckOutoutCtrlReg3), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 3."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg3, retval
        End If
        
        ProgressBar1.value = 80
        
        ' Control reg 4
        If (Not WriteRegister(Me, 14, GetCheckBoxBits(CheckOutoutCtrlReg4), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 4."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg4, retval
        End If
        
        ProgressBar1.value = 75
        
        ' Control reg 5
        If (Not WriteRegister(Me, 15, GetCheckBoxBits(CheckOutoutCtrlReg5), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 5."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg5, retval
        End If
        
        ProgressBar1.value = 70
        
        ' Control reg 6
        If (Not WriteRegister(Me, 16, GetCheckBoxBits(CheckOutoutCtrlReg6), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 6."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg6, retval
        End If
        
        ProgressBar1.value = 65
        
        ' Control reg 7
        If (Not WriteRegister(Me, 17, GetCheckBoxBits(CheckOutoutCtrlReg7), retval)) Then
            MsgBox "Failed to write VSCP Node CAN output control register 7."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckOutoutCtrlReg7, retval
        End If
        
        ProgressBar1.value = 60
        
        ' Write Protection Timers
        For i = 0 To 7
            If (Not WriteRegister(Me, 26 + i * 2, (((Val(editProtectionTimer(i).Text) And 65280) / 256) And 255), retval)) Then
                MsgBox "Failed to write VSCP Node output protection timer register."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                wrk1 = retval
                wrk1 = wrk1 * 256
            End If
            
            If (Not WriteRegister(Me, 27 + i * 2, (Val(editProtectionTimer(i).Text) And 255), retval)) Then
                MsgBox "Failed to write VSCP Node output protection timer register."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                wrk1 = wrk1 + retval
                editProtectionTimer(i).Text = Trim(str(wrk1))
            End If
            
            ProgressBar1.value = ProgressBar1.value - 6
        Next
        
    
    ' =======================================================================
    ' Inputs
    ' =======================================================================
    ElseIf TabStrip1.SelectedItem.Index = 3 Then
    
        ' Control reg 0
        If (Not WriteRegister(Me, 18, GetCheckBoxBits(CheckInputCtrlReg0), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 0."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg0, retval
        End If
        
        ProgressBar1.value = 90
        
        ' Control reg 1
        If (Not WriteRegister(Me, 19, GetCheckBoxBits(CheckInputCtrlReg1), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 1."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg1, retval
        End If
        
        ProgressBar1.value = 80
        
        ' Control reg 2
        If (Not WriteRegister(Me, 20, GetCheckBoxBits(CheckInputCtrlReg2), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 2."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg2, retval
        End If
        
        ProgressBar1.value = 70
        
        ' Control reg 3
        If (Not WriteRegister(Me, 21, GetCheckBoxBits(CheckInputCtrlReg3), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 3."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg3, retval
        End If
        
        ProgressBar1.value = 60
        
        ' Control reg 4
        If (Not WriteRegister(Me, 22, GetCheckBoxBits(CheckInputCtrlReg4), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 4."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg4, retval
        End If
        
        ProgressBar1.value = 50
        
        ' Control reg 5
        If (Not WriteRegister(Me, 23, GetCheckBoxBits(CheckInputCtrlReg5), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 5."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg5, retval
        End If
        
        ProgressBar1.value = 40
        
        
        ' Control reg 6
        If (Not WriteRegister(Me, 24, GetCheckBoxBits(CheckInputCtrlReg6), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 6."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg6, retval
        End If
        
        ProgressBar1.value = 30
    
    
        ' Control reg 7
        If (Not WriteRegister(Me, 25, GetCheckBoxBits(CheckInputCtrlReg7), retval)) Then
            MsgBox "Failed to write VSCP Node Input control register 7."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputCtrlReg7, retval
        End If
        
        ProgressBar1.value = 20
        
        ' Read inputs status
        If (Not ReadRegister(Me, 5, value)) Then
            MsgBox "Failed to read input status register"
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckInputStatus, value
        End If
        
        
    ' =======================================================================
    ' A/D
    ' =======================================================================
    ElseIf TabStrip1.SelectedItem.Index = 4 Then
    
        ' Peridic events
        If (Not WriteRegister(Me, 7, GetCheckBoxBits(CheckPeriodicEvents), retval)) Then
            MsgBox "Failed to write A/D periodic register."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckPeriodicEvents, retval
        End If
        
        ProgressBar1.value = 95
        
        ' High Alarm
        If (Not WriteRegister(Me, 8, GetCheckBoxBits(CheckHighAlarm), retval)) Then
            MsgBox "Failed to write High Alarm register."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckHighAlarm, retval
        End If
        
        ProgressBar1.value = 90
        
        ' Low Alarm
        If (Not WriteRegister(Me, 9, GetCheckBoxBits(CheckLowAlarm), retval)) Then
            MsgBox "Failed to write Low Alarm register."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            setCheckBoxBits CheckLowAlarm, retval
        End If
        
        ProgressBar1.value = 85
        
        
        ' Write Min A/D alarm values
        For i = 0 To 7
            If (Not WriteRegister(Me, 71 + i * 2, (((Val(editMinAlarm(i).Text) And 65280) / 256) And 255), retval)) Then
                MsgBox "Failed to write A/D min alarm register LSB."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                wrk1 = retval
                wrk1 = wrk1 * 256
            End If
            
            If (Not WriteRegister(Me, 72 + i * 2, (Val(editMinAlarm(i).Text) And 255), retval)) Then
                MsgBox "Failed to write A/D min alarm register MSB."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                wrk1 = wrk1 + retval
                editMinAlarm(i).Text = Trim(str(wrk1))
            End If
            
            
            ProgressBar1.value = ProgressBar1.value - 5
            
        Next
        
        ' Write Max A/D alarm values
        For i = 0 To 7
        
            If (Not WriteRegister(Me, 87 + i * 2, (((Val(editMaxAlarm(i).Text) And 65280) / 256) And 255), retval)) Then
                MsgBox "Failed to write A/D max alarm register MSB."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                wrk1 = retval
                wrk1 = wrk1 * 256
            End If
            
            If (Not WriteRegister(Me, 88 + i * 2, (Val(editMaxAlarm(i).Text) And 255), retval)) Then
                MsgBox "Failed to write A/D max alarm register LSB."
                Screen.MousePointer = vbDefault
                Exit Sub
            Else
                wrk1 = wrk1 + retval
                editMaxAlarm(i).Text = Trim(str(wrk1))
            End If
            
            ProgressBar1.value = ProgressBar1.value - 5
            
        Next
        
        ' Hysteresis
        If (Not WriteRegister(Me, 103, Val(editHysteresis.Text), retval)) Then
            MsgBox "Failed to write A/D max alarm register LSB."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            editHysteresis.Text = Trim(str(retval))
        End If
        
        
    
    ' =======================================================================
    ' PWM
    ' =======================================================================
    ElseIf TabStrip1.SelectedItem.Index = 5 Then
    
        ' PWM value MSB
        If (Not WriteRegister(Me, 66, (((Val(editPWM.Text) And 65280) / 256) And 255), retval)) Then
            MsgBox "Failed to write PWM register MSB."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            wrk1 = retval
            wrk1 = wrk1 * 256
        End If
        
        ' PWM value LSB
        If (Not WriteRegister(Me, 67, (Val(editPWM.Text) And 255), retval)) Then
            MsgBox "Failed to write PWM register LSB."
            Screen.MousePointer = vbDefault
            Exit Sub
        Else
            wrk1 = wrk1 + retval
            editPWM.Text = Trim(str(wrk1))
        End If
    
    End If
    
    ProgressBar1.value = 0
    
    Screen.MousePointer = vbDefault
    
End Sub

Private Sub CheckOutputStatus_Click(Index As Integer)
    
    Dim i As Integer
    Dim output As Byte
    Dim retval As Byte
    
    Screen.MousePointer = vbHourglass
     
    output = 0
    If (1 = CheckOutputStatus(0).value) Then output = output + 128
    If (1 = CheckOutputStatus(1).value) Then output = output + 64
    If (1 = CheckOutputStatus(2).value) Then output = output + 32
    If (1 = CheckOutputStatus(3).value) Then output = output + 16
    If (1 = CheckOutputStatus(4).value) Then output = output + 8
    If (1 = CheckOutputStatus(5).value) Then output = output + 4
    If (1 = CheckOutputStatus(6).value) Then output = output + 2
    If (1 = CheckOutputStatus(7).value) Then output = output + 1
    
    If (Not WriteRegister(Me, 6, output, retval)) Then
        MsgBox "Failed to write VSCP Node output status register"
        Screen.MousePointer = vbDefault
        Exit Sub
    Else
        setOutputStatus retval
        
    End If
    
    Screen.MousePointer = vbDefault
    
End Sub

Sub setControlRegister(value As Byte)

    ' Module Control Register.
    If value And 1 Then  ' VRef-
        CheckCtrlReg(0).value = 1
    Else
        CheckCtrlReg(0).value = 0
    End If
       
    If value And 2 Then  ' VRef+
        CheckCtrlReg(1).value = 1
    Else
        CheckCtrlReg(1).value = 0
    End If
       
    If value And 16 Then  ' Make PWM value persistent.
        CheckCtrlReg(4).value = 1
    Else
        CheckCtrlReg(4).value = 0
    End If
       
    If value And 32 Then  ' Enable Input Change events.
        CheckCtrlReg(5).value = 1
    Else
        CheckCtrlReg(5).value = 0
    End If
       
    If value And 64 Then  ' Enable Output periodic events.
        CheckCtrlReg(6).value = 1
    Else
        CheckCtrlReg(6).value = 0
    End If
       
    If value And 128 Then  ' Enable Input periodic events.
         CheckCtrlReg(7).value = 1
    Else
        CheckCtrlReg(7).value = 0
    End If
    
End Sub

Sub SetSerialChannelControl(value As Byte)

    ' - Enable confirm events.
    If value And 1 Then
        CheckSerial(0).value = 1
    Else
        CheckSerial(0).value = 0
    End If
       
    ' - Enable serial stream events output to serial channel.
    If value And 16 Then
        CheckSerial(4).value = 1
    Else
        CheckSerial(4).value = 0
    End If
        
    ' - Enable serial stream events on input from serial channel.
    If value And 32 Then
        CheckSerial(5).value = 1
    Else
        CheckSerial(5).value = 0
    End If
       
    ' - Port open status. Set to one to open zero to close port. Read to see if open/closed.
    If value And 64 Then
        CheckSerial(6).value = 1
    Else
        CheckSerial(6).value = 0
    End If
        
    ' - Persistent open/close. Set state for port when board is started.
    If value And 128 Then
        CheckSerial(7).value = 1
    Else
        CheckSerial(7).value = 0
    End If
    
End Sub

Sub setOutputStatus(retval As Byte)
  
        If (retval And 1) Then
            CheckOutputStatus(7).value = 1
        Else
            CheckOutputStatus(7).value = 0
        End If
        
        If (retval And 2) Then
            CheckOutputStatus(6).value = 1
        Else
            CheckOutputStatus(6).value = 0
        End If
        
        If (retval And 4) Then
            CheckOutputStatus(5).value = 1
        Else
            CheckOutputStatus(5).value = 0
        End If
        
        If (retval And 8) Then
            CheckOutputStatus(4).value = 1
        Else
            CheckOutputStatus(4).value = 0
        End If
        
        If (retval And 16) Then
            CheckOutputStatus(3).value = 1
        Else
            CheckOutputStatus(3).value = 0
        End If
        
        If (retval And 32) Then
            CheckOutputStatus(2).value = 1
        Else
            CheckOutputStatus(2).value = 0
        End If
        
        If (retval And 64) Then
            CheckOutputStatus(1).value = 1
        Else
            CheckOutputStatus(1).value = 0
        End If
        
        If (retval And 128) Then
            CheckOutputStatus(0).value = 1
        Else
            CheckOutputStatus(0).value = 0
        End If
        

End Sub



Private Sub Form_Load()

    Me.Width = 15000
    Me.Height = 8000
    frameGeneral.Left = 360
    frameInput.Left = 360
    frameOutput.Left = 360
    frameAD.Left = 360
    framePWM.Left = 360

    frameGeneral.Top = 600
    frameInput.Top = 600
    frameOutput.Top = 600
    frameAD.Top = 600
    framePWM.Top = 600
  
    
    frameGeneral.Visible = False
    frameInput.Visible = False
    frameOutput.Visible = False
    frameAD.Visible = False
    framePWM.Visible = False

    
    frameGeneral.Visible = True
    
    ComboBaudrate.ListIndex = 7
    ComboCANBitrate.ListIndex = 6
    
End Sub

Private Sub TabStrip1_Click()
    frameGeneral.Visible = False
    frameInput.Visible = False
    frameOutput.Visible = False
    frameAD.Visible = False
    framePWM.Visible = False

    
    If TabStrip1.SelectedItem.Index = 1 Then
        frameGeneral.Visible = True
    ElseIf TabStrip1.SelectedItem.Index = 2 Then
        frameOutput.Visible = True
    ElseIf TabStrip1.SelectedItem.Index = 3 Then
        frameInput.Visible = True
    ElseIf TabStrip1.SelectedItem.Index = 4 Then
        frameAD.Visible = True
    ElseIf TabStrip1.SelectedItem.Index = 5 Then
        framePWM.Visible = True
    End If
    
End Sub

