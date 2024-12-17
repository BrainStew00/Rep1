#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

#include "ErrWarn.h"
#include "dati.h"
#include "interpolazione.h"
#include "Trim.h"

#define pi 3.14159265358979323846
#define g 9.80665

/**
 * Ricerca delle condizioni di trim
 * Vengono esaminati tutti i possibili angoli alpha dal min al max
 * Per ognuno si calcola il deltae che equilibra il momento
 */
CMD Trim(PLANE velivolo, STATE *Stato_deg, AC *Der, double rho) {
	char msg[MAX_LUNGHEZZA_STRINGA];
	CMD cmd;
	double alphamin = NAN, alpha = NAN, alphamax = NAN, alpha_precedente = NAN,
		   deltaalpha = 0.1, alphatrim = NAN, deltaein = NAN, deltaein_rad = NAN, deltaetrim = NAN;
	double resCz = 1e-6, Risz = NAN, Risz1 = NAN, Risz2 = NAN, Risz_precedente = NAN, theta = NAN, Cztot = NAN;
	bool trovato, fine_ricerca, Risz_precedente_disponibile;
	int conta;


	CMD_Initialize(&cmd);
	printf("\n Ricerca delle condizioni di trim:\n");

	conta = 0;
	alphamin = -5;
	alphamax = 20;
	deltaalpha = 0.1;
	fine_ricerca = false;
	while ((!fine_ricerca) && (conta < 1000))
	{
		Risz_precedente_disponibile = false; // Non ho ancora un Risz precedente con cui fare il confronto
            // Ricerca per i valori di alpha dal min al max attuali. Aggiungi deltaalpha per essere sicuro di includere il lato destro
		for (alpha = alphamin; alpha <= alphamax + deltaalpha; alpha += deltaalpha)
		{
			conta++; // Incrementa contatore di ricerca
			// Calcola il residuo
			*Der = interpolazione(velivolo.Der, alpha);
			deltaein_rad= (-Der->Cm - Der->CmA * DEG_TO_RAD(alpha)) / (Der->Cmde); //NOTE: In questo punto Der->Cmde e' zero e deltaein � infinito

			theta = alpha + Stato_deg->Gamma;
			Cztot = (Der->CZ) + (Der->CZA * DEG_TO_RAD(alpha)) + (Der->CZde * deltaein_rad);
			Risz1 = 0.5 * Cztot * rho * velivolo.WSurf * pow((Stato_deg->V), 2);
			Risz2 = velivolo.Mass * g * cos(DEG_TO_RAD(theta));
			Risz = Risz1 + Risz2; //m * g * cos(alpha + gamma) + 1/2 * Cztot * rho * V^2

			deltaein = RAD_TO_DEG(deltaein_rad);
	        //printf("\r%6d: al= %+.12f   Risz = %+1.6e   de= %+2.6f", conta, alpha, Risz, deltaein);

			deltaetrim = deltaein;
			if (fabs(Risz) <= resCz) // Se Risz e' zero entro l'errore prestabilito
			{
				alphatrim = alpha;
				trovato = true;
				fine_ricerca = true;
				break; // Interrompi ciclo for
			}
			else // Se Risz non e' ancora zero
			{
				if (Risz_precedente_disponibile)	// Se Risz_precedente e' disponibile controllo se Risz ha cambiato segno
				{
					if ((Risz_precedente * Risz) < 0) // Se Risz ha cambiato di segno il Risz va a zero in un alpha compreso tra
					{                                 // il valore precedente di alpha e quello attuale
						alphamin = alpha_precedente;  // Sposto di conseguenza l'intervallo di ricerca
						alphamax = alpha;
						deltaalpha /= 10; // Riduco di 10 volte il passo di ricerca
						fine_ricerca = false;
						break; // Interrompi ciclo for
					}
				}
				Risz_precedente = Risz;   // Salviamo Risz per il confronto successivo
				alpha_precedente = alpha; // Salva alpha corrispondente
				Risz_precedente_disponibile = true;  // Da adesso in poi Risz_precedente e' disponibile
			}

		}

	}

	//  printf("\n %f %f \n",Risz2,Risz1); // prova di stampa per controllare
	if ((!trovato) && (fabs(Risz) > resCz)) {
		ErrWarn_DisplayErrMsgAndExit(E008, "Trim non valutabile");
	}
	else if (deltaetrim > velivolo.ElevatorMax) {
		sprintf(msg, "L'angolo dell'equilibratore %.1f in condizioni di trim supera il suo valore massimo %.0f.", deltaetrim, velivolo.ElevatorMax);
		deltaetrim = velivolo.ElevatorMax;
		cmd.DeltaEq = deltaetrim;
		ErrWarn_DisplayWarnMsgAndContinue(W019, msg);
	}
	else if (deltaetrim < -velivolo.ElevatorMax) {
		sprintf(msg, "L'angolo dell'equilibratore %.1f in condizioni di trim e' inferiore al suo valore massimo %.0f.", deltaetrim, -velivolo.ElevatorMax);
		deltaetrim = -velivolo.ElevatorMax;
		cmd.DeltaEq = deltaetrim;
		ErrWarn_DisplayWarnMsgAndContinue(W020, msg);
	}
	else {
		cmd.DeltaEq = deltaetrim;
		cmd.DeltaAil = 0;
		cmd.DeltaRud = 0;
		cmd.EngPow = NAN;
		Stato_deg->u = (Stato_deg->V) * cos(DEG_TO_RAD(alphatrim)); //Controllare gradi o rad
		Stato_deg->v = 0;
		Stato_deg->w = (Stato_deg->V) * sin(DEG_TO_RAD(alphatrim));
		Stato_deg->p = 0;
		Stato_deg->q = 0;
		Stato_deg->r = 0;
		Stato_deg->Phi = 0;
		Stato_deg->Theta = alphatrim + Stato_deg->Gamma;
		Stato_deg->Psi = 0;
	}
	return cmd;
}
