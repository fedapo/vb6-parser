# VB6 - Visual Basic 6

https://en.wikipedia.org/wiki/Visual_Basic_(classic) \
https://en.wikipedia.org/wiki/BASIC

BASIC = Beginner's All-purpose Symbolic Instruction Code

> At least for the people who send me mail about a new language that they're
> designing, the general advice is: do it to learn about how to write a compiler.
> Don't have any expectations that anyone will use it, unless you hook up with
> some sort of organization in a position to push it hard. It's a lottery, and
> some can buy a lot of the tickets. There are plenty of beautiful languages
> (more beautiful than C) that didn't catch on. But someone does win the lottery,
> and doing a language at least teaches you something. \
> _Dennis Ritchie (1941-2011)_ \
> _Creator of the C programming language and of Unix_

---
## Press

Visual Basic 6 Renewed to Run on Windows 8 \
https://www.infoq.com/news/2012/02/vb6_supported_on_win8

---
## Complete VB6 Grammars

ANTLR4-based Grammars

https://github.com/antlr/grammars-v4/tree/master/vb6

https://github.com/uwol/vb6parser \
https://github.com/uwol/vb6parser/blob/master/src/main/antlr4/io/proleap/vb6/VisualBasic6.g4

Grammar Zoo: http://slebok.github.io/zoo/index.html

http://boost.2283326.n4.nabble.com/Parser-operator-Difference-td4675788.html

---
## Expression Parsing

https://en.wikipedia.org/wiki/Operator-precedence_parser

---
## CodeProject Articles

Crafting an interpreter Part 1 - Parsing and Grammars \
https://www.codeproject.com/Articles/10115/Crafting-an-interpreter-Part-1-Parsing-and-Grammar \

Visual Basic 6.0: A giant more powerful than ever\
https://www.codeproject.com/Articles/710181/Visual-Basic-6-0-A-giant-more-powerful-than-ever

---
## Misc

**Grako** (grammar compiler) is a tool that takes grammars in a variation of EBNF as input, and outputs memoizing (Packrat) PEG parsers in Python. \
A generator of PEG/Packrat parsers from EBNF grammars. \
https://pypi.python.org/pypi/grako/

**TatSu** is a tool that takes grammars in a variation of EBNF as input, and outputs memoizing (Packrat) PEG parsers in Python. \
https://pypi.org/project/TatSu/

GOLD Parsing System \
http://goldparser.org/engine/1/vb6/index.htm \
http://goldparser.org/about/comparison-parsers.htm

http://www.eclipse.org/gmt/modisco/technologies/VisualBasic/#download

https://tomassetti.me/how-to-write-a-transpiler/

The vb2Py project is developing a suite of conversion tools to aid in translating Visual Basic projects into Python.\
http://vb2py.sourceforge.net/index.html

http://vb6awards.blogspot.com/2016/04/microsoft-update-or-open-source-vb6.html

---
## VB6 Named parameters

```vb
Function foo(Optional val1 = 1, Optional val2 = 2, Optional val3 = 3)
    MsgBox "val1: " & val1 & " val2: " & val2 & " val3: " & val3
    foo = val3
End Function

Private Sub Form_Load()
    MsgBox "foo returned: " & foo(val3:=4)
End Sub
```

## VB6 Dot notation (x.y)

Access to members of

- Modules
- Classes, forms, controls
- Types

Types of members

- Data members
- Member functions
- Member subroutines
- Properties

```
module.variable
module.function
module.subroutine
module.enum
module.type

object.variable
object.function
object.subroutine
object.property
```
