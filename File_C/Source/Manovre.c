#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>
#include "dati.h"
#include "user_input.h"
#include "Manovre.h"

#define MAX_NUM_PUNTI_SPEZZATA		100
#define MAX_NUM_MANOVRE     100
#define MAX_NUM_TIPI_COMANDO 5
#define MAX_NUM_TIPI_MANOVRA 9
#define TEMPO_MIN_DA_FINE_SIMULAZIONE 1
//

DESCR_MANOVRA sequenza_manovre[MAX_NUM_MANOVRE];

double durata_simulazione;
#define DURATA_SIMULAZIONE_DEFAULT 0.1

/* Descrizione dei comandi sotto forma di linea spezzata
 *
 * Esempio: rampa+gradino seguita da impulso di durata 0.1s
 *
 * {  //time, value
 *     {10  ,    0},
 *     {15  ,   10},
 *     {18  ,   10},
 *     {18  ,   21},
 *     {18.1,   21},
 *     {18.1,   10}
 * }
 *
 *       ^
 * value |
 *       |
 *       |
 *    21 |                  X*X
 *       |                  | |
 *       |                  | |
 *       |                  | |
 *       |                  | |
 *    10 |            X*****X X********************
 *       |          *
 *       |        *
 *       |      *
 *       *****X---------------------------------------->
 *           10      15    18                      time
 *                           18.01
 *  */

TIMEVALUE ManovraSpezzata_DeltaAil[MAX_NUM_PUNTI_SPEZZATA];
TIMEVALUE ManovraSpezzata_DeltaEq[MAX_NUM_PUNTI_SPEZZATA];
TIMEVALUE ManovraSpezzata_DeltaRud[MAX_NUM_PUNTI_SPEZZATA];
TIMEVALUE ManovraSpezzata_EngPow[MAX_NUM_PUNTI_SPEZZATA];
double durata_manovra[MAX_NUM_MANOVRE];
void Manovre_Converti_sequenza_manovre_in_sequenza_singolo_comando(TIPO_COMANDO tipo_comando_da_convertire, DESCR_MANOVRA seq_manovre[], int max_num_manovre, TIMEVALUE vett_manovra_spezzata[],
		int max_numero_punti_spezzata, double durata_min_sim);

int Manovre_verifica_tempo_manovre(int i, double durata_simulazione_par);

#if INCLUDI_MANOVRE_DI_TEST == 1
int Manovra_predefinita_inserita = 0;
#endif

/**************************************************************************************************
 * Prototipi
 **************************************************************************************************/
double Manovre_TimeValueInterpola(double t, TIMEVALUE ManovraSpezzata[]);

void Manovre_Print_Single_Command(FILE *fOut, char *NomeComando, TIMEVALUE vett_manovra_spezzata[], int max_numero_punti_spezzata);

/**************************************************************************************************
 * Funzioni
 **************************************************************************************************/

/**
 * Manovre_Inizializza
 * Inizializza i dati di questo file
 */
void Manovre_Inizializza(void) {
	durata_simulazione = DURATA_SIMULAZIONE_DEFAULT;
	Azzera_comandi(durata_simulazione);
}

/**
 * Reset dei comandi
 * Crea una swquenza di comandi nulli della durata richiesta
 */
void Azzera_comandi(double durataSimulazione) //azzera i valori dei vettori dei comandi: deve essere richiamata sia nel caso si voglia avere il trim, sia prima di ogni nuova sequenza di manovra
{
	int i;

	/* Azzera sequenza dei comandi */
	for (i = 0; i < MAX_NUM_PUNTI_SPEZZATA; i++) {
		ManovraSpezzata_DeltaAil[i].time = 0;
		ManovraSpezzata_DeltaAil[i].value = 0;
		ManovraSpezzata_DeltaEq[i].time = 0;
		ManovraSpezzata_DeltaEq[i].value = 0;
		ManovraSpezzata_DeltaRud[i].time = 0;
		ManovraSpezzata_DeltaRud[i].value = 0;
		ManovraSpezzata_EngPow[i].time = 0;
		ManovraSpezzata_EngPow[i].value = 0;
	}
	/* Imposta il secondo punto con riferimento temporale precedente per indicare la fine della sequenza */
	ManovraSpezzata_DeltaAil[1].time = -1;
	ManovraSpezzata_DeltaAil[1].value = 0;
	ManovraSpezzata_DeltaEq[1].time = -1;
	ManovraSpezzata_DeltaEq[1].value = 0;
	ManovraSpezzata_DeltaRud[1].time = -1;
	ManovraSpezzata_DeltaRud[1].value = 0;
	ManovraSpezzata_EngPow[1].time = -1;
	ManovraSpezzata_EngPow[1].value = 0;

	/* Azzera sequenza delle manovre */
	for (i = 0; i < MAX_NUM_MANOVRE; i++) {
		sequenza_manovre[i].tipocomando = TUTTI_I_COMANDI;
		sequenza_manovre[i].istante_inizio = 0;
		sequenza_manovre[i].manovra = FINE_MANOVRE;
	}

	/* Imposta il solo comando di fine manovra per creare un comando della durata richiesta */
	sequenza_manovre[0].tipocomando = TUTTI_I_COMANDI;
	sequenza_manovre[0].istante_inizio = durataSimulazione;
	sequenza_manovre[0].manovra = FINE_MANOVRE;

	/* Converti sequenza_manovre[] in ManovraSpezzata_XXX[] */
	Manovre_Converti_sequenza_manovre_in_sequenze_comandi(durataSimulazione);
}

/**
 * Legge i comandi all'istante t interpolando le linee spezzate che li descrivono
 */
CMD Manovre_getCommand(double t) {
	CMD comando;

	comando.DeltaAil = Manovre_TimeValueInterpola(t, ManovraSpezzata_DeltaAil);
	comando.DeltaEq = Manovre_TimeValueInterpola(t, ManovraSpezzata_DeltaEq);
	comando.DeltaRud = Manovre_TimeValueInterpola(t, ManovraSpezzata_DeltaRud);
	comando.EngPow = Manovre_TimeValueInterpola(t, ManovraSpezzata_EngPow);

	return comando;
}

/**
 * Restituisce il valore del comando all'istante t specificato in base alla funzione lineare a tratti
 * che ne descrive l'andamento e che viene passata come parametro.
 * La funzione e' descritta da una lista di coppie coordinate (t, valore) che identificano
 * i punti di congiunzione tra i segmenti.
 *
 *
 *                ^
 *                |
 *                |
 *             v2 |                    *
 *                |                   /
 *                |                  /
 *                |                 /
 *             v  |................X
 *                |               /:
 *                |              / :
 *                |             /  :
 *                |            /   :
 *             v1 *           *    :
 *                |                :
 *                |                :
 *                +-----------*----*---*---------------------------->
 *                            t1   t   t2
 *
 *
 *
 */
double Manovre_TimeValueInterpola(double t, TIMEVALUE ManovraSpezzata[]) {
	int i;
	double t1, t2, v1, v2, v, delta_t, delta_v;
#define MAX_DELTA_X_UGUAGLIANZA_FLOAT  1e-6

    v = 0;
	/* Estendi la funzione a sinistra fino a t = -1 */
	t2 = -1;
	v2 = ManovraSpezzata[0].value;
	for (i = 0; i < MAX_NUM_PUNTI_SPEZZATA; i++) {
		t1 = t2; // Il punto di t2 della precedente iterazione diventa il t1 di questa
		v1 = v2;
		t2 = ManovraSpezzata[i].time;
		v2 = ManovraSpezzata[i].value;
		delta_t = t2 - t1;
		delta_v = v2 - v1;
		if (t1 > t2)	// se il punto indica la fine della sequenza
				{
			v = v1; // Restituisci il valore dell'ultimo punto definito
			break; // Interrompi - fine della spezzata
		}
		else if (fabs(delta_t) <= MAX_DELTA_X_UGUAGLIANZA_FLOAT) // Se t1 e t2 sono uguali
		{													// Siamo in presenza del fronte di un impulso
			// Non fare nulla, nei passi successivi si vedrà se si rimane nella parte orizzontale dell'impulso
		}
		else if ((t >= t1) && (t <= t2))	// Se l'istante e' a all'interno del segmento corrente
				{
			v = v1;
			v += (t - t1) * delta_v / delta_t;
			break; // Interrompi la ricerca
		}
		else  // Se la sequenza non e' finita e t non e' nel segmento corrente
		{
			// Non fare nulla, continua con la ricerca
		}
	}
	return v;
}

/**
 * Restituisce la durata complessiva delle manovre
 * Analizza le sequenze e prende la piu' lunga
 */
double Manovre_getDurata(void) {

	int i;
	double t1, t2, durata;
#define MAX_DELTA_X_UGUAGLIANZA_FLOAT  1e-6

	t2 = 0;
	durata = 0;
	for (i = 0; i < MAX_NUM_PUNTI_SPEZZATA; i++) {
		t1 = t2; // Il punto di t2 della precedente iterazione diventa il t1 di questa
		t2 = ManovraSpezzata_DeltaAil[i].time;
		if (t1 > t2)	// se il punto indica la fine della sequenza
				{
			if (t1 > durata) {
				durata = t1;
			}
			break; // Interrompi - fine della spezzata
		}
	}
	for (i = 0; i < MAX_NUM_PUNTI_SPEZZATA; i++) {
		t1 = t2; // Il punto di t2 della precedente iterazione diventa il t1 di questa
		t2 = ManovraSpezzata_DeltaEq[i].time;
		if (t1 > t2)	// se il punto indica la fine della sequenza
				{
			if (t1 > durata) {
				durata = t1;
			}
			break; // Interrompi - fine della spezzata
		}
	}
	for (i = 0; i < MAX_NUM_PUNTI_SPEZZATA; i++) {
		t1 = t2; // Il punto di t2 della precedente iterazione diventa il t1 di questa
		t2 = ManovraSpezzata_DeltaRud[i].time;
		if (t1 > t2)	// se il punto indica la fine della sequenza
				{
			if (t1 > durata) {
				durata = t1;
			}
			break; // Interrompi - fine della spezzata
		}
	}
	for (i = 0; i < MAX_NUM_PUNTI_SPEZZATA; i++) {
		t1 = t2; // Il punto di t2 della precedente iterazione diventa il t1 di questa
		t2 = ManovraSpezzata_EngPow[i].time;
		if (t1 > t2)	// se il punto indica la fine della sequenza
				{
			if (t1 > durata) {
				durata = t1;
			}
			break; // Interrompi - fine della spezzata
		}
	}
	return durata;
}

/**************************************************************************************************
 * Menu di immissione manovre da tastiera
 * Parametri:
 * 	durata_simulazione: durata della simulazione.
 **************************************************************************************************/
void Manovre_lettura_da_tastiera(double durata_simulazione) {
	int comando, i = 0, manovra, finito = 0, continuing;
	double istante_fine_manovra_precedente[NUMERO_COMANDI];
	double durata_manovra;
	double max_durata;
	TIPO_COMANDO comando_in_corso;
    #define ISTANTE_INIZIALE_MINIMO  30

	#if INCLUDI_MANOVRE_DI_TEST == 1
	if (Manovra_predefinita_inserita != 0) // Se e' stata inserita una manovra di test
	{
		return;   // Non proseguire
	}
	#endif

	for (comando_in_corso = 0; comando_in_corso < NUMERO_COMANDI; comando_in_corso++) {
		istante_fine_manovra_precedente[comando_in_corso] = ISTANTE_INIZIALE_MINIMO;
	}

	Azzera_comandi(durata_simulazione);

	printf("\n----------------------------------------------------------------");
	printf("\n                      Inserimento manovre");
	printf("\n----------------------------------------------------------------");

	i = 0; // Reset indice alla manovra da inserire
	while (!finito) {
		comando = user_input_richiedi_int("\n Inserisci comando: "
				"\n [0] ---> Solo TRIM"
				"\n [1] ---> Alettone"
				"\n [2] ---> Equilibratore"
				"\n [3] ---> Timone"
				"\n [4] ---> Manetta"
				"\n [5] ---> Tutti i comandi (da usare per riportare i comandi al trim durante le manovre) "
				"\n", 0,  // Valore minimo accettabile
				MAX_NUM_TIPI_COMANDO); // Valore massimo accettabile
		if (comando == 0) {		// Se e' stato scelto il solo TRIM come sequenza di manovre
			finito = 1;         // Segnala fine immissione
			break;              // Concludi loop di immissione comandi
		}
		else
			switch (comando) {
				case 1:
					comando_in_corso = AILERON;
					break;
				case 2:
					comando_in_corso = ELEVATOR;
					break;
				case 3:
					comando_in_corso = RUDDER;
					break;
				case 4:
					comando_in_corso = THROTTLE;
					break;
				case 5:
					comando_in_corso = TUTTI_I_COMANDI;
					break;
				default:
					break;
			}
		/* Se si immette una manovra che agisce su TUTTI_I_COMANDI, e' necessario che le manovre di ogni singolo comando siano concluse. */
		if (comando_in_corso == TUTTI_I_COMANDI) {
			if (istante_fine_manovra_precedente[TUTTI_I_COMANDI] < istante_fine_manovra_precedente[AILERON]) {
				istante_fine_manovra_precedente[TUTTI_I_COMANDI] = istante_fine_manovra_precedente[AILERON];
			}
			if (istante_fine_manovra_precedente[TUTTI_I_COMANDI] < istante_fine_manovra_precedente[ELEVATOR]) {
				istante_fine_manovra_precedente[TUTTI_I_COMANDI] = istante_fine_manovra_precedente[ELEVATOR];
			}
			if (istante_fine_manovra_precedente[TUTTI_I_COMANDI] < istante_fine_manovra_precedente[RUDDER]) {
				istante_fine_manovra_precedente[TUTTI_I_COMANDI] = istante_fine_manovra_precedente[RUDDER];
			}
			if (istante_fine_manovra_precedente[TUTTI_I_COMANDI] < istante_fine_manovra_precedente[THROTTLE]) {
				istante_fine_manovra_precedente[TUTTI_I_COMANDI] = istante_fine_manovra_precedente[THROTTLE];
			}
		}

		if (istante_fine_manovra_precedente[comando_in_corso] == ISTANTE_INIZIALE_MINIMO) // Se il dato di fine manovra e' ancora quello iniziale --> questa e' la prima manovra
		{
			printf("\n La prima manovra deve iniziare almeno %.0f secondi dopo l'inizio della simulazione.\n", (float) ISTANTE_INIZIALE_MINIMO);
		}
		else
		{
			printf("\n La conclusione della manovra precedente avviene all'istante %f.\n", istante_fine_manovra_precedente[comando_in_corso]);
		}

		sequenza_manovre[i].tipocomando = comando_in_corso;
		sequenza_manovre[i].istante_inizio = user_input_richiedi_double("Inserire l'istante iniziale della manovra:",  // Messaggio
				istante_fine_manovra_precedente[comando_in_corso], // Minimo accettabile
				durata_simulazione - TEMPO_MIN_DA_FINE_SIMULAZIONE);                           // Massimo accettabile: un secondo dal termine della simulazione
		max_durata = durata_simulazione - sequenza_manovre[i].istante_inizio; // Mas durata della manovra
		manovra = user_input_richiedi_int("\n Inserire tipo di manovra: "
				"\n [1] ---> Impulso "
				"\n [2] ---> Rampa "
				"\n [3] ---> Rampa + gradino "
				"\n [4] ---> Gradino "
				"\n [5] ---> Doublet "
				"\n [6] ---> Doppio gradino scontrato "
				"\n [7] ---> Trapezio "
				"\n [8] ---> Doppio trapezio scontrato "
				"\n [9] ---> Impostazione del TRIM"
				"\n", 1,                     // Min valore accettabile
				MAX_NUM_TIPI_MANOVRA); // Max valore accettabile
		switch (manovra) {
			case 1: // [1] ---> Impulso
			{
				sequenza_manovre[i].manovra = IMPULSO;
				printf("\n Inserire l'ampiezza dell'impulso: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.impulso.ampiezza);
				sequenza_manovre[i].param_manovra.impulso.deltaT =
						user_input_richiedi_double("Inserire durata: ",
								0, // Valore minimo
								max_durata); // Valore max
				durata_manovra = sequenza_manovre[i].param_manovra.impulso.deltaT;
			}
				break;
			case 2: // [2] ---> Rampa
			{
				sequenza_manovre[i].manovra = RAMPA;
				printf("\n Inserire l'ampiezza della rampa: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.rampa.ampiezza);
				sequenza_manovre[i].param_manovra.rampa.durata = user_input_richiedi_double("Inserire la durata", 0, // Valore minimo
						max_durata); // Valore max
				durata_manovra = sequenza_manovre[i].param_manovra.rampa.durata;
			}
				break;
			case 3: // [3] ---> Rampa + gradino
			{
				sequenza_manovre[i].manovra = RAMPA_GRADINO;
				printf("\n Inserire l'ampiezza di rampa e gradino: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.rampa_gradino.ampiezza);

				sequenza_manovre[i].param_manovra.rampa_gradino.durata = user_input_richiedi_double("Inserire la durata", 0, // Valore minimo
						max_durata); // Valore max
				durata_manovra = sequenza_manovre[i].param_manovra.rampa.durata;

			}
				break;
			case 4: // [4] ---> Gradino
			{
				sequenza_manovre[i].manovra = GRADINO;
				printf("\n Inserire l'ampiezza del gradino: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.gradino.ampiezza);
				durata_manovra = 0;
			}
				break;
			case 5: // [5] ---> Doublet
			{
				sequenza_manovre[i].manovra = DOUBLET;
				printf("\n Inserire l'ampiezza del primo impulso: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.doublet.ampiezza);
				sequenza_manovre[i].param_manovra.doublet.deltaT = user_input_richiedi_double("Inserire il primo intervallo di tempo", 0, // Valore minimo
						max_durata / 2); // Valore max
				durata_manovra = 2 * sequenza_manovre[i].param_manovra.doublet.deltaT;
			}
				break;
			case 6: // [6] ---> Doppio gradino scontrato
			{
				sequenza_manovre[i].manovra = DOPPIO_GRADINO_SCONTRATO;
				printf("\n Inserire l'ampiezza del primo gradino: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.ampiezza_1);

				sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.durata_1 = user_input_richiedi_double("Inserire la durata del primo gradino", 0, // Valore minimo
						max_durata); // Valore max
				durata_manovra = sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.durata_1; // Il calcolo della durata manovra si fa a pezzi

				sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.tempo_intermedio = user_input_richiedi_double("Inserire il tempo intermedio tra i due gradini", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.tempo_intermedio;

				printf("\n Inserire l'ampiezza del secondo gradino: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.ampiezza_2);

				sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.durata_2 = user_input_richiedi_double("Inserire la durata del secondo gradino", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.durata_2;
			}
				break;
			case 7: // [7] ---> Trapezio
			{
				sequenza_manovre[i].manovra = TRAPEZIO;
				printf("\n Inserire l'ampiezza della salita: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita);
				sequenza_manovre[i].param_manovra.trapezio.tempo_salita = user_input_richiedi_double("Inserire la durata della salita", 0, // Valore minimo
						max_durata); // Valore max
				durata_manovra = sequenza_manovre[i].param_manovra.trapezio.tempo_salita; // Il calcolo della durata manovra si fa a pezzi
				sequenza_manovre[i].param_manovra.trapezio.tempo_regime = user_input_richiedi_double("Inserire la durata di regime", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.trapezio.tempo_regime;
				printf("\n Inserire l'ampiezza della discesa: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa);
				sequenza_manovre[i].param_manovra.trapezio.tempo_discesa = user_input_richiedi_double("Inserire la durata di discesa", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.trapezio.tempo_discesa;
			}
				break;
			case 8: // [8] ---> Doppio trapezio scontrato
			{
				sequenza_manovre[i].manovra = DOPPIO_TRAPEZIO_SCONTRATO;
				printf("\n Inserire l'ampiezza della prima rampa: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_1);
				sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1 =
						user_input_richiedi_double("Inserire la durata della prima rampa",
								                   0, // Valore minimo
						                           max_durata); // Valore max
				durata_manovra = 2 * sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1; // Il calcolo della durata manovra si fa a pezzi. tempo_rampa_1 si usa in salita e in discesa
				sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_1 = user_input_richiedi_double("Inserire la durata della prima fase di regime", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_1;
				sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi = user_input_richiedi_double("Inserire l'ampiezza dell'intervallo di tempo tra le due rampe",
						0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi;
				printf("\n Inserire l'ampiezza della seconda rampa: ");
				scanf("%lf", &sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_2);
				sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2 = user_input_richiedi_double("Inserire la durata della seconda rampa", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += 2 * sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2; // tempo_rampa_2 si usa in salita e in discesa
				sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_2 = user_input_richiedi_double("Inserire la durata della seconda fase di regime", 0, // Valore minimo
						max_durata - durata_manovra); // Valore max
				durata_manovra += sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_2;
			}
				break;

			case 9: {
				sequenza_manovre[i].manovra = IMPOSTA_TRIM;
				sequenza_manovre[i].param_manovra.imposta_trim.durata = user_input_richiedi_double("Inserire la durata", 0, // Valore minimo
						max_durata); // Valore max
				durata_manovra = sequenza_manovre[i].param_manovra.imposta_trim.durata;
			}
				break;
			default:
				durata_manovra = 0;
				break;
		}

		/* Aggiorna istante finale della manovra corrente */
		istante_fine_manovra_precedente[comando_in_corso] = sequenza_manovre[i].istante_inizio + durata_manovra;
		if (comando_in_corso == TUTTI_I_COMANDI) // Se ho inserito una manovra relativa a tutti i comandi
				{ // Bisogna impostare l'istante di fine manovra di ogni singolo comando
			istante_fine_manovra_precedente[AILERON] = istante_fine_manovra_precedente[TUTTI_I_COMANDI];
			istante_fine_manovra_precedente[ELEVATOR] = istante_fine_manovra_precedente[TUTTI_I_COMANDI];
			istante_fine_manovra_precedente[RUDDER] = istante_fine_manovra_precedente[TUTTI_I_COMANDI];
			istante_fine_manovra_precedente[THROTTLE] = istante_fine_manovra_precedente[TUTTI_I_COMANDI];
		}
		continuing = user_input_richiedi_int("\n Si desidera proseguire con un altro comando? "
				"\n [0] ---> No "
				"\n [1] ---> Si "
				"\n", 0,  // Val minimo
				1); // Val massimo

		i++;  // Incrementa indie delle manovre
		if (continuing == 0) {
			finito = 1;
		}
		fflush(stdin);

	}
	/* Aggiungi la manovra di fine comandi che li estende fino a fine simulazione */
	sequenza_manovre[i].tipocomando = TUTTI_I_COMANDI;
	sequenza_manovre[i].istante_inizio = durata_simulazione;
	sequenza_manovre[i].manovra = FINE_MANOVRE;
	/* Converti la sequenza immessa in singoli comandi impostando la durata minima della simulazione*/
	Manovre_Converti_sequenza_manovre_in_sequenze_comandi(durata_simulazione);
	Manovre_Print_All_Command(stdout); // debug
}



/**************************************************************************************************
 *
 *
 **************************************************************************************************/
#if INCLUDI_MANOVRE_DI_TEST == 1 // Compila solo se richiesta l'inclusione delle manovre di test
void Manovre_Inserisci_manovra_di_test(int test, USERVAL *pUserValues)
{
	int i;


	i = 0;
	if (test == 1) {	// 1: Impulso di alettone e rampa di manetta
		Manovra_predefinita_inserita = 1; // Segnala che e' stata inserita una manovra di test
 		strcpy(pUserValues->titoloSim, "Impulso di alettone 20x0.05 e rampa di manetta 80x10"); // Imposta titolos
		pUserValues->h = 1000;
		pUserValues->V = 52;
		pUserValues->Gamma = 0;
		pUserValues->durataSimulazione = 500;

		Azzera_comandi(pUserValues->durataSimulazione);

		i = 0; // Reset indice alla manovra da inserire
		sequenza_manovre[i].istante_inizio = 100;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = IMPULSO;
		sequenza_manovre[i].param_manovra.impulso.ampiezza = 20;
		sequenza_manovre[i].param_manovra.impulso.deltaT = 0.05;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 95;
		sequenza_manovre[i].tipocomando = THROTTLE;
		sequenza_manovre[i].manovra = RAMPA;
		sequenza_manovre[i].param_manovra.rampa.ampiezza = 80;
		sequenza_manovre[i].param_manovra.rampa.durata = 10;
		(i)++; // Punta alla prossima manovra
	}
	else if (test == 2) {	// 2: Virata circolare a destra, h costante
		Manovra_predefinita_inserita = 2; // Segnala che e' stata inserita manovra di test 2
		strcpy(pUserValues->titoloSim, "Virata circolare a destra, h costante"); // Imposta titolos
		pUserValues->h = 2000;
		pUserValues->V = 52;
		pUserValues->Gamma = 0;
		pUserValues->durataSimulazione = 100;

		Azzera_comandi(pUserValues->durataSimulazione);

		i = 0; // Reset indice alla manovra da inserire
		sequenza_manovre[i].istante_inizio = 40;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita = 1;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime = 3;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa = 1;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita = 0.6;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = 0.6;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 40;
		sequenza_manovre[i].tipocomando = ELEVATOR;
		sequenza_manovre[i].manovra = RAMPA_GRADINO;
		sequenza_manovre[i].param_manovra.rampa.ampiezza = 0.02;
		sequenza_manovre[i].param_manovra.rampa.durata = 5;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 40;
		sequenza_manovre[i].tipocomando = RUDDER;
		sequenza_manovre[i].manovra = RAMPA_GRADINO;
		sequenza_manovre[i].param_manovra.rampa.ampiezza = -0.0031;
		sequenza_manovre[i].param_manovra.rampa.durata = 5;
		(i)++; // Punta alla prossima manovra
	}
	else if (test == 3) {	// 3: Virata con discesa a sinistra e livellamento finale
		Manovra_predefinita_inserita = 3; // Segnala che e' stata inserita manovra di test 3
		strcpy(pUserValues->titoloSim, "Virata con discesa a sinistra e livellamento finale	"); // Imposta titolos
		pUserValues->h = 3000;
		pUserValues->V = 56;
		pUserValues->Gamma = 0;
		pUserValues->durataSimulazione = 600;

		Azzera_comandi(pUserValues->durataSimulazione);

		i = 0; // Reset indice alla manovra da inserire
		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = THROTTLE;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita = 5;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime = 245;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa = 5;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita = 42; // 2000 rpm in teoria
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = 42; // 2000 rpm in teoria
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = DOPPIO_TRAPEZIO_SCONTRATO;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_1     = -1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1  = 1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_1 = 3;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi = 119.3;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_2 = -1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2 = 1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_2 = 3;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = ELEVATOR;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita  = 0.08;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = -0.120749;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita     = 5;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime     = 117.3;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa    = 5;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = RUDDER;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita  = 0.066;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = 0.066;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita     = 5;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime     = 117.3;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa    = 5;
		(i)++; // Punta alla prossima manovra
	}
	else if (test == 4) {	// 4: Virata a destra di 180 gradia quota e modulo di velocità costante"
		Manovra_predefinita_inserita = 4; // Segnala che e' stata inserita manovra di test 4
		strcpy(pUserValues->titoloSim, "Virata a destra di 180 gradi a quota e modulo di velocita' costante"); // Imposta titolos
		pUserValues->h = 3000;
		pUserValues->V = 56;
		pUserValues->Gamma = 0;
		pUserValues->durataSimulazione = 300;

		Azzera_comandi(pUserValues->durataSimulazione);

		i = 0; // Reset indice alla manovra da inserire
		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = DOPPIO_TRAPEZIO_SCONTRATO;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_1     = 1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1  = 1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_1 = 3;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi = 119.3;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_2     = 1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2  = 1;
		sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_2 = 3;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = ELEVATOR;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita  = 0.08;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = 0.08;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita     = 4;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime     = 119.3;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa    = 4;
		(i)++; // Punta alla prossima manovra

		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = RUDDER;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita = 4;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime = 150;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa = 4;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita  = -0.044;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = -0.044;
		(i)++; // Punta alla prossima manovra

		//sequenza_manovre[i].istante_inizio = 20;
		//sequenza_manovre[i].tipocomando = RUDDER;
		//sequenza_manovre[i].manovra = DOPPIO_TRAPEZIO_SCONTRATO;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_1     =  -0.044;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1  = 4;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_1 = 4;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi = 119.3;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.ampiezza_2     =  -0.044;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2  = 4;
		//sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_2 = 4;
		//(i)++; // Punta alla prossima manovra
	}
	/* Aggiungi la manovra di fine comandi che li estende fino a fine simulazione */
	sequenza_manovre[i].tipocomando = TUTTI_I_COMANDI;
	sequenza_manovre[i].istante_inizio = pUserValues->durataSimulazione;
	sequenza_manovre[i].manovra = FINE_MANOVRE;
	/* Converti la sequenza immessa in singoli comandi impostando la durata minima della simulazione*/
	Manovre_Converti_sequenza_manovre_in_sequenze_comandi(pUserValues->durataSimulazione);
	printf("Manovra: %s\n", pUserValues->titoloSim);
	Manovre_Print_All_Command(stdout); // debug
	system("PAUSE");

}
#endif //  == 1


																					///**************************************************************************************************
																					// * Manovre_verifica_tempo_manovre
																					// * Verifica la correttezza temporale delle manovre
																					// **************************************************************************************************/
																					//int Manovre_verifica_tempo_manovre(int i, double durata_simulazione_par) {
																					//	int corretto = 0; //ED: 0 se e' tutto corretto, 1 se eccede durata simulazione, 2 se inizia quando la simulazione non e' conclusa.
																					//	double durata_manovra_precedente = durata_manovra[i - 1];
																					//	double TempoADisposizione;
																					//	switch (sequenza_manovre[i].manovra) {
																					//		case IMPULSO: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.impulso.deltaT;
																					//		}
																					//			break;
																					//		case RAMPA: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.rampa.durata;
																					//		}
																					//			break;
																					//		case RAMPA_GRADINO: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.rampa_gradino.durata;
																					//		}
																					//			break;
																					//		case GRADINO: {
																					//			durata_manovra[i] = 0; // Il gradino non ha una durata sequenza_manovre[i].param_manovra.gradino.durata;
																					//		}
																					//			break;
																					//		case DOUBLET: {
																					//			durata_manovra[i] = 2 * sequenza_manovre[i].param_manovra.doublet.deltaT;
																					//		}
																					//			break;
																					//		case DOPPIO_GRADINO_SCONTRATO: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.durata_1 + sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.tempo_intermedio
																					//					+ sequenza_manovre[i].param_manovra.doppio_gradino_scontrato.durata_2;
																					//		}
																					//			break;
																					//		case TRAPEZIO: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.trapezio.tempo_salita + sequenza_manovre[i].param_manovra.trapezio.tempo_regime
																					//					+ sequenza_manovre[i].param_manovra.trapezio.tempo_discesa;
																					//		}
																					//			break;
																					//		case DOPPIO_TRAPEZIO_SCONTRATO: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1 + sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_1
																					//					+ sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi + sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2
																					//					+ sequenza_manovre[i].param_manovra.doppio_trapezio_scontrato.tempo_regime_2;
																					//		}
																					//			break;
																					//		case IMPOSTA_TRIM: {
																					//			durata_manovra[i] = sequenza_manovre[i].param_manovra.imposta_trim.durata;
																					//		}
																					//			break;
																					//		default:
																					//			break;
																					//	}
																					//	if (i == 0) {
																					//		if (durata_manovra[i] + sequenza_manovre[i].istante_inizio > durata_simulazione_par) {
																					//			corretto = 1;
																					//			printf("\n Errore! La manovra inserita eccede la durata massima di simulazione. Il tempo di simulazione ancora disponibile e' %f s.\n", durata_simulazione_par);
																					//		}
																					//	}
																					//	else {
																					//		TempoADisposizione = durata_simulazione_par - (sequenza_manovre[i - 1].istante_inizio + durata_manovra_precedente);
																					//		if (durata_manovra[i] + sequenza_manovre[i].istante_inizio > durata_simulazione_par) {
																					//			corretto = 1;
																					//			printf("\n Errore! La manovra inserita eccede la durata massima di simulazione. Il tempo di simulazione ancora disponibile e' %f s.\n", TempoADisposizione);
																					//		}
																					//		if ((sequenza_manovre[i - 1].istante_inizio + durata_manovra_precedente) > sequenza_manovre[i].istante_inizio) {
																					//			corretto = 2;
																					//			printf("\n Errore! La manovra inserita ha inizio quando quella precedente non si e' ancora conclusa. \n");
																					//		}
																					//	}
																					//	return corretto;
																					//}

/**
 * Legge la lista delle manovre sequenza_manovre[] e la trasfoma nelle funzioni spezzate con l'andamento di ogni comando
 * Da eseguire dopo aver modificato il vettore della sequenza delle manovre sequenza_manovre[].
 *
 * Parametri:
 * durata_min_simulazione: durata minima della simulazione - se necessario i comandi vengono prolungati fino ad arrivare a durata_min_simulazione
 */
void Manovre_Converti_sequenza_manovre_in_sequenze_comandi(double durata_simulazione_par) {
	/* Crea i comandi per il solo aileron */
	Manovre_Converti_sequenza_manovre_in_sequenza_singolo_comando(AILERON,                  // tipo_comando_da_convertire
			sequenza_manovre,         // seq_manovre[]
			MAX_NUM_MANOVRE,          // max_num_manovre
			ManovraSpezzata_DeltaAil, // vett_manovra_spezzata[]
			MAX_NUM_PUNTI_SPEZZATA,   // int max_numero_punti_spezzata
			durata_simulazione_par);      // Durata della simulazione

	/* Crea i comandi per il solo ELEVATOR */
	Manovre_Converti_sequenza_manovre_in_sequenza_singolo_comando(ELEVATOR,                 // tipo_comando_da_convertire
			sequenza_manovre,         // seq_manovre[]
			MAX_NUM_MANOVRE,          // max_num_manovre
			ManovraSpezzata_DeltaEq,  // vett_manovra_spezzata[]
			MAX_NUM_PUNTI_SPEZZATA,   // int max_numero_punti_spezzata
			durata_simulazione_par);      // Durata della simulazione

	/* Crea i comandi per il solo RUDDER */
	Manovre_Converti_sequenza_manovre_in_sequenza_singolo_comando(RUDDER,                   // tipo_comando_da_convertire
			sequenza_manovre,         // seq_manovre[]
			MAX_NUM_MANOVRE,          // max_num_manovre
			ManovraSpezzata_DeltaRud, // vett_manovra_spezzata[]
			MAX_NUM_PUNTI_SPEZZATA,   // int max_numero_punti_spezzata
			durata_simulazione_par);      // Durata della simulazione

	/* Crea i comandi per il solo THRUST */
	Manovre_Converti_sequenza_manovre_in_sequenza_singolo_comando(THROTTLE,                 // tipo_comando_da_convertire
			sequenza_manovre,         // seq_manovre[]
			MAX_NUM_MANOVRE,          // max_num_manovre
			ManovraSpezzata_EngPow,   // vett_manovra_spezzata[]
			MAX_NUM_PUNTI_SPEZZATA,   // int max_numero_punti_spezzata
			durata_simulazione_par);      // Durata della simulazione

}

/**
 * Esamina la sequenza di manovre contenuta in seq_manovre[], filtra i comandi specificati in tipo_comando_da_convertire
 * e li trasforma in una sequenza di punti in vett_manovra_spezzata[];
 * max_num_manovre: numero di elementi  del vettore seq_manovre[]
 * max_numero_punti_spezzata: numero di elementi del vettore vett_manovra_spezzata[]
 */
void Manovre_Converti_sequenza_manovre_in_sequenza_singolo_comando(TIPO_COMANDO tipo_comando_da_convertire, DESCR_MANOVRA seq_manovre[], int max_num_manovre, TIMEVALUE vett_manovra_spezzata[],
		int max_numero_punti_spezzata, double durata_simulazione_par) {
	int seq_manovra_Ix, msIx;
	double start_time;
	double start_value;
	double current_time;
	double current_value;
	bool fine_manovre = false;

	/* Azzera il vettore */
	for (msIx = 0; msIx < max_numero_punti_spezzata; msIx++) {
		vett_manovra_spezzata[msIx].time = 0;
		vett_manovra_spezzata[msIx].value = 0;
	}
	/* Istant di tempo antecedente per forzare fine sequenza */
	vett_manovra_spezzata[1].time = -1;

	/* Istante e comandi iniziale */
	current_time = 0;
	current_value = 0;

	// Primo punto
	msIx = 0;
	vett_manovra_spezzata[msIx].time = 0;
	vett_manovra_spezzata[msIx].value = 0;

	/* Scandisci elenco manovre */
	fine_manovre = false;
	for (seq_manovra_Ix = 0; seq_manovra_Ix < max_num_manovre; seq_manovra_Ix++) {
		if (fine_manovre == true) // Se sono concluse le manovre
				{
			break;                // Interrompi la scansione
		}
		else if ((seq_manovre[seq_manovra_Ix].tipocomando == tipo_comando_da_convertire) ||	// Se il comando attuale e' quello da convertire
				(seq_manovre[seq_manovra_Ix].tipocomando == TUTTI_I_COMANDI))              // Oppure riguarda tutti i comandi
				{ /* procedi alla conversione */
			start_time = seq_manovre[seq_manovra_Ix].istante_inizio;	// Leggi istante inizio manovra
			if (start_time < current_time) // Se il suo tempo di inizio e' errato
					{                              // Interrompi
				printf("\n\n"
						"Errore nella sequenza di manovre:\n"
						"L'istante di inizio della manovra %d precede la fine della precedente manovra (%f < %f)\n", seq_manovra_Ix, start_time, current_time);
				user_input_StampaMsgAttendiTasto("");
				break;
			}
			else
				switch (seq_manovre[seq_manovra_Ix].manovra) // Se il tempo e' corretto analizza la manovra
				{

					/**
					 *             ^                 durata
					 *             |               |<------>| IMPULSO
					 *             |               |        |
					 *    Ampiezza | _ _ _ _ _ _ _  ________
					 *             |               B        C
					 *             |               |        |
					 *             |               |        |
					 *             |               |        |
					 *             |_______________A        D_________________
					 *             |
					 *            _|__________________________________________> t
					 *             |
					 */
					case IMPULSO: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.impulso.ampiezza;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto C */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.impulso.deltaT;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto D */
						current_value = start_value; // Ripristina il comando pre manovra
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = start_value;
					}
						break;

						/**
						 *    ^
						 *    |               RAMPA
						 *    |
						 *    |            -->|----|<-- durata
						 *    |
						 *    |                   /B
						 *    |                  / |
						 *    |                 /  |
						 *    |                /   |
						 *    |_______________A    C_________________
						 *    |
						 *   _|_______________________________________> t
						 *    |
						 */
					case RAMPA: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.rampa.ampiezza;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.rampa.durata;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto C */
						current_value = start_value; // Ripristina il comando pre manovra
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = start_value;
					}
						break;

						/**
						 *    ^
						 *    |               GRADINO
						 *    |                _______________________
						 *    |               B
						 *    |               |
						 *    |               |
						 *    |               |
						 *    |_______________A
						 *    |
						 *   _|_______________________________________> t
						 *    |
						 */
					case GRADINO: {
						current_time = start_time;   // Istante corrente

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.gradino.ampiezza;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;
					}
						break;

						/**
						 *    ^
						 *    |               TRAPEZIO
						 *    |
						 *    |                   ____________
						 *    |                  B            C
						 *    |                 /              \
				         *    |                /                \
				         *    |_______________A                  D____
						 *    |
						 *   _|_______________________________________> t
						 *    |
						 */
					case TRAPEZIO: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.trapezio.ampiezza_salita;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.trapezio.tempo_salita;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto C */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.trapezio.tempo_regime;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto D */
						current_value -= seq_manovre[seq_manovra_Ix].param_manovra.trapezio.ampiezza_discesa;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.trapezio.tempo_discesa;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;
					}
						break;

						/*
						 *           ^
						 *           |         DOUBLET
						 *  ampiezza_|          ________
						 *           |         B        C
						 *           |         |        |
						 *           |         |        |
						 *        0.0|_________A        |         __________________
						 *           |                  |        F
						 *           |                  |        |
						 *           |                  |        |
						 *           |                  D________E
						 *           |
						 *           |_____________________________________________> tempo
						 *
						 *                     |-deltaT-|-deltaT-|
						 */
					case DOUBLET: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.doublet.ampiezza;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto C */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doublet.deltaT;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto D */
						current_value -= 2 * seq_manovre[seq_manovra_Ix].param_manovra.doublet.ampiezza;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto E */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doublet.deltaT;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto F */
						current_value = start_value; // Ripristina valore iniziale
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

					}
						break;

						/**
						 *    ^
						 *    |               RAMPA_GRADINO
						 *    |
						 *    |                   B________________
						 *    |                  /
						 *    |                 /
						 *    |                /
						 *    |_______________A
						 *    |
						 *   _|_______________________________________> t
						 *    |
						 */

					case RAMPA_GRADINO: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.rampa_gradino.ampiezza;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.rampa_gradino.durata;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;
					}
						break;

						/**
						 *    ^
						 *    |               IMPOSTA_TRIM
						 *    |
						 *    |______________________________
						 *    |                              A
						 *    |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _\
				         *    |                                \
				         *    |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  B____
						 *    |                                 /
						 *    |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ /
						 *    |                               /
						 *    |______________________________A
						 *    |
						 *    |
						 *   _|_______________________________________> t
						 *    |
						 */

					case IMPOSTA_TRIM: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value = 0;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.imposta_trim.durata;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;
					}
						break;

						/*
						 *                        DOPPIO GRADINO SCONTRATO
						 *
						 *  	          ^                             tempo
						 *  	          |             |--durata_1--|--intermedio--|--durata_2--|
						 *  	          |             |            |              |            |
						 *                |_ _ _ _ _ _ _ ____________               |            |
						 *  	        / |             B            C              |            |
						 *  	       |  |             |            |              |            |
						 *  Ampiezza 1 |  |             |            |              |            |
						 *  	       |  |             |            |              |            |
						 *  	        \ |_____________A            D______________              ____________
						 *  	        / |                                         E            H
						 *  Ampiezza 2 |  |                                         |            |
						 *              \ |_ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _  F____________G
						 *  	          |
						 *  	          |
						 *  	          |______________________________________________________________________> tempo
						 *
						 */

					case DOPPIO_GRADINO_SCONTRATO: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.doppio_gradino_scontrato.ampiezza_1;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto C */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_gradino_scontrato.durata_1;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto D */
						current_value = start_value;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto E */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_gradino_scontrato.tempo_intermedio;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto F */
						current_value -= seq_manovre[seq_manovra_Ix].param_manovra.doppio_gradino_scontrato.ampiezza_2;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto G */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_gradino_scontrato.durata_2;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto H */
						current_value = start_value; // Ripristina valore iniziale
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;
					}
						break;

						/*
						 *                  ^
						 *                  |      DOPPIO TRAPEZIO SCONTRATO           tB-tA = tD-tC = tempo_rampa_1
						 *                 _| _ _ _ _ _ _ _  ________                  tC-tB = tempo_regime_1
						 *                / |               B        C                 tE-tD = intervallo_intermedio_trapezio
						 *               /  |              /          \                tF-tE = tH-tG = tempo_rampa_2
						 *   ampiezza_1 |   |             /            \               tG-tF = tempo_regime_2
						 *              |   |            /              \
				         *               \  |           /                \
				         *                \_|__________A                  D_______________                   ______________
						 *                / |                                             E                 H
						 *               /  |                                              \               /
						 *   ampiezza_2 |   |                                               \             /
						 *               \  |                                                \           /
						 *                \_| _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ _ F_________G
						 *                  |
						 *                  |________________________________________________________________________________> tempo [s] *
						 **/
					case DOPPIO_TRAPEZIO_SCONTRATO: {
						current_time = start_time;   // Istante corrente
						start_value = current_value; // Salva il valore del comando pre manovra

						/* Punto A */
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto B */
						current_value += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.ampiezza_1;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto C */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.tempo_regime_1;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto D */
						current_value = start_value;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.tempo_rampa_1;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto E */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.intervallo_intermedio_trapezi;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto F */
						current_value -= seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.ampiezza_2;
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto G */
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.tempo_regime_2;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						/* Punto H */
						current_value = start_value; // Ripristina valore iniziale
						current_time += seq_manovre[seq_manovra_Ix].param_manovra.doppio_trapezio_scontrato.tempo_rampa_2;
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;
					}
						break;

						/**
						 *    ^
						 *    |               FINE_MANOVRE
						 *    |
						 *    |....___________________________A
						 *    |
						 *   _|_______________________________________> t
						 *    |
						 */

					case FINE_MANOVRE: {
						current_time = start_time;   // Istante corrente
						if (current_time < durata_simulazione_par)	// Se la durata attuale della simulazione non raggiunge il minimo specificato
								{
							current_time = durata_simulazione_par;  // Impostalo
						}
						/* Punto A (ultimo punto)*/
						msIx++;
						vett_manovra_spezzata[msIx].time = current_time;
						vett_manovra_spezzata[msIx].value = current_value;

						fine_manovre = true; // Segnala fine manovre per uscire dal ciclo
					}
						break;

					default:
						break;
				}
		}
		else // Se il comando non e' quello da convertire ne' riguarda tutti i comandi
		{
			// Non fare nulla, ignoralo
		}
	}
}

/**
 * Stampa i vertici della spezzata che compone ogni comando
 * parametro:
 * 	impostare fOut a stdout per stampare a video, altrimenti inserire il puntatore al file in cui scrivere
 *
 */
void Manovre_Print_All_Command(FILE *fOut) {
	float tmax;

	tmax = Manovre_getDurata(); // Leggi durata complessiva delle manovre

	printf("Durata manovre: %f\n", tmax);

	Manovre_Print_Single_Command(fOut, "AILERON", ManovraSpezzata_DeltaAil, MAX_NUM_PUNTI_SPEZZATA);
	Manovre_Print_Single_Command(fOut, "ELEVATOR", ManovraSpezzata_DeltaEq, MAX_NUM_PUNTI_SPEZZATA);
	Manovre_Print_Single_Command(fOut, "RUDDER", ManovraSpezzata_DeltaRud,  MAX_NUM_PUNTI_SPEZZATA);
	Manovre_Print_Single_Command(fOut, "THROTTLE", ManovraSpezzata_EngPow,  MAX_NUM_PUNTI_SPEZZATA);
}

/* Stampa a video l'andamento temporale del comando specificato */
void Manovre_Print_Single_Command(FILE *fOut, char *NomeComando, TIMEVALUE vett_manovra_spezzata[], int max_numero_punti_spezzata) {
	int i;
	double t, last_t;

	if (fOut != NULL)
	{
		fprintf(fOut,
				"              ""------------------------------------\n"
				"              ""           Comandi %s \n"
				"              ""------------------------------------\n"
				"\n", NomeComando);

		fprintf(fOut,
				"              ""        t      |   Value \n"
				"              ""      ---------|-------------\n");
		//"123456|1234567890|123456789|1234567890|1234567");
		t = -1;
		for (i = 0; i <= max_numero_punti_spezzata; i++) {
			last_t = t;	// Salva il valore precedente
			t = vett_manovra_spezzata[i].time;
			if (t < last_t)	// Se l'istante attuale e' antecedente all'ultimo --> fine comandi
					{
				break;
			}
			else {
				fprintf(fOut,
						"              ""      %8.2f | %12.6f \n", t, vett_manovra_spezzata[i].value);
			}
		}
		fprintf(fOut, "\n");
	}
}
