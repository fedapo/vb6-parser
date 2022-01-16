//

#include <csetjmp>
#include <iostream>

#define VB6_GOSUB(sub) if(!setjmp(_gs)){goto sub;}
#define VB6_RETURN longjmp(_gs,1)

using namespace std;

/*
Sub foo()
    GoSub mysub1

    Exit Sub
mysub1:
    Debug.Print "mysub1"
    Return

mysub2:
    Debug.Print "mysub2"
    Return
End Sub

#define GOSUB(sub) if(!setjmp(_gs)){goto sub;}
#define RETURN longjmp(_gs,1)
*/

void test_gosub(ostream& os)
{
  os << "---- begin " << __func__ << " ----\n";

  jmp_buf _gs;
  VB6_GOSUB(sub1);

  os << "  after calling subroutine 1\n";

  VB6_GOSUB(sub2);

  os << "  after calling subroutine 2\n";

  return;
sub1:
  os << "  in subroutine 1\n";
  VB6_RETURN;

sub2:
  os << "  in subroutine 2\n";
  VB6_RETURN;
}
