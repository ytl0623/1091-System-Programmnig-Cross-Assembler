using namespace std ;

#define WORDLENGTH 20

typedef int int2[2] ;     // table, entry
typedef char word[WORDLENGTH] ;
typedef char str100[100] ;
typedef bool type[4] ;     // format1-4
typedef word word10[10] ;

int gPlace ;
int gBase ;
bool use_base ;
enum ErrorType { XNUMBER = 34512, CSTRING = 87232, EMPTY = 29742, DONE = 65535 } ;

class TableA {
  private:
	  word table[100] ; // reserve [0], place data from [1]
	  int my_index ; // point to the vacancy

  public:
	  TableA() {
      Clean() ;
	  }

	  ~TableA() {
		  Clean() ;
	  }

    bool Add( word token ) {
      if ( strcmp( token, "" ) != 0 ) {
        int i = 0 ;

        while ( i < WORDLENGTH && token[i] != '\0' ) {
          table[my_index][i] = token[i] ;
          i++ ;
        }

        my_index++ ;
        return true ;
      }
      else {
        return false ;
      }
    }

    void Clean() {
      for ( int i = 0 ; i < 100 ; i++ ) {
        memset( table[i], '\0', WORDLENGTH );
      }

      my_index = 1 ;
    }

    bool Find( word temp, int &index ) {
      int i = 1 ;
      bool exist = false ;

      while ( !exist && i < my_index ) {
        if ( strcmp( temp, table[i] ) == 0 )
          exist = true ;
        else
          exist = false ;

        i++ ;
      }

      if (exist)
        index = i - 1 ;
      else
        index = 0 ;

      return exist ;
    }

    bool FindAllowCapital( word source, int &index ) {
      // for table 1&2

      int i = 1 ;
      bool exist = false ;
      bool diff = false ;
      char baseS = '\0' ;
      char baseT = '\0' ;

      while ( !exist && i < my_index ) {
        word temp = {'\0'} ;
        strcpy( temp, table[i] ) ;
        int j = 0 ;
        diff = false ; // assume that there is no difference

        while ( j < WORDLENGTH && !diff ) {
          int s = 0 ;
          int t = 0 ;

          if ( source[j] >= 'A' && source[j] <= 'Z' )
            baseS = 'A' ;
          else
            baseS = 'a' ;

          if ( table[i][j] >= 'A' && table[i][j] <= 'Z' )
            baseT = 'A' ;
          else
            baseT = 'a' ;

          if ( source[j] == '\0' )
            s = 0 ;
          else
            s = source[j] - baseS ;

          if ( temp[j] == '\0' )
            t = 0 ;
          else
            t = temp[j] - baseT ;

          if ( s != t )
            diff = true ;

          j++ ;
        }

        if ( !diff )
          exist = true ; // means that diff did not set to be true, j == WORDLENGTH
        
        i++ ;
      }

      if (exist)
        index = i - 1 ;
      else
        index = 0 ;

      return exist ;
    }
    
    void GetStr( int index, word str ) {
      strcpy( str, table[index] ) ;
    }
    
    int CmpWith( int index, int serial ) {
      word str ;
      memset( str, '\0', WORDLENGTH ) ;
      
      if ( serial == 0 ) {
        strcpy( str, "START" ) ;
      }
      else if ( serial == 1 ) {
        strcpy( str, "END" ) ;
      }
      else if ( serial == 2 ) {
        strcpy( str, "BYTE" ) ;
      }
      else if ( serial == 3 ) {
        strcpy( str, "WORD" ) ;
      }
      else if ( serial == 4 ) {
        strcpy( str, "RESB" ) ;
      }
      else if ( serial == 5 ) {
        strcpy( str, "RESW" ) ;
      }
      else if ( serial == 6 ) {
        strcpy( str, "EQU" ) ;
      }
      else if ( serial == 7 ) {
        strcpy( str, "BASE" ) ;
      }
      else if ( serial == 8 ) {
        strcpy( str, "LTORG" ) ;
      }
      else {
        strcpy( str, "ERROR" ) ;
      }

      return strcmp( table[index] , str ) ;
    }
    
    void Print() {
      int i = 0 ;
      while ( i < my_index ) {
        printf( "%s\n", table[i] ) ;
        i++ ;
      }
    }
    
};

class TableB {
  private:
	word table[100] ;

	int Hash( word token ) {
		int total = 0 ;
		for( int i = 0 ; i < WORDLENGTH && token[i] != '\0' ; i++ ) {
			total = total + token[i] ;
		}

		return total % 100 ;
	}

	bool isEmptyAt( int index ) {
		if ( table[index][0] == '\0' ) return true ;
		else return false ;
	}
	void TransferToCapital( word token, word output ) {     // 全轉大寫
		for( int i = 0 ; i < WORDLENGTH && token[i] != '\0' ; i++ ) {
		  if ( 'a' <= token[i] && token[i] <= 'z' ) {
		    output[i] = token[i] - 'a' + 'A' ;     // !!!!!
			}
			else {     // 大寫或數字直接照抄
				output[i] = token[i] ;
			}
		}
	}
  public:
  	TableB() {
        Clean() ;
	}
	~TableB() {
		Clean() ;
	}
	bool Add( word token ) {
		int i = Hash( token ) ;
		// if hash table might be full, use j = i
		// if ( !isEmptyAt( i ) ) j++
		// while ( !isEmptyAt( i ) && j != i ) {
		// if j is a vacancy, it will not be in the if/while part
		// if j is not available，get in the if part then plus one quikly, when while reach j = i
		//    means hash table is no longer available
		while ( !isEmptyAt( i ) ) {
	        i++ ;
	        i = i % 100 ; // go back 0, and keep doing
		}

		if ( strcmp( token, "" ) != 0 ) {
			int j = 0 ;
			while ( j < WORDLENGTH && token[j] != '\0' ) {
		    	table[i][j] = token[j] ;
				j++ ;
			}

		    return true ;
		}
		else{
			return false ;
		}
	}
  
	bool AddWithTransferToCapital( word token ) {
		word temp = {'\0'} ;
		TransferToCapital( token, temp ) ;
		return Add( temp ) ;
	}
	
  void Clean(){
		for( int i = 0 ; i < 100 ; i++ ) {
			memset( table[i], '\0', WORDLENGTH );
		}
	}
	bool Find( word temp, int &index ) {
		int i = Hash( temp ) ;
		bool exist = false ;
		while ( !exist && !isEmptyAt( i ) ) {
			// if collision, i+1, so stop finding if find a empty space (means there is no temp in the table)
			if ( strcmp( temp, table[i] ) == 0 ) exist = true ;
			i++ ;
			i = i % 100 ;
		}

		if ( exist ) {
			if ( i == 0 ) {
				index = 99 ;
			}
			else index = i - 1 ;
		}
		else index = -1 ;
		return exist ;
	}
	bool FindAllowCapital( word source, int &index ) {
	    word temp = {'\0'} ;
		TransferToCapital( source, temp ) ;
		return Find( temp, index ) ;
	}
	void GetAsHexNum( int index, int &num ) {
		num = 0 ;
		int temp ;
		for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
			num = num * 16 ;
			if ( table[index][i] >= 'A' ) {
				temp = table[index][i] - 'A' + 10 ;
			}
			else {
				temp = table[index][i] - '0' ;
			}
			num = num + temp ;
		}
	}
	void GetAsDecNum( int index, int &num ) {
		num = 0 ;
		int temp ;
		for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
			num = num * 10 ;
			temp = table[index][i] - '0' ;
			num = num + temp ;
		}
	}

	void GetAsHexNuml( int index, long long int &num ) {
		num = 0 ;
		int temp ;
		for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
			num = num * 16 ;
			if ( table[index][i] >= 'A' ) {
				temp = table[index][i] - 'A' + 10 ;
			}
			else {
				temp = table[index][i] - '0' ;
			}
			num = num + temp ;
		}
	}
	void GetAsDecNuml( int index, long long int &num ) {
		num = 0 ;
		int temp ;
		for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
			num = num * 10 ;
			temp = table[index][i] - '0' ;
			num = num + temp ;
		}
	}

	void GetString( int index, word output ) {
		strcpy( output, table[index] ) ;
	}
	void GetStrLength( int index, int &length ) {
		int i = 0 ;
		while ( i < WORDLENGTH && table[index][i] != '\0' ) i++ ;

		length = i ; // table[i] == '\0', not inded
	}
	void Print(){
		int i = 0 ;
		while ( i < 99 ) {
			printf( "%s\n", table[i] ) ;
			i++ ;
		}
	}
};

class TableI {
  private:
  	bool SIC[100] ;
    word table[100] ; // reserve [0], place data from [1]
    type types[100] ;
    str100 code[100] ;
    int my_index ; // point to the vacancy
  
  public:
    TableI() {
      Clean() ;
    }
    
    ~TableI() {
      Clean() ;
    }
    
	bool Type2_r2_minus1( int index ) {
		if ( strcmp( table[index], "shiftl" ) == 0 || strcmp( table[index], "shiftr" ) == 0 || strcmp( table[index], "SHIFTL" ) == 0 || strcmp( table[index], "SHIFTR" ) == 0 ) {
			return true ;
		}
		else return false ;
	}
	bool Add( char sic, word opcode, type format, str100 machinecode ) {
    if ( strcmp( opcode, "" ) != 0 ) {
      if ( sic == 'C' || sic == 'c' ) {
        SIC[my_index] = true ; // SIC
      }
      else {
      SIC[my_index] = false ; // SIC/XE
      }
    
      int i = 0 ;
      while ( i < WORDLENGTH && opcode[i] != '\0' ) {
        table[my_index][i] = opcode[i] ;
        i++ ;
      }

      // copy format into types
      for( int i = 0 ; i < 4 ; i++ )
        types[my_index][i] = format[i] ;

      code[my_index][0] = machinecode[0] ;
      code[my_index][1] = machinecode[1] ;

      my_index++ ;
      
      return true ;
		}
		else{
			return false ;
		}
	}

	void Clean(){
		for( int i = 0 ; i < 100 ; i++ ) {
			memset( table[i], '\0', WORDLENGTH );
			memset( types[i], false, 4 ) ;
			memset( code[i], '\0', 2 ) ;
		}

		my_index = 1 ;
	}
	bool Find( word temp, int& index ) {
		int i = 1 ;
		bool exist = false ;
		while ( !exist && i < my_index ) {
			if ( strcmp( temp, table[i] ) == 0 ) {
				exist = true ;
			}
			else exist = false ;
			i = i +1 ;
		}

		if ( exist ) {
			index = i - 1 ;
		}
		else{
		    index = 0 ;
		}
		return exist ;
	}
	bool FindAllowCapital( word source, int &index ) {
		// for table 1&2
		int i = 1 ;
		bool exist = false ;
		bool diff = false ;
		char baseS = '\0' ;
		char baseT = '\0' ;

		while ( !exist && i < my_index ) {
			word temp = { '\0' } ;
			strcpy( temp, table[i] ) ;
			int j = 0 ;
			diff = false ; // assume that there is no difference
			while ( j < WORDLENGTH && !diff ) {
				int s = 0 ;
				int t = 0 ;
				if ( source[j] >= 'A' && source[j] <= 'Z' ) baseS = 'A' ;
		        else baseS = 'a' ;
		        if ( table[i][j] >= 'A' && table[i][j] <= 'Z' ) baseT = 'A' ;
		        else baseT = 'a' ;

				if ( source[j] == '\0' ) s = 0 ;
				else s = source[j] - baseS ;
				if ( temp[j] == '\0' ) t = 0 ;
				else t = temp[j] - baseT ;
				if ( s != t ) diff = true ;

				j++ ;
			}

			if ( !diff ) exist = true ; // means that diff did not set to be true ， j == WORDLENGTH
			i++ ;
		}

		if ( exist ) {
			index = i - 1 ;
		}
		else index = 0 ;

		return exist ;
	}
	bool FindWithToken( int index ) {
		if ( ( index < my_index ) && strcmp( table[index], "" ) != 0 ) return true ;
		else return false ;
	}
	int GetOPCode( int index ) {
		int num = 0 ;
		int temp = 0  ;
		for( int i = 0 ; code[index][i] != '\0' && i < 2 ; i++ ) {
			num = num * 16 ;
			if ( code[index][i] >= 'A' && code[index][i] <= 'F' ) {
				temp = code[index][i] - 'A' + 10 ;
			}
			else {
				temp = code[index][i] - '0' ;
			}
			num = num + temp ;
		}
		return num ;
	}
	int SIC_CheckOP( int index ) {
		// just can be call when SIC
		if ( strcmp ( table[index], "rsub" ) == 0  ) return 0 ;
		else return 1 ;
	}
	bool CheckIfSIC( int index ) {
		if ( SIC[index] ) return true ;
		else return false ;
	}
	void GetType( int index, type return_types ) {
		return_types[0] = types[index][0] ;
		return_types[1] = types[index][1] ;
		return_types[2] = types[index][2] ;
		return_types[3] = types[index][3] ;
	}
	void Type2_GetCheckList( int index, int &op1type, int &op2type ) {
		// type 0 : n
		// type 1 : reg
		if ( strcmp( table[index], "svc" ) == 0 ) {
			// SVC n
			op1type = 0 ;  // n
			op2type = -1 ; // should not exists
		}
		else if ( strcmp( table[index], "clear" ) == 0  ) {
			// CLEAR r1
			op1type = 1 ;  // reg
			op2type = -1 ; // should not exists
		}
		else if ( strcmp( table[index], "tixr" ) == 0  ) {
			// TIXR r1
			op1type = 1 ;  // reg
			op2type = -1 ; // should not exists
		}
		else if ( strcmp( table[index], "shiftl" ) == 0  ) {
			// SHIFTL r1,n
			op1type = 1 ; // reg
			op2type = 0 ; // n
		}
		else if ( strcmp( table[index], "shiftr" ) == 0  ) {
			// SHIFTR r1,n
			op1type = 1 ; // reg
			op2type = 0 ; // n
		}
		else if ( strcmp( table[index], "addr" ) == 0  ) {
			// ADDR r1,r2
			op1type = 1 ; // reg
			op2type = 1 ; // reg
		}
		else if ( strcmp( table[index], "compr" ) == 0  ) {
			// COMPR r1,r2
			op1type = 1 ; // reg
			op2type = 1 ; // reg
		}
		else if ( strcmp( table[index], "divr" ) == 0  ) {
			// DIVR r1,r2
			op1type = 1 ; // reg
			op2type = 1 ; // reg
		}
		else if ( strcmp( table[index], "mulr" ) == 0  ) {
			// MULR r1,r2
			op1type = 1 ; // reg
			op2type = 1 ; // reg
		}
		else if ( strcmp( table[index], "rmo" ) == 0  ) {
			// RMO r1,r2
			op1type = 1 ; // reg
			op2type = 1 ; // reg
		}
		else if ( strcmp( table[index], "subr" ) == 0  ) {
			// RMO r1,r2
			op1type = 1 ; // reg
			op2type = 1 ; // reg
		}
		else {
			op1type = -1 ; // error
			op2type = -1 ; // error
		}
	}
	/*
	int GetOPCheckList( int index, char &optype1, char &optype2 ) {
		// get the index of an array
		// check the number of operand
		if ( types[index][0] ) {
			return 0 ;
		}
		else if ( types[index][1] ) {
			if ( strcmp( table[index], "clear" ) == 0 || strcmp( table[index], "tixr" ) == 0 ) {
				optype1 = 'r' ;
				return 1 ;
			}
			else if ( strcmp( table[index], "svc" ) == 0 ) {
				optype1 = 'n' ;
				return 1 ;
			}
			else if ( strcmp( table[index], "shiftl" ) == 0 || strcmp( table[index], "shiftr" ) == 0 ) {
				optype1 = 'r' ;
				optype2 = 'n' ;
				return 2 ;
			}
			else {
				optype1 = 'r' ;
				optype2 = 'r' ;
				return 2 ;
			}
		}
		else {
			// type 3 || type 4
			if ( strcmp ( table[index], "rsub" ) == 0  ) return 0 ;
			else return 1 ;

		}
	}
	*/
	void Print(){
		int i = 0 ;
		while ( i < my_index ) {
			printf( "%s ", table[i] ) ;

			if ( types[i][0] ) printf( "1 " ) ;
			else if ( types[i][1] ) printf( "2 " ) ;
			else printf( "3/4 " ) ;

			printf( "%s\n", code[i] ) ;
			i++ ;
		}
	}

};

class TableS {
  private:
	word table[100] ;
	int address[100] ;

	int Hash( word token ) {
		int total = 0 ;
		for( int i = 0 ; i < WORDLENGTH && token[i] != '\0' ; i++ ) {
			total = total + token[i] ;
		}

		return total % 100 ;
	}

	bool isEmptyAt( int index ) {
		if ( table[index][0] == '\0' ) return true ;
		else return false ;
	}
  public:
  	TableS() {
        Clean() ;
	}
	~TableS() {
		Clean() ;
	}
	bool Add( word token ) {
		int i = Hash( token ) ;
		// if hash table might be full, use j = i
		// if ( !isEmptyAt( i ) ) j++
		// while ( !isEmptyAt( i ) && j != i ) {
		// if j is a vacancy, it will not be in the if/while part
		// if j is not available，get in the if part then plus one quikly, when while reach j = i
		//    means hash table is no longer available
		while ( !isEmptyAt( i ) ) {
	        i++ ;
	        i = i % 100 ; // go back 0, and keep doing
		}

		if ( strcmp( token, "" ) != 0 ) {
			int j = 0 ;
			while ( j < WORDLENGTH && token[j] != '\0' ) {
		    	table[i][j] = token[j] ;
				j++ ;
			}

		    return true ;
		}
		else{
			return false ;
		}
	}
	void Clean(){
		for( int i = 0 ; i < 100 ; i++ ) {
			memset( table[i], '\0', WORDLENGTH ) ;
			address[i] = -1 ;
		}
	}

	bool Find( word temp, int &index ) {
		int i = Hash( temp ) ;
		bool exist = false ;
		while ( !exist && !isEmptyAt( i ) ) {
			// if collision, i+1, so stop finding if find a empty space (means there is no temp in the table)
			if ( strcmp( temp, table[i] ) == 0 ) exist = true ;
			i++ ;
			i = i % 100 ;
		}

		if ( exist ) {
			if ( i == 0 ) {
				index = 99 ;
			}
			else index = i - 1 ;
		}
		else index = -1 ;
		return exist ;
	}
	int GetAddr( int index ) {
		return address[index] ;
	}
	void ModifyAddressAdd( int num ) {
		for( int i = 0 ; i < 100 ; i++ ) {
			if ( address[i] > -1 ) address[i] = address[i] + num ;
		}
	}
	void ModifyAddressAt( int index, int num ) {
		if ( address[index] != -1 ) {
			address[index] = address[index] + num ;
		}
		else ;
	}
	void ModifyAddressToBe( int index, int num ) {
		if ( address[index] != -1 ) {
			address[index] = num ;
		}
		else ;
	}
	void Print(){
		int i = 0 ;
		while ( i < 99 ) {
			printf( "%s\n", table[i] ) ;
			i++ ;
		}
	}

	bool IfDuplicate( int index ) {
		// to solve duplicate definition
		// check the index of value at table5
		// if the address if not != -1, then duplicate definition
		if ( address[index] != -1 ) return true ; // not equal to -1, duplicate
		else return false ;
	}
	void ADDRess( int index ) {
		address[index] = gPlace ;
	}

};

TableI table1 ;
TableA table2 ;
TableA table3 ;
TableA table4 ;
TableS table5 ;
TableB table6 ;
TableB table7 ;

class LiteralTable{
  private:
	word table[100] ;
	int address[100] ; // address of literal
	bool isnum[100] ;

	int Hash( word token ) {
		int total = 0 ;
		for( int i = 0 ; i < WORDLENGTH && token[i] != '\0' ; i++ ) {
			total = total + token[i] ;
		}

		return total % 100 ;
	}

	bool isEmptyAt( int index ) {
		if ( table[index][0] == '\0' ) return true ;
		else return false ;
	}
  public:
  	LiteralTable() {
        Clean() ;
	}
	~LiteralTable() {
		Clean() ;
	}
	int GetAddr( int index ) {
		return address[index] ;
	}
	void Remove( int index ) {
		printf( "The literal : ='%s' is removed\n", table[index] ) ;
		printf( "Because it might out of the range of WORD\n" ) ;
		memset( table[index], '\0', WORDLENGTH ) ;
		address[index] = -1 ;
		isnum[index] = false ;
	}
	void GetAsDecNum( int index, int &num ) {
		num = 0 ;
		int temp ;
		for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
			num = num * 10 ;
			temp = table[index][i] - '0' ;
			num = num + temp ;
		}
	}
	bool Add( int type, int entry, word temp ) {
		word token ;
		if ( type == 6 ) {
			table6.GetString( entry, token ) ;
		}
		else {
			table7.GetString( entry, token ) ;
		}
		int i = Hash( token ) ;
		// if hash table might be full, use j = i
		// if ( !isEmptyAt( i ) ) j++
		// while ( !isEmptyAt( i ) && j != i ) {
		// if j is a vacancy, it will not be in the if/while part
		// if j is not available，get in the if part then plus one quikly, when while reach j = i
		//    means hash table is no longer available
		while ( !isEmptyAt( i ) && strcmp( token, table[i] ) != 0 ) {
	        i++ ;
	        i = i % 100 ; // go back 0, and keep doing
		}

		if ( strcmp( token, "" ) != 0  && strcmp( token, table[i] ) != 0 ) {
			int j = 0 ;
			while ( j < WORDLENGTH && token[j] != '\0' ) {
		    	table[i][j] = token[j] ;
				j++ ;
			}

			strcpy( temp, token ) ;

			if ( type == 6 ) {
				isnum[i] = true ; // number
			}
			else{
				isnum[i] = false ; // string
			}
		    return true ;
		}
		else if ( strcmp( token, table[i] ) == 0) {
			// duplicate
			strcpy( temp, token ) ;
			return false ;
		}
		else{
			return false ;
		}
	}
	void Clean(){
		for( int i = 0 ; i < 100 ; i++ ) {
			memset( table[i], '\0', WORDLENGTH ) ;
			address[i] = -1 ;
			isnum[i] = false ;
		}
	}
	bool Find( word temp, int &index ) {
		int i = Hash( temp ) ;
		bool exist = false ;
		while ( !exist && !isEmptyAt( i ) ) {
			// if collision, i+1, so stop finding if find a empty space (means there is no temp in the table)
			if ( strcmp( temp, table[i] ) == 0 ) exist = true ;
			i++ ;
			i = i % 100 ;
		}

		if ( exist ) {
			if ( i == 0 ) {
				index = 99 ;
			}
			else index = i - 1 ;
		}
		else index = -1 ;
		return exist ;
	}
	void IsNum( int index, int &type, int &num ) {
		int i = 0 ;
		bool err = false ;
		bool Hex = false ;
		bool Dec = false ;
		while ( i < WORDLENGTH && table[index][i] != '\0' && !err ) {
			if ( table[index][i] >= 'A' && table[index][i] <= 'F' ) {
				Hex = true ;
			}
			else if ( table[index][i] >= '0' && table[index][i] <= '9' ) {
				Dec = true ;
			}
			else {
				err = true ;
			}
		}

		if ( !err ) {
			if ( Hex ) {
				type = 1 ;
			}
			else {
				type = 0 ;
			}

			if ( type == 1 ) {
				num = 0 ;
				int temp ;
				for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
					num = num * 16 ;
					if ( table[index][i] >= 'A' ) {
						temp = table[index][i] - 'A' + 10 ;
					}
					else {
						temp = table[index][i] - '0' ;
					}
						num = num + temp ;
				}
			}
			else {
				num = 0 ;
				int temp ;
				for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
					num = num * 10 ;
					temp = table[index][i] - '0' ;
					num = num + temp ;
				}
			}
		}
		else {
			type = -1 ;
		}
	}
	void GetByte( int index, char b[3] ) {
		b[0] = table[index][0] ;
		b[1] = table[index][1] ;
		b[2] = '\0' ;
	}
	void GetWord( int index, char w[7] ) {
		for( int i = 0 ; i < 6 ; i++ ) {
			w[i] = table[index][i] ;
		}
		w[6] = '\0' ;
	}
	bool Getisnum( int index ) {
		return isnum[index] ;
	}

	/*
	void ModifyAddress( int i, int &size[100] ) {

		if ( address[i] == -1 ) {

			if ( isnum[i] == true ) {
				// =X''
				int type = 0 ;
				int value = 0 ;
				IsNum( i, type, value ) ;
				if ( value <= 0xFF ) {
					size = 1 ; // byte
					memset( symname, '\0', WORDLENGTH ) ;
					symname[0] = '=' ;
					strcat( symname, table[i] ) ;
					address[i] = gPlace ;
				}
				else if ( value <= 0xFFFFFF ) {

				}
				else {

				}
			}
			else {
				// =C''

			}

		}
	}
	*/
	void PrintDataAt( int index ) {
		printf( "table[%d] : %s\n", index, table[index] ) ;
		printf( "address : %d\n", address[index] ) ;
	}
	void Print(){
		int i = 0 ;
		while ( i < 99 ) {
			printf( "%s\n", table[i] ) ;
			i++ ;
		}
	}

	bool IsEmptyAt( int index ) {
		if ( strcmp( table[index], "" ) == 0 ) return true ;
		else return false ;
	}

	bool IfDuplicate( int index ) {
		// to solve duplicate definition
		// check the index of value at table5
		// if the address if not != -1, then duplicate definition
		if ( address[index] != -1 ) return true ; // not equal to -1, duplicate
		else return false ;
	}
	void ADDRess( int index ) {
		address[index] = gPlace ;
	}
	int GetOP( int index ) {
		int num = 0 ;
		if ( isnum[index] ) {
			num = 0 ;
			int temp ;
			for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
				num = num * 16 ;
				if ( table[index][i] >= 'A' ) {
					temp = table[index][i] - 'A' + 10 ;
				}
				else {
					temp = table[index][i] - '0' ;
				}
				num = num + temp ;
			}
		}
		else {
			num = 0 ;
			for( int i = 0 ; table[index][i] != '\0' && i < WORDLENGTH ; i++ ) {
				num = num * 0x100 ;
				num = num + table[index][i] ;
			}
		}
		return num ;
	}
	void ModifyAddressAdd( int num ) {
		for( int i = 0 ; i < 100 ; i++ ) {
			if ( address[i] > -1 ) address[i] = address[i] + num ;
		}
	}
	void ModifyAddressAt( int index, int num ) {
		if ( address[index] != -1 ) {
			address[index] = address[index] + num ;
		}
		else ;
	}
	void ModifyAddressToBe( int index, int num ) {
		if ( address[index] != -1 ) {
			address[index] = num ;
		}
		else ;
	}
};

LiteralTable table8 ;
