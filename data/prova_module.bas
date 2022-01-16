Attribute VB_Name = "prova_module"
Option Explicit

Enum MyEnum1
    v0 = False ' this is ok only because it can be cast to an integer
    v1 = 1
    v2 = "2" ' this is ok only because it can be cast to an integer
    v3
End Enum

Type MyRec1
    f1 As Integer
    f2 As String
End Type

Type MyRec2
    f1 As Integer
    f2 As String
    f3 As MyRec1
End Type

Private myprop As String

Property Get prova_property() As String
    prova_property = myprop
End Property

Property Let prova_property(v As String) ' using "Let" because it is a built-in type?
    myprop = v
End Property

Sub prova_data_member_access()
    Debug.Print "== prova_data_member_access =="

    Dim obj1 As prova_module.MyRec1

    obj1.f1 = 45
    obj1.f2 = "prova"

    Dim obj2 As prova_module.MyRec2

    obj2.f1 = 1
    obj2.f2 = "xxx"
    obj2.f3.f1 = 2
    obj2.f3.f2 = "yyy"
End Sub

Function func1() As Integer
    Debug.Print "== func1 =="
    func1 = 45
End Function

Function func2() As prova_module.MyRec1
    Debug.Print "== func2 =="
    func2.f1 = 45
    func2.f2 = "prova"
End Function

Sub Main()
    prova_data_member_access

    Dim frm As prova_project.prova_form
    Set frm = New prova_form

    frm.id = 345

    VB.Global.Load frm.Command1(1) ' loads a form or control into memory

    frm.Command1(1).Caption = "Nuovo"
    frm.Command1(1).Top = 750
    frm.Command1(1).Left = 750
    frm.Command1(1).Width = 1000
    frm.Command1(1).Height = 400
    frm.Command1(1).Visible = True
    frm.Command1(1).Enabled = True

    'VB.Global.Load frm ' loads a form or control into memory

    frm.Show

    func1

    ' Compile Error: Expected Sub, Function, or Property
    'func2.f1
    'func2().f1

    Debug.Print func2.f1
    Debug.Print func2().f2

    Debug.Print MyEnum1.v0
    Debug.Print MyEnum1.v1
    Debug.Print MyEnum1.v2
    Debug.Print MyEnum1.v3

    prova_property = "xyxy"
    Debug.Print prova_property

    prova_form.Show
End Sub
