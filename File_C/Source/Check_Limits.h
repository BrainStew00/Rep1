#ifndef CHECK_LIMITS_H_
#define CHECK_LIMITS_H_
#include "dati.h"

extern double Debug_AumentoSogliaVelocita_Max;

#define QUOTA_DI_TANGENZA_M		4116	// Quota di tangenza in m
#define VELOCITA_MAX_M_S        (Debug_AumentoSogliaVelocita_Max + (228 / 3.6))		// Velocita' max in m/s
#define VELOCITA_STALLO_M_S     ( 81 / 3.6)		// Velocita' di stallo in m/s

void Check(CMD *pcomandi, PLANE *pplane);

// Mantiene i comandi entro il range consentito
void Check_Limits_Keep_Cmd_In_Range(CMD *pcomandi, PLANE *pplane);

// Verifica che la simulazione proceda entro i limiti, in caso contrario stampa un messaggio di errore e restituisce il valore
// true per interrompere la simulazione
void Check_Limits_set_error_and_warning(double t, STATE *pstate, double P_al, double P_max, PLANE *pplane);



#endif /* CHECK_LIMITS_H_ */
