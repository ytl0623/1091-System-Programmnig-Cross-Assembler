using namespace std ;

class Buffer{
	private:
		word buffer ;
		int buf_index  ;

		bool AllNumbers() {
			int i = 0 ;
      
			while ( i < WORDLENGTH && buffer[i] != '\0' ) {
				if ( !( buffer[0] >= '0' && buffer[0] <= '9' ) )
          return false ;
				i++ ;
			}
			return true ;
		}

	public:
		Buffer() {
			Clean() ;
		}

		bool AddChar( char c ) {
			if ( buf_index >= 48 ) {
				// buffer[49] = '\0'
				return false ;
			}
			else {
				buffer[buf_index] = c ;
				buf_index++ ;
			}
		}

		ErrorType GetIfToken( word iftoken, char peek, ErrorType ifneed ) {
			// iftoken is the result of GetIfToken!!!
			// used when ' ' or delimeter occur
			// check if buffer is empty, get a token if not NULL
			// when X'F1' happened, read X ，AddChar('X')
			// whlie get '\'', find the fact that it's X, clean X

		  if ( buf_index <= 0 ) {
				return EMPTY ;
			}
			else if ( buf_index == 1 && peek == '\'' && ( buffer[0] == 'X' || buffer[0] == 'C' || buffer[0] == 'x' || buffer[0] == 'c'  ) ) {
				// X'F1'
				ErrorType information = EMPTY ;
        
				if ( buffer[0] == 'X' || buffer[0] == 'x' ) {
 		      information = XNUMBER ;
				}
				else
          information = CSTRING ;
        
				Clean() ;
				return information ;
			}
			else {
				// means NOT NULL and not ( X'F1' || C'EOF' )
				// table4 done at Getdelimeter(), so it should NOT be here
        
				int index = 0 ;

				if ( ifneed == XNUMBER ) {
					// solve : X'F1'
					if ( table6.FindAllowCapital( buffer, index ) ) {
						sprintf( iftoken, "(6,%d)", index ) ;
					}
					else {
						table6.AddWithTransferToCapital(buffer) ;
						table6.FindAllowCapital( buffer, index ) ;
						sprintf( iftoken, "(6,%d)", index ) ;
					}
				}
				else if ( ifneed == CSTRING ) {
					// C'EOF'
					if ( table7.Find( buffer, index ) ) {
						sprintf( iftoken, "(7,%d)", index ) ;
					}
					else {
						table7.Add( buffer ) ;
						table7.Find( buffer, index ) ;
						sprintf( iftoken, "(7,%d)", index ) ;
					}
				}
				else if ( table1.FindAllowCapital( buffer, index ) ) {
					sprintf( iftoken, "(1,%d)", index ) ;
				}
				else if ( table2.FindAllowCapital( buffer, index ) ) {
					sprintf( iftoken, "(2,%d)", index ) ;
				}
				else if ( table3.FindAllowCapital( buffer, index ) ) {
					sprintf( iftoken, "(3,%d)", index ) ;
				}
				else if ( AllNumbers() ) {
					// #ABC 因為#後只接受10進位，所以：
					// # + symbol
					// 目前假定只有'X'會出現16進位，所以這個地方不可以用16進位用的FUNCTION
          
					if ( table6.Find( buffer, index ) ) {
						sprintf( iftoken, "(6,%d)", index ) ;
					}
					else {
						table6.AddWithTransferToCapital(buffer) ;
						table6.Find( buffer, index ) ;
						sprintf( iftoken, "(6,%d)", index ) ;
					}
				}
				else {
					// NOT 1~4,6~7, 5 left
          
					if ( table5.Find( buffer, index ) ) {
						sprintf( iftoken, "(5,%d)", index ) ;
					}
					else {
						table5.Add( buffer ) ;
						table5.Find( buffer, index ) ;
						sprintf( iftoken, "(5,%d)", index ) ;
					}
				}

				Clean() ;
				return DONE ;
			}
		}
    
		void GetDelimeter( word input, word token ) {
			// when find a delimeter, call this function
      
			int fortheindex = 0 ;
			table4.Find( input, fortheindex ) ;
			sprintf( token, "(4,%d)", fortheindex ) ;
		}
    
		void GetBuffer( word temp ) {
			strcpy( temp, buffer ) ;
		}
    
		int GetSize() {
			return buf_index ;
		}
    
		void Clean() {
			memset( buffer, '\0', WORDLENGTH ) ;
			buf_index = 0 ;
		}
};
