#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "dati.h"
#include "ErrWarn.h"
#include "interpolazione.h"

AC interpolazione(double Der[64][126], double alphanew) {
	AC Dernew;
	double *ptr;
	int i, j, trovato = 0;
	double dalpha, delta;
	char msg[MAX_LUNGHEZZA_STRINGA];


	AC_Initialize(&Dernew);
	ptr = &Dernew.Alpha;
	*ptr = alphanew;

	if ((alphanew < -5) || (alphanew > 20)) {
		sprintf(msg, "alpha=%.2f non rispetta i limiti (-5..20)", alphanew);
		ErrWarn_DisplayErrMsgAndExit(E016, msg);
	}
	ptr++;
	i = 0;
	while (!trovato) {
		if ((alphanew >= Der[0][i]) && (alphanew <= Der[0][i + 1])) {
			dalpha = alphanew - Der[0][i];
			trovato = 1;
		}
		else
			i++;
	}
	for (j = 1; j < 64; j++) {
		delta = (Der[j][i + 1] - Der[j][i]) / (Der[0][i + 1] - Der[0][i]);
		*ptr = Der[j][i] + delta * dalpha;
		ptr++;
	}
	return Dernew;
}
