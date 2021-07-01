using namespace std ;

struct XEInstr{
	bool done ;
	bool x ;
	int location ;
	int size ;
	int instrtype ;
	int type2_op1type ;
	int type2_op2type ;
	int type3_optype ;
	bool op1_hex ;
	int2 symbol ;
  int2 opcode ;
  int2 op1[5] ;
  int2 op2[5] ;
  str100 error ;
  str100 input ;
};


class SICXE {
	private:
		int sicxe_index ;
		struct XEInstr instructions[100] ;

		static void ArrReset( word10 source ) {
			for( int i = 0 ; i < 10 ; i++ ) {
				memset( source[i], '\0', WORDLENGTH ) ;
			}
		}

		static void StrToArr( str100 source, word10 dest ) {
			int i = 0 ;
			int j = 0 ; // the index of destination
			int k = 0 ; // the index of single token, which can be reuse
			ArrReset( dest ) ;
			while ( ( i < 200 && j < 10 && source[i] != '\0' ) ) {
				dest[j][k] = source[i] ;
				if ( source[i] == ')' ) {
					j++ ;
					k = 0 ;
				}
				else k++ ;
				i++ ;
			}
		}

		void Set(){
			for( int i = 0 ; i < 20 ; i++ ) {
				instructions[i].done = true ;
				instructions[i].x = false ;
				instructions[i].op1_hex = false ;
				memset( instructions[i].error, '\0', 50 ) ;
				memset( instructions[i].input, '\0', 200 ) ;
				instructions[i].instrtype = -2 ;
				instructions[i].location = 0 ;
				instructions[i].type2_op1type = -1 ;
				instructions[i].type2_op2type = -1 ;
				instructions[i].type3_optype = -1 ;
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
			int index = sicxe_index ;
			if ( token[0] == 6 ) {
				table6.GetAsHexNum( token[1], num ) ;
				if ( num > 0xFFFFFF ) {
          cout << token << " " << "[Syntax Error] Operand out of range\n" ;

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
			int index = sicxe_index ;
			if ( token[0] == 6 ) {
				table6.GetAsHexNum( token[1], num ) ;
				if ( num > 0xFF ) {
          cout << token << " " << "[Syntax Error] Operand out of range\n" ;

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
			int index = sicxe_index ;
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
            cout << "[Syntax Error] Missing hex_num\n" ;

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
						instructions[index].instrtype = 0  ; // pseudo
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
										sicxe_index = sicxe_index + 1 ;
										instructions[sicxe_index].done = false ;
										strcpy( instructions[sicxe_index].input, "=" ) ;
										strcat( instructions[sicxe_index].input, b ) ;
										strcat( instructions[sicxe_index].input, "\t" ) ;
										strcat( instructions[sicxe_index].input, "BYTE" ) ;
										strcat( instructions[sicxe_index].input, "\t" ) ;
										strcat( instructions[sicxe_index].input, b ) ;

										instructions[sicxe_index].instrtype = 0 ;
										instructions[sicxe_index].location = gPlace ;
										instructions[sicxe_index].op1[0][0] = 8 ;
										instructions[sicxe_index].op1[0][1] = z ;
										instructions[sicxe_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sicxe_index].opcode[0] = 2 ;
										instructions[sicxe_index].opcode[1] = 3 ;
										instructions[sicxe_index].size = 1 ;
										gPlace = gPlace + 1 ;
										sicxe_index = sicxe_index + 1 ;
									}
									else if ( templtorg <= 0xFFFFFF ) {
										// WORD
										char w[7] = { '\0' } ;
										table8.GetWord( z, w ) ;
										sicxe_index = sicxe_index + 1 ;
										instructions[sicxe_index].done = false ;
										strcpy( instructions[sicxe_index].input, "=" ) ;
										strcat( instructions[sicxe_index].input, w ) ;
										strcat( instructions[sicxe_index].input, "\t" ) ;
										strcat( instructions[sicxe_index].input, "WORD" ) ;
										strcat( instructions[sicxe_index].input, "\t" ) ;
										strcat( instructions[sicxe_index].input, w ) ;

										instructions[sicxe_index].instrtype = 0 ;
										instructions[sicxe_index].location = gPlace ;
										instructions[sicxe_index].op1[0][0] = 8 ;
										instructions[sicxe_index].op1[0][1] = z ;
										instructions[sicxe_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sicxe_index].opcode[0] = 2 ;
										instructions[sicxe_index].opcode[1] = 4 ;
										instructions[sicxe_index].size = 3 ;
										gPlace = gPlace + 3 ;
										sicxe_index = sicxe_index + 1 ;
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
            cout << "[Syntax Error] Redundant token\n" ;

						strcpy( instructions[index].error, "[Syntax Error] Redundant token." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 2 ) == 0 || table2.CmpWith( temptable[i][1], 3 ) == 0 ) {
					// {label} BYTE X''… || C''… || decimal
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
								cout << "[Syntax Error] Missing Operand!\n" ;

								strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}
						}

					}
					else {
            cout << "[Syntax Error] Missing Operand!\n" ;

						strcpy( instructions[index].error, "[Syntax Error] Missing Operand." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else if ( table2.CmpWith( temptable[i][1], 4 ) == 0 || table2.CmpWith( temptable[i][1], 5 ) == 0 ) {
					// {label} RESB decimal || {label} RESW decimal

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
						cout << "[Syntax Error] It's not a decimal number!\n" ;

						strcpy( instructions[index].error, "[Syntax Error] It's not a decimal number!" ) ;
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
							instructions[index].instrtype = 0  ;     // pseudo
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
							instructions[index].instrtype = 0  ;     // pseudo
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
							cout << "[Syntax Error] It's not a decimal number or symbol!\n" ;
							strcpy( instructions[index].error, "[Syntax Error] It's not a decimal number or symbol!" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
							skip = true ;
						}
					}
					else {
						cout << "[Syntax Error] Missing symbol When EQU!\n" ;

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
						cout << "[Syntax Error] It's not a decimal number or symbol!\n" ;

						strcpy( instructions[index].error, "[Syntax Error] It's not a decimal number or symbol!" ) ;
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
										sicxe_index = sicxe_index + 1 ;
										instructions[sicxe_index].done = false ;
										strcpy( instructions[sicxe_index].input, "=" ) ;
										strcat( instructions[sicxe_index].input, b ) ;
										strcat( instructions[sicxe_index].input, "\t" ) ;
										strcat( instructions[sicxe_index].input, "BYTE" ) ;
										strcat( instructions[sicxe_index].input, b ) ;

										instructions[sicxe_index].instrtype = 0 ;
										instructions[sicxe_index].location = gPlace ;
										instructions[sicxe_index].op1[0][0] = 8 ;
										instructions[sicxe_index].op1[0][1] = z ;
										instructions[sicxe_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sicxe_index].opcode[0] = 2 ;
										instructions[sicxe_index].opcode[1] = 3 ;
										instructions[sicxe_index].size = 1 ;
										gPlace = gPlace + 1 ;
										sicxe_index = sicxe_index + 1 ;
									}
									else if ( templtorg <= 0xFFFFFF ) {
										// WORD
										char w[7] = { '\0' } ;
										table8.GetWord( z, w ) ;
										sicxe_index = sicxe_index + 1 ;
										instructions[sicxe_index].done = false ;
										strcpy( instructions[sicxe_index].input, "=" ) ;
										strcat( instructions[sicxe_index].input, w ) ;
										strcat( instructions[sicxe_index].input, "\t" ) ;
										strcat( instructions[sicxe_index].input, "WORD" ) ;
										strcat( instructions[sicxe_index].input, w ) ;

										instructions[sicxe_index].instrtype = 0 ;
										instructions[sicxe_index].location = gPlace ;
										instructions[sicxe_index].op1[0][0] = 8 ;
										instructions[sicxe_index].op1[0][1] = z ;
										instructions[sicxe_index].op1_hex = table8.Getisnum( z ) ;
										instructions[sicxe_index].opcode[0] = 2 ;
										instructions[sicxe_index].opcode[1] = 4 ;
										instructions[sicxe_index].size = 3 ;
										gPlace = gPlace + 3 ;
										sicxe_index = sicxe_index + 1 ;
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
						cout << "[Syntax Error] Redundant token!\n" ;

						strcpy( instructions[index].error, "[Syntax Error] Redundant token." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else {
					// error
					cout << instructions[index].input << "[Syntax Error] Unknown Pseudo Code!\n" ;

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

		void DoOperand( int2 temptable[10], int &i, bool ty4) {
			/*
				<m>： <imm>|<direct>|<indirect>|<index>|<literal>
				<imm>： # <symbol> | # decimal
				<direct>： <symbol> | address
				<indirect>： @ <symbol>
				<index>： <symbol>, X
				<literal>： = X'...' | C'...'
			*/
			int index = sicxe_index ;
			if ( temptable[i][0] == 0 ) {
				cout << "[Syntax Error] It's type3 or type4 and not RSUB\n" ;

				strcpy( instructions[index].error, "[Syntax Error] It's type3 or type4 and not RSUB\n" ) ;
				instructions[index].done = true ;    // do not process any more
				instructions[index].instrtype = -1 ; //  skipped
			}
			else{
				/*
					optype = 0 ) <imm>： # <symbol> | # decimal
					optype = 1 ) <direct>： <symbol> | address
					optype = 2 ) <indirect>： @ <symbol>
					optype = 3 ) <index>： <symbol>, X
					optype = 4 ) <literal>： = X'...' | C'...'
				*/

				if ( temptable[i][0] == 4 ) {
					if ( temptable[i][1] == 12 ) {
						// # <symbol> | # decimal
						if ( temptable[i+1][0] == 5 || ( temptable[i+1][0] == 6 && IfDec( temptable[i+1][1] ) ) ) {
							instructions[index].done = false ;
							if ( ty4 ) {
								instructions[index].instrtype = 4 ;
								instructions[index].size = 4 ;
							}
							else {
								instructions[index].instrtype = 3 ;
								instructions[index].size = 3 ;
							}

							instructions[index].location = gPlace ;
							instructions[index].op1_hex = false ; // #decimal or #<symbol>
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].opcode[0] = temptable[i-1][0] ;
							instructions[index].opcode[1] = temptable[i-1][1] ;
							instructions[index].type3_optype = 0 ; // optype = 0 ) <imm>： # <symbol> | # decimal
							i = i + 2 ;
						}
						else {
							cout << "[Syntax Error] There should be a symbol or number after # \n" ;
							strcpy( instructions[index].error, "[Syntax Error] There should be a symbol or number after #\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
						}
					}
					else if ( temptable[i][1] == 13 ) {
						// @ <symbol>
						if ( temptable[i+1][0] == 5 ) {
							instructions[index].done = false ;
							if ( ty4 ) {
								instructions[index].instrtype = 4 ;
								instructions[index].size = 4 ;
							}
							else {
								instructions[index].instrtype = 3 ;
								instructions[index].size = 3 ;
							}

							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i+1][0] ;
							instructions[index].op1[0][1] = temptable[i+1][1] ;
							instructions[index].op2[0][0] = 0 ;
							instructions[index].opcode[0] = temptable[i-1][0] ;
							instructions[index].opcode[1] = temptable[i-1][1] ;
							instructions[index].type3_optype = 2 ; // optype = 2 ) <indirect>： @ <symbol>
							i = i + 2 ;
						}
						else {
							cout << "[Syntax Error] There should be a symbol after @ \n" ;

							strcpy( instructions[index].error, "[Syntax Error] There should be a symbol after @\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
						}
					}
					else if ( temptable[i][1] == 11 ) {
						// = X''… | = C''…
						//   i   -> =
						// i + 1 -> '
						// i + 2 -> ( 6, # ) || ( 7, # )
						if ( temptable[i+1][0] == 4 && temptable[i+1][1] == 9 ) {
							if ( temptable[i+2][0] == 6 || temptable[i+2][0] == 7) {
								instructions[index].done = false ;
								if ( ty4 ) {
									instructions[index].instrtype = 4 ;
									instructions[index].size = 4 ;
								}
								else {
									instructions[index].instrtype = 3 ;
									instructions[index].size = 3 ;
								}
								instructions[index].location = gPlace ;
								instructions[index].opcode[0] = temptable[i-1][0] ;
								instructions[index].opcode[1] = temptable[i-1][1] ;
								instructions[index].type3_optype = 4 ; // optype = 4 ) <literal>： = X''… | C''

								if ( temptable[i+2][0] == 6 ) {
									instructions[index].op1_hex = true ;
								}

								word temp ;
								int ii = 0 ;

								if ( !table8.Find( temp, ii ) ) {
									table8.Add( temptable[i+2][0], temptable[i+2][1], temp ) ;
									table8.Find( temp, ii ) ;
								}

								instructions[index].op1[0][0] = 8 ;
								instructions[index].op1[0][1] = ii ;
								i = i + 4 ;
							}
							else {
								cout << "[Syntax Error] There is an error occur after X=' or C='\n" ;

								strcpy( instructions[index].error, "[Syntax Error] [Syntax Error] There is an error occur after X=' or C='\n" ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
							}
						}
						else {
							cout << "[Syntax Error] There should be '\'' after =\n" ;

							strcpy( instructions[index].error, "[Syntax Error] There should be '\'' after =\n" )   ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
						}
					}
				}
				else if ( temptable[i][0] == 5 ) {
					// <symbol> | <symbol>, X
					// need to check if ,X
					if ( temptable[i+1][0] == 0 ) {
						// symbol
						instructions[index].done = false ;
						if ( ty4 ) {
								instructions[index].instrtype = 4 ;
								instructions[index].size = 4 ;
							}
							else {
								instructions[index].instrtype = 3 ;
								instructions[index].size = 3 ;
						}

						instructions[index].location = gPlace ;
						instructions[index].op1[0][0] = temptable[i][0] ;
						instructions[index].op1[0][1] = temptable[i][1] ;
						instructions[index].op2[0][0] = 0 ;
						instructions[index].opcode[0] = temptable[i-1][0] ;
						instructions[index].opcode[1] = temptable[i-1][1] ;
						instructions[index].type3_optype =  1 ; // optype = 1 ) <direct>： <symbol> | address
						instructions[index].x = false ;
						i = i + 1 ;
					}
					else if ( temptable[i+1][0] == 4 && temptable[i+1][1] == 1 ) {
						if ( temptable[i+2][0] == 3 && temptable[i+2][1] == 2 ) {
							// <symbol>, X
							instructions[index].done = false ;
							if ( ty4 ) {
								instructions[index].instrtype = 4 ;
								instructions[index].size = 4 ;
							}
							else {
								instructions[index].instrtype = 3 ;
								instructions[index].size = 3 ;
							}

							instructions[index].location = gPlace ;
							instructions[index].op1[0][0] = temptable[i][0] ;
							instructions[index].op1[0][1] = temptable[i][1] ;
							instructions[index].op2[0][0] = temptable[i+2][0] ;
							instructions[index].op2[0][1] = temptable[i+2][1] ;
							instructions[index].opcode[0] = temptable[i-1][0] ;
							instructions[index].opcode[1] = temptable[i-1][1] ;
							instructions[index].type3_optype =  3 ; // optype = 3 ) <index>： <symbol>, X
							instructions[index].x = true ;
						}
						else{
							cout << "[Syntax Error] Missing operand 'X' after <symbol>\n" ;

							strcpy( instructions[index].error, "[Syntax Error] Missing operand 'X' after <symbol>\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
						}
					}
					else{
						printf( "[Syntax Error] Redundant operand : (%d, %d) ", temptable[i+1][0], temptable[i+1][1] ) ;
						printf( "there is redundant token after <symbol> and not ','\n" ) ;

						strcpy( instructions[index].error, "[Syntax Error] Redundant operand!" ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
					}

				}
				else if ( temptable[i][0] == 6 ) {
					// address
					int addr = 0 ;
					table6.GetAsHexNum( temptable[i][1], addr ) ;
					if ( addr <= 0xFFFF ) {
						instructions[index].done = false ;
						if ( ty4 ) {
							instructions[index].instrtype = 4 ;
							instructions[index].size = 4 ;
						}
						else {
							instructions[index].instrtype = 3 ;
							instructions[index].size = 3 ;
						}

						instructions[index].location = gPlace ;
						instructions[index].op1[0][0] = temptable[i][0] ;
						instructions[index].op1[0][1] = temptable[i][1] ;
						instructions[index].op1_hex = true ;
						instructions[index].op2[0][0] = 0 ;
						instructions[index].opcode[0] = temptable[i-1][0] ;
						instructions[index].opcode[1] = temptable[i-1][1] ;
						instructions[index].type3_optype = 1 ; // optype = 1 ) <direct>： <symbol> | address
					}
					else {
						cout << "[Syntax Error] Address might be out of range!\n" ;

						strcpy( instructions[index].error, "[Syntax Error] Address might be out of range!\n" ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
					}
				}
				else {
					// error
					printf( "[Syntax Error] Unknown Operand!\n" ) ;
					strcpy( instructions[index].error, "[Syntax Error] Unknown Operand!\n" ) ;
					instructions[index].done = true ;    // do not process any more
					instructions[index].instrtype = -1 ; //  skipped
				}

			}
		}


		int IfType3or4( int2 temptable[10], int i, bool &skip ) {
			type types ;
			bool ty4 = false ;
			int index = sicxe_index ;
			int op = 0 ;
			if ( temptable[i][0] == 4 && temptable[i][1] == 2 ) {
				i = i + 1 ;
				ty4 = true ;
			}

			if ( temptable[i][0] == 1 ) {
				table1.GetType( temptable[i][1], types ) ;
				if ( types[2] || types[3] ) {
					op = table1.SIC_CheckOP( temptable[i][1] ) ;
					if ( op == 0 ) {
							// RSUB
							int j = i+1 ;
							bool f = false ;
							while ( j != 10 && !f ) {
								if ( temptable[j][0] != 0 ) {
									f = true ;
								}
								j++ ;
							}
							if (f) {
								cout << "[Syntax Error] Instruction format error!\n" ;

								strcpy( instructions[index].error, "[Syntax Error] Instruction format error." ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}
							else {
								instructions[index].done = false ;
								instructions[index].location = gPlace ;

								if ( !ty4 ) {
									instructions[index].instrtype = 3 ;
									instructions[index].size = 3 ;
								}
								else{
									instructions[index].instrtype = 4 ;
									instructions[index].size = 4 ;
								}

								instructions[index].op1[0][0] = 0 ;
								instructions[index].op2[0][0] = 0 ;
								instructions[index].opcode[0]= temptable[i][0] ; // symbol should be stored in table[0]
								instructions[index].opcode[1] = temptable[i][1] ;
							}

							if ( ty4 ) return 4 ;
							else return 3 ;
					}
					else {
						int k = i+1 ;
						DoOperand( temptable, k, ty4 ) ;
						if ( ty4 ) return 4 ;
						else return 3 ;
					}

				}
				else return 0 ;// other type, another function would deal with that
			}
			else return 0 ; // not an instruction
		}

		bool IfType2( int2 temptable[10], int i, bool &skip ) {
			type types ;
			int index = sicxe_index ;
			if ( temptable[i][0] == 1 ) {
				table1.GetType( temptable[i][1], types ) ;
				if ( types[1] ) {
					// type 2, get operand
					int op1type = -1 ;
					int op2type = -1 ;
					table1.Type2_GetCheckList( temptable[i][1], op1type, op2type ) ;

					// type -1 : empty
					// type  0 : n
					// type  1 : reg
					int j = i + 1 ;
					if ( op1type == 0 ) {
						if ( temptable[j][0] == 6 ) {
							// number
							int temp_num = 0 ;
							table6.GetAsDecNum( temptable[j][1], temp_num ) ;
							if ( temp_num <= 0xFFFF ) {
								instructions[index].op1[0][0] = temptable[j][0] ;
								instructions[index].op1[0][1] = temptable[j][1] ;

							}
							else{
								cout << "[Syntax Error] Operand out of range!\n" ;

								strcpy( instructions[index].error, "[Syntax Error] Operand out of range!" ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}

						}
						else {
							cout << "[Syntax Error] This instruction require a number\n" ;

							strcpy( instructions[index].error, "[Syntax Error] This instruction require a number\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
							skip = true ;
						}
					}
					else if ( op1type == 1 ) {
						if ( temptable[j][0] == 3 ) {
							// reg
							instructions[index].op1[0][0] = temptable[j][0] ;
							instructions[index].op1[0][1] = temptable[j][1] ;
						}
						else{
							cout << "[Syntax Error] This instruction require a register\n" ;

							strcpy( instructions[index].error, "[Syntax Error] This instruction require a register\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
							skip = true ;
						}
					}
					else {
						if ( op1type == -1 ) {
							if ( temptable[j][0] == 3 ) {
								// reg
								instructions[index].op1[0][0] = temptable[j][0] ;
								instructions[index].op1[0][1] = temptable[j][1] ;
							}
						}
					}

					j = j + 2 ; // skip the ','
					if ( op2type == 0 ) {
						if ( temptable[j][0] == 6 ) {
							// number
							int temp_num = 0 ;
							table6.GetAsDecNum( temptable[j][1], temp_num ) ;
							if ( temp_num <= 0xFFFF ) {
								instructions[index].op2[0][0] = temptable[j][0] ;
								instructions[index].op2[0][1] = temptable[j][1] ;
							}
							else{
								cout << "[Syntax Error] Operand out of range!\n" ;

								strcpy( instructions[index].error, "[Syntax Error] Operand out of range!" ) ;
								instructions[index].done = true ;    // do not process any more
								instructions[index].instrtype = -1 ; //  skipped
								skip = true ;
							}
						}
						else{
							cout << "[Syntax Error] This instruction require a number\n" ;

							strcpy( instructions[index].error, "[Syntax Error] This instruction require a number\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
							skip = true ;
						}
					}
					else if ( op2type == 1 ) {
						if ( temptable[j][0] == 3 ) {
							// number
							instructions[index].op2[0][0] = temptable[j][0] ;
							instructions[index].op2[0][1] = temptable[j][1] ;
						}
						else{
							cout << "[Syntax Error] This instruction require a register\n" ;

							strcpy( instructions[index].error, "[Syntax Error] This instruction require a register\n" ) ;
							instructions[index].done = true ;    // do not process any more
							instructions[index].instrtype = -1 ; //  skipped
							skip = true ;
						}
					}
					else ;

					if ( !skip ) {
						instructions[index].done = false ;
						instructions[index].instrtype = 2 ;
						instructions[index].location = gPlace ;
						// printf( "op1type is : %d\n", op1type ) ;
						instructions[index].type2_op1type = op1type ;
						instructions[index].type2_op2type = op2type ;
						instructions[index].opcode[0] = temptable[i][0] ;
						instructions[index].opcode[1] = temptable[i][1] ;
						instructions[index].size = 2 ;
					}
				}
				else return false ;// other type, another function would deal with that
			}
			else return false ; // not an instruction
		}

		bool IfType1( int2 temptable[10], int i, bool &skip) {
			type types ;
			int index = sicxe_index ;
			if ( temptable[i][0] == 1 ) {
				table1.GetType( temptable[i][1], types ) ;
				if ( types[0] ) {
					// type 1
					bool redundant = false ;
					int j = i+1 ;
					 while ( j < 10 ) {
					 	if ( temptable[j][0] != 0 ) redundant = true ;
						j++ ;
					}

					if ( !redundant ) {
						// there should NOT be any operand
						instructions[index].done = false ;
						instructions[index].instrtype = 1 ;
						instructions[index].location = gPlace ;
						instructions[index].opcode[0] = temptable[i][0] ;
						instructions[index].opcode[1] = temptable[i][1] ;
						instructions[index].size = 1 ;
					}
					else {
						cout << "[Syntax Error] Redundant token\n" ;

						strcpy( instructions[index].error, "[Syntax Error] Redundant token." ) ;
						instructions[index].done = true ;    // do not process any more
						instructions[index].instrtype = -1 ; //  skipped
						skip = true ;
					}
				}
				else return false ;// other type, another function would deal with that
			}
			else return false ; // not an instruction
		}

		bool IfInstr( int2 temptable[10], int i, bool &skip ) {
			int index = sicxe_index ;
			bool dontcare = false ;
			if ( IfType1( temptable, i, dontcare ) ) {
				gPlace = gPlace + 1 ;
				return true ;
			}
			else if ( IfType2( temptable, i, dontcare ) ) {
				gPlace = gPlace + 2 ;
				return true ;
			}
			else if ( IfType3or4( temptable, i, dontcare ) == 3 ) {
				gPlace = gPlace + 3 ;
				return true ;
			}
			else if ( IfType3or4( temptable, i, dontcare ) == 4 ) {
				gPlace = gPlace + 4 ;
				return true ;
			}
			else if ( ( temptable[i][0] == 1 ) || ( temptable[i][0] == 4 && temptable[i][1] == 2 ) ) {
				// error
        if ( strcpy( instructions[index].input, "fix" ) == 0 ) {
          ;
        }
        else {
          cout << "[Syntax Error] Incorrect instruction format!\n" ;

          strcpy( instructions[index].error, "[Syntax Error] Unknown Pseudo Code." ) ;
          instructions[index].done = true ;    // do not process any more
          instructions[index].instrtype = -1 ; //  skipped
          skip = true ;
          return true ;
        }
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
  	SICXESource(){
			sicxe_index = 0 ;
			Set() ;
		}

		void Add( str100 source, str100 input ) {
			int temp_index = sicxe_index ;

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

			StrToArr( source, temp ) ;
			for( int j = 0 ; j < 10 ; j++ ) {
				Function::CheckTokenValue( temp[j], temptable[j][0], temptable[j][1] ) ;
			}

			if ( FindSymbol( temptable[i][0], temptable[i][1], duplicate ) ) {
				// symbol might be duplicate, so need to check if duplicate
				if ( duplicate ) {
					instructions[temp_index].done = true ;    // do not process any more
					instructions[temp_index].instrtype = -1 ; //  skipped
					skip = true ;
					printf( "[Syntax Error] While dealing with %s : ", source ) ;
					printf( "Duplicate Symbol occur!\n " ) ;
				}
				else{
					instructions[temp_index].symbol[0] = temptable[i][0] ;
					instructions[temp_index].symbol[1] = temptable[i][1] ;
					i++ ;
				}
			}

			// if FindSymbol and not duplicate, get the next token.
			// if !FindSymbol then the token remained

			if ( !skip ) {
				if ( IfInstr( temptable, i, skip ) ) ;
				else if ( IfPseudo( temptable, i, skip ) );
				else if ( temptable[i][0] == 4 && temptable[i][1] == 10 ) {
					// comment
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

			sicxe_index++ ;
		}

		void Trans( FILE *fp ) {
			int i = 0 ;
			long long int tempcode = 0 ;
			int temp = 0 ;
			fprintf( fp, "Line\t\tLoc\t\tSource statement\tObject code\n" ) ;

			while ( i < sicxe_index ) {
				tempcode = 0 ;
				temp = 0 ;

				if ( !instructions[i].done ) {
					if ( instructions[i].instrtype == 1 ) {
						tempcode = table1.GetOPCode( instructions[i].opcode[1] ) ;
						fprintf( fp, "%d\t%04X\t%s\t%02X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;

					// =============================================================================================================

					}
					else if ( instructions[i].instrtype == 2 ) {
						// opcode	op1	op2
						//   8       4   4

						// type -1 : empty
						// type  0 : n
						// type  1 : reg

						tempcode = table1.GetOPCode( instructions[i].opcode[1] ) ;
						tempcode = tempcode * 0x100 ;
						int r1 = 0 ;
						int r2 = 0 ;
						if ( instructions[i].type2_op1type == 0 ) {
							// r1 = n
							table6.GetAsDecNum( instructions[i].op1[0][1], r1 ) ; // check before

						}
						else if ( instructions[i].type2_op1type == 1 ) {
							r1 = instructions[i].op1[0][1] - 1 ; // the index of register is begin form 1, but register accually begin from 0
						}
						else {
							printf( "%d  ) ", instructions[i].type2_op1type  ) ;
							printf( "Bug in SICXE Trans type2\n" ) ;
						}

						//printf( "%s\n", instructions[i].input ) ;
						//printf( "operands ( %d, %d )\n", instructions[i].op2[0][0], instructions[i].op2[0][1] ) ;
						if ( instructions[i].type2_op2type == 0 ) {
							// r2 = n
							table6.GetAsDecNum( instructions[i].op2[0][1], r2 ) ; // check before
							if ( table1.Type2_r2_minus1( instructions[i].opcode[1] ) ) {
								r2 = r2 - 1 ;
							}
							else ;
						}
						else if ( instructions[i].type2_op2type == 1 ) {
							r2 = instructions[i].op2[0][1] - 1  ; // the index of register is begin from 1 so - 1
						}
						else r2 = 0x0 ;

						tempcode = tempcode + r1 * 0x10 ;
						tempcode = tempcode + r2 ;

						fprintf( fp, "%d\t%04X\t%s\t%04X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
					// ===============================================================================================================

					}
					else if ( instructions[i].instrtype == 3 ) {
						tempcode = table1.GetOPCode( instructions[i].opcode[1] ) ;
						tempcode = tempcode >> 2 ; // 6bit
						tempcode = tempcode << 2 ;
						bool p = false ;
						/*
							optype = 0 ) <imm>： # <symbol> | # decimal
							optype = 1 ) <direct>： <symbol> | address
							optype = 2 ) <indirect>： @ <symbol>
							optype = 3 ) <index>： <symbol>, X
							optype = 4 ) <literal>： = X''… | C''
						*/

						// ==========
						// to check the flag of n and i
						if ( instructions[i].opcode[0] == 1 && instructions[i].opcode[1] == 36 ) {
							tempcode = tempcode + 3 ;
						}
						else if ( instructions[i].type3_optype == 0 ) {
							tempcode = tempcode + 1 ; // imm : n = 0, i = 1
						}
						else if ( instructions[i].type3_optype == 1 ) {
							if ( instructions[i].op1[0][0] == 6 ) {
								// address, n = 0, i = 0
								tempcode = tempcode + 3 ;
							}
							else {
								// symbol, n = 1, i = 1
								tempcode = tempcode + 3 ;
							}
						}
						else if ( instructions[i].type3_optype == 2 ) {
							tempcode = tempcode + 2 ; // n = 1, i = 0 ;
						}
						else if ( instructions[i].type3_optype == 3 ) {
							tempcode = tempcode + 3 ;
						}
						else if ( instructions[i].type3_optype == 4 ) {
							tempcode = tempcode + 3 ;
						}
						else {
							printf( "Debug Error in Trans type3\n" ) ;
							printf( "%d \t %s\n", instructions[i].location, instructions[i].input ) ;
							printf( "( %d, %d )", instructions[i].opcode[0], instructions[i].opcode[1] ) ;
							printf( "( %d, %d )", instructions[i].op1[0][0], instructions[i].op1[0][1] ) ;

							printf( "type3_optype is : %d", instructions[i].type3_optype ) ;
						}

						tempcode = tempcode * 0x10 ; // 8 bit done
													 // to make a space for x, b, p, e

						// ===========
						// b p x e

						if ( instructions[i].type3_optype == 0 ) {
						 // b=p=0
							if ( instructions[i].opcode[0] == 1 && instructions[i].opcode[1] == 36 ) ; // p = 0
							else {
								if ( instructions[i].op1[0][0] == 5 ) {
									p = true ;
									tempcode = tempcode + 0x2 ; // p = 1
								}
								else ; // p = 0
							}
						}
						else if ( instructions[i].type3_optype == 4 ) {
							p = true ;
							tempcode = tempcode + 0x2 ;
						}
						else {
							if ( instructions[i].op1[0][0] == 5 ) {
								// symbol
								p = true ;
								tempcode = tempcode + 0x2 ; // p
							}
							else if ( instructions[i].op1[0][0] == 6 ) {

								/* see the number as address
								int num = 0 ;
								table6.GetAsDecNum( instructions[i].op1[0][1], num ) ;
								if ( num == 0 || num == instructions[i+1].location ) ; // p = 0 ;
								else {
									p = true ;
									tempcode = tempcode + 0x2 ; 					  // p
								}
								*/
							}
							else if ( instructions[i].op1[0][0] == 8 ) ; // p = 0
							else ; // p = 0
						}

						if ( instructions[i].x ) tempcode = tempcode + 0x8 ; // x

						// to Get the value of operand
						if ( instructions[i].op1[0][0] == 5 ) {
							temp = table5.GetAddr( instructions[i].op1[0][1] ) ;
						}
						else if ( instructions[i].op1[0][0] == 6 ) {
							if ( instructions[i].type3_optype == 0 ) {
								table6.GetAsDecNum( instructions[i].op1[0][1], temp ) ;
							}
							else {
								table6.GetAsHexNum( instructions[i].op1[0][1], temp ) ; // address
							}
						}
						else if ( instructions[i].op1[0][0] == 8 ) {
							temp = table8.GetAddr( instructions[i].op1[0][1] ) ;
						}
						else if ( instructions[i].opcode[0] == 1 && instructions[i].opcode[1] == 36 ) temp = 0 ;
						else {
							printf( "[fatal] Bug in sicxe trans type3 to fetch operand\n" ) ;
							printf( "instr : %s\n", instructions[i].input ) ;
							printf( "(%d,%d)\n", instructions[i].opcode[0], instructions[i].opcode[1] ) ;
							printf( "type3_optype : %d\n", instructions[i].type3_optype ) ;
							printf( "(%d,%d)", instructions[i].op1[0][0], instructions[i].op1[1][1] ) ;

						}

						// ===============
						// Calculate the DISP with program counter

						if ( instructions[i].op1[0][0] == 5 ) temp = temp - instructions[i+1].location ; // DISP = TA - PC
						else if ( instructions[i].op1[0][0] == 8 ) temp = temp - instructions[i+1].location ;
						else if ( instructions[i].opcode[0] == 1 && instructions[i].opcode[1] == 36 ) temp = 0 ;
						else ;

						// ==============
						// Check if the value out of range, if out of range
						// try to use base register to fix the error
						// else report error

						if ( ( temp >= 0x0000 && temp <= 0x07FF ) || ( temp >= -2047 && temp <= -1 ) ) {
							if ( temp >= 0x0000 && temp <= 0x07FF ) ;  // normal
							else {
								temp = temp - 0xFFFFF000 ;
							}
							fprintf( fp, "%d\t%04X\t%s\t%03X%03X\n", i, instructions[i].location, instructions[i].input, tempcode, temp ) ;
						}
						else if ( use_base ) {
							// overflow when use pc
							// get the oringinal TA
							if ( instructions[i].op1[0][0] == 5 ) temp = temp + instructions[i+1].location ; // TA = TA - PC + PC
							else if ( instructions[i].op1[0][0] == 8 ) temp = temp + instructions[i+1].location ;
							else temp = 0 ;

							// TA - base ;
							if ( instructions[i].op1[0][0] == 5 ) temp = temp - gBase ; // DISP = TA - base
							else if ( instructions[i].op1[0][0] == 8 ) temp = temp - gBase ;
							else if ( instructions[i].opcode[0] == 1 && instructions[i].opcode[1] == 36 ) temp = 0 ;
							else temp = 0;

							if ( ( temp >= 0x0000 && temp <= 0x07FF ) || ( temp >= -2047 && temp <= -1 ) ) {
								if ( p ) {
									// when p is true, tempcode + 2.
									// when b is true, tempcode + 4.
									// when p is true, means it need to be relocated if fail to relocate with pc
									// try base register if use_base, and we can know if relocation is needed with  p

									tempcode = tempcode + 0x2 ;
								}
								else ; // p is false mean no relocation is nneded

								if ( temp >= 0x0000 && temp <= 0x07FF ) ;  // positive
								else {
									temp = temp - 0xFFFFF000 ;
								}

								fprintf( fp, "%d\t%04X\t%s\t%03X%03X\n", i, instructions[i].location, instructions[i].input, tempcode, temp ) ;
							}
							else {
								printf( "dec : %d ) ", temp  ) ;
								printf( "%X : %s\n", temp, "[Syntax Error] The disp might be over flow in trans type 3\n" ) ;
								printf( "Using PC to relocate : Failure, Try to relocate with base register : failure" ) ;
								fprintf( fp, "%d\t%s\n", i, "[Syntax Error] The disp might be over flow in trans type 3" ) ;
							}

						}
						else {
							printf( "dec : %d ) ", temp  ) ;
							printf( "%X : %s\n", temp, "[Syntax Error] The disp might be over flow in trans type 3\n" ) ;
							fprintf( fp, "%d\t%s\n", i, "[Syntax Error] The disp might be over flow in trans type 3" ) ;
						}

					// ================================================================================================================

					}
					else if ( instructions[i].instrtype == 4 ) {
						tempcode = table1.GetOPCode( instructions[i].opcode[1] ) ;
						tempcode = tempcode >> 2 ; // 6bit
						tempcode = tempcode << 2 ; // 6bit + 2b'0

						/*
							optype = 0 ) <imm>： # <symbol> | # decimal
							optype = 1 ) <direct>： <symbol> | address
							optype = 2 ) <indirect>： @ <symbol>
							optype = 3 ) <index>： <symbol>, X
							optype = 4 ) <literal>： = X''… | C''
						*/
						if ( instructions[i].opcode[0] == 1 && instructions[i].opcode[1] == 36 ) {
							tempcode = tempcode + 3 ;
						}
						else if ( instructions[i].type3_optype == 0 ) {
							tempcode = tempcode + 1 ; // imm : n = 0, i = 1
						}
						else if ( instructions[i].type3_optype == 1 ) {
							if ( instructions[i].op1[0][0] == 6 ) {
								// address, n = 0, i = 0
								tempcode = tempcode + 3 ;
							}
							else {
								// symbol, n = 1, i = 1
								tempcode = tempcode + 3 ;
							}
						}
						else if ( instructions[i].type3_optype == 2 ) {
							tempcode = tempcode + 2 ; // n = 1, i = 0 ;
						}
						else if ( instructions[i].type3_optype == 3 ) {
							tempcode = tempcode + 1 ;
						}
						else if ( instructions[i].type3_optype == 4 ) {
							tempcode = tempcode + 3 ;
						}
						else {
							printf( "Debug Error in Trans type4\n" ) ;
						}


						tempcode = tempcode * 0x1000000 ;
						if ( instructions[i].x ) tempcode = tempcode + 0x800000 ; // x

						if ( instructions[i].instrtype == 4 ) tempcode = tempcode + 0x100000 ; // e = 1
						else ; // type 3, e = 0



						if ( instructions[i].op1[0][0] == 5 ) temp = table5.GetAddr( instructions[i].op1[0][1] ) ;
						else if ( instructions[i].op1[0][0] == 6 ) {
							if ( instructions[i].type3_optype == 0 ) {
								table6.GetAsDecNum( instructions[i].op1[0][1], temp ) ;
							}
							else {
								table6.GetAsHexNum( instructions[i].op1[0][1], temp ) ; // address
							}
						}
						else if ( instructions[i].op1[0][0] == 8 ) {

							temp = table8.GetAddr( instructions[i].op1[0][1] ) ;
						}
						else temp = table8.GetAddr( instructions[i].op1[0][1] ) ;

						tempcode = tempcode + temp ;
						fprintf( fp, "%d\t%04X\t%s\t%08X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;

					// ========================================================================================

					}
					else if ( instructions[i].instrtype == 0 ) {
						if ( instructions[i].opcode[1] == 3 ) {
							// BYTE
							if ( instructions[i].op1[0][0] == 8 ) {
								tempcode = table8.GetOP( instructions[i].op1[0][1] ) ;
								fprintf( fp, "%d\t%X\t%s\t%2X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
							else if ( instructions[i].op1[0][0] == 6 ) {
								if ( instructions[i].op1_hex )	table6.GetAsHexNuml( instructions[i].op1[0][1], tempcode ) ;
								else table6.GetAsDecNuml( instructions[i].op1[0][1], tempcode ) ;
								fprintf( fp, "%d\t%04X\t%s\t%2X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
							else {
								word s ;
								memset( s, '\0', WORDLENGTH ) ;
								table7.GetString( instructions[i].op1[0][1], s ) ;
								for( int j = 0 ; j < WORDLENGTH && s[j] != '\0' ; j++ ) {
									tempcode = tempcode * 0x100 ;
									tempcode = tempcode + s[j] ;
								}
								fprintf( fp, "%d\t%04X\t%s\t%X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
						}
						else if ( instructions[i].opcode[1] == 4 ) {
							// WORD
							if ( instructions[i].op1[0][0] == 8 ) {
								tempcode = table8.GetOP( instructions[i].op1[0][1] ) ;
								fprintf( fp, "%d\t%04X\t%s\t%06X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
							else if ( instructions[i].op1[0][0] == 6 ) {
								if ( instructions[i].op1_hex )	table6.GetAsHexNuml( instructions[i].op1[0][1], tempcode ) ;
								else table6.GetAsDecNuml( instructions[i].op1[0][1], tempcode ) ;
								fprintf( fp, "%d\t%04X\t%s\t%06X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
							else {
								word s ;
								memset( s, '\0', WORDLENGTH ) ;
								table7.GetString( instructions[i].op1[0][1], s ) ;
								for( int j = 0 ; j < WORDLENGTH ; j++ ) {
									tempcode = tempcode * 0x100 ;
									tempcode = tempcode + s[j] ;
								}
								fprintf( fp, "%d\t%04X\t%s\t%X\n", i, instructions[i].location, instructions[i].input, tempcode ) ;
							}
						}
						else if ( instructions[i].opcode[1] == 7 ) {
							// EQU
							if ( instructions[i].op1[0][0] == 5 ) {
								// Symbol EQU Symbol
								int j = 1 ;
								temp = 0 ;
								int tempequ = 0 ;
								temp = table5.GetAddr( instructions[i].op1[0][1] ) ;
								while (  j < 5 && instructions[i].op1[j][0] == 4 ) {
									if ( instructions[i].op1[j][0] == 4 && instructions[i].op1[j][1] == 2 ) {
										temp = temp + table5.GetAddr( instructions[i].op1[j+1][1] ) ;
									}
									else if ( instructions[i].op1[j][0] == 4 && instructions[i].op1[j][1] == 3 ) {
										temp = temp - table5.GetAddr( instructions[i].op1[j+1][1] ) ;
									}
									else if ( instructions[i].op1[j][0] == 4 && instructions[i].op1[j][1] == 4 ) {
										temp = temp * table5.GetAddr(instructions[i].op1[j+1][1] ) ;
									}
									else if ( instructions[i].op1[j][0] == 4 && instructions[i].op1[j][1] == 5 ) {
										temp = temp / table5.GetAddr( instructions[i].op1[j+1][1] ) ;
									}
									else ;
								}

								table5.ModifyAddressToBe( instructions[i].symbol[1], temp ) ;
								instructions[i].location = temp ;
								fprintf( fp, "%d\t%04X\t%s\n", i, instructions[i].location, instructions[i].input ) ;
							}
							else if ( instructions[i].op1[0][0] == 4 && instructions[i].op1[0][0] == 4 ) {
								table5.ModifyAddressToBe( instructions[i].symbol[1], gPlace ) ;
								instructions[i].location = gPlace ;
								fprintf( fp, "%d\t%04X\t%s\n", i, instructions[i].location, instructions[i].input ) ;
							}
							else {
								// Symbol EQU Dec
								table6.GetAsDecNum( instructions[i].op1[0][1], temp ) ;
								table5.ModifyAddressToBe( instructions[i].symbol[1], temp ) ;
								instructions[i].location = temp ;
								fprintf( fp, "%d\t%04X\t%s\n", i, instructions[i].location, instructions[i].input ) ;
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
							fprintf( fp, "%d\t%s\n", i, instructions[i].input ) ; // V5, LAST MODIFIED : HERE
						}
					}
					else {
						//
						printf( "[Unknown Error] In trans\n" ) ;
						printf( "Not an instr and pseudo\n" ) ;
						printf( "Might Not be here\n" ) ;
					}
				}
				else if ( instructions[i].done && instructions[i].instrtype >= 0 ) {
					fprintf( fp, "%d\t%04X\t%s\n", i, instructions[i].location, instructions[i].input ) ; // pseudo finished
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