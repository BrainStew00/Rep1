#include <stdio.h>
#include <stdbool.h>
#include "user_input.h"
#include "debug.h"

bool Debug_On = true;                       // true: Attiva l visualizzazione
bool Debug_Blocca_per_visualizzare = false; // true: attiva la sospensione dell'esecuzione ogni Debug_Cnt_BreakIncrement
int  Debug_Cnt_BreakIncrement = 5;          // Ogni quante stampe di debug si ferma
int  Debug_NextBreakCnt = 0;                // Prossimo valore di contatore per cui si blocca l'esecuzione
int  Debug_Cnt = 0;
double Debug_AumentoSogliaVelocita_Max = 10;

int debug_integrazione_stampa_ogni_n_cicli;
int debug_integrazione_conta_cicli = 0;
bool debug_integrazione_attiva_stampa_in_questo_ciclo = false;
bool debug_integrazione_stampa_residui_in_riga = false;

/* funzione di print di debug
 * Stampa :
 * numero progressivo, messaggio, valore
 * Esempio
 * debug_print_double("deltaT", deltaT);
 * visualizza:
 * [ 23] deltaT 1230.47*/

void debug_print_double(char *msg, double value)
{
	if (Debug_On)
	{
		printf("\n[%3d] %s %f", Debug_Cnt, msg, value);	    // Stampa dato
		if (Debug_Blocca_per_visualizzare)                  // Se richiesto l'arresto per visualizzazione
		{
			if (Debug_Cnt >= Debug_NextBreakCnt)                   // Se contatore ha superato la soglia
			{
				user_input_StampaMsgAttendiTasto("");           // Interrompi e attendi la pressione del tasto
				Debug_NextBreakCnt += Debug_Cnt_BreakIncrement;         // Prossima interruzione tra Debug_Cnt_Increment visualizzazioni
			}
		}

		Debug_Cnt++;                                        // Aggiorna contatore
	}
}


void debug_integrazione_abilita_visualizzazione(void) // Chiede all'utente se stampare le informazioni di debug relative a integrazione.c
{
	printf("\nScegli ogni quanti cicli stampare i residui (0: nessuna stampa):");
	scanf("\%i", &debug_integrazione_stampa_ogni_n_cicli);
	printf("\n");

	if (debug_integrazione_stampa_ogni_n_cicli != 0) { // Se e' richiesta la visualizzazione dei residui
		int temp;
		temp = user_input_richiedi_int("Scegli tipo di stampa [1:in colonna, 2: in riga]", 1, 2);
		if (temp == 1) {
			debug_integrazione_stampa_residui_in_riga = false;
		}
		else {
			debug_integrazione_stampa_residui_in_riga = true;
		}
	}
}

/**
 * Restituisce il valore true se la stampa di debug e' attiva
 */
bool debug_integrazione_visualizzazione_abilitata(void)
{
	bool bRetVal;

	if (debug_integrazione_stampa_ogni_n_cicli != 0)  // Se e' richiesta la visualizzazione dei residui
	{
		bRetVal = true;
	}
	else
	{
		bRetVal = false;
	}
	return bRetVal;
}


void debug_integrazione_aggiorna_contatore_cicli(void)
{
	if (debug_integrazione_stampa_ogni_n_cicli == 0) { // Se stampa di debug non richiesta
		debug_integrazione_attiva_stampa_in_questo_ciclo = false;             // Azzera flag di abilitazione stampa
	}
	else {                                             // Se stampa di debug richiesta abilita la stampa per un ciclo ogni debug_integrazione_stampa_ogni_n_cicli
		if (debug_integrazione_conta_cicli < debug_integrazione_stampa_ogni_n_cicli) {
			debug_integrazione_conta_cicli++;
			debug_integrazione_attiva_stampa_in_questo_ciclo = false;
		}
		else {
			debug_integrazione_conta_cicli = 0;
			debug_integrazione_attiva_stampa_in_questo_ciclo = true;
		}
	}
}

bool debug_integrazione_stampa_in_questo_ciclo(void)
{
	return debug_integrazione_attiva_stampa_in_questo_ciclo;
}

void debug_integrazione_stampa_residui(double t, double residui[]) // Stampa dei residui come da impostazioni utente
{
	int i;
	static int contalinee = 1000;

	if (debug_integrazione_stampa_residui_in_riga == false) {
		printf("t:                %f\n", t);
		printf("Residuo u:        %f\n", residui[0]);
		printf("Residuo v:        %f\n", residui[1]);
		printf("Residuo w:        %f\n", residui[2]);
		printf("Residuo p:        %f\n", residui[3]);
		printf("Residuo q:        %f\n", residui[4]);
		printf("Residuo r:        %f\n", residui[5]);
		printf("Residuo fi:       %f\n", residui[6]);
		printf("Residuo theta:    %f\n", residui[7]);
		printf("Residuo psi:      %f\n", residui[8]);
		printf("Residuo h:        %f\n", residui[9]);
	}
	else { // Stampa residui in riga
		if (++contalinee >= 30) { // Titolo ogni 30 righe
			contalinee = 0;
			// 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789 123456789
			printf("       t          u          v          w          p          q          r         fi      theta        psi          h\n");
		}

		printf("%8.2f", t);
		for (i = 0; i <= 9; i++) {
			printf("%11.6f", residui[i]);
		}
		printf("\n");
	}
}


