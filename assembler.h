using namespace std ;

SIC sic ;
SICXE sicxe ;

class Assembler {
	private:
		FILE* outFile ;

	public:
		FILE* inFile ;
		str100 inFileName ;
		str100 outFileName ;

	private:
		bool LexicalAnalysis( str100 instruction,str100 input ) {
			int index = 0 ;
			int i = 0 ;
			bool singleQuotation = false ;
			bool read = false ;
			bool stop = false ;
			char c = '\0' ;
			word instructionsult ;
			word ctostr ;
			str100 output ;
			memset( output, '\0', 200 ) ;
			memset( input, '\0', 200 ) ;
			Buffer buffer ;
			ErrorType instructioncord = DONE ; // in order to solve the problem that how to instructioncognize symbol and string
			read = Function::GetAChar( inFile, c ) ;

			while ( read && !stop && c != '\n' ) {
				memset( ctostr, '\0', WORDLENGTH ) ;
				memset( instructionsult, '\0', WORDLENGTH ) ;
				ctostr[0] = c ;
				input[i] = c ;

				if ( singleQuotation && c != '\'' ) {
					// read a single quotation mark befoinstruction
					// until read the second ', I should continuosly read a char and put it into the buffer

					buffer.AddChar( c ) ;
					i++ ;
				}
				else if ( c == ' ' || c == '\t' ) {
					instructioncord = buffer.GetIfToken( instructionsult, c, instructioncord ) ; // instructioncord should be DONE or EMPTY when ' ' occur

					if ( instructioncord == DONE )
            strcat( output, instructionsult ) ;

					i++ ;
				}
				else if ( table4.Find( ctostr, index ) ) {
					if ( !singleQuotation && c == '\'' ) {
						// means the first '\''
						singleQuotation = true ;
					}
					else if ( singleQuotation && c == '\'' ) {
						singleQuotation = false ;
					}

					instructioncord = buffer.GetIfToken( instructionsult, c, instructioncord ) ;

					if ( instructioncord == DONE )
            strcat( output, instructionsult ) ;

					buffer.GetDelimeter( ctostr, instructionsult ) ;
					strcat( output, instructionsult ) ;

					if ( c == '.' ) {
						while ( Function::GetAChar( inFile, c ) && c != '\n' ) {
							// read until '\n' when the comment occur ( .comment )
							i++ ;
							input[i] = c ;
						}
						stop = true ;
					}
					else {
						i++ ;
					}
				}
				else {
					buffer.AddChar(c) ;
					i++ ;
				}

				if ( !stop )
          read = Function::GetAChar( inFile, c ) ;
			}

			if ( c != '\n' ) {

				instructioncord = buffer.GetIfToken( instructionsult, c, instructioncord ) ; // instructioncord should be DONE or EMPTY when ' ' occur
				if ( instructioncord == DONE ) strcat( output, instructionsult ) ;
			}

			if ( !singleQuotation ) {
	    	// means the only one quotation mark did not occur

				instructioncord = buffer.GetIfToken( instructionsult, c, instructioncord ) ; // instructioncord should be DONE when '\n' occur

				if ( instructioncord == DONE )
          strcat( output, instructionsult ) ;
			}
			else {
				instructioncord = EMPTY ;
				buffer.Clean() ;
				singleQuotation = false ;
			}

			strcpy( instruction, output ) ;
			memset( output, '\0', 200 ) ;
			return read ;
		}

	public:
		void SIC_Pass1() {
			inFile = Function::ReadFile(inFileName) ;

      strcpy( outFileName, "LexicalAnalysis_" ) ;
			strcat ( outFileName, inFileName ) ;
			outFile = fopen( outFileName, "w" ) ;

			str100 input ;
			str100 instruction ;
			use_base = false ;

			while ( LexicalAnalysis( instruction, input ) ) {
				sic.Add( instruction, input ) ;
				fprintf( outFile, "%s\n", instruction ) ;
			}

			if ( strcmp( instruction, "" ) != 0 ) {
        cout << instruction << "2222" << endl ;
				sic.Add( instruction, input ) ;
				fprintf( outFile, "%s\n", instruction ) ;
			}

			fclose(inFile) ;
			fclose(outFile) ;
		}

		void SIC_Pass2() {
			strcpy( outFileName, "Assembler_" ) ;
			strcat( outFileName, inFileName ) ;
			FILE *fp = fopen( outFileName, "w" ) ;

			sic.Trans(fp) ;

			fclose(fp) ;
		}

		void SICXE_Pass1() {
			inFile = Function::ReadFile( inFileName ) ;

			strcpy( outFileName, "LexicalAnalysis_" ) ;
			strcat ( outFileName, inFileName ) ;
			outFile = fopen( outFileName, "w" ) ;

			str100 input ;
			str100 instruct ;
			use_base = false ;

			while ( LexicalAnalysis( instruct, input ) ) {
				sicxe.Add( instruct, input ) ;
				fprintf( outFile, "%s\n", instruct ) ;
			}

			if ( strcmp( instruct, "" ) != 0 ) {
				sicxe.Add( instruct, input ) ;
				fprintf( outFile, "%s\n", instruct ) ;
			}

			fclose(inFile) ;
			fclose(outFile) ;
		}

		void SICXE_Pass2() {
			strcpy( outFileName, "Assembler_" ) ;
			strcat( outFileName, inFileName ) ;
			FILE *fp = fopen( outFileName, "w" ) ;

			sicxe.Trans(fp) ;

			fclose(fp) ;
		}

};