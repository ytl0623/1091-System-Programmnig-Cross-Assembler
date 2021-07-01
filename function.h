using namespace std ;

class Function {
	public:
    static bool CheckTokenValue( word token, int &tokentype, int &tokenvalue ) {
      // get the number, assume that the token in this form: (##, ####)

      int i = 0 ;
      bool done = false ;

      if ( token[0] == '(' ) {
        i = 1 ;
        tokentype = 0 ;
        tokenvalue = 0 ;

        while ( token[i] != ',' && i < 20 ) {     // WORDLENGTH: 20
          if ( token[i] != ' ' ) {
            tokentype = tokentype * 10 ;
            tokentype = tokentype + token[i] - '0' ;
          }
          i++ ;
        }

        i++ ; // token[i] != ','

        while ( token[i] != ')' && i < 20 ) {
          if ( token[i] != ' ' ) {
            tokenvalue = tokenvalue * 10 ;
            tokenvalue = tokenvalue + token[i] - '0' ;
          }
          i++ ;
        }

        return true ;
      }
      else if ( token[0] == '\0' ) {
        tokentype = 0 ;
        tokenvalue = 0 ;
        return true ;
      }
      else {
        return false ;
      }
    }

    static bool GetAChar( FILE *fp, char &ch ) {
      if ( fscanf( fp, "%c", &ch ) != EOF ) {
        return true ;
      }
      else{
        ch = '\0' ;
        return false ;
      }
    }

    static bool GetTheNext( FILE *fp, char &ch, char reserve ) {
      bool got = GetAChar( fp, ch ) ;
      bool stop = false ;

      if ( reserve != '\0' )
        stop = ( ch == reserve ) ;

        while ( ( ch == '\t' || ch == ' ' ) && got && !stop ) {
          got = GetAChar( fp, ch ) ; // for it is '\t' or ' ', retry
          stop = ( ch == reserve ) ;
        }

        return got ;
    }

    static bool GetAWord( FILE *fp, word temp, char reserve ) {
      int i = 0 ;
      char c = '\0' ;
      bool got = true ;
      bool stop = false ;

      if ( GetTheNext( fp, c, reserve ) ) {
        if ( reserve != '\0' )
          stop = ( c == reserve ) ;

        while ( c != '\n' && i < WORDLENGTH && got && !stop ) {
            temp[i] = c ;
            i++ ;
            got = GetTheNext( fp, c, reserve ) ; // got a char, so even detect the EOF, the word still need to be goten

            if ( reserve != '\0' )
              stop = ( c == reserve ) ;
        }

        return true ;
      }
      else
        return false ; // if EOF is the head of a char, didn't get a new word
    }

    static bool ReadTableI( int nth ) {
      FILE *fp = fopen( "Table0.table", "r" ) ;

      char sic = '\0' ;
      word opcode ;
      word format ;
      type temptype ;
      str100 machinecode ;
      char n = '\0' ;
      memset( temptype, false, 4 ) ;
      memset( opcode, '\0', WORDLENGTH ) ;
      memset( format, '\0', WORDLENGTH ) ;
      memset( machinecode, '\0', 3 ) ;

      while ( fscanf( fp, "%c%c", &sic, &n ) != EOF ) {
        GetAWord( fp, opcode, '\t' ) ;
        GetAWord( fp, format, '\t' ) ;
        GetAWord( fp, machinecode, '\0' ) ;

        if ( strcmp( format, "1" ) == 0 ) {
          temptype[0] = true ;
        }
        else if ( strcmp( format, "2" ) == 0 ) {
          temptype[1] = true ;
        }
        else if ( strcmp( format, "3/4" ) == 0 ) {
          temptype[2] = true ;
          temptype[3] = true ;
        }

        table1.Add( sic, opcode, temptype, machinecode ) ;
        sic = '\0' ;

        memset( temptype, false, 4 ) ;
        memset( opcode, '\0', WORDLENGTH ) ;
        memset( format, '\0', WORDLENGTH ) ;
        memset( machinecode, '\0', 3 ) ;
      }

      fclose( fp ) ;
      return true ;
    }

    static bool ReadTable( int nth ) {
      char nth_c[2] = {'\0'} ;     // char to str
      nth_c[0] = '0' + nth ;     // int to char

      str100 filename = "Table" ;
      strcat( filename, nth_c ) ;
      strcat( filename, ".table" ) ;
      FILE *fp = fopen( filename, "r" ) ;

      word temp ;
      memset( temp, '\0', WORDLENGTH ) ;

      while ( GetAWord( fp, temp, '\0' ) ) {
        if ( nth == 2 )
          table2.Add( temp ) ;
        else if ( nth == 3 )
          table3.Add( temp ) ;
        else if ( nth == 4 )
          table4.Add( temp ) ;

        memset( temp, '\0', WORDLENGTH ) ;
      }

      fclose(fp) ;
      return true ;
    }

    static void SetTable() {
      ReadTableI(1) ;
      ReadTable(2) ;
      ReadTable(3) ;
      ReadTable(4) ;
    }

    static FILE* ReadFile( str100 filename ) {
      cout << endl << "Input a file name: " ;
      cin >> filename ;
      FILE *fp = fopen( filename, "r" ) ;

      while ( fp == NULL ) {
        cout << endl << filename << " dose not exist!" << endl ;
        fflush(stdin) ;
        cout << endl << "Input a file name: " ;
        cin >> filename ;
        fp = fopen( filename, "r" ) ;
      }

      return fp ;
    }

};
