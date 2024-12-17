#ifndef ENGINE_H_INCLUDED
#define ENGINE_H_INCLUDED
#include "dati.h"


void engine_dati();
double engineCalcFuelConsumption(double power_kw, double sfc_kgs_w, double delta_t_s);
ENG engine_variabili();

/**
 * Calcola la potenza max erogabile dal motore alla quota specificata
 */
double engineCalcPmax(ENG *peng, double h);

#endif // ENGINE_H_INCLUDED
