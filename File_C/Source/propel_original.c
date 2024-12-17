#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "dati.h"
#include "Atmosphere.h"
#include "propel_original.h" // Verifica protitipi

#define pi 3.14159265358979323846
#define MAX_NUM_STAZIONI	150

/**
 * Calcolo della spinta
 * input:
 * 	pprop: Puntatore alle caratteristiche dell'elica
 * 	RPM_ref: velocita' di rotazione
 * 	Vel: velocita' del velivolo
 * 	h: quota
 * output:
 *   tep.Torque
 *   tep.Thrust
 *   tep.RPM (copia degli RPM ricevuti in ingresso
 *   tep.t =
 *   tep.q =
 *   tep.J =
 *   tep.eff
 *   tep.P_al
 *   tep.Pmax*
 */

TEP propel_original(PROP *pprop, float RPM_ref, float Vel, double rho1) {
	TEP tep;
	float diam = pprop->Diametro; //diametro elica
	float Raggio = diam / 2.0; //raggio elica
	double diam_ogiva = pprop->DiametroOgiva;
	double Raggio_ogiva_ad = diam_ogiva / diam; // Sarebbe raggio ogiva/raggio elica ma il rapporto e' lo stesso
	int   Num_Stazioni = pprop->NumStazioni;
	double n = RPM_ref / 60.0; //round-per-second
	double omega = n * 2.0 * pi; //velocia' angolare [rad/s]
	double rstep_ad; // Lunghezza di una stazione (adimensionale)
	double rstep; //step della stazione j-esima
	double r1[MAX_NUM_STAZIONI]; //creazione vettore delle 49 stazioni (-> CSI in propeller.txt)
	double theta1 = NAN;
	double alpha1 = NAN;
	double t2[MAX_NUM_STAZIONI], a2[MAX_NUM_STAZIONI], b2[MAX_NUM_STAZIONI];
	double th = NAN, phi1 = NAN, DtDr = NAN, DqDr = NAN, cl = NAN, cd = NAN, CT = NAN, CQ = NAN, tem1 = NAN, tem2 = NAN;
	double a = NAN, anew = NAN;
	double b = NAN, bnew = NAN;
	int j = 0;
	int finished = 0;
	/*Inizializzazione r1*/
	double rad = NAN;
	double Vlocal = NAN, V0 = NAN, V2 = NAN;

	for (j = 0; j < MAX_NUM_STAZIONI; j++) {
		r1[j] = NAN;
		t2[j] = NAN;
		a2[j] = NAN;
		b2[j] = NAN;
	}

        // Calcola il raggio di ogni stazione
    for (j = 0; j < MAX_NUM_STAZIONI; j++) {
        if(j < Num_Stazioni) {
            r1[j] = pprop->CSI[j] * Raggio;
        }
        else {
            break;
        }
    }

	TEP_Initialize(&tep);

	tep.RPM = RPM_ref;
	tep.Thrust = 0.0; //inizializzazione vettore spinta
	tep.Torque = 0.0; //inizializzazione vettore coppia

	rstep_ad = 1.0 / Num_Stazioni;
		// Cerca l'indice j dove finisce l'ogiva
	for(j = 0; j < Num_Stazioni; j++)
	{
		if (pprop->CSI[j] >= (Raggio_ogiva_ad + rstep_ad / 2)) // Se ho raggiunto la coordinata dove finisce l'ogiva con un margine di mezzo step
		{
			break; // Interrompi, j corrisponde all'inizio della parte attiva della pala
		}
	}
	for (/* j Inizializzato due righe sopra */; j < Num_Stazioni; j++) {
		rad = r1[j]; //coordinata j-esima stazione (-> CSI in propeller.txt)

        rstep = r1[j] - r1[j-1];
        theta1 = pprop->BA[j]; //calcolo angolo di svergolamento della j-esima stazione
        t2[j] = theta1; //angolo di svergolamento della j-esima stazione (-> BA su propeller.txt)
		th = DEG_TO_RAD(theta1); //angolo di svergolamento [rad]
		a = 0.1; //inizializzazione axial inflow factor (vedi pag.4 PROPEL.pdf)
		b = 0.01; //inizializzazione angular inflow (swirl) factor (vedi pag.4 PROPEL.pdf)
		finished = 0; //inizializzione flag
		int sum = 0; //inizializzione variabile di supporto
		while (finished == 0) {
			V0 = Vel * (1 + a); //componente del flusso all'incirca uguale alla velocit� di avanzamento del velivolo (Vinf), aumentata tramite l'axial inflow factor
			V2 = omega * rad * (1 - b); //componente del flusso all'incirca uguale alla velocit� angolare della sezione della pala (omega*rad), ridotta tramite l'angular inflow factor
			phi1 = atan2(V0, V2); //angolo tra le due componenti del flusso V0 e V2
			alpha1 = th - phi1; //angolo di attacco raltivo alla j-esima sezione della pala
			cl = pprop->Cl0 + pprop->ClA * alpha1; //L coefficiente di portanza
			cd = pprop->Cd0 + pprop->CdA * alpha1 + pprop->CdA2 * alpha1 * alpha1; // CD coefficiente di resistenza CD = CD0+CD1*CL+CD2*CL^2 (NB nel nostro caso, CD = CD0+CD_alpha*alpha+CD_alpha2*alpha^2 -> slide lezione 2)
			Vlocal = sqrt(V0 * V0 + V2 * V2); // velocit� locale del flusso
			CT = cl * cos(phi1) - cd * sin(phi1); //CT coefficiente di spinta adimensionale
			DtDr = 0.5 * rho1 * Vlocal * Vlocal * pprop->NPale * pprop->CH[j] * CT; //contributo di spinta della j-esima sezione
			CQ = cd * cos(phi1) + cl * sin(phi1); //CQ coefficiente di coppia adimensionale
			DqDr = 0.5 * rho1 * Vlocal * Vlocal * pprop->NPale * pprop->CH[j] * rad * CQ; //contributo di coppia della j-esima sezione
			tem1 = DtDr / (4.0 * pi * rad * rho1 * Vel * Vel * (1 + a)); //fattore correttivo del coefficiente "a"
			tem2 = DqDr / (4.0 * pi * rad * rad * rad * rho1 * Vel * (1 + a) * omega); //fattore correttivo del coefficiente "b"
			anew = 0.5 * (a + tem1); //nuovo valore coefficiente "a"
			bnew = 0.5 * (b + tem2); //nuovo valore coefficiente "b"
			//processo iterativo per arrivare a convergenza
			if (fabs(anew - a) < 1e-5) {
				if (fabs(bnew - b) < 1e-5) {
					finished = 1;
				}
			}
			a = anew; //definizione valore finale coefficiente "a"
			b = bnew; //definizione valore finale coefficiente "b"
			if (++sum > 500) {
				finished = 1;
			}
		}
    	a2[j] = a; //definizione valore finale coefficiente "a" per la j-esima stazione
    	b2[j] = b; //definizione valore finale coefficiente "b" per la j-esima stazione
		tep.Thrust = tep.Thrust + DtDr * rstep; //sommatoria dei contributi di spinta dalla stazione 1 alla stazione j
		tep.Torque = tep.Torque + DqDr * rstep; //sommatoria dei contributi di coppia dalla stazione 1 alla stazione j
	}

	// Elimina il warning di variabile impostata e non utilizzata con un cast a void
	(void) a2[0];
	(void) b2[0];
	(void) t2[0];

	tep.t = tep.Thrust / (rho1 * n * n * diam * diam * diam * diam); //coefficiente di spinta adimensionale
	tep.q = tep.Torque / (rho1 * n * n * diam * diam * diam * diam * diam); //coefficiente di coppia adimensionale
	tep.J = Vel / (n * diam); //rapporto di avanzamento
	if (tep.t < 0) {
		tep.eff = 0.0; //efficienza elica
	}
	else {
		tep.eff = tep.t / tep.q * tep.J / (2.0 * pi); //efficienza elica
	}
	tep.P_al = tep.Torque * omega / 1000; // Potenza in kW

	return tep;
}

