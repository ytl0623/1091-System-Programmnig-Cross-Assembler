# include <iostream>
# include <fstream>
# include <cstdio>
# include <stdio.h>
# include <cstdlib>
# include <stdlib.h>
# include <cstring>
# include <string.h>
# include <cmath>
# include <math.h>
# include <time.h>
# include <vector>
# include <stack>
# include <queue>
# include <iomanip>
# include <string.h>

# include "table.h"
# include "buffer.h"
# include "function.h"
# include "SIC.h"
# include "SICXE.h"
# include "assembler.h"
using namespace std ;

int main() {
	Function::SetTable() ;
	string command ;
  Assembler assembler ;

  while (1) {
    cout << "*****************************************" << endl ;
    cout << "***** Cross Assembler               *****" << endl ;
    cout << "***** 0 : QUIT                      *****" << endl ;
    cout << "***** 1 : SIC                       *****" << endl ;
    cout << "***** 2 : SIC/XE                    *****" << endl ;
    cout << "*****************************************" << endl ;
    cout << "Input a command(0, 1, 2): " ;
    cin >> command ;

    if ( command == "0" )
      return 0 ;

    else if ( command == "1" ) {
  		assembler.SIC_Pass1() ;
   		assembler.SIC_Pass2() ;
      cout << endl ;
		}
		else if ( command == "2" ) {
			assembler.SICXE_Pass1() ;
			assembler.SICXE_Pass2() ;
      cout << endl ;
		}
    else {
    	cout << endl << "Command does not exit!" << endl << endl ;
      fflush(stdin) ;
    }
  }
}