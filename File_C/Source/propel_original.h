#ifndef PROPEL_ORIGINAL_H_INCLUDED
#define PROPEL_ORIGINAL_H_INCLUDED

/**
 * Calcolo della spinta
 * Parametri:
 * 	pprop: Puntatore alle caratteristiche dell'elica
 * 	RPM_ref: velocita' di rotazione
 * 	Vel: velocita' del velivolo
 * 	h: quota
 */
TEP propel_original(PROP *pprop, float RPM_ref, float Vel, double h);

#endif // PROPEL_ORIGINAL_H_INCLUDED
