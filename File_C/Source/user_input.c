#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "dati.h"
#include "chart.h"
#include "ErrWarn.h"
#include "Manovre.h"
#include "user_input.h"


#define DEFAULT_V                             52
#define DEFAULT_H                           1000
#define DEFAULT_GAMMA                          0
#define DEFAULT_SIMUL_MASSA_VAR            false
#define DEFAULT_DURATA_SIMULAZ               300
#define DEFAULT_DELTA_T                     0.01
#define DEFAULT_TITOLO_SIM                   "-"
#define DEFAULT_LIMITAZIONE_POTENZA_ATTIVA false



/**
 * Menu' iniziale

 * Il file 'input/dati_iniziali.txt' viene usato per memorizzare le impostazioni dell'utente.
 * Dal file vengono lette le condizioni iniziali di volo (quota/velocita'/gamma) e il nome del file di salvataggio.
 * Se il file non esiste o se ci sono errori nella lettura imposta i valori di default.
 * Propone un menù per modificare i valori o per impostare i valori di default.
 * Al termine salva le impostazioni nello stesso file per poterle rileggere alla successiva esecuzione.
 */
void user_input_ini_menu(USERVAL *pUserValues){
	bool finito;
	int comando;
	int comando_max;
	bool lettura_corretta;
	bool file_esiste;
	bool aggiorna_nome_file;


	lettura_corretta = user_input_leggi_dati_iniziali("input/dati_iniziali.txt", pUserValues);
	if (lettura_corretta == true)
	{
		printf("\nLettura dati iniziali dal file input\\dati_iniziali.txt: Dati ripristinati con successo\n");
	}
	else	// Se errore nella lettura imposta condizioni di default;
	{
	    ErrWarn_DisplayWarnMsgAndContinue(W009, "Errore nella lettura del file input\\dati_iniziali.txt\n"
	    		                                "Verranno impostati i valori di default");
		pUserValues->Gamma = DEFAULT_GAMMA;
		pUserValues->V     = DEFAULT_V;
		pUserValues->h     = DEFAULT_H;
		pUserValues->simMassaVariabile = DEFAULT_SIMUL_MASSA_VAR;
		pUserValues->durataSimulazione = DEFAULT_DURATA_SIMULAZ;
		pUserValues->deltaT = DEFAULT_DELTA_T;
		strcpy(pUserValues->titoloSim, DEFAULT_TITOLO_SIM);
		pUserValues->LimitazioneDiPotenzaAttiva = DEFAULT_LIMITAZIONE_POTENZA_ATTIVA;
		chart_autoset_data_filename(pUserValues);
	}

	finito = false;
	while (!finito) {
		printf("\n"
               "+-----------------------------------------------------------------------------+\n"
               "|                                                                             |\n"
               "|                        SIMULATORE CESSNA 172 SKYHAWK                        |\n"
               "|                                                                             |\n"
               "| Gruppo di lavoro 2                                              Giugno 2024 |\n"
		       "+-----------------------------------------------------------------------------+\n");

		printf("\n"
               "Modifica dei parametri di simulazione:\n"
               "\n");
		printf("[1] Titolo simulazione:          %s\n",   pUserValues->titoloSim);
		printf("[2] Quota:                       %4.0f [m]\n",   pUserValues->h);
		printf("[3] Velocita':                  %5.2f [m/s]\n", pUserValues->V);
        printf("[4] Angolo di rampa:            %5.2f [\370]\n", pUserValues->Gamma); // \370
        printf("[5] Durata simulazione:        %6.0f [s]\n", pUserValues->durataSimulazione);
        printf("[6] Passo di integrazione:      %.3f [s]\n", pUserValues->deltaT);
        printf("[7] Simulazione a massa variabile: ");
        if (pUserValues->simMassaVariabile) {
			printf("SI\n");
        }
        else {
			printf("NO\n");
        }
        printf("[8] Limitazione di potenza:        ");
        if (pUserValues->LimitazioneDiPotenzaAttiva) {
			printf("Attiva\n");
        }
        else {
			printf("Non attiva\n");
        }

        printf("[9] Nome del file di salvataggio dati: %s.dat\n", pUserValues->nomeFileSalvataggioDati);
        file_esiste = chart_check_if_dat_file_exist(pUserValues->nomeFileSalvataggioDati);
        if (file_esiste){
        	printf("    *** ATTENZIONE: il file esiste e verra' sovrascritto ***\n");
        }
        printf("[10] Imposta automaticamente nome del file in base ai parametri iniziali\n");

        printf("\n");
        printf("[11] Imposta dati di default\n");
        comando_max = 11;                       // MAx comando accettabile
        #if (INCLUDI_MANOVRE_DI_TEST == 1)
        {
			printf("[12] Manovra di test 1: Impulso di alettone e rampa di manetta\n"                      );
			printf("[13] Manovra di test 2: Virata circolare a destra, h costante\n"                       );
			printf("[14] Manovra di test 3: Virata con discesa a sinistra e livellamento finale\n"         );
			printf("[15] Manovra di test 4: Virata a destra di 180 gradi a quota e modulo di velocità costante\n");
			comando_max += 4;                       // MAx comando accettabile
        }
        #endif
        printf("[0] Esegui simulazione con i parametri visualizzati\n");

		comando = user_input_richiedi_int("\n Inserisci comando:\n", 0, comando_max);

		aggiorna_nome_file = false;
		switch(comando)
		{
			case 0:
				user_input_salva_dati_iniziali("input/dati_iniziali.txt", pUserValues); // Salva i dati correnti
				finito = 1;
				break;

			case 1: // Titolo
				user_input_richiedi_stringa("Inserisci titolo della simulazione", pUserValues->titoloSim);
				user_input_elimina_spazi_ini_fin(pUserValues->titoloSim); // Elimina eventuali spazi iniziali e finali
				aggiorna_nome_file = true;
				break;
			case 2: // Quota
				pUserValues->h = (double)user_input_richiedi_int("Inserire la quota", 0, 4116); // Richiedi intero, converti in double
				aggiorna_nome_file = true;
				break;

			case 3: // Velocita
				pUserValues->V = user_input_richiedi_double("Inserire la velocita'", 30, 65);
				aggiorna_nome_file = true;
				break;

			case 4: // Angolo di rampa
				pUserValues->Gamma = user_input_richiedi_double("Inserire l'angolo di rampa", -30, 30);
				aggiorna_nome_file = true;
				break;

			case 5: // Durata simulazione
				pUserValues->durataSimulazione = (double)user_input_richiedi_int("Inserire durata della simulazione", 1, 100000); // Richiedi intero, converti in double
				aggiorna_nome_file = true;
				break;

			case 6: // Passo di integrazione
				pUserValues->deltaT = user_input_richiedi_double("Inserire passo di integrazione", 0.001, 1.0);
				break;

			case 7: // Simulazione a massa variabile
				if (pUserValues->simMassaVariabile) {
					pUserValues->simMassaVariabile = false;
				}
				else{
					pUserValues->simMassaVariabile = true;
				}
				aggiorna_nome_file = true;
				break;

			case 8: // Limitazione di potenza attiva
				if (pUserValues->LimitazioneDiPotenzaAttiva) {
					pUserValues->LimitazioneDiPotenzaAttiva = false;
				}
				else{
					pUserValues->LimitazioneDiPotenzaAttiva = true;
				}
				aggiorna_nome_file = true;
				break;

			case 9: // InseriNome del file di salvataggio dati
				chart_ask_filename(pUserValues->nomeFileSalvataggioDati);
				break;

			case 10: // Imposta automaticamente nome del file in base ai parametri iniziali
				    // aggiungendo un numero coda in modo che il file non esista
				chart_autoset_data_filename(pUserValues);
				break;

			case 11: // Imposta dati di default (tranne il titolo)
				pUserValues->Gamma = DEFAULT_GAMMA;
				pUserValues->V     = DEFAULT_V;
				pUserValues->h     = DEFAULT_H;
				pUserValues->simMassaVariabile = DEFAULT_SIMUL_MASSA_VAR;
				pUserValues->durataSimulazione = DEFAULT_DURATA_SIMULAZ;
				pUserValues->deltaT = DEFAULT_DELTA_T;
				pUserValues->LimitazioneDiPotenzaAttiva = DEFAULT_LIMITAZIONE_POTENZA_ATTIVA;
				aggiorna_nome_file = true;
				break;

			#if INCLUDI_MANOVRE_DI_TEST == 1 // Compila solo se richiesta l'inclusione delle manovre di test
			case 12: // [12] Manovra di test 1: Impulso di alettone e rampa di manetta
				Manovre_Inserisci_manovra_di_test(1, pUserValues); // Inserisci la manovra di test, aggiorna le condizioni iniziali
				aggiorna_nome_file = true;
				finito = 1;		// Esegui subito la simulazione
				break;

			case 13: // [13] Manovra di test 2: Virata circolare a destra, h costante
				Manovre_Inserisci_manovra_di_test(2, pUserValues); // Inserisci la manovra di test, aggiorna le condizioni iniziali
				aggiorna_nome_file = true;
				finito = 1;		// Esegui subito la simulazione
				break;

			case 14: // [14] Manovra di test 3: Virata con discesa a sinistra e livellamento finale
				Manovre_Inserisci_manovra_di_test(3, pUserValues); // Inserisci la manovra di test, aggiorna le condizioni iniziali
				aggiorna_nome_file = true;
				finito = 1;		// Esegui subito la simulazione
				break;

			case 15: // [15] Manovra di test 4: Virata a destra di 180°a quota e modulo di velocità costante
				Manovre_Inserisci_manovra_di_test(4, pUserValues); // Inserisci la manovra di test, aggiorna le condizioni iniziali
				aggiorna_nome_file = true;
				finito = 1;		// Esegui subito la simulazione
				break;
			#endif // INCLUDI_MANOVRE_DI_TEST == 1

			default:
				break;

		}
		if (aggiorna_nome_file) {
			chart_autoset_data_filename(pUserValues);
		}
	}
}



/**
 * Caricamento dei valori dal file dati_iniziali.txt
 * Se il file non esiste o se ci sono errori di lettura restituisce il valore falso.
 * Se lettura avvenuta correttamente restituisce il valore vero
 *
 * Formato del file dati_iniziali.txt
 *
 *	Riga 1:   Quota h [m]                    0 < h < 4116\n"
 *	Riga 2:   Velocita' V [m/s]              0 < V < 65\n"
 *	Riga 3:   Angolo di rampa gamma [deg]  -30 < gamma < 30\n"
 *	Riga 4:   Nome del file di salvataggio dati
 *	Riga 5:   Titolo simulazione
 *	Riga 6:   Scelta simulazione a massa variabile
 *	Riga 7:   Durata simulazione
 *	Riga 8:   Passo di integrazione
 *	Riga 9:   Scelta della simulazione con limitazione di potenza
 *
 *	Esempio:
 *	3000
 *	60
 *	2
 *	1000m_52ms_-1.0deg_mVar_300s_000_trim
 *	trim
 *	1
 *	300
 *	0.01
 *	1
 *
 */
bool user_input_leggi_dati_iniziali(char *nomefile, USERVAL *pUserVal)
{
	bool letturaok = false;
	FILE *in;
	double val;
	char riga[MAX_LUNGHEZZA_STRINGA];
	char msg[MAX_LUNGHEZZA_STRINGA];
	int len;
	int num_dati_letti;
	char *presult;


	letturaok = true; // Assumi lettura eseguita correttamente

	in = fopen(nomefile, "r");
	if (in == NULL) {
		sprintf(msg, "Non e' stato possibile aprire in lettura il file %s\n"
				     "Verranno impostati i valori di default", nomefile);
		ErrWarn_DisplayWarnMsgAndContinue(W010, msg);
		letturaok = false;
	}

	// ---------- Lettura h ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= 0)  && (val <= 4116)) {
				pUserVal->h = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W011, "Il valore presente nel file dati_iniziali.txt per la quota supera i limiti del range prestabiliti\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura V ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= 30)  && (val <= 65)) {
				pUserVal->V = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W012, "Il valore presente nel file dati_iniziali.txt per la velocita' supera i limiti del range prestabiliti\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura Gamma ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= -30)  && (val <= +30)) {
				pUserVal->Gamma = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W013, "Il valore presente nel file dati_iniziali.txt per l'angolo di rampa supera i limiti del range prestabiliti\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura nomeFileSalvataggioDati ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			// Rimuove il carattere di nuova riga dall'input
			len = strlen(riga);							// Lunghezza della stringa
			if (len > 0 && riga[len-1] == '\n') {          // Se stringa non vuota e l'ultimo carattere e' un ritorno a capo
				riga[len-1] = '\0';                        // Sostituisci il ritorno a capo con con il carattere di fine stringa
			}

			len = strlen(riga);							// Lunghezza della stringa
			if (len > 0)                        		// Se e' una stringa vuota
			{                                                   // Crea il nome completo del fiile
				strcpy(pUserVal->nomeFileSalvataggioDati, riga);
			} else {
				ErrWarn_DisplayWarnMsgAndContinue(W014, "Il valore presente nel file dati_iniziali.txt per il nome del file di salvataggio non e' corretto\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura titoloSim ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			// Rimuove il carattere di nuova riga dall'input
			len = strlen(riga);							// Lunghezza della stringa
			if (len > 0 && riga[len-1] == '\n') {          // Se stringa non vuota e l'ultimo carattere e' un ritorno a capo
				riga[len-1] = '\0';                        // Sostituisci il ritorno a capo con con il carattere di fine stringa
			}

			len = strlen(riga);							// Lunghezza della stringa
			if (len > 0)                        		// Se e' una stringa vuota
			{                                                   // Crea il nome completo del fiile
				strcpy(pUserVal->titoloSim, riga);
			} else {
				ErrWarn_DisplayWarnMsgAndContinue(W015, "Il valore presente nel file dati_iniziali.txt per il titolo della simulazione non e' corretto\n"
						                                "Verranno impostati i valori di default");

				letturaok = false;
			}
		}
	}


	// ---------- Lettura massa fissa/variabile ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= 0)  && (val <= 1)) {
				pUserVal->simMassaVariabile = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W016, "Il valore presente nel file dati_iniziali.txt per la scelta della simulazione a massa variabile non ha un valore accettabile\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura durata simulazione ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= 1.0)  && (val <= 100000.0)) {
				pUserVal->durataSimulazione = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W017, "Il valore presente nel file dati_iniziali.txt per durata della simulazione supera i limiti del range prestabiliti\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura passo di integrazione  ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= 0.01)  && (val <= 1.0)) {
				pUserVal->deltaT = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W018, "Il valore presente nel file dati_iniziali.txt per il passo di integrazione supera i limiti del range prestabiliti\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}

	// ---------- Lettura limitazione di potenza attiva ----------
	if (letturaok)
	{
		presult = fgets(riga, MAX_LUNGHEZZA_STRINGA - 1, in); // Leggi riga dal file
		if (presult == NULL)		                           // Se errore
		{
			letturaok = false;                                 // Interrompi lettura
		}
		else                                                   // Se lettura avvenuta correttamente
		{
			num_dati_letti = sscanf(riga, "%lf\n", &val); // lettura da stringa
			if ((num_dati_letti == 1) && (val >= 0)  && (val <= 1)) {
				pUserVal->LimitazioneDiPotenzaAttiva = val;
			}
			else {
				ErrWarn_DisplayWarnMsgAndContinue(W023, "Il valore presente nel file dati_iniziali.txt per la scelta della limitazione di potenza non ha un valore accettabile\n"
						                                "Verranno impostati i valori di default");
				letturaok = false;
			}
		}
	}



    if (in != NULL) {   // Se il file e' aperto
        fclose(in);     // chiudilo
    }

	return letturaok;
}


/**
 * Salvataggio dati nel file dati_iniziali.txt
 *
 * Formato del file dati_iniziali.txt
 *
 *	Riga 1:   Quota h [m]                    0 < h < 4116\n"
 *	Riga 2:   Velocita' V [m/s]              0 < V < 65\n"
 *	Riga 3:   Angolo di rampa gamma [deg]  -30 < gamma < 30\n"
 *	Riga 4:   Nome del file di salvataggio dati
 *	Riga 5:   Titolo simulazione
 *	Riga 6:   Scelta simulazione a massa variabile
 *	Riga 7:   Durata simulazione
 *	Riga 8:   Passo di integrazione
  *	Riga 9:   Scelta della simulazione con limitazione di potenza
 *
 *	Esempio:
 *	3000
 *	60
 *	2
 *	1000m_52ms_-1.0deg_mVar_300s_000_trim
 *	trim
 *	1
 *	300
 *	0.01
 *	1
 *
 */
void user_input_salva_dati_iniziali(char *nomefile, USERVAL *pUserVal)
{
	FILE *filesave;
	char msg[MAX_LUNGHEZZA_STRINGA];

	filesave = fopen(nomefile, "w");

	if (filesave == NULL) {
		sprintf(msg, "Non e' stato possibile salvare i dati utente nel file %s", nomefile);
		ErrWarn_DisplayWarnMsgAndContinue(W021, msg);
	}
	else
	{
		fprintf(filesave, "%f\n", pUserVal->h);
		fprintf(filesave, "%f\n", pUserVal->V);
		fprintf(filesave, "%f\n", pUserVal->Gamma);
		fprintf(filesave, "%s\n", pUserVal->nomeFileSalvataggioDati);
		fprintf(filesave, "%s\n", pUserVal->titoloSim);
		fprintf(filesave, "%d\n", pUserVal->simMassaVariabile);
		fprintf(filesave, "%f\n", pUserVal->durataSimulazione);
		fprintf(filesave, "%f\n", pUserVal->deltaT);
		fprintf(filesave, "%d\n", pUserVal->LimitazioneDiPotenzaAttiva);
		fclose(filesave);
	}
}




/**
 * Chiede all'utente di inserire un double
 * Parametri:
 * prompt: messaggio da visualizzare
 * min: valore minimo accettabile
 * max: valore max accettabile
 */
double user_input_richiedi_double(char *prompt, double dmin, double dmax) {
	bool continua = true;
	int num_dati_letti;
	double valore;

	while (continua) {
		fflush(stdin);
		printf("\n%s"
				"\nInserisci un numero tra %.3f e %.3f: ", prompt, dmin, dmax);
		num_dati_letti = scanf("%lf", &valore);

		if ((num_dati_letti == 1) && (valore <= dmax) && (valore >= dmin)) {
			continua = false;
		}
		else {
			printf("\n\nErrore: Immettere un dato compreso tra %f e %f\n\n", dmin, dmax);
		}
	}
	return valore;
}

/**
 * Chiede all'utente di inserire un double
 * premendo direttamente enter si mantiene il valore attuale
 * Parametri:
 * prompt: messaggio da visualizzare
 * unita_misura: stringa con unita' di misura da stampare a fianco del valore attuale. Inserire "" se adimensionale
 * attuale: valore attuale
 * min: valore minimo accettabile
 * max: valore max accettabile
 */
double user_input_richiedi_double_con_default(char *prompt, char *unita_misura, double attuale, double dmin, double dmax) {
	bool continua = true;
	int num_dati_letti;
	double valore;
	char riga_input[MAX_LUNGHEZZA_STRINGA] = "";
	size_t len;

	printf("\n"
		   "\n----------------------------------------------------------------"
		   "\n                 %s"
		   "\n                 Valore attuale: %g %s"
		   "\n----------------------------------------------------------------"
		   "\n", prompt, attuale, unita_misura);

	while (continua) {
		fflush(stdin);
		printf("\nPremi <enter> senza immettere dati per mantenere il valore attuale "
			   "\nOppure inserisci un numero tra %g e %g [%s]: ", dmin, dmax, unita_misura);
		fgets(riga_input, MAX_LUNGHEZZA_STRINGA, stdin);
		// Rimuove il carattere di nuova linea dall'input
		len = strlen(riga_input);							// Lunghezza della stringa
		if (len > 0 && riga_input[len - 1] == '\n') {          // Se stringa non vuota e l'ultimo carattere e' un ritorno a capo
			riga_input[len - 1] = '\0';                        // Sostituisci il ritorno a capo con con il carattere di fine stringa
		}
		if (strlen(riga_input) == 0)                         // Se e' stato premuto enter senza immettere dati
		{
			valore = attuale;                                   // Restituisci il valore attuale
			continua = false;                                   // Fine immissione
		}
		else                                                    // Se sono stati immessi dei dati
		{
			num_dati_letti = sscanf(riga_input, "%lf", &valore);            // Lettura da tringa

			if ((num_dati_letti == 1) && (valore <= dmax) && (valore >= dmin)) {
				continua = false;
			}
			else {
				printf("\n\nErrore: Immettere un dato compreso tra %f e %f\n\n", dmin, dmax);
			}
		}

	}
	return valore;
}


/**
 * Chiede all'utente di inserire un intero
 * Parametri:
 * prompt: messaggio da visualizzare
 * min: valore minimo accettabile
 * max: valore max accettabile
 */
int user_input_richiedi_int(char *prompt, int min, int max) {
	bool continua = true;
	int num_dati_letti, scelta;

	while (continua) {
		fflush(stdin);
		printf("\n%s"
				"\nInserisci un intero tra %d e %d: ", prompt, min, max);
		num_dati_letti = scanf("%d", &scelta);

		if ((num_dati_letti == 1) && (scelta <= max) && (scelta >= min)) {
			continua = false;
		}
		else {
			printf("\n\nErrore: Immettere un dato compreso tra %d e %d\n\n", min, max);
		}
	}
	return scelta;
}


/**
 * Chiede all'utente di inserire una stringa.
 * Propone il valore attuale della stringa e non lo modifica se l'utente preme il tasto "enter"
 * Parametri:
 * prompt: messaggio da visualizzare
 * stringa: puntatore alla stringa da inserire
 */
void user_input_richiedi_stringa(char prompt[], char stringa[]) {
	char nuova_stringa[MAX_LUNGHEZZA_STRINGA] = "";
	size_t len;

	/* Lettura nome */
	fflush(stdin);
	printf("\n\n%s"
			"\nValore attuale: %s", prompt, stringa);
	printf("\n(enter -> mantieni attuale): ");

	fgets(nuova_stringa, MAX_LUNGHEZZA_STRINGA, stdin);
	// Rimuove il carattere di nuova linea dall'input
	len = strlen(nuova_stringa);							// Lunghezza della stringa
	if (len > 0 && nuova_stringa[len - 1] == '\n') {          // Se stringa non vuota e l'ultimo carattere e' un ritorno a capo
		nuova_stringa[len - 1] = '\0';                        // Sostituisci il ritorno a capo con con il carattere di fine stringa
	}
	if (strlen(nuova_stringa) != 0)                         // Se e' stata immessa una stringa
	{
		strcpy(stringa, nuova_stringa);                     // Copia nel risultato
	}
}

/**
 * Elimina i caratteri di spaziatura a fine inizio stringa
 */
void user_input_elimina_spazi_ini_fin(char stringa[]) {
	int i, j, start, end, len;
	char temp[MAX_LUNGHEZZA_STRINGA];

	strcpy(temp, stringa); // Copia locale della stringa
	len = strlen(stringa);	// Lunghezza della stringa

    // elimina spazi iniziali: cerca il primo carattere non di spaziatura
	start = 0;
	while (start < len) {
		if (isspace(temp[start])) { // Se il carattere e' di spaziatura
			start++;                // passa ad esaminare il carattere successivo
		}
		else {                  // Se il carattere non e' di spaziatura
			break;              // Interrompi ricerca
		}
	}

    // elimina spazi finali: cerca l'ultimo carattere non di spaziatura
	end = len - 1; // Iniza dall'ultimo carattere
	while (end > start) {
		if (isspace(temp[end])) { // Se il carattere e' di spaziatura
			end--;                // passa ad esaminare il carattere precedente
		}
		else {                  // Se il carattere non e' di spaziatura
			break;              // Interrompi ricerca
		}
	}

	if (start > end) // Se i caratteri sono tutti di spaziatura
	{
		stringa[0] = 0; // Il risultato e' la stringa vuota
	}
	else              // Se ci sono caratteri non di spaziatura copiali
	{
		j = 0;
		for (i = start; i <= end; i++)
		{
				stringa[j] = temp[i];
				j++;
		}
		stringa[j] = 0; // aggiungi carattere di fine stringa
	}
}

/**
 * Visualizza il messaggio msg, attende la pressione del tasto enter per proseguire
 */
void user_input_StampaMsgAttendiTasto(char *msg) {
	char nuova_stringa[MAX_LUNGHEZZA_STRINGA] = "";

	if (strlen(msg) != 0) {
		printf("\n"
				"%s");
	}
	printf("\n"
			"*** Premere un tasto per continuare ***", msg);

	/* Lettura nome */
	fflush(stdin);

	fgets(nuova_stringa, MAX_LUNGHEZZA_STRINGA, stdin);
}
