using namespace std ;

struct Instr{
	bool done ;
	bool x ;
	int location ;
	int size ;
	int instrtype ;
	bool op1_hex ;
	int2 symbol ;
  int2 opcode ;
  int2 op1[5] ;
  int2 op2[5] ;
  str100 error ;
  str100 input ;
};

class SIC {
	private:
		static void ArrReset( word10 source ) {
			for( int i = 0 ; i < 10 ; i++ ) {
				memset( source[i], '\0', WORDLENGTH ) ;
			}
		}

		static void StrToArr( str100 source, word10 dest ) {
			int i = 0 ;
			int j = 0 ; // the index of destination
			int k = 0 ; // the index of single token, which can be reuse
      
			ArrReset(dest) ;
      
			while ( ( i < 100 && j < 10 && source[i] != '\0' ) ) {
				dest[j][k] = source[i] ;
        
				if ( source[i] == ')' ) {
					j++ ;
					k = 0 ;
				}
				else
          k++ ;
        
				i++ ;
			}
      /*
      source: (5,15)(2,1)(6,93)
      dest[0]: (5,15)
      dest[1]: (2,1)
      dest[2]: (6,93)
      */
		}

		void Default(){
			for( int i = 0 ; i < 100 ; i++ ) {
				instructions[i].done = true ;
				instructions[i].x = false ;
				instructions[i].op1_hex = false ;
				memset( instructions[i].error, '\0', 100 ) ;
				instructions[i].instrtype = -2 ;
				instructions[i].location = 0 ;
        
				for( int j = 0 ; j < 5 ; j++ ) {
					instructions[i].op1[j][0] = 0 ;
					instructions[i].op1[j][1] = 0 ;
					instructions[i].op2[j][0] = 0 ;
					instructions[i].op2[j][1] =0 ;
				}

				instructions[i].opcode[0] = 0 ;
				instructions[i].opcode[1] = 0 ;
				instructions[i].size = 0 ;
				instructions[i].symbol[0] = 0 ;
				instructions[i].symbol[1] = 0 ;
			}
		}

		bool IfDec( int index ) {
			word temp ;
			table2.GetStr( index, temp ) ;
			bool dec = true ;
			int i = 0 ;
			while ( temp[i] != '\0' && i < WORDLENGTH ) {
				if ( temp[i] <= '0' || temp[i] >= '9' ) {
				    dec = false ;
				}

				i++ ;
			}

			return dec ;
		}

		void WordCheckOP1( int2 token, bool &skip ) {
			int num = 0 ;
			int index = sic_index ;
			if ( token[0] == 6 ) {
				table6.GetAsHexNum( token[1], num ) ;
				if ( num > 0xFFFFFF ) {
					printf( "[Syntax Error] Operand out of range\n" ) ;
					strcpy( instructions[sic_index].error, "[Syntax Error] Operand out of range." ) ;
					instructions[sic_index].done = true ;    // do not process any more
					instructions[sic_index].instrtype = -1 ; //  skipped
					skip = true ;
				}
				else {
					instructions[index].done = false ;
					instructions[index].instrtype = 0  ; //pseudo
					instructions[index].location = gPlace ;
					instructions[index].op1[0][0] = token[0] ;
					instructions[index].op1[0][1] = token[1] ;
					instructions[index].op2[0][0] = 0 ;
					instructions[index].op2[0][1] = 0 ;
					instructions[index].size = 3 ;
				}
			}
			else {
				// table7, string
				instructions[index].done = false ;
				instructions[index].instrtype = 0  ; //pseudo
				instructions[index].location = gPlace ;
				instructions[index].op1[0][0] = token[0] ;
				instructions[index].op1[0][1] = token[1] ;
				instructions[index].op2[0][0] = 0 ;
				instructions[index].op2[0][1] = 0 ;
				table7.GetStrLength( token[1], instructions[index].size ) ;
			}
		}

		void ByteCheckOP1( int2 token, bool skip ) {
			int num = 0 ;
			int index = sic_index ;
			if ( token[0] == 6 ) {
				table6.GetAsHexNum( token[1], num ) ;
				if ( num > 0xFF ) {
					printf( "[Syntax Error] Operand out of range\n" ) ;
					strcpy( instructions[index].error, "[Syntax Error] Operand out of range." ) ;
					instructions[index].done = true ;    // do not process any more
					instructions[index].instrtype = -1 ; //  skipped
					skip = true ;
				}
				else {
					instructions[index].done = false ;
					instructions[index].instrtype = 0  ; //pseudo
					instructions[index].location = gPlace ;
					instructions[index].op1[0][0] = token[0] ;
					instructions[index].op1[0][1] = token[1] ;
					instructions[index].op2[0][0] = 0 ;
					instructions[index].op2[0][1] = 0 ;
					instructions[index].size = 1 ;
				}
			}
			else{
				// table7, string
				instructions[index].done = false ;
				instructions[index].instrtype = 0  ; //pseudo
				instructions[index].location = gPlace ;
				instructions[index].op1[0][0] = token[0] ;
				instructions[index].op1[0][1] = token[1] ;
				instructions[index].op2[0][0] = 0 ;
				instructions[index].op2[0][1] = 0 ;
				table7.GetStrLength( token[1], instructions[index].size ) ;
			}
		}
    
		bool IfPseudo( int2 temptable[10], int i, bool &skip ) {
			int index = sic_index ;
			if ( temptable[i][0] == 2 ) {
				if ( table2.CmpWith( temptable[i][1], 0 ) == 0 ) {
					// {label} START hex_num
					if ( temptable[i+1][0] == 6 ) {
						int temp = 0 ;
						table6.GetAsHexNum( temptable[i+1][1], temp ) ;
						gPlace = temp ;
						instructions[index].done = true ;
						instructions[index].instrtype = 0  ; //pseudo
						instructions[index].location = gPlace ;
						instructions[index].op1[0][0] = temptable[i+1][0] ;
						instructions[index].op1[0][1] = temptable[i+1][1] ;
						instructions[index].op2[0][0] = 0 ;
						instructions[index].op2[0][1] = 0 ;
						instructions[index].opcode[0] = temptable[i][0] ;
						instructions[index].opcode[1] = temptable[i][1] ;
						instructions[index].size = 0 ;
						// sic_
					}
					else {
						printf( "[Syntax Error] Missing hex_num\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Missing hex_num." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 1 ) == 0 ) {
					// {label} END {label}
					if ( temptable[i+1][0] == 5 || temptable[i+1][0] == 0 ) {
						instructions[index].done = true ;
						instructions[index].instrtype = 0  ; //pseudo
						instructions[index].location = gPlace ;

						if ( temptable[i+1][0] == 5 ) {
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
						}
						else{
							instructions[index].op1[0][0] = 0 ;
							instructions[index].op1[0][1] = 0 ;
						}

						instructions[index].op2[0][0] = 0 ;
						instructions[index].op2[0][1] = 0 ;
						instructions[index].opcode[0] = temptable[i][0] ;
						instructions[index].opcode[1] = temptable[i][1] ;
						instructions[index].size = 0 ;

						// ===========================================================
						// Dealing with literal
						int templtorg = 0 ;
						for ( int z = 0 ; z < 100 ; z++ ) {
							templtorg = 0 ;
							if ( !table8.IsEmptyAt( z ) ) {
								if ( !table8.IfDuplicate( z ) ) {
									// address of table8[z] is -1
									table8.ADDRess( z ) ; // give it address
									templtorg = table8.GetOP( z ) ;
									if ( templtorg <= 0xFF ) {
										// BYTE
										// sicxe_index is not empty any more so need to plus 1
										char b[3] = { '\0' } ;
										table8.GetByte( z, b ) ;
										sic_index = sic_index + 1 ;
										instructions[sic_index].done = false ;
										strcpy( instructions[sic_index].input, "=" ) ;
										strcat( instructions[sic_index].input, b ) ;
										strcat( instructions[sic_index].input, "\t" ) ;
										strcat( instructions[sic_index].input, "BYTE" ) ;
										strcat( instructions[sic_index].input, "\t" ) ;
										strcat( instructions[sic_index].input, b ) ;

										instructions[sic_index].instrtype = 0 ;
										instructions[sic_index].location = gPlace ;
										instructions[sic_index].op1[0][0] = 8 ;
										instructions[sic_index].op1[0][1] = z ;
										instructions[sic_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sic_index].opcode[0] = 2 ;
										instructions[sic_index].opcode[1] = 3 ;
										instructions[sic_index].size = 1 ;
										gPlace = gPlace + 1 ;
										sic_index = sic_index + 1 ;
									}
									else if ( templtorg <= 0xFFFFFF ) {
										// WORD
										char w[7] = { '\0' } ;
										table8.GetWord( z, w ) ;
										sic_index = sic_index + 1 ;
										instructions[sic_index].done = false ;
										strcpy( instructions[sic_index].input, "=" ) ;
										strcat( instructions[sic_index].input, w ) ;
										strcat( instructions[sic_index].input, "\t" ) ;
										strcat( instructions[sic_index].input, "WORD" ) ;
										strcat( instructions[sic_index].input, "\t" ) ;
										strcat( instructions[sic_index].input, w ) ;

										instructions[sic_index].instrtype = 0 ;
										instructions[sic_index].location = gPlace ;
										instructions[sic_index].op1[0][0] = 8 ;
										instructions[sic_index].op1[0][1] = z ;
										instructions[sic_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sic_index].opcode[0] = 2 ;
										instructions[sic_index].opcode[1] = 4 ;
										instructions[sic_index].size = 3 ;
										gPlace = gPlace + 3 ;
										sic_index = sic_index + 1 ;
									}
									else{
										table8.Remove( z ) ;
									}
								}
								else ; // already defined, do nothing
							}
							else ; // empty, do nothing
						}
					}
					else {
						printf( "[Syntax Error] Redundant token\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Redundant token." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 2 ) == 0 || table2.CmpWith( temptable[i][1], 3 ) == 0 ) {
					// {label} BYTE X''… | C''… | decimal
					bool byte = table2.CmpWith( temptable[i][1], 2 ) == 0 ;
					if ( ( temptable[i+1][0] == 4 && temptable[i+1][1] == 9 ) || temptable[i+1][0] == 6 || temptable[i+1][0] == 8 ) {
						if ( temptable[i+1][0] == 6 ) {
							// correct, check if in the range
							// and record
							if ( byte ) ByteCheckOP1( temptable[i+1], skip ) ;
							else WordCheckOP1( temptable[i+1], skip ) ;

							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
						}
						else if ( temptable[i+1][0] == 8 ) {
							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
						}
						else if ( temptable[i+1][0] == 4 && temptable[i+1][1] == 9 ) {
							if ( temptable[i+2][0] == 6 || temptable[i+2][0] == 7 ) {
								// X'##' || C'##'
								// correct
								if ( byte ) ByteCheckOP1( temptable[i+2], skip ) ;
								else WordCheckOP1( temptable[i+2], skip ) ;
								instructions[index].opcode[0] = temptable[i][0] ;
								instructions[index].opcode[1] = temptable[i][1] ;
								instructions[index].op1_hex = true ;
							}
							else {
								printf( "[Syntax Error] Missing Operand!\n" ) ;
								strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}
						}

					}
					else {
						printf( "[Syntax Error] Missing Operand!\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 4 ) == 0 || table2.CmpWith( temptable[i][1], 5 ) == 0 ) {
					// {label} RESB decimal									{label} RESW decimal.

					if ( temptable[i+1][0] == 6 && IfDec( temptable[i+1][1] ) ) {

						int temp = 0 ;
						table6.GetAsDecNum( temptable[i+1][1], temp ) ;

						if ( table2.CmpWith( temptable[i][1], 4 ) == 0 ) {
							instructions[index].done = true ;
							instructions[index].instrtype = 0  ; //pseudo
							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].op2[0][1] = 0 ;
							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
							instructions[index].size = temp ;
						}
						else if ( table2.CmpWith( temptable[i][1], 5 ) == 0 ) {
							instructions[index].done = true ;
							instructions[index].instrtype = 0  ; //pseudo
							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].op2[0][1] = 0 ;
							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
							instructions[index].size = 3 * temp ;
						}
						else ;

					}
					else {
						printf( "[Syntax Error] Missing Operand!\n" ) ;
						printf( "It's not a decimal number!\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 6 ) == 0 ) {
					// label EQU label | decimal | *  四則運算限定於label與label
					if ( i == 1 && temptable[i-1][0] == 5  ) {
						// the first label exist
						if ( ( temptable[i+1][0] == 6 && IfDec( temptable[i+1][1] ) ) ) {
							instructions[index].done = false ;
							instructions[index].instrtype = 0  ; //pseudo
							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].op2[0][1] = 0 ;
							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
							instructions[index].size = 0 ;
						}
						else if ( temptable[i+1][0] == 5 ) {
							instructions[index].done = false ;
							instructions[index].instrtype = 0  ; //pseudo
							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].op2[0][1] = 0 ;
							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
							instructions[index].size = 0 ;
							// label EQU label
							int j = i + 2 ;
							int k = 1 ;
							while ( j < 10 && k < 5 && temptable[j][0] == 4 && temptable[j+1][0] == 5 ) {
								instructions[index].op1[k][0] = temptable[j][0] ;
								instructions[index].op1[k][1] = temptable[j][1] ;
								instructions[index].op1[k+1][0] = temptable[j+1][0] ;
								instructions[index].op1[k+1][1] = temptable[j+1][1] ;
								k= k + 2 ;
								j = j + 2 ;
							}
						}
						else if ( temptable[i+1][0] == 4 && temptable[i+1][0] == 4 ) {
							// Label EQU *
							instructions[index].done = false ;
							instructions[index].instrtype = 0  ; //pseudo
							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].op2[0][1] = 0 ;
							instructions[index].opcode[0] = temptable[i][0] ;
							instructions[index].opcode[1] = temptable[i][1] ;
							instructions[index].size = 0 ;

						}
						else {
							printf( "[Syntax Error] Missing Operand!\n" ) ;
							printf( "It's not a decimal number or symbol!\n" ) ;
							strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
							skip = true ;
						}
					}
					else {
						printf( "[Syntax Error] Missing symbol When EQU!\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Missing symbol When EQU!" ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 7 ) == 0  ) {
					// {label} BASE decimal | symbol
					if ( ( temptable[i+1][0] == 6 && IfDec( temptable[i+1][1] ) ) || temptable[i+1][0] == 5 ) {
						instructions[index].done = false ;
						instructions[index].instrtype = 0  ; //pseudo
						instructions[index].location = gPlace ;
						instructions[index].op1[0][0] = temptable[i+1][0] ;
						instructions[index].op1[0][1] = temptable[i+1][1] ;
						instructions[index].op2[0][0] = 0 ;
						instructions[index].op2[0][1] = 0 ;
						instructions[index].opcode[0] = temptable[i][0] ;
						instructions[index].opcode[1] = temptable[i][1] ;
						instructions[index].size = 0 ;
					}
					else {
						printf( "[Syntax Error] Missing Operand!\n" ) ;
						printf( "It's not a decimal number or symbol!\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 8 ) == 0 ) {
					// {label} LTORG
					bool been = false ;
					for( int j = i + 1 ; j < 10 && temptable[j][0] == 0 ; j++ ) {
						been = true ;
					}

					if ( !been ) {
						instructions[index].done = true ;
						instructions[index].instrtype = 0  ; //pseudo
						instructions[index].location = gPlace ;
						instructions[index].op1[0][0] = temptable[i+1][0] ;
						instructions[index].op1[0][1] = temptable[i+1][1] ;
						instructions[index].op2[0][0] = 0 ;
						instructions[index].op2[0][1] = 0 ;
						instructions[index].opcode[0] = temptable[i][0] ;
						instructions[index].opcode[1] = temptable[i][1] ;
						instructions[index].size = 0 ;

						// label ltorg
						// =======================================
						// Dealing with literal

						int templtorg = 0 ;
						for ( int z = 0 ; z < 100 ; z++ ) {
							templtorg = 0 ;
							if ( !table8.IsEmptyAt( z ) ) {
								if ( !table8.IfDuplicate( z ) ) {
									// address of table8[z] is -1
									table8.ADDRess( z ) ; // give it address
									templtorg = table8.GetOP( z ) ;
									if ( templtorg <= 0xFF ) {
										// BYTE
										// sicxe_index is not empty any more so need to plus 1
										char b[3] = { '\0' } ;
										table8.GetByte( z, b ) ;
										sic_index = sic_index + 1 ;
										instructions[sic_index].done = false ;
										strcpy( instructions[sic_index].input, "=" ) ;
										strcat( instructions[sic_index].input, b ) ;
										strcat( instructions[sic_index].input, "\t" ) ;
										strcat( instructions[sic_index].input, "BYTE" ) ;
										strcat( instructions[sic_index].input, b ) ;

										instructions[sic_index].instrtype = 0 ;
										instructions[sic_index].location = gPlace ;
										instructions[sic_index].op1[0][0] = 8 ;
										instructions[sic_index].op1[0][1] = z ;
										instructions[sic_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sic_index].opcode[0] = 2 ;
										instructions[sic_index].opcode[1] = 3 ;
										instructions[sic_index].size = 1 ;
										gPlace = gPlace + 1 ;
										sic_index = sic_index + 1 ;
									}
									else if ( templtorg <= 0xFFFFFF ) {
										// WORD
										char w[7] = { '\0' } ;
										table8.GetWord( z, w ) ;
										sic_index = sic_index + 1 ;
										instructions[sic_index].done = false ;
										strcpy( instructions[sic_index].input, "=" ) ;
										strcat( instructions[sic_index].input, w ) ;
										strcat( instructions[sic_index].input, "\t" ) ;
										strcat( instructions[sic_index].input, "WORD" ) ;
										strcat( instructions[sic_index].input, w ) ;

										instructions[sic_index].instrtype = 0 ;
										instructions[sic_index].location = gPlace ;
										instructions[sic_index].op1[0][0] = 8 ;
										instructions[sic_index].op1[0][1] = z ;
										instructions[sic_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sic_index].opcode[0] = 2 ;
										instructions[sic_index].opcode[1] = 4 ;
										instructions[sic_index].size = 3 ;
										gPlace = gPlace + 3 ;
										sic_index = sic_index + 1 ;
									}
									else{
										table8.Remove( z ) ;
									}
								}
								else ; // already defined, do nothing
							}
							else ; // empty, do nothing
						}
					}
					else {
						printf( "[Syntax Error] Redundant token!\n" ) ;
						strcpy( instructions[index].error, "[Syntax Error] Redundant token." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else {
					// error
					printf( "[Syntax Error] Unknown Pseudo Code!\n" ) ;
					strcpy( instructions[index].error, "[Syntax Error] Unknown Pseudo Code." ) ;
					instructions[index].done = true ;    // do not process any more
					instructions[index].instrtype = -1 ; //  skipped
					skip = true ;
				}

				if ( !skip ) gPlace = gPlace + instructions[index].size ;
				return true ; // though an error occur, but processed
			}
			else return false ;
		}
    
		bool IfInstr( int2 temptable[10], int i, bool &skip ) {
			/*
				standard：<INS> <m>
				<m>： <direct>|<index>
				<direct>： <symbol> 根據附錄A的規定決定<symbol>要不要
				<index>： <symbol>, X
			  // use i to avoid get the token of a symbol
      */
      
			int index = sic_index ;
			if ( temptable[i][0] == 1 ) {
				// true
				if ( table1.FindWithToken( temptable[i][1] ) ) {
					if ( table1.CheckIfSIC( temptable[i][1] ) ) {
						int op = table1.SIC_CheckOP( temptable[i][1] ) ;
						if ( op == 0 ) {
							int j = i+1 ;
							bool f = false ;
							while ( j != 10 && !f ) {
								if ( temptable[j][0] != 0 ) {
									f = true ;
								}
								j++ ;
							}
							if ( f ) {
								printf( "[Syntax Error] Fail to Fetch operand! \n" ) ;
								printf( "Instruction format error!\n" ) ;
								strcpy( instructions[index].error, "[Syntax Error] Instruction format error." ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}
							else {
								instructions[index].done = false ;
								instructions[index].instrtype = 3 ;
								instructions[index].location = gPlace ;
								instructions[index].size = 3 ;
								instructions[index].op1[0][0] = 0 ;
								instructions[index].op2[0][0] = 0 ;
								instructions[index].opcode[0]= temptable[i][0] ; // symbol should be stored in table[0]
								instructions[index].opcode[1] = temptable[i][1] ;
							}
						}
						else {
							if ( temptable[i+1][0] == 5 ) {
								if ( temptable[i+2][0] == 4 && temptable[i+2][1] == 1 ) {
									// ','
									if ( temptable[i+3][0] == 3 && temptable[i+3][1] == 2 ) {
										// 'X'
										instructions[index].done = false ;
										instructions[index].x = true ;
										instructions[index].instrtype = 3 ;
										instructions[index].location = gPlace ;
										instructions[index].size = 3 ;
										instructions[index].op1[0][0] = temptable[i+1][0] ;
										instructions[index].op1[0][1] = temptable[i+1][1] ;
										instructions[index].op2[0][0] = temptable[i+3][0] ;
										instructions[index].op2[0][1] = temptable[i+3][1] ;
										instructions[index].opcode[0]= temptable[i][0] ; // symbol should be stored in table[0]
										instructions[index].opcode[1] = temptable[i][1] ;
									}
									else {
										printf( "[Syntax Error] Missing token 'X' !\n" ) ;
										strcpy( instructions[index].error, "[Syntax Error] Missing token 'X' !" ) ;
										instructions[index].done = true ;    // do not process any more
										instructions[index].instrtype = -1 ; //  skipped
										skip = true ;
									}
								}
								else if ( temptable[i+2][0] == 0 ) {
									int j = i+2 ;
									bool f = false ;
									while ( j != 10 && !f ) {
										if ( temptable[j][0] != 0 ) {
											f = true ;
										}
										j++ ;
									}
									if ( f ) {
										printf( "[Syntax Error] Fail to Fetch operand! \n" ) ;
										printf( "Instruction format error!\n" ) ;
										strcpy( instructions[index].error, "[Syntax Error] Instruction format error." ) ;
										instructions[index].done = true ;    // do not process any more
										instructions[index].instrtype = -1 ; //  skipped
										skip = true ;
									}
									else {
										instructions[index].done = false ;
										instructions[index].instrtype = 3 ;
										instructions[index].location = gPlace ;
										instructions[index].size = 3 ;
										instructions[index].op1[0][0] = temptable[i+1][0] ;
										instructions[index].op1[0][1] = temptable[i+1][1] ;
										instructions[index].op2[0][0] = 0 ;
										instructions[index].opcode[0]= temptable[i][0] ; // symbol should be stored in table[0]
										instructions[index].opcode[1] = temptable[i][1] ;
									}
								}
								else {
									printf( "[Syntax Error] Redundant token !\n" ) ;
									strcpy( instructions[index].error, "[Syntax Error] Redundant token!" ) ;
									instructions[index].done = true ;    // do not process any more
									instructions[index].instrtype = -1 ; //  skipped
									skip = true ;
								}
							}
							else {
								printf( "[Syntax Error] Missing operand!\n" ) ;
								strcpy( instructions[index].error, "[Syntax Error] Missing operand!" ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}
						}
					}
					else {
						printf( "It's NOT an SIC insruction. It might be SIC/XE instruction \n" ) ;
						printf( "This is SIC Assembler, not SIC/XE Assembler.\n" ) ;

						strcpy( instructions[index].error, "[Syntax Error] It's NOT an SIC insruction. It might be SIC/XE instruction" ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else {
					strcpy( instructions[index].error, "[Syntax Error] The token might NOT be in the instruction table" ) ;
					instructions[index].done = true ;    // do not process any more
					instructions[index].instrtype = -1 ; //  skipped
					skip = true ;
				}

				if ( !skip ) gPlace = gPlace + instructions[index].size ;
				return true ; // though an error occur, but processed
			}
			else return false ;

		}

		bool FindSymbol( int type, int value, bool &duplicate ) {
			if ( type == 5 ) {
				if ( table5.IfDuplicate( value ) ) {
					// check the index of value at table5
					// if the address if not != -1, then duplicate definition
					duplicate = true ;
				}
				else {
					table5.ADDRess( value ) ;
				}

				return true ;
			}
			else return false ;
		}
    
	public:
		int sic_index ;
		Instr instructions[100] ;

		SIC(){
			sic_index = 0 ;
			Default() ;
		}

		void Add( str100 instruction, str100 input ) {
      //cout << instruction << input << endl ;
      //(5,15)(2,1)(6,93)COPY   START   1000
      
			int temp_index = sic_index ;

			instructions[temp_index].location = gPlace ;
			strcpy( instructions[temp_index].input, input ) ;
			word10 temp ;

			bool duplicate = false ;
			bool unknowninstr = false ;
			bool unknownpseudo = false ;
			bool type4 = false ;
			bool skip = false ;
			int2 temptable[10] ;
			int pseudoSize = 0 ;
			int i = 0 ;

			StrToArr( instruction, temp ) ;
      
			for ( int j = 0 ; j < 10 ; j++ ) {
				Function::CheckTokenValue( temp[j], temptable[j][0], temptable[j][1] ) ;
			}

			if ( FindSymbol( temptable[i][0], temptable[i][1], duplicate ) ) {
				// symbol might be duplicate, so need to check if duplicate

				if (duplicate) {
					instructions[temp_index].done = true ;    // do not process any more
					instructions[temp_index].instrtype = -1 ; //  skipped
					skip = true ;
					printf( "[Syntax Error] Duplicate Symbol occur!\n " ) ;
				}
				else {
					instructions[temp_index].symbol[0] = temptable[i][0] ;
					instructions[temp_index].symbol[1] = temptable[i][1] ;
					i++ ;
				}
			}

			// if FindSymbol() and not duplicate, get the next token.
			// if !FindSymbol then the token remained

			if ( !skip ) {
				if ( IfInstr( temptable, i, skip ) ) ;
				else if ( IfPseudo( temptable, i, skip ) );
				else if ( temptable[i][0] == 4 && temptable[i][1] == 10 ) {
					// command
					instructions[temp_index].done = true ;    // do not process any more
					instructions[temp_index].instrtype = -1 ; // to skip this line, see it as an error
					skip = true ;
					strcpy( instructions[temp_index].error, input ) ;
				}
				else if ( temptable[i][0] == 0 ) {
					instructions[temp_index].done = true ;    // do not process any more
					instructions[temp_index].instrtype = -2 ; // to skip this line, see it as an error
					skip = true ;
					strcpy( instructions[temp_index].error, "[Syntax Error] Not an instr or pseudo" ) ;
				}
				else {
					instructions[temp_index].done = true ;    // do not process any more
					instructions[temp_index].instrtype = -1 ; // to skip this line, see it as an error
					skip = true ;
					strcpy( instructions[temp_index].error, "[Syntax Error] Not an instr or pseudo" ) ;
				}
			}
			sic_index++ ;
		}

		void Trans( FILE *fp ) {     // fp: outfile
			int i = 0 ;
			long long int tempcode = 0 ;
			int temp = 0 ;
			fprintf( fp, "Line\tLoc\tSource statement\tObject code\n" ) ;
      
			while ( i < sic_index ) {
				tempcode = 0 ;
				temp = 0 ;

				if ( !instructions[i].done ) {
					if ( instructions[i].instrtype == 3 ) {
						tempcode = table1.GetOPCode( instructions[i].opcode[1] ) ;
						tempcode = tempcode * 0x10000 ;
            
						if ( instructions[i].x )
              tempcode = tempcode + 0x8000 ;

						if ( instructions[i].op1[0][0] != 0 )
              temp = table5.GetAddr( instructions[i].op1[0][1] ) ;
            
						tempcode = tempcode + temp ;
            
						fprintf( fp, "%d\t%X\t%s\t%06X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
            //printf( "%d\t%X\t%s\t%06X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
            //2       1000    FIRST   STL     RETADR  141033
					}
					else if ( instructions[i].instrtype == 0 ) {
						if ( instructions[i].opcode[1] == 3 ) {
							// BYTE
							if ( instructions[i].op1[0][0] == 6 ) {
								if ( instructions[i].op1_hex )	table6.GetAsHexNuml( instructions[i].op1[0][1], tempcode ) ;
								else table6.GetAsDecNuml( instructions[i].op1[0][1], tempcode ) ;
								fprintf( fp, "%d\t%X\t%s\t%02X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
							else {
								word s ;
								memset( s, '\0', WORDLENGTH ) ;
								table7.GetString( instructions[i].op1[0][1], s ) ;
								for( int j = 0 ; j < WORDLENGTH && s[j] != '\0' ; j++ ) {
									tempcode = tempcode * 0x100 ;
									tempcode = tempcode + s[j] ;
								}
								fprintf( fp, "%d\t%X\t%s\t%X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
						}
						else if ( instructions[i].opcode[1] == 4 ) {
							// WORD
							if ( instructions[i].op1[0][0] == 6 ) {
								if ( instructions[i].op1_hex )	table6.GetAsHexNuml( instructions[i].op1[0][1], tempcode ) ;
								else table6.GetAsDecNuml( instructions[i].op1[0][1], tempcode ) ;
								fprintf( fp, "%d\t%X\t%s\t%06X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
							else {
								word s ;
								memset( s, '\0', WORDLENGTH ) ;
								table7.GetString( instructions[i].op1[0][1], s ) ;
								for( int j = 0 ; j < WORDLENGTH ; j++ ) {
									tempcode = tempcode * 0x100 ;
									tempcode = tempcode + s[j] ;
								}
								fprintf( fp, "%d\t%X\t%s\t%X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
						}
						else if ( instructions[i].opcode[1] == 7 ) {
							// EQU
							// TODO : Label EQU symbol + symbol * symbol
							if ( instructions[i].op1[0][0] == 5 ) {
								// Symbol EQU Symbol
								temp = table5.GetAddr( instructions[i].op1[0][1] ) ;
								table5.ModifyAddressToBe( instructions[i].symbol[1], temp ) ;
							}
							else if ( instructions[i].op1[0][0] == 4 && instructions[i].op1[0][0] == 4 ) {
								table5.ModifyAddressToBe( instructions[i].symbol[1], gPlace ) ;
							}
							else {
								// Symbol EQU Dec
								table6.GetAsDecNum( instructions[i].op1[0][1], temp ) ;
								table5.ModifyAddressToBe( instructions[i].symbol[1], temp ) ;
							}
						}
						else if ( instructions[i].opcode[1] == 8 ) {
							// BASE
							if ( instructions[i].op1[0][0] == 5 ) {
								// Symbol EQU Symbol
								temp = table5.GetAddr( instructions[i].op1[0][1] ) ;
								gBase = temp ;
								use_base = true ;
							}
							else {
								// Symbol EQU Decimal
								table6.GetAsDecNum( instructions[i].op1[0][1], temp ) ;
								gBase = temp ;
								use_base = true ;
							}
						}
					}
				}
				else if ( instructions[i].done && instructions[i].instrtype >= 0 ) {
					fprintf( fp, "%d\t%X\t%s\n", i, instructions[i].location, instructions[i].input ) ; // pseudo finished
				}
				else if ( instructions[i].done && instructions[i].instrtype == -2 ) {
				 // empty
					fprintf( fp, "\n" ) ;
				}
				else {
					fprintf( fp, "%d\t%s\n", i, instructions[i].error ) ;
				}

				i++ ;
			}
		}
    
};