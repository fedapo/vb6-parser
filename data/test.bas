Attribute ModuleName = "MyForm"
Attribute ProgID = "a1-b2-c3-d4"
' test.bas
Option Explicit
' compare options
Option Compare Text
Option Compare Binary
' array base options
Option Base 0
Option Base 1

' test.bas

Global g_v1 As String, g_v2 As Integer
Public g_v3 As Long
Private g_v4 As Single

' we miss the declaration of arrays
'Dim arr0() As String
'Dim arr1(10) As Integer
'Dim arr2(10, 10) As Long

' what about this? 
'Dim values As Integer * 10

Enum Colors
    Black = 0
    White = 1
    Blue = 2
    Red = 3
    Green = 4
End Enum

Public Type Point3d
    x As Double
    y As Double
End Type

Sub Func(ByRef obj As Canvas)
    Dim i As Integer
    i = 23

    Dim pnt As Point3d
    pnt.x = 1.0
    pnt.y = 1.2
    ' we miss the dot notation for subroutines and functions
    'Call obj.Plot(pnt)

    If check(87) Then
      Print "check1"
    ElseIf check(92) Then
      Print "check2"
    Else
      Print "check3"
    End If

    While cond(34, i)
        Print str
        For i = 1 To 100 Step 2
            Dim str As String
            str = CStr(i)
            Call Print(str)
        Next i
    Wend
End Sub
