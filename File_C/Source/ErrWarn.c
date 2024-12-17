/**
 * ErrWarn.C
 * Collezionatore di errori e warning
 */

 #include <stdio.h>
 #include <stdlib.h>
 #include "user_input.h"
 #include "ErrWarn.h"


 void ErrWarn_Initialize(void)
 {


 }

 void ErrWarn_DisplayErrMsgAndExit(int ErrNum, char *msg)
 {
	 printf("\n"
			"\n"
			"****************\n"
			"* ERRORE E%03d *\n"
			"****************\n", ErrNum);
	 printf("\n"
			"%s\n"
			"La simulazione viene interrotta\n", msg);
	 user_input_StampaMsgAttendiTasto("");
	 exit(ErrNum);
 }

 void ErrWarn_DisplayWarnMsgAndContinue(int WarnNum, char *msg)
 {
	 static int lastWarNum=-9999;
	 if(WarnNum != lastWarNum) // Se warning da stampare diverso dal precedente
	 {
		 lastWarNum = WarnNum; // Salva warning number
		 printf("\n"); // Vai a capo
	 }
	 printf("\rWARNING W%03d: %s", WarnNum, msg); // Torna a inizio riga e stampa il messaggio
 }
