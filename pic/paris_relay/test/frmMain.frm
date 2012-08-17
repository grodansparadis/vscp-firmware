VERSION 5.00
Object = "{1C77A74A-D0B8-4FA1-8300-60DF3E9E1E1B}#1.0#0"; "canalocx.ocx"
Begin VB.Form frmSmartRelay 
   Caption         =   "VSCP SmartRelay Testcode "
   ClientHeight    =   10170
   ClientLeft      =   60
   ClientTop       =   345
   ClientWidth     =   14625
   LinkTopic       =   "Form1"
   ScaleHeight     =   10170
   ScaleWidth      =   14625
   StartUpPosition =   3  'Windows Default
   Begin VB.CommandButton btnSetZone 
      Caption         =   "Write"
      Height          =   375
      Left            =   10440
      TabIndex        =   259
      Top             =   7680
      Width           =   1095
   End
   Begin VB.Frame Frame2 
      Caption         =   "Zone"
      Height          =   855
      Left            =   7560
      TabIndex        =   257
      Top             =   7320
      Width           =   4095
      Begin VB.CommandButton btnGetZone 
         Caption         =   "Read"
         Height          =   375
         Left            =   1680
         TabIndex        =   260
         Top             =   360
         Width           =   1095
      End
      Begin VB.TextBox txtZone 
         Height          =   285
         Left            =   240
         TabIndex        =   258
         Text            =   "0"
         Top             =   360
         Width           =   1215
      End
   End
   Begin VB.CheckBox Check7 
      Caption         =   "0"
      Height          =   495
      Left            =   13560
      TabIndex        =   240
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Check6 
      Caption         =   "1"
      Height          =   495
      Left            =   13080
      TabIndex        =   239
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Check5 
      Caption         =   "2"
      Height          =   495
      Left            =   12600
      TabIndex        =   238
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Check4 
      Caption         =   "3"
      Height          =   495
      Left            =   12120
      TabIndex        =   237
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Check3 
      Caption         =   "4"
      Height          =   495
      Left            =   11640
      TabIndex        =   236
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Check2 
      Caption         =   "5"
      Height          =   495
      Left            =   11160
      TabIndex        =   235
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Check1 
      Caption         =   "6"
      Height          =   495
      Left            =   10680
      TabIndex        =   234
      Top             =   8640
      Width           =   375
   End
   Begin VB.CheckBox Bit7 
      Caption         =   "7"
      Height          =   495
      Left            =   10200
      TabIndex        =   233
      Top             =   8640
      Width           =   375
   End
   Begin VB.CommandButton cmdReadDM 
      Caption         =   "Read DM"
      Height          =   495
      Left            =   7680
      TabIndex        =   232
      Top             =   8880
      Width           =   1215
   End
   Begin VB.CommandButton cmdWriteMatrix 
      Caption         =   "Write DM"
      Height          =   495
      Left            =   7680
      TabIndex        =   231
      Top             =   9480
      Width           =   1215
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   6
      Left            =   2040
      TabIndex        =   218
      Top             =   9720
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   6
      Left            =   2640
      TabIndex        =   217
      Top             =   9720
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   6
      Left            =   3240
      TabIndex        =   216
      Top             =   9720
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   6
      Left            =   3840
      TabIndex        =   215
      Top             =   9720
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   6
      Left            =   4440
      TabIndex        =   214
      Top             =   9720
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   6
      Left            =   5040
      TabIndex        =   213
      Top             =   9720
      Width           =   495
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   6
      Left            =   6960
      TabIndex        =   212
      Top             =   9720
      Width           =   495
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   6
      ItemData        =   "frmMain.frx":0000
      Left            =   5640
      List            =   "frmMain.frx":0016
      Style           =   2  'Dropdown List
      TabIndex        =   211
      Top             =   9720
      Width           =   1215
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   5
      Left            =   2040
      TabIndex        =   210
      Top             =   9360
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   5
      Left            =   2640
      TabIndex        =   209
      Top             =   9360
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   5
      Left            =   3240
      TabIndex        =   208
      Top             =   9360
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   5
      Left            =   3840
      TabIndex        =   207
      Top             =   9360
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   5
      Left            =   4440
      TabIndex        =   206
      Top             =   9360
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   5
      Left            =   5040
      TabIndex        =   205
      Top             =   9360
      Width           =   495
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   5
      Left            =   6960
      TabIndex        =   204
      Top             =   9360
      Width           =   495
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   5
      ItemData        =   "frmMain.frx":0053
      Left            =   5640
      List            =   "frmMain.frx":0069
      Style           =   2  'Dropdown List
      TabIndex        =   203
      Top             =   9360
      Width           =   1215
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   4
      Left            =   2040
      TabIndex        =   202
      Top             =   9000
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   4
      Left            =   2640
      TabIndex        =   201
      Top             =   9000
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   4
      Left            =   3240
      TabIndex        =   200
      Top             =   9000
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   4
      Left            =   3840
      TabIndex        =   199
      Top             =   9000
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   4
      Left            =   4440
      TabIndex        =   198
      Top             =   9000
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   4
      Left            =   5040
      TabIndex        =   197
      Top             =   9000
      Width           =   495
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   4
      Left            =   6960
      TabIndex        =   196
      Top             =   9000
      Width           =   495
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   4
      ItemData        =   "frmMain.frx":00A6
      Left            =   5640
      List            =   "frmMain.frx":00BC
      Style           =   2  'Dropdown List
      TabIndex        =   195
      Top             =   9000
      Width           =   1215
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   3
      Left            =   2040
      TabIndex        =   194
      Top             =   8640
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   3
      Left            =   2640
      TabIndex        =   193
      Top             =   8640
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   3
      Left            =   3240
      TabIndex        =   192
      Top             =   8640
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   3
      Left            =   3840
      TabIndex        =   191
      Top             =   8640
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   3
      Left            =   4440
      TabIndex        =   190
      Top             =   8640
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   3
      Left            =   5040
      TabIndex        =   189
      Top             =   8640
      Width           =   495
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   3
      Left            =   6960
      TabIndex        =   188
      Top             =   8640
      Width           =   495
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   3
      ItemData        =   "frmMain.frx":00F9
      Left            =   5640
      List            =   "frmMain.frx":010F
      Style           =   2  'Dropdown List
      TabIndex        =   187
      Top             =   8640
      Width           =   1215
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   2
      Left            =   2040
      TabIndex        =   186
      Top             =   8280
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   2
      Left            =   2640
      TabIndex        =   185
      Top             =   8280
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   2
      Left            =   3240
      TabIndex        =   184
      Top             =   8280
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   2
      Left            =   3840
      TabIndex        =   183
      Top             =   8280
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   2
      Left            =   4440
      TabIndex        =   182
      Top             =   8280
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   2
      Left            =   5040
      TabIndex        =   181
      Top             =   8280
      Width           =   495
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   2
      Left            =   6960
      TabIndex        =   180
      Top             =   8280
      Width           =   495
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   2
      ItemData        =   "frmMain.frx":014C
      Left            =   5640
      List            =   "frmMain.frx":0162
      Style           =   2  'Dropdown List
      TabIndex        =   179
      Top             =   8280
      Width           =   1215
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   1
      Left            =   2040
      TabIndex        =   178
      Top             =   7920
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   1
      Left            =   2640
      TabIndex        =   177
      Top             =   7920
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   1
      Left            =   3240
      TabIndex        =   176
      Top             =   7920
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   1
      Left            =   3840
      TabIndex        =   175
      Top             =   7920
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   1
      Left            =   4440
      TabIndex        =   174
      Top             =   7920
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   1
      Left            =   5040
      TabIndex        =   173
      Top             =   7920
      Width           =   495
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   1
      Left            =   6960
      TabIndex        =   172
      Top             =   7920
      Width           =   495
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   1
      ItemData        =   "frmMain.frx":019F
      Left            =   5640
      List            =   "frmMain.frx":01B5
      Style           =   2  'Dropdown List
      TabIndex        =   171
      Top             =   7920
      Width           =   1215
   End
   Begin VB.ComboBox ComboAction 
      Height          =   315
      Index           =   0
      ItemData        =   "frmMain.frx":01F2
      Left            =   5640
      List            =   "frmMain.frx":0208
      Style           =   2  'Dropdown List
      TabIndex        =   170
      Top             =   7560
      Width           =   1215
   End
   Begin VB.TextBox txtActionParameter 
      Height          =   285
      Index           =   0
      Left            =   6960
      TabIndex        =   169
      Top             =   7560
      Width           =   495
   End
   Begin VB.TextBox txtDMtypefilter 
      Height          =   285
      Index           =   0
      Left            =   5040
      TabIndex        =   168
      Top             =   7560
      Width           =   495
   End
   Begin VB.TextBox txtDMtypemask 
      Height          =   285
      Index           =   0
      Left            =   4440
      TabIndex        =   167
      Top             =   7560
      Width           =   495
   End
   Begin VB.TextBox txtDMclassfilter 
      Height          =   285
      Index           =   0
      Left            =   3840
      TabIndex        =   166
      Top             =   7560
      Width           =   495
   End
   Begin VB.TextBox txtDMclassmask 
      Height          =   285
      Index           =   0
      Left            =   3240
      TabIndex        =   165
      Top             =   7560
      Width           =   495
   End
   Begin VB.TextBox txtDMflags 
      Height          =   285
      Index           =   0
      Left            =   2640
      TabIndex        =   164
      Top             =   7560
      Width           =   495
   End
   Begin VB.TextBox txtDMoid 
      Height          =   285
      Index           =   0
      Left            =   2040
      TabIndex        =   163
      Top             =   7560
      Width           =   495
   End
   Begin VB.Timer Timer1 
      Interval        =   1000
      Left            =   11880
      Top             =   9720
   End
   Begin VB.TextBox txtNickname 
      Height          =   285
      Left            =   13560
      TabIndex        =   161
      Text            =   "2"
      Top             =   7440
      Width           =   735
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 8"
      Height          =   3495
      Index           =   7
      Left            =   10920
      TabIndex        =   141
      Top             =   3720
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   7
         Left            =   2160
         TabIndex        =   255
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   7
         Left            =   240
         TabIndex        =   156
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   7
         Left            =   1680
         TabIndex        =   155
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   7
         Left            =   2160
         TabIndex        =   154
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   7
         Left            =   2160
         TabIndex        =   153
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   7
         Left            =   840
         TabIndex        =   152
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   7
         Left            =   1440
         TabIndex        =   151
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   150
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   149
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   148
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   147
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   146
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   145
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   7
         Left            =   240
         TabIndex        =   144
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   7
         Left            =   2400
         TabIndex        =   143
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   7
         Left            =   2400
         TabIndex        =   142
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   40
         Left            =   2160
         TabIndex        =   256
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   31
         Left            =   2160
         TabIndex        =   160
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   30
         Left            =   2160
         TabIndex        =   159
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   29
         Left            =   360
         TabIndex        =   158
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   28
         Left            =   240
         TabIndex        =   157
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 7"
      Height          =   3495
      Index           =   6
      Left            =   7320
      TabIndex        =   121
      Top             =   3720
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   6
         Left            =   2160
         TabIndex        =   253
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   6
         Left            =   240
         TabIndex        =   136
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   6
         Left            =   1680
         TabIndex        =   135
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   6
         Left            =   2160
         TabIndex        =   134
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   6
         Left            =   2160
         TabIndex        =   133
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   6
         Left            =   840
         TabIndex        =   132
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   6
         Left            =   1440
         TabIndex        =   131
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   130
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   129
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   128
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   127
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   126
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   125
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   6
         Left            =   240
         TabIndex        =   124
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   6
         Left            =   2400
         TabIndex        =   123
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   6
         Left            =   2400
         TabIndex        =   122
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   39
         Left            =   2160
         TabIndex        =   254
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   27
         Left            =   2160
         TabIndex        =   140
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   26
         Left            =   2160
         TabIndex        =   139
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   25
         Left            =   360
         TabIndex        =   138
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   24
         Left            =   240
         TabIndex        =   137
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 6"
      Height          =   3495
      Index           =   5
      Left            =   3720
      TabIndex        =   101
      Top             =   3720
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   5
         Left            =   2160
         TabIndex        =   251
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   5
         Left            =   240
         TabIndex        =   116
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   5
         Left            =   1680
         TabIndex        =   115
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   5
         Left            =   2160
         TabIndex        =   114
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   5
         Left            =   2160
         TabIndex        =   113
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   5
         Left            =   840
         TabIndex        =   112
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   5
         Left            =   1440
         TabIndex        =   111
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   110
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   109
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   108
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   107
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   106
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   105
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   5
         Left            =   240
         TabIndex        =   104
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   5
         Left            =   2400
         TabIndex        =   103
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   5
         Left            =   2400
         TabIndex        =   102
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   38
         Left            =   2160
         TabIndex        =   252
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   23
         Left            =   2160
         TabIndex        =   120
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   22
         Left            =   2160
         TabIndex        =   119
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   21
         Left            =   360
         TabIndex        =   118
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   20
         Left            =   240
         TabIndex        =   117
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 5"
      Height          =   3495
      Index           =   4
      Left            =   120
      TabIndex        =   81
      Top             =   3720
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   4
         Left            =   2160
         TabIndex        =   249
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   4
         Left            =   240
         TabIndex        =   96
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   4
         Left            =   1680
         TabIndex        =   95
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   4
         Left            =   2160
         TabIndex        =   94
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   4
         Left            =   2160
         TabIndex        =   93
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   4
         Left            =   840
         TabIndex        =   92
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   4
         Left            =   1440
         TabIndex        =   91
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   90
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   89
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   88
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   87
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   86
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   85
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   84
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   4
         Left            =   2400
         TabIndex        =   83
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   4
         Left            =   2400
         TabIndex        =   82
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   37
         Left            =   2160
         TabIndex        =   250
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   19
         Left            =   2160
         TabIndex        =   100
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   18
         Left            =   2160
         TabIndex        =   99
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   17
         Left            =   360
         TabIndex        =   98
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   16
         Left            =   240
         TabIndex        =   97
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 4"
      Height          =   3495
      Index           =   3
      Left            =   10920
      TabIndex        =   61
      Top             =   120
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   3
         Left            =   2160
         TabIndex        =   247
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   3
         Left            =   240
         TabIndex        =   76
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   3
         Left            =   1680
         TabIndex        =   75
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   3
         Left            =   2160
         TabIndex        =   74
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   3
         Left            =   2160
         TabIndex        =   73
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   3
         Left            =   840
         TabIndex        =   72
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   3
         Left            =   1440
         TabIndex        =   71
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   70
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   69
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   68
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   67
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   66
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   65
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   64
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   3
         Left            =   2400
         TabIndex        =   63
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   3
         Left            =   2400
         TabIndex        =   62
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   36
         Left            =   2160
         TabIndex        =   248
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   15
         Left            =   2160
         TabIndex        =   80
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   14
         Left            =   2160
         TabIndex        =   79
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   13
         Left            =   360
         TabIndex        =   78
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   12
         Left            =   240
         TabIndex        =   77
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 3"
      Height          =   3495
      Index           =   2
      Left            =   7320
      TabIndex        =   41
      Top             =   120
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   2
         Left            =   2160
         TabIndex        =   245
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   2
         Left            =   240
         TabIndex        =   56
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   2
         Left            =   1680
         TabIndex        =   55
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   2
         Left            =   2160
         TabIndex        =   54
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   2
         Left            =   2160
         TabIndex        =   53
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   2
         Left            =   840
         TabIndex        =   52
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   2
         Left            =   1440
         TabIndex        =   51
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   50
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   49
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   48
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   47
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   46
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   45
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   2
         Left            =   240
         TabIndex        =   44
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   2
         Left            =   2400
         TabIndex        =   43
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   2
         Left            =   2400
         TabIndex        =   42
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   35
         Left            =   2160
         TabIndex        =   246
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   11
         Left            =   2160
         TabIndex        =   60
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   10
         Left            =   2160
         TabIndex        =   59
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   9
         Left            =   360
         TabIndex        =   58
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   8
         Left            =   240
         TabIndex        =   57
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 2"
      Height          =   3495
      Index           =   1
      Left            =   3720
      TabIndex        =   21
      Top             =   120
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   1
         Left            =   2160
         TabIndex        =   243
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   1
         Left            =   240
         TabIndex        =   36
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   1
         Left            =   1680
         TabIndex        =   35
         Top             =   1080
         Width           =   255
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   1
         Left            =   2160
         TabIndex        =   34
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   1
         Left            =   2160
         TabIndex        =   33
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   1
         Left            =   840
         TabIndex        =   32
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   1
         Left            =   1440
         TabIndex        =   31
         Top             =   360
         Width           =   495
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   30
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   29
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   28
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   27
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   26
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   25
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   1
         Left            =   240
         TabIndex        =   24
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   1
         Left            =   2400
         TabIndex        =   23
         Top             =   2880
         Width           =   975
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   1
         Left            =   2400
         TabIndex        =   22
         Top             =   2280
         Width           =   975
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   34
         Left            =   2160
         TabIndex        =   244
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   7
         Left            =   2160
         TabIndex        =   40
         Top             =   240
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   6
         Left            =   2160
         TabIndex        =   39
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   5
         Left            =   360
         TabIndex        =   38
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   4
         Left            =   240
         TabIndex        =   37
         Top             =   1440
         Width           =   615
      End
   End
   Begin VB.CommandButton cmdExit 
      Caption         =   "Exit"
      Height          =   495
      Left            =   13320
      TabIndex        =   3
      Top             =   9600
      Width           =   1215
   End
   Begin VB.Frame Frame1 
      Caption         =   "Relay 1"
      Height          =   3495
      Index           =   0
      Left            =   120
      TabIndex        =   0
      Top             =   120
      Width           =   3495
      Begin VB.TextBox txtSubZone 
         Height          =   285
         Index           =   0
         Left            =   2160
         TabIndex        =   241
         Text            =   "0"
         Top             =   1680
         Width           =   1215
      End
      Begin VB.CommandButton cmdRead 
         Caption         =   "Read"
         Height          =   495
         Index           =   0
         Left            =   2400
         TabIndex        =   20
         Top             =   2280
         Width           =   975
      End
      Begin VB.CommandButton btnWrite 
         Caption         =   "Write"
         Height          =   495
         Index           =   0
         Left            =   2400
         TabIndex        =   19
         Top             =   2880
         Width           =   975
      End
      Begin VB.CheckBox chkFlagRelayEnabled 
         Caption         =   "Relay Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   18
         Top             =   3120
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagProtectionEnable 
         Caption         =   "Protection Timer Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   17
         Top             =   2880
         Width           =   2055
      End
      Begin VB.CheckBox chkFlagStopEvent 
         Caption         =   "STOP Event Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   16
         Top             =   2640
         Width           =   1815
      End
      Begin VB.CheckBox chkFlagStartEvent 
         Caption         =   "START Event Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   15
         Top             =   2400
         Width           =   1935
      End
      Begin VB.CheckBox chkFlagOFFEvent 
         Caption         =   "OFF Event Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   14
         Top             =   2160
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagONEvent 
         Caption         =   "ON Event Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   13
         Top             =   1920
         Width           =   1695
      End
      Begin VB.CheckBox chkFlagAlarmEvent 
         Caption         =   "Alarm Event Enable"
         Height          =   255
         Index           =   0
         Left            =   240
         TabIndex        =   12
         Top             =   1680
         Width           =   1935
      End
      Begin VB.CommandButton cmdRelayPulse 
         Caption         =   "2"
         Height          =   495
         Index           =   0
         Left            =   1440
         TabIndex        =   9
         Top             =   360
         Width           =   495
      End
      Begin VB.CommandButton cmdRelayOn 
         Caption         =   "1"
         Height          =   495
         Index           =   0
         Left            =   840
         TabIndex        =   8
         Top             =   360
         Width           =   495
      End
      Begin VB.TextBox txtPulseTime 
         Height          =   285
         Index           =   0
         Left            =   2160
         TabIndex        =   6
         Text            =   "0"
         Top             =   1080
         Width           =   1215
      End
      Begin VB.TextBox txtProtectionTime 
         Height          =   285
         Index           =   0
         Left            =   2160
         TabIndex        =   4
         Text            =   "0"
         Top             =   480
         Width           =   1215
      End
      Begin VB.CheckBox chkRelayStatus 
         Height          =   255
         Index           =   0
         Left            =   1680
         TabIndex        =   2
         Top             =   1080
         Width           =   255
      End
      Begin VB.CommandButton cmdRelayOff 
         Caption         =   "0"
         Height          =   495
         Index           =   0
         Left            =   240
         TabIndex        =   1
         Top             =   360
         Width           =   495
      End
      Begin VB.Label Label1 
         Caption         =   "Subzone"
         Height          =   255
         Index           =   32
         Left            =   2160
         TabIndex        =   242
         Top             =   1440
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Flags"
         Height          =   255
         Index           =   3
         Left            =   240
         TabIndex        =   11
         Top             =   1440
         Width           =   615
      End
      Begin VB.Label Label1 
         Alignment       =   1  'Right Justify
         Caption         =   "Status :"
         Height          =   255
         Index           =   0
         Left            =   360
         TabIndex        =   10
         Top             =   1080
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Pulse time"
         Height          =   255
         Index           =   2
         Left            =   2160
         TabIndex        =   7
         Top             =   840
         Width           =   1215
      End
      Begin VB.Label Label1 
         Caption         =   "Protection time"
         Height          =   255
         Index           =   1
         Left            =   2160
         TabIndex        =   5
         Top             =   240
         Width           =   1215
      End
   End
   Begin CANALOCXLib.Canal Canal1 
      Left            =   12480
      Top             =   9600
      _Version        =   65536
      _ExtentX        =   1296
      _ExtentY        =   1085
      _StockProps     =   0
   End
   Begin VB.Label Label15 
      Caption         =   "This appp talks to CAN through the CANAL daemon so this app must be started and at least one CAN driver added."
      Height          =   855
      Left            =   10800
      TabIndex        =   261
      Top             =   9240
      Width           =   2415
   End
   Begin VB.Label Label14 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H0080FF80&
      Caption         =   "oid"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   2040
      TabIndex        =   230
      Top             =   7320
      Width           =   495
   End
   Begin VB.Label Label13 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H0080FF80&
      Caption         =   "flags"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   2640
      TabIndex        =   229
      Top             =   7320
      Width           =   495
   End
   Begin VB.Label Label12 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H0080FF80&
      Caption         =   "type"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   4440
      TabIndex        =   228
      Top             =   7320
      Width           =   1095
   End
   Begin VB.Label Label11 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H0080FF80&
      Caption         =   "class "
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   3240
      TabIndex        =   227
      Top             =   7320
      Width           =   1095
   End
   Begin VB.Label Label10 
      Alignment       =   2  'Center
      Appearance      =   0  'Flat
      BackColor       =   &H0080FF80&
      Caption         =   "action"
      ForeColor       =   &H80000008&
      Height          =   255
      Left            =   5640
      TabIndex        =   226
      Top             =   7320
      Width           =   1815
   End
   Begin VB.Label Label9 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 7"
      Height          =   255
      Left            =   240
      TabIndex        =   225
      Top             =   9720
      Width           =   1695
   End
   Begin VB.Label Label8 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 6"
      Height          =   255
      Left            =   240
      TabIndex        =   224
      Top             =   9360
      Width           =   1695
   End
   Begin VB.Label Label7 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 5"
      Height          =   255
      Left            =   240
      TabIndex        =   223
      Top             =   9000
      Width           =   1695
   End
   Begin VB.Label Label6 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 4"
      Height          =   255
      Left            =   240
      TabIndex        =   222
      Top             =   8640
      Width           =   1695
   End
   Begin VB.Label Label5 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 3"
      Height          =   255
      Left            =   240
      TabIndex        =   221
      Top             =   8280
      Width           =   1695
   End
   Begin VB.Label Label4 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 2"
      Height          =   255
      Left            =   240
      TabIndex        =   220
      Top             =   7920
      Width           =   1695
   End
   Begin VB.Label Label3 
      Alignment       =   1  'Right Justify
      Caption         =   "Decision Matrix Row 1"
      Height          =   255
      Left            =   240
      TabIndex        =   219
      Top             =   7560
      Width           =   1695
   End
   Begin VB.Label Label2 
      Caption         =   "Nickname"
      Height          =   255
      Left            =   12720
      TabIndex        =   162
      Top             =   7440
      Width           =   735
   End
End
Attribute VB_Name = "frmSmartRelay"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = False
Attribute VB_PredeclaredId = True
Attribute VB_Exposed = False
Dim gSeconds As Long
Dim gDMrows As Byte
Dim gDMoffset As Byte





' Read the decision matrix content
Private Sub cmdReadDM_Click()
    Screen.MousePointer = vbHourglass
    If (-1 <> GetDmInfo()) Then
        For i = 0 To gDMrows - 1
            For j = 0 To 7
                regval = ReadRegister(gDMoffset + 8 * i + j)
                If (regval <> -1) Then
                    Select Case j
                        Case 0
                            txtDMoid(i) = regval
                            
                        Case 1
                            txtDMflags(i) = regval
                            
                        Case 2
                            txtDMclassmask(i) = regval
                            
                        Case 3
                            txtDMclassfilter(i) = regval
                            
                        Case 4
                            txtDMtypemask(i) = regval
                            
                        Case 5
                            txtDMtypefilter(i) = regval
                            
                        Case 6
                            If (regval <= 5) Then
                                ComboAction(i).ListIndex = regval
                            Else
                                ComboAction(i).ListIndex = 0
                            End If
                            
                        Case 7
                            txtActionParameter(i) = regval
                            
                    End Select
                End If
            Next j
        Next i
    End If
    Screen.MousePointer = vbNormal
End Sub

Private Sub cmdWriteMatrix_Click()
    Screen.MousePointer = vbHourglass
    If (-1 <> GetDmInfo()) Then
        For i = 0 To gDMrows - 1
            For j = 0 To 7
                Select Case j
                    Case 0
                        regval = Val(txtDMoid(i))
                            
                    Case 1
                        regval = Val(txtDMflags(i))
                            
                    Case 2
                        regval = Val(txtDMclassmask(i))
                            
                    Case 3
                        regval = Val(txtDMclassfilter(i))
                            
                    Case 4
                        regval = Val(txtDMtypemask(i))
                            
                    Case 5
                        regval = Val(txtDMtypefilter(i))
                            
                    Case 6
                        If (regval <= 5) Then
                            regval = ComboAction(i).ListIndex
                        Else
                            regval = 0
                        End If
                            
                    Case 7
                        regval = Val(txtActionParameter(i))
                            
                End Select
                
                Canal1.MsgId = &HB00
                Canal1.bMsgExtendedId = True
                Canal1.MsgSizeData = 3
                Canal1.MsgData(0) = Val(txtNickname)
                Canal1.MsgData(1) = gDMoffset + 8 * i + j
                Canal1.MsgData(2) = regval
                Canal1.Send

            Next j
        Next i
    End If
    Screen.MousePointer = vbNormal
End Sub

Private Sub Form_Load()
  
'Canal1.DeviceDllPath = "D:/dev/can/can/src/canal/drivers/peak/win32/Release/peakdrv.dll"
'Canal1.Open "CANUSB;500", 1
Canal1.DeviceDllPath = ""
Canal1.Open "", 0

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = &H2
Canal1.MsgData(1) = &H9
Canal1.MsgData(2) = &H1
Canal1.Send

Canal1.MsgData(2) = &H0
Canal1.Send

Canal1.MsgData(2) = &H1
Canal1.Send

End Sub

Private Sub Form_Unload(Cancel As Integer)
Canal1.Close
End Sub

Private Sub cmdExit_Click()
    End
End Sub
'
' Write Zone
'
Private Sub btnSetZone_Click()
    Dim zone As Byte
    
    zone = Val(txtZone)
    
    Canal1.MsgId = &HB00
    Canal1.bMsgExtendedId = True
    Canal1.MsgSizeData = 3
    Canal1.MsgData(0) = Val(txtNickname)
    Canal1.MsgData(1) = 66
    Canal1.MsgData(2) = zone
    Canal1.Send
End Sub
'
' Read Zone
'
Private Sub btnGetZone_Click()
    Dim zone As Byte
    Dim regvalue As Byte
    
    ' Subzone
    zone = 0
    regvalue = ReadRegister(66)
    If (regvalue <> -1) Then
        zone = regvalue
    End If
    txtZone = Str(zone)
End Sub
'
' READ
'
Private Sub cmdRead_Click(Index As Integer)
Dim regvalue As Integer
Dim pulsetimer As Long
Dim protecttimer As Long
Dim subzone As Byte
Dim flags As Byte

Screen.MousePointer = vbHourglass

regvalue = ReadRegister(2 + Index)
If (regvalue <> -1) Then
    chkRelayStatus(Index).Value = regvalue
End If

' Pulse Timer
pulsetimer = 0
regvalue = ReadRegister(18 + 2 * Index)
If (regvalue <> -1) Then
    pulsetimer = regvalue * 256
End If
regvalue = ReadRegister(19 + 2 * Index)
If (regvalue <> -1) Then
    pulsetimer = pulsetimer + regvalue
End If
txtPulseTime(Index) = Str(pulsetimer)

' Protect Timer
protecttimer = 0
regvalue = ReadRegister(34 + 2 * Index)
If (regvalue <> -1) Then
    protecttimer = regvalue * 256
End If
regvalue = ReadRegister(35 + 2 * Index)
If (regvalue <> -1) Then
    protecttimer = protecttimer + regvalue
End If
txtProtectionTime(Index) = Str(protecttimer)

' Subzone
subzone = 0
regvalue = ReadRegister(50 + Index)
If (regvalue <> -1) Then
    subzone = regvalue
End If
txtSubZone(Index) = Str(subzone)

' Flags
flags = 0
regvalue = ReadRegister(10 + Index)
If (regvalue <> -1) Then
    flags = regvalue
End If

If (flags And 1) Then
Else
End If

If (flags And 2) Then
    chkFlagAlarmEvent(Index).Value = 1
Else
    chkFlagAlarmEvent(Index).Value = 0
End If

If (flags And 4) Then
    chkFlagProtectionEnable(Index).Value = 1
Else
    chkFlagProtectionEnable(Index).Value = 0
End If

If (flags And 8) Then
    chkFlagONEvent(Index).Value = 1
Else
    chkFlagONEvent(Index).Value = 0
End If

If (flags And 16) Then
    chkFlagOFFEvent(Index).Value = 1
Else
    chkFlagOFFEvent(Index).Value = 0
End If

If (flags And 32) Then
    chkFlagStartEvent(Index).Value = 1
Else
    chkFlagStartEvent(Index).Value = 0
End If

If (flags And 64) Then
    chkFlagStopEvent(Index).Value = 1
Else
    chkFlagStopEvent(Index).Value = 0
End If

If (flags And 128) Then
    chkFlagRelayEnabled(Index).Value = 1
Else
    chkFlagRelayEnabled(Index).Value = 0
End If

Screen.MousePointer = vbNormal
End Sub

'
' WRITE
'
Private Sub btnWrite_Click(Index As Integer)

Dim regvalue As Integer
Dim pulsetimer As Long
Dim protecttimer As Long
Dim subzone As Byte
Dim flags As Byte

pulsetimer = Val(txtPulseTime(Index))
protecttimer = Val(txtProtectionTime(Index))
subzone = Val(txtSubZone(Index))
flags = 0

If (chkFlagAlarmEvent(Index).Value = 1) Then
    flags = flags + 2
End If

If (chkFlagProtectionEnable(Index).Value = 1) Then
    flags = flags + 4
End If

If (chkFlagONEvent(Index).Value = 1) Then
    flags = flags + 8
End If

If (chkFlagOFFEvent(Index).Value = 1) Then
    flags = flags + 16
End If

If (chkFlagStartEvent(Index).Value = 1) Then
    flags = flags + 32
End If

If (chkFlagStopEvent(Index).Value = 1) Then
    flags = flags + 64
End If

If (chkFlagRelayEnabled(Index).Value = 1) Then
    flags = flags + 128
End If

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)
Canal1.MsgData(1) = 10 + Index  ' flags
Canal1.MsgData(2) = flags
Canal1.Send

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)
Canal1.MsgData(1) = 18 + 2 * Index
Canal1.MsgData(2) = (pulsetimer / 256)
Canal1.Send

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)
Canal1.MsgData(1) = 19 + 2 * Index
Canal1.MsgData(2) = (pulsetimer And 255)
Canal1.Send

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)
Canal1.MsgData(1) = 34 + 2 * Index
Canal1.MsgData(2) = (protecttimer / 256)
Canal1.Send

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)
Canal1.MsgData(1) = 35 + 2 * Index
Canal1.MsgData(2) = (protecttimer And 255)
Canal1.Send

Canal1.MsgId = &HB00
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)
Canal1.MsgData(1) = 50 + Index
Canal1.MsgData(2) = subzone
Canal1.Send

cmdRead_Click (Index)
End Sub

Private Sub cmdRelayOff_Click(Index As Integer)
' Screen.MousePointer = vbHourglass
Canal1.MsgId = &HB00    ' Write
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname)   ' Node Nickname
Canal1.MsgData(1) = 2 + Index ' Relay status register
Canal1.MsgData(2) = &H0
Canal1.Send
End Sub

Private Sub cmdRelayOn_Click(Index As Integer)
Canal1.MsgId = &HB00    ' Write
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname) ' Node Nickname
Canal1.MsgData(1) = 2 + Index ' Relay status register
Canal1.MsgData(2) = &H1
Canal1.Send
End Sub

Private Sub cmdRelayPulse_Click(Index As Integer)
Canal1.MsgId = &HB00    ' Write
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 3
Canal1.MsgData(0) = Val(txtNickname) ' Node Nickname
Canal1.MsgData(1) = 2 + Index ' Relay status register
Canal1.MsgData(2) = &H2
Canal1.Send
End Sub



Private Sub Timer1_Timer()
    gSeconds = gSeconds + 1
End Sub

Private Sub EmptyInQueue()
    Dim count As Long
    
    count = Canal1.DataAvailable
    For i = 1 To count
        Canal1.Receive
    Next
    
End Sub


Private Function ReadRegister(register As Byte) As Integer

' Empty the input queue
EmptyInQueue

Canal1.MsgId = &H900                    ' Read
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 2
Canal1.MsgData(0) = Val(txtNickname)    ' Node Nickname
Canal1.MsgData(1) = register            ' Relay status register
Canal1.Send

gSeconds = 0
While gSeconds < 3
    If (Canal1.Receive) Then
        If Canal1.bMsgExtendedId Then
            If ((Canal1.MsgId And 65535) = (2560 + Val(txtNickname))) Then
                If (Canal1.MsgData(0) = register) Then
                    ReadRegister = Canal1.MsgData(1)
                    Exit Function
                End If
            End If
        End If
    End If
    DoEvents
Wend

ReadRegister = -1

End Function


' Get Decision Matrix Info
Private Function GetDmInfo() As Integer

' Empty the input queue
EmptyInQueue

Canal1.MsgId = &H2000                    ' Get decision matrix info
Canal1.bMsgExtendedId = True
Canal1.MsgSizeData = 1
Canal1.MsgData(0) = Val(txtNickname)    ' Node Nickname
Canal1.Send

gSeconds = 0
While gSeconds < 3
    If (Canal1.Receive) Then
        If Canal1.bMsgExtendedId Then
            If ((Canal1.MsgId And 65535) = (8448 + Val(txtNickname))) Then ' 0x2100 + nickname
                GetDmInfo = 0
                gDMrows = Canal1.MsgData(0)
                gDMoffset = Canal1.MsgData(1)
                Exit Function
            End If
        End If
    End If
    DoEvents
Wend

GetDmInfo = -1

End Function
