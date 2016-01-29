VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "VSCPEvent"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
Attribute VB_Ext_KEY = "SavedWithClassBuilder6" ,"Yes"
Attribute VB_Ext_KEY = "Top_Level" ,"Yes"
Option Explicit

'local variable(s) to hold property value(s)
Private mvarhead As Byte 'local copy
Private mvarvscp_class As Integer 'local copy
Private mvarvscp_type As Integer 'local copy
Private mvarobid As Long 'local copy
Private mvartimestamp As Long 'local copy
Private mvarGUID(16) As Byte 'local copy
Private mvardata(512 - 25) As Byte 'local copy
Private mvarcntData As Integer 'local copy
Public Property Let cntData(ByVal vData As Integer)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.cntData = 5
    mvarcntData = vData
End Property


Public Property Get cntData() As Integer
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.cntData
    cntData = mvarcntData
End Property


Public Property Get data(idx As Integer) As Byte
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.data
    data = mvardata(idx)
End Property




Public Property Get GUID(idx As Integer) As Byte
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.GUID
    GUID = mvarGUID(idx)
End Property



Public Property Let timestamp(ByVal vData As Long)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.timestamp = 5
    mvartimestamp = vData
End Property


Public Property Get timestamp() As Long
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.timestamp
    timestamp = mvartimestamp
End Property



Public Property Let obid(ByVal vData As Long)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.obid = 5
    mvarobid = vData
End Property


Public Property Get obid() As Long
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.obid
    obid = mvarobid
End Property



Public Property Let vscp_type(ByVal vData As Integer)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.vscp_type = 5
    mvarvscp_type = vData
End Property


Public Property Get vscp_type() As Integer
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.vscp_type
    vscp_type = mvarvscp_type
End Property



Public Property Let vscp_class(ByVal vData As Integer)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.vscp_class = 5
    mvarvscp_class = vData
End Property


Public Property Get vscp_class() As Integer
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.vscp_class
    vscp_class = mvarvscp_class
End Property



Public Property Let head(ByVal vData As Byte)
'used when assigning a value to the property, on the left side of an assignment.
'Syntax: X.head = 5
    mvarhead = vData
End Property


Public Property Get head() As Byte
'used when retrieving value of a property, on the right side of an assignment.
'Syntax: Debug.Print X.head
    head = mvarhead
End Property


'
' Constructor
'
Private Sub Class_Initialize()
    mvarhead = 0
    mvarvscp_class = 0
    mvarvscp_type = 0
    mvarobid = 0
    mvartimestamp = 0
    mvarcntData = 0
End Sub
'
' Create object from VSCP TCP Server compilant string.
'
Public Function createEventFromString(buf As String) As Boolean
    
  Dim i As Integer
  Dim pos As Integer
  Dim guidstr As String
  
  On Error Resume Next
  
  ' head
  mvarhead = Val(buf)
  pos = InStr(buf, ",")
  buf = Right(buf, Len(buf) - pos)
  
  ' class
  mvarvscp_class = Val(buf)
  pos = InStr(buf, ",")
  buf = Right(buf, Len(buf) - pos)
  
  ' type
  mvarvscp_type = Val(buf)
  pos = InStr(buf, ",")
  buf = Right(buf, Len(buf) - pos)
  
  ' obid
  mvarobid = Val(buf)
  pos = InStr(buf, ",")
  buf = Right(buf, Len(buf) - pos)
  
  ' timestamp
  mvartimestamp = Val(buf)
  pos = InStr(buf, ",")
  buf = Right(buf, Len(buf) - pos)
  
  ' GUID
  pos = InStr(buf, ",")
  guidstr = Left(buf, pos - 1)
  buf = Right(buf, Len(buf) - pos)
  
  For i = 0 To 15
    mvarGUID(i) = Val(guidstr)
    pos = InStr(guidstr, ":")
    guidstr = Right(guidstr, Len(guidstr) - pos)
  Next
  
  mvarcntData = 0
  pos = InStr(buf, ",")
  While (0 <> pos)
    mvardata(mvarcntData) = Val(buf)
    buf = Right(buf, Len(buf) - pos)
    pos = InStr(buf, ",")
    mvarcntData = mvarcntData + 1
  Wend
  
 createEventFromString = True
 
End Function
'
' Fetch the VSCP event as a VSCP TCP server compilant string
'
Public Function getStr() As String
  
  Dim buf As String
  Dim i As Integer
  
  buf = str(mvarhead) + ","
  buf = buf + str(mvarvscp_class) + ","
  buf = buf + str(mvarvscp_type) + ","
  buf = buf + str(mvarobid) + ","
  buf = buf + str(mvartimestamp) + ","
  buf = buf + getStringGUID + ","
  For i = 0 To mvarcntData
    buf = buf + str(mvardata(i))
    If i <> mvarcntData Then
      buf = buf + ","
    End If
  Next
  
  getStr = buf

End Function
'
' Return GUID in string form
'
Public Function getStringGUID() As String
  
  Dim strGUID As String
  Dim i As Integer
  
  For i = 0 To 15
    
    strGUID = strGUID + LTrim(str(mvarGUID(i)))
    
    If i <> 15 Then
      strGUID = strGUID + ":"
    End If
    
  Next
  
  getStringGUID = strGUID
End Function
