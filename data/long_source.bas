Attribute ModuleName = "MyForm"
Attribute ProgID = "ca-fe-de-ad"
Option Explicit
Option Compare Text
Option Compare Binary
' first comment
Rem second comment
' long_source.bas

Dim g_logger0
Dim a_long As Long

Const str As String = "una stringa"
Const a_bool = True
Const an_object = Nothing

' declaration of global constants with no type
Const an_integer = 1234%, a_long = 1234&, a_hex_long = &Hcafedead&, an_oct_long = &01234&, an_integer2 = 1234
Const a_single_float = 1234!, a_double_float = 1234#, a_float = 2.8

' declaration of global variables with type
Dim g_logger As Long
Dim name As New Form

Type PatRecord
    name As String
    age As Integer
End Type

Enum PatTypes
    inpatient
    outpatient
End Enum

Global g_logger As Long, v1, XRes As New Object, ptr As Module.MyRec, g_active As Boolean

Const e As Single = 2.8, pi As Double = 3.14, u As Integer = -1

Private Const PI As Double = 3.1415
Private Declare Sub BeepVB Lib "kernel32.dll" Alias "Beep" (ByVal time As Long, ByVal xx As Single)
Private Declare Function BeepVB Lib "kernel32.dll" Alias "Beep" (ByVal time As Long, ByVal xx As Single) As Long
Const e As Double = 2.8, pi As Double = 3.14, u As Integer = -1
Global g_logger As Long, v1, XRes As Object, ptr As MyRec, g_active As Boolean
Private Declare Sub PFoo Lib "mylib.dll" Alias "PFoo" (ByVal val As Long)

Enum MyEnum1
    c1 = 0
    c2 = 1
End Enum

Public Type MyRecord1
    v1 As String
    v2 As Long
End Type

Public Event OnChange(ByVal Text As String)

Sub foo(Optional ByVal name As String = "pippo")
End Sub

Sub foo(ByVal name As String, ByRef val As Integer)
  Dim x As Long
  x = 4
End Sub

Private Function OneFunc(ByVal name As String, ByRef val As Integer) As Integer
End Function

Private Function NoParamFunc() As Object
End Function

Private Sub my_sub(ByRef str As String, ByVal valid As Boolean)
End Sub

Private Sub my_sub(ByRef str As String, ByVal valid As Boolean, Optional ByVal flag As Boolean = True)
    var1.func().pnt1.X = 34
End Sub

Private Sub my_sub(ByRef str As String, Optional ByVal valid As Boolean = false)
    ' This is comment line 1
    ' Comment line 2
    Set var1 = "  ciao\"

    Let var2 = 54.7

    var3 = Func("descr", 54.7)

    str = CStr(i)

    Dim var1 As String, var2 As Integer

    ReDim var1(15)

    Exit Function

    GoSub label1

    On Error Resume Next

label1:
    Call Foo(13)
    Foo "Sea", Nothing, False

    RaiseEvent OnChange("hi!")

    With obj
        ' we miss the dot notation for subroutines and functions
        'Call Module1.foo(True, .Name)
        Call foo(True, .Name)
    End With
End Sub

Sub Func(ByRef obj As Canvas)
    Dim i As Integer
    i = 23
    
    Dim pnt As Point3d
    pnt.x = 1.0
    pnt.y = 1.2
    ' we miss the dot notation for subroutines and functions
    'Call obj.Plot(pnt)
    
    While cond(34, i)
        Print str
        For i = 1 To 100 Step 2
            Dim str As String
            str = CStr(i) 
            Call Print(str)
        Next i
    Wend
End Sub
