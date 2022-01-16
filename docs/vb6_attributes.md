# VB6 Attributes

## Class (.cls)

```vb
VERSION 1.0 CLASS
BEGIN
  MultiUse = -1  'True
  Persistable = 0  'NotPersistable
  DataBindingBehavior = 0  'vbNone
  DataSourceBehavior  = 0  'vbNone
  MTSTransactionMode  = 0  'NotAnMTSObject
END
Attribute VB_Name = "Interactive"
Attribute VB_GlobalNameSpace = False
Attribute VB_Creatable = True
Attribute VB_PredeclaredId = False
Attribute VB_Exposed = False
...
```

## Form (.frm), Control (.ctl), Property Page (.pag)

```vb
VERSION 5.00
Object = "{8DDE6232-1BB0-11D0-81C3-0080C7A2EF7D}#3.0#0"; "flp32a30.ocx"
Begin VB.Form frmCalendar
...
```

```vb
VERSION 5.00
Object = "{8DDE6232-1BB0-11D0-81C3-0080C7A2EF7D}#3.0#0"; "flp32a30.ocx"
Begin VB.UserControl ConfSelection
...
```

```vb
VERSION 5.00
Object = "{8DDE6232-1BB0-11D0-81C3-0080C7A2EF7D}#3.0#0"; "flp32a30.ocx"
Begin VB.PropertyPage PropertyPage1
...
```

## Module (.bas)

```vb
Attribute VB_Name = "CENTSERV"
...
```

## Attributes

https://christopherjmcclellan.wordpress.com/2015/04/21/vb-attributes-what-are-they-and-why-should-we-use-them/

### Module Level Attributes

```vb
VB_Name = "Interactive"
VB_GlobalNameSpace = False
VB_Creatable = True
VB_PredeclaredId = False
VB_Exposed = False
```

### Other Attributes

There are also a number of attributes that can be applied to module variables (fields), properties, and procedures.

| Attribute | Description |
| --------- | ----------- |
|VB_VarUserMemId|     Determines the order of the variables in the Object Broswer. A value of 0 (zero) declares the variable to be the default member of the class.|
|VB_VarDescription|   The value of this attribute will be displayed in the Object Broswer.|
|VB_UserMemId| |
VB_Description| |

```vb
LmoIEIDXRad.VB_VarHelpID = -1
Item.VB_UserMemId = 0
NewEnum.VB_UserMemId = -4
```

There is one more special value for `VB_UserMemId` and that value is -4.
Negative 4 always indicates that the function being marked should return
a [_NewEnum] enumerator.

```vb
' Header
Attribute VB_Name = "ClassOrModuleName"
Attribute VB_GlobalNameSpace = False ' ignored
Attribute VB_Creatable = False ' ignored
Attribute VB_PredeclaredId = False ' a Value of True creates a default global instance
Attribute VB_Exposed = True ' Controls how the class can be instanced.

' Module Scoped Variables
Attribute variableName.VB_VarUserMemId = 0 ' Zero indicates that this is the default member of the class.
Attribute variableName.VB_VarDescription = "some string" ' Adds the text to the Object Browser information for this variable.

' Procedures
Attribute procName.VB_Description = "some string" ' Adds the text to the Object Browser information for the procedure.
Attribute procName.VB_UserMemId = someInteger
    '  0: Makes the function the default member of the class.
    ' -4: Specifies that the function returns an Enumerator.
```

In a control.

```vb
Public Sub SpeechMarkBeg()
Attribute SpeechMarkBeg.VB_MemberFlags = "40"

Public Property Get MaxLength() As Single
Attribute MaxLength.VB_Description = "When the length of the dictation hits this number (in minutes) then the system stops recording and asks the user if he/she wants to proceed."
```
