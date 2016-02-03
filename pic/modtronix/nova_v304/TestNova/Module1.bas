Attribute VB_Name = "Module1"
Option Explicit

' Globals
Public bConnected As Boolean            ' True when connected to VSCP server
Public bDataAvailable As Boolean        ' True when TCP data is available
Public systemtick As Long               ' Holds 200 ms tick count from timer event
Public connectionState As Integer       ' state machine state for connections
Public connectionSubState As Integer    ' sub state machine state for connected state
Public incoming_events() As VSCPEvent   ' Incoming event array


' constants
Public Const CONNECTION_STATE_NONE = 0
Public Const CONNECTION_STATE_WAIT_CONNECT = 1
Public Const CONNECTION_STATE_USERNAME = 2
Public Const CONNECTION_STATE_PASSWORD = 3
Public Const CONNECTION_STATE_CONNECTED = 4
Public Const CONNECTION_STATE_QUIT = 5

Public Const CONNECTION_SUB_STATE_NONE = 0
Public Const CONNECTION_SUB_STATE_COMMAND_CDTA = 1
Public Const CONNECTION_SUB_STATE_COMMAND_RETR = 2


Sub Main()
    bConnected = False
    bDataAvailable = False
    systemtick = 0
    connectionState = CONNECTION_STATE_NONE
    connectionSubState = CONNECTION_SUB_STATE_NONE
    
    ' Show mainform
    frmMain.Show
End Sub



'
' This is the receive routine for heart beats that fill the
' GUID listbox on the main form.
' (uses direct socket programing)
'
Sub receiveEvent(str As String)

    Dim obj As VSCPEvent
    Dim strGUID As String
    Dim wrkstr As String
    Dim buf As String
    Dim cnt As Integer
    Dim i As Integer
    Dim bFound As Boolean

    'vscp_input_buffer.Append buf
    'staticCntMsg.Text = str((UBound(vscp_input_buffer) + 1))
        
    Set obj = New VSCPEvent

    If (True = obj.createEventFromString(str)) Then
        If obj.vscp_class = 20 Then
            If obj.vscp_type = 9 Then
                
                strGUID = obj.getStringGUID
                
                bFound = False
                
                ' Check if the GUID is already available
                For i = 0 To frmMain.ListGUID.ListCount - 1
                    If 0 = StrComp(frmMain.ListGUID.List(i), strGUID, vbTextCompare) Then
                        bFound = True
                    End If
                Next
                If Not bFound Then
                    frmMain.ListGUID.AddItem strGUID
                End If
            End If
        End If
        'incoming_events.Append obj
    End If
 
End Sub

'
' Level II Read (using the vscptcpctrl)
'
Function ReadRegisterMany(frm As Form, reg0 As Byte, reg1 As Byte, reg2 As Byte, reg3 As Byte, cnt As Integer) As Boolean
    Dim i As Integer
    Dim j As Integer
    
    For j = 0 To 3
    
        ReadRegisterMany = False
    
        frm.Vscptcpctrl1.head = 0
        frm.Vscptcpctrl1.vscpclass = 1024
        frm.Vscptcpctrl1.vscptype = 1
        frm.Vscptcpctrl1.cntData = 26
        
        ' Register to read
        frm.Vscptcpctrl1.data(0) = reg0
        frm.Vscptcpctrl1.data(1) = reg1
        frm.Vscptcpctrl1.data(2) = reg2
        frm.Vscptcpctrl1.data(3) = reg3
        
        ' # positions to read
        frm.Vscptcpctrl1.data(4) = ((cnt / 256) And 255)
        frm.Vscptcpctrl1.data(5) = (cnt And 255)
        
        frm.Vscptcpctrl1.SetGUIDFromString frm.editGUID.Text
        For i = 0 To 15
            frm.Vscptcpctrl1.data(8 + i) = frm.Vscptcpctrl1.GUID(i)
        Next
    
        ' Clear the queue
        frm.Vscptcpctrl1.ClearInputQueue
        
        If (frm.Vscptcpctrl1.sendEventEx()) Then
        
            frm.Vscptcpctrl1.WaitForInput 2
    
            While frm.Vscptcpctrl1.getEvent()
            
                If (1024 = frm.Vscptcpctrl1.vscpclass And 3 = frm.Vscptcpctrl1.vscptype) Then
                    ReadRegisterMany = True
                    Exit Function
                End If
                
            Wend
            
        End If
    
    Next
    
End Function
'
' Level I Read (using the vscptcpctrl)
'
Function ReadRegister(frm As Form, reg As Byte, content As Byte) As Boolean
    
    Dim i As Integer
    Dim j As Integer
    
    For j = 0 To 3

        ReadRegister = False
    
        frm.Vscptcpctrl1.head = 0
        frm.Vscptcpctrl1.vscpclass = 0
        frm.Vscptcpctrl1.vscptype = 9
        frm.Vscptcpctrl1.cntData = 17
        
        frm.Vscptcpctrl1.SetGUIDFromString frm.editGUID.Text
        For i = 0 To 15
            frm.Vscptcpctrl1.data(i) = frm.Vscptcpctrl1.GUID(i)
        Next
    
        ' Set register to read
        frm.Vscptcpctrl1.data(16) = reg
    
        ' Clear the queue
        frm.Vscptcpctrl1.ClearInputQueue
    
        If (frm.Vscptcpctrl1.sendEventEx()) Then
        
            frm.Vscptcpctrl1.WaitForInput 2
    
            While frm.Vscptcpctrl1.getEvent()
            
                If (0 = frm.Vscptcpctrl1.vscpclass And 10 = frm.Vscptcpctrl1.vscptype) Then
                    ReadRegister = True
                    content = frm.Vscptcpctrl1.data(1)
                    Exit Function
                End If
                
            Wend
            
        End If
    
    Next
        
End Function

'
' Level I Write (using the vscptcpctrl)
'
Function WriteRegister(frm As Form, reg As Byte, value As Byte, retval As Byte) As Boolean
    
    Dim i As Integer
    Dim j As Integer
    
    On Error Resume Next
    
    For j = 0 To 3

        WriteRegister = False
    
        frm.Vscptcpctrl1.head = 0
        frm.Vscptcpctrl1.vscpclass = 0
        frm.Vscptcpctrl1.vscptype = 11
        frm.Vscptcpctrl1.cntData = 18
        
        frm.Vscptcpctrl1.SetGUIDFromString frm.editGUID.Text
        For i = 0 To 15
            frm.Vscptcpctrl1.data(i) = frm.Vscptcpctrl1.GUID(i)
        Next
    
        ' Set register to write
        frm.Vscptcpctrl1.data(16) = reg
        
        ' Set value to write
        frm.Vscptcpctrl1.data(17) = value
    
        ' Clear the queue
        frm.Vscptcpctrl1.ClearInputQueue
    
        If (frm.Vscptcpctrl1.sendEventEx()) Then
        
            frm.Vscptcpctrl1.WaitForInput 2
    
            While frm.Vscptcpctrl1.getEvent()
            
              If (0 = frm.Vscptcpctrl1.vscpclass And 10 = frm.Vscptcpctrl1.vscptype) Then
                    retval = frm.Vscptcpctrl1.data(1)
                    WriteRegister = True
                    Exit Function
                End If
                
            Wend
            
        End If
    
    Next
        
End Function

'
' Level II Write (using the vscptcpctrl)
'
Function WriteRegister2(frm As Form, reg As Long, value As Byte, retval As Byte) As Boolean
    
    Dim i As Integer
    Dim j As Integer
    
    'On Error Resume Next
    
    For j = 0 To 3

        WriteRegister2 = False
    
        frm.Vscptcpctrl1.head = 0
        frm.Vscptcpctrl1.vscpclass = 1024
        frm.Vscptcpctrl1.vscptype = 2
        frm.Vscptcpctrl1.cntData = 25
        
        ' Set register to write
        frm.Vscptcpctrl1.data(0) = 0
        frm.Vscptcpctrl1.data(1) = 0
        frm.Vscptcpctrl1.data(2) = Int(reg / 256)
        frm.Vscptcpctrl1.data(3) = Int(reg And 255)
        
        frm.Vscptcpctrl1.data(4) = 0
        frm.Vscptcpctrl1.data(5) = 0
        frm.Vscptcpctrl1.data(6) = 0
        frm.Vscptcpctrl1.data(7) = 0
        
        frm.Vscptcpctrl1.SetGUIDFromString frm.editGUID.Text
        For i = 0 To 15
            frm.Vscptcpctrl1.data(8 + i) = frm.Vscptcpctrl1.GUID(i)
        Next
    
        ' Set value to write
        frm.Vscptcpctrl1.data(24) = value
    
        ' Clear the queue
        frm.Vscptcpctrl1.ClearInputQueue
    
        If (frm.Vscptcpctrl1.sendEventEx()) Then
        
            frm.Vscptcpctrl1.WaitForInput 2
    
            While frm.Vscptcpctrl1.getEvent()
            
                If (1024 = frm.Vscptcpctrl1.vscpclass And 3 = frm.Vscptcpctrl1.vscptype) Then
                    WriteRegister2 = True
                    Exit Function
                End If
                
            Wend
            
        End If
    
    Next
        
End Function

Sub setCheckBoxBits(ctrl As Object, data As Byte)

    If (data And 1) Then
        ctrl(7).value = 1
    Else
        ctrl(7).value = 0
    End If
        
    If (data And 2) Then
        ctrl(6).value = 1
    Else
        ctrl(6).value = 0
    End If
        
    If (data And 4) Then
        ctrl(5).value = 1
    Else
        ctrl(5).value = 0
    End If
        
    If (data And 8) Then
        ctrl(4).value = 1
    Else
        ctrl(4).value = 0
    End If
        
    If (data And 16) Then
        ctrl(3).value = 1
    Else
        ctrl(3).value = 0
    End If
        
    If (data And 32) Then
         ctrl(2).value = 1
    Else
        ctrl(2).value = 0
    End If
        
    If (data And 64) Then
        ctrl(1).value = 1
    Else
        ctrl(1).value = 0
    End If
        
    If (data And 128) Then
        ctrl(0).value = 1
    Else
        ctrl(0).value = 0
    End If
    
End Sub


Function GetCheckBoxBits(ctrl As Object) As Byte
    
    Dim rv As Byte
    
    rv = 0
    
    If (1 = ctrl(0).value) Then
        rv = rv + 128
    End If
    
    If (1 = ctrl(1).value) Then
        rv = rv + 64
    End If
    
    If (1 = ctrl(2).value) Then
        rv = rv + 32
    End If
    
    If (1 = ctrl(3).value) Then
        rv = rv + 16
    End If
    
    If (1 = ctrl(4).value) Then
        rv = rv + 8
    End If
    
    If (1 = ctrl(5).value) Then
        rv = rv + 4
    End If
    
    If (1 = ctrl(6).value) Then
        rv = rv + 2
    End If
    
    If (1 = ctrl(7).value) Then
        rv = rv + 1
    End If
    
    GetCheckBoxBits = rv

End Function
