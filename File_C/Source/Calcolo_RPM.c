#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "dati.h"
#include "propel_original.h"
#include "Calcolo_RPM.h" // Verifica congruenza prototipi
#include "Atmosphere.h"
#include "time.h"
#include "ErrWarn.h"


#define g 9.80665
TEP Calcolo_RPM(ENG eng, AC *der, CMD *cmd, PLANE velivolo, STATE *stato_deg, PROP *pprop) {
	char msg[MAX_LUNGHEZZA_STRINGA];
	TEP tep;
	double rpm = NAN, T_trim = NAN, res = 0.01, Cx_tot = NAN, upper_limit = eng.RPMmax, lower_limit = eng.RPMmin;
	double rho = NAN;
	double Vel, quota;
	int trovato = 0;

	Vel = stato_deg->V;
	quota = stato_deg->h;
	rho = Atmosphere_Rho(quota);
	TEP_Initialize(&tep);

	Cx_tot = (der->CX) + (der->CXA) * DEG_TO_RAD(der->Alpha) + (der->CXde) * DEG_TO_RAD(cmd->DeltaEq);
	T_trim = velivolo.Mass * g * sin(DEG_TO_RAD(stato_deg->Theta)) - 0.5 * Cx_tot * rho * velivolo.WSurf * pow((stato_deg->V), 2);
	//printf("\n T_Trim = %f\n", T_trim);
	//printf("\n %f \n", eng.RPMmax);
		/* Verifica che il thrust di trim richiesto non sia superiore alle possibilita'� del motore */
	tep = propel_original(pprop, upper_limit, Vel, rho);
	if (T_trim >= tep.Thrust) {
		sprintf(msg, "Trim non valutabile, la spinta richiesta (%.0fN) e' oltre il limite (%.0fN) ", T_trim, tep.Thrust);
		ErrWarn_DisplayErrMsgAndExit(E001, msg);
	}
		/* Verifica che il thrust di trim richiesto non sia inferiore al range consentito dal motore */
	tep = propel_original(pprop, lower_limit,  Vel, rho);
	if (T_trim < tep.Thrust) {
		sprintf(msg, "Trim non valutabile, la spinta richiesta (%.0fN) e' inferiore al minimo (%.0fN) ", T_trim, tep.Thrust);
		ErrWarn_DisplayErrMsgAndExit(E002, msg);
	}
	else {
		while (!trovato) {
			rpm = (upper_limit + lower_limit) / 2;
			//DEBUG clock_t start, end;
			//DEBUG double tempo_di_esecuzione;
			// Registra il tempo di inizio
			//DEBUG start = clock();
			tep = propel_original(pprop, rpm,  Vel, rho);

			// Registra il tempo di fine
			//DEBUG end = clock();

			// Calcola il tempo di esecuzione in secondi
			//DEBUG tempo_di_esecuzione = ((double) (end - start)) / CLOCKS_PER_SEC;

			//DEBUG printf("Il tempo di esecuzione della funzione propel original �: %f secondi\n", tempo_di_esecuzione);

			if (fabs(T_trim - (tep.Thrust)) <= res) {
				tep.RPM = rpm;
				trovato = 1;
			}
			else {
				if (tep.Thrust < T_trim) {
					lower_limit = rpm;
				}
				else {
					upper_limit = rpm;
				}
			}
		}
	}

	return tep;
}
