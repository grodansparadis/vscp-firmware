VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "VSCPTCP"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Ext_KEY = "SavedWithClassBuilder6" ,"Yes"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"
Option Explicit

'local variable(s) to hold property value(s)
Private mvarbConnected As Boolean 'local copy

Public Property Let bConnected(ByVal vData As Byte)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.head = 5
    mvarbConnected = vData
End Property


Public Property Get bConnected() As Byte
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.head
    bConnected = mvarbConnected
End Property
