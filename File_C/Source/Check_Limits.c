/****
 * Funzioni di controllo dei limiti
 *
 **/
#include "dati.h"
#include "stdlib.h"
#include "stdio.h"
#include "stdbool.h"
#include "Atmosphere.h"
#include "ErrWarn.h"
#include "Check_Limits.h"


// Mantiene i comandi entro il range consentito
// Se i comandi sono fuori dei limiti vengono riportati entro i limiti
void Check_Limits_Keep_Cmd_In_Range(CMD *pcomandi, PLANE *pplane) {
   	char msg[MAX_LUNGHEZZA_STRINGA];

	double elevator_max = pplane->ElevatorMax;
	double elevator_min = pplane->ElevatorMin;

	double aileron_max =  pplane->Ailerons;
	double aileron_min = -pplane->Ailerons;

	double rudder_max =  pplane->Rudder;
	double rudder_min = -pplane->Rudder;

		/* Mantenimento comando Elevator entro i limiti */
	if (pcomandi->DeltaEq > elevator_max) {
		sprintf(msg, "Il valore di comando del timone di profondita' %f supera il limite max, corretto a %f", pcomandi->DeltaEq, elevator_max);
		ErrWarn_DisplayWarnMsgAndContinue(W001, msg);
		pcomandi->DeltaEq = elevator_max;
	}
	else if (pcomandi->DeltaEq < elevator_min) {
		sprintf(msg, "Il valore di comando del timone di profondita' %f supera il limite min, corretto a %f", pcomandi->DeltaEq, elevator_min);
		ErrWarn_DisplayWarnMsgAndContinue(W002, msg);
		pcomandi->DeltaEq = elevator_min;
	}
	else {
		/* DeltaEq entro i limiti, non fare nulla */
	}

		/* Mantenimento comando Rudder entro i limiti */
	if (pcomandi->DeltaRud > rudder_max) {
		sprintf(msg, "Il valore di comando del timone di direzione' %f supera il limite max, corretto a %f", pcomandi->DeltaRud, rudder_max);
		ErrWarn_DisplayWarnMsgAndContinue(W003, msg);
		pcomandi->DeltaRud = rudder_max;
	}
	else if (pcomandi->DeltaRud < rudder_min) {
		sprintf(msg, "Il valore di comando del timone di direzione' %f supera il limite min, corretto a %f", pcomandi->DeltaRud, rudder_min);
		ErrWarn_DisplayWarnMsgAndContinue(W004, msg);
		pcomandi->DeltaRud = rudder_min;
	}
	else {
		/* DeltaRud entro i limiti, non fare nulla */
	}

	/* Mantenimento comando Aileron entro i limiti */
	if (pcomandi->DeltaAil > aileron_max) {
		sprintf(msg, "Il valore di comando degli alettoni' %f supera il limite max, corretto a %f", pcomandi->DeltaAil, aileron_max);
		ErrWarn_DisplayWarnMsgAndContinue(W005, msg);
		pcomandi->DeltaAil = aileron_max;
	}
	else if (pcomandi->DeltaRud < aileron_min) {
		sprintf(msg, "Il valore di comando degli alettoni' %f supera il limite min, corretto a %f", pcomandi->DeltaAil, aileron_min);
		ErrWarn_DisplayWarnMsgAndContinue(W006, msg);
		pcomandi->DeltaAil = aileron_min;
	}
	else {
		/* DeltaRud entro i limiti, non fare nulla */
	}

		/* Mantenimento comando EngPow entro i limiti */

	{      //MOD001
		if (pcomandi->EngPow > 100.0) {	// Limita il comando a 0-100%
			sprintf(msg, "Il valore di manetta comandato supera il massimo consentito: valore impostato a 100%");
			ErrWarn_DisplayWarnMsgAndContinue(W007, msg);
			pcomandi->EngPow = 100.0;
		}
		else if (pcomandi->EngPow < 0.0) {
			sprintf(msg, "Il valore di manetta comandato inferiore al minimo consentito: valore impostato a 0%");
			ErrWarn_DisplayWarnMsgAndContinue(W008, msg);
			pcomandi->EngPow = 0.0;
		}
		else {
			/* EngPow entro i limiti, non fare nulla */
		}
	}
}


/**
 * Verifica che la simulazione proceda entro i limiti, in caso contrario stampa un messaggio di errore e interrompe la simulazione
 * Parametri:
 * t: istante di tempo
 * *pstate: stato attuale
 * P_al: potenza assorbita dall'elica
 * *pplane parametri del velivolo
 * *peng: parametri dell motore
 */
	//

void Check_Limits_set_error_and_warning(double t, STATE *pstate, double P_al, double P_max, PLANE *pplane)
{
	char msg[MAX_LUNGHEZZA_STRINGA];


		// Controllo quota sotto zero
	if (pstate->h < 0){
		sprintf(msg, "[t=%6.2f] Il velivolo si e' schiantato a terra", t);
		ErrWarn_DisplayErrMsgAndExit(E003, msg);
	}

		// Controllo quota di tangenza
	if (pstate->h > QUOTA_DI_TANGENZA_M){
		sprintf(msg, "[t=%6.2f] Il velivolo ha superato la quota di tangenza", t);
		ErrWarn_DisplayErrMsgAndExit(E004, msg);
	}

		// Controllo velocita' di stallo
	if (pstate->V < VELOCITA_STALLO_M_S){
		sprintf(msg, "[t=%6.2f] La velocita' del velivolo %.1f m/s e' inferiore alla velocita' di stallo di %.1f m/s.", t, pstate->V, VELOCITA_STALLO_M_S);
		ErrWarn_DisplayErrMsgAndExit(E005, msg);
	}

		// Controllo velocita' max
	if (pstate->V > VELOCITA_MAX_M_S){
		sprintf(msg, "[t=%6.2f] La velocita' del velivolo %.1f m/s e' superiore alla velocita' massima %.1f m/s.", t, pstate->V, VELOCITA_MAX_M_S);
		ErrWarn_DisplayErrMsgAndExit(E006, msg);
	}

		// Controllo carburante
	if (pplane->FuelMass <= 0){
		sprintf(msg, "[t=%6.2f] Il carburante e' terminato.", t);
		ErrWarn_DisplayErrMsgAndExit(E007, msg);
	}

		// Controllo velocita' di Mach
	{
		double Mach;
		double vsuono_h;

		vsuono_h = Atmosphere_vsuono(pstate->h);
		Mach = pstate->V / vsuono_h;
		if (Mach > pplane->MDR){
			sprintf(msg, "Il Mach di volo %.3f supera il Mach di Drag Rise %.3f,", Mach, pplane->MDR);
			ErrWarn_DisplayErrMsgAndExit(E009, msg);
		}
	}
}


