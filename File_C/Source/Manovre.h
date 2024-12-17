#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dati.h"


/**************************************************************************************************
 * Definizione tipi di dato
 **************************************************************************************************/


typedef enum
{
	IMPULSO,
	RAMPA,
	GRADINO,
	TRAPEZIO,
	DOUBLET,
	RAMPA_GRADINO,
	IMPOSTA_TRIM,               // Manda a zero il comando nel tempo s
	DOPPIO_GRADINO_SCONTRATO,
	DOPPIO_TRAPEZIO_SCONTRATO,
	FINE_MANOVRE		// Valore speciale per concludere l'elenco delle manovre
} MANOVRA;

typedef enum
{
	AILERON,            // 0
	ELEVATOR,           // 1
	RUDDER,             // 2
	THROTTLE,           // 3
	TUTTI_I_COMANDI,    // 4
	NUMERO_COMANDI      // 5
} TIPO_COMANDO;


typedef union ManovraDescr
{
	struct
	{
		double ampiezza;
		double deltaT;
	} impulso;

	struct
	{
		double ampiezza;
		double durata;
	} rampa;

	struct
	{
		double ampiezza;
	} gradino;

	struct
	{
		double ampiezza_salita; // Ampiezza rispetto a valore iniziale
		double tempo_salita;
		double tempo_regime;
		double ampiezza_discesa; // Ampiezza rispetto a regime
		double tempo_discesa;
	} trapezio;

	struct
	{
		double ampiezza;
		double durata;
	} rampa_gradino;

	struct
	{
		double durata;
	} imposta_trim;

	struct
	{
		double ampiezza_1;
		double durata_1;
		double tempo_intermedio;
		double ampiezza_2;
		double durata_2;
	} doppio_gradino_scontrato;

	struct
	{
		double ampiezza_1;
		double tempo_rampa_1;
		double tempo_regime_1;
		double intervallo_intermedio_trapezi;
		double ampiezza_2;
		double tempo_rampa_2;
		double tempo_regime_2;
	} doppio_trapezio_scontrato;
	struct
	{
		double ampiezza;
		double deltaT;
	} doublet;

} PARAM_MANOVRA;

typedef struct
{
	double time;
	double value;
} TIMEVALUE;


/**
 * Descrizione della sequenza di manovre
 *  */
typedef struct
{
	double       istante_inizio;
	TIPO_COMANDO tipocomando;
	MANOVRA       manovra;
	PARAM_MANOVRA param_manovra;
} DESCR_MANOVRA;

/**************************************************************************************************
 * Variabili globali
 **************************************************************************************************/
extern DESCR_MANOVRA sequenza_manovre[];
extern TIMEVALUE ManovraSpezzata_DeltaAil[];
extern TIMEVALUE ManovraSpezzata_DeltaEq[];
extern TIMEVALUE ManovraSpezzata_DeltaRud[];
extern TIMEVALUE ManovraSpezzata_EngPow[];

/**************************************************************************************************
 * Prototipi
 **************************************************************************************************/

void Manovre_Inizializza(void);

/**
 * Legge i comandi all'istante t interpolando le linee spezzate che li descrivono
 */
CMD Manovre_getCommand(double t);

/**
 * Restituisce la durata complessiva delle manovre
 * Analizza le sequenze e prende la piu' lunga
 */
double Manovre_getDurata(void);


void Azzera_comandi(double durataSimulazione);

void Manovre_lettura_da_tastiera(double durata_simulazione);
void Manovre_Inserisci_manovra_di_test(int test, USERVAL *pUserValues);

/**
 * Stampa i vertici della spezzata che compone ogni comando
 * parametro:
 * 	impostare fOut a stdout per stampare a video, altrimenti inserire il puntatore al file in cui scrivere
 *
 */
void Manovre_Print_All_Command(FILE *fOut);

void Manovre_debug_Immetti_Comandi_debug(void);
void Manovre_debug_Immetti_Comandi_debug_2(void);
void Manovre_Converti_sequenza_manovre_in_sequenze_comandi(double durata_simulazione_par);
