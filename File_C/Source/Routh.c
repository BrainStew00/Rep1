#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "ErrWarn.h"
#include "dati.h"
#include "Routh.h"
#include "Chart.h"

#define g 9.80665
#define CDu 0
#define CLu 0
#define Cmu 0
#define CD0 0.0235
#define k 0.047

void Routh(PLANE velivolo, AC der, double Vel, double rho) {
	double mu = NAN, Iycap = NAN, Cz_alpha_dot = NAN, CDe = NAN, CLe = NAN, CWe = NAN, Cl_alpha = NAN, Cl_alpha_dot = NAN, CTu = NAN, Cd_alpha = NAN, Cmq = der.CmQ, Cm_alpha_dot = der.CmAP, Cm_alpha =
			der.CmA;
	double A1 = NAN, B1 = NAN, C1 = NAN, D1 = NAN, E1 = NAN, delta = NAN, omega_ph_n = NAN, omega_sp_n_cap = NAN, omega_sp_n = NAN, omega_ph = NAN, omega_sp = NAN, zita_ph = NAN, zita_sp = NAN,
			eta_ph = NAN, eta_sp = NAN, T_ph = NAN, t_dim_ph = NAN, T_sp = NAN, t_dim_sp = NAN;
    FILE *fSimDescr;


	fSimDescr = chart_simulation_description_file_get_file_handler();

	CLe = g * velivolo.Mass * 2 / (rho * pow(Vel, 2) * velivolo.WSurf);
	mu = (2 * velivolo.Mass) / (rho * velivolo.WSurf * velivolo.Chord);
	Iycap = (8 * velivolo.JY) / (rho * velivolo.WSurf * pow(velivolo.Chord, 3));
	Cz_alpha_dot = der.CZAP;
	CWe = CLe;
	CDe = CD0 + k * pow(CLe, 2);
	Cl_alpha = der.CXA * sin(DEG_TO_RAD(der.Alpha)) - der.CZA * cos(DEG_TO_RAD(der.Alpha));
	Cd_alpha = 2 * k * Cl_alpha * CLe;
	Cl_alpha_dot = -Cz_alpha_dot * cos(DEG_TO_RAD(der.Alpha));
	CTu = -3 * CDe;

	A1 = 2 * mu * Iycap * (2 * mu + Cl_alpha_dot);
	B1 = 2 * mu * Iycap * (Cl_alpha + CDe - CTu) - Iycap * CTu * Cl_alpha_dot - 2 * mu * Cmq * Cl_alpha_dot - 4 * pow(mu, 2) * (Cmq + Cm_alpha_dot);
	C1 = 2 * mu * (Cmq * (CTu - Cl_alpha - CDe) - 2 * mu * Cm_alpha + Cm_alpha_dot * CTu) + Iycap * (2 * CWe * (CWe - Cd_alpha) + CTu * Cl_alpha + CDe * Cl_alpha) + Cmq * Cl_alpha_dot * CTu;
	D1 = -2 * pow(CWe, 2) * Cm_alpha_dot + 2 * mu * CTu * Cm_alpha + CTu * Cmq * Cl_alpha - 2 * CWe * Cmq * (CLe - Cd_alpha) + 2 * CDe * Cmq * CTu;
	E1 = -2 * pow(CWe, 2) * Cm_alpha;

	delta = B1 * C1 * D1 - A1 * pow(D1, 2) - pow(B1, 2) * E1;

	if ((Cm_alpha < 0) && (delta > 0)) {

		omega_ph_n = (CWe / (sqrt(2) * mu)) * 2 * (Vel / velivolo.Chord);
		zita_ph = -CTu / (2 * sqrt(2) * CWe);
		eta_ph = -zita_ph * omega_ph_n;
		omega_ph = omega_ph_n * sqrt(fabs(pow(zita_ph, 2) - 1));
		T_ph = 2 * PI / omega_ph;
		t_dim_ph = log(0.5) / eta_ph;
		omega_sp_n_cap = sqrt(-Cm_alpha / Iycap);
		omega_sp_n = omega_sp_n_cap * 2 * (Vel / velivolo.Chord);
		zita_sp = (Iycap * Cl_alpha - 2 * mu * (Cmq + Cm_alpha_dot)) / (2 * sqrt(-2 * mu * Iycap * (2 * mu * Cm_alpha + Cmq * Cl_alpha)));
		eta_sp = -zita_sp * omega_sp_n;
		omega_sp = omega_sp_n * sqrt(fabs(pow(zita_sp, 2) - 1));
		T_sp = 2 * PI / omega_sp;
		t_dim_sp = log(0.5) / eta_sp;

		printf("\n----------------------------------------------------------------");
		printf("\n                    Condizioni di stabilita'");
		printf("\n----------------------------------------------------------------");
		printf("\n");
        printf("\n      Il velivolo e' staticamente e dinamicamente stabile.");
		printf("\n");
		printf("\n             ------------ MODO FUGOIDE ------------");
        printf("\n");
        printf("\n                   Autovalori:		                ");
        printf("\n                         1) lambda1 = %.3f + i %.3f	", eta_ph, omega_ph);
        printf("\n                         2) lambda2 = %.3f - i %.3f	", eta_ph, omega_ph);
        printf("\n                Pulsazione naturale = %.3f [rad/s]	", omega_ph_n);
        printf("\n    Pulsazione del sistema smorzato = %.3f [rad/s]	", omega_ph);
        printf("\n Coefficiente di smorzamento (zita) = %.3f		    ", zita_ph);
        printf("\n                Total damping (eta) = %.3f		    ", eta_ph);
        printf("\n                            Periodo = %.3f [s]		", T_ph);
        printf("\n              Tempo di dimezzamento = %.3f [s]		", t_dim_ph);
        printf("\n");
		printf("\n            ---------- MODO CORTO PERIODO ----------");
        printf("\n");
        printf("\n                   Autovalori:		                ");
        printf("\n                         1) lambda1 = %.3f + i %.3f	", eta_sp, omega_sp);
        printf("\n                         2) lambda2 = %.3f - i %.3f	", eta_sp, omega_sp);
        printf("\n                Pulsazione naturale = %.3f [rad/s]	", omega_sp_n);
        printf("\n    Pulsazione del sistema smorzato = %.3f [rad/s]	", omega_sp);
        printf("\n Coefficiente di smorzamento (zita) = %.3f		    ", zita_sp);
        printf("\n                Total damping (eta) = %.3f		    ", eta_sp);
        printf("\n                            Periodo = %.3f [s]		", T_sp);
        printf("\n              Tempo di dimezzamento = %.3f [s]		", t_dim_sp);

		if (fSimDescr != NULL) {
			fprintf(fSimDescr, "\n----------------------------------------------------------------");
			fprintf(fSimDescr, "\n                    Condizioni di stabilita'");
			fprintf(fSimDescr, "\n----------------------------------------------------------------");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n      Il velivolo e' staticamente e dinamicamente stabile.");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n             ------------ MODO FUGOIDE ------------");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n                   Autovalori:		                ");
			fprintf(fSimDescr, "\n                         1) lambda1 = %.3f + i %.3f	", eta_ph, omega_ph);
			fprintf(fSimDescr, "\n                         2) lambda2 = %.3f - i %.3f	", eta_ph, omega_ph);
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n                Pulsazione naturale = %.3f [rad/s]	", omega_ph_n);
			fprintf(fSimDescr, "\n    Pulsazione del sistema smorzato = %.3f [rad/s]	", omega_ph);
			fprintf(fSimDescr, "\n Coefficiente di smorzamento (zita) = %.3f		    ", zita_ph);
			fprintf(fSimDescr, "\n                Total damping (eta) = %.3f		    ", eta_ph);
			fprintf(fSimDescr, "\n                            Periodo = %.3f [s]		", T_ph);
			fprintf(fSimDescr, "\n              Tempo di dimezzamento = %.3f [s]		", t_dim_ph);
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n            ---------- MODO CORTO PERIODO ----------");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n                   Autovalori:		                ");
			fprintf(fSimDescr, "\n                         1) lambda1 = %.3f + i %.3f	", eta_sp, omega_sp);
			fprintf(fSimDescr, "\n                         2) lambda2 = %.3f - i %.3f	", eta_sp, omega_sp);
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n                Pulsazione naturale = %.3f [rad/s]	", omega_sp_n);
			fprintf(fSimDescr, "\n    Pulsazione del sistema smorzato = %.3f [rad/s]	", omega_sp);
			fprintf(fSimDescr, "\n Coefficiente di smorzamento (zita) = %.3f		    ", zita_sp);
			fprintf(fSimDescr, "\n                Total damping (eta) = %.3f		    ", eta_sp);
			fprintf(fSimDescr, "\n                            Periodo = %.3f [s]		", T_sp);
			fprintf(fSimDescr, "\n              Tempo di dimezzamento = %.3f [s]		", t_dim_sp);
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n");
		}

	}
	else if (Cm_alpha >= 0) {
		ErrWarn_DisplayErrMsgAndExit(E017, "Velivolo staticamente e dinamicamente instabile.\n"
				                           "Modificare i dati in input.");
		if (fSimDescr != NULL) {
			fprintf(fSimDescr, "\n----------------------------------------------------------------");
			fprintf(fSimDescr, "\n                    Condizioni di stabilita'");
			fprintf(fSimDescr, "\n----------------------------------------------------------------");
			fprintf(fSimDescr, "\n");
			fprintf(fSimDescr, "\n        Velivolo staticamente e dinamicamente instabile.");
			fprintf(fSimDescr, "\n                  Modificare i dati in input.");
			fprintf(fSimDescr, "\n");
		}

	}
	else {
		ErrWarn_DisplayErrMsgAndExit(E018, "Velivolo dinamicamente instabile.\n"
				                            "Modificare i dati in input.");
		if (fSimDescr != NULL) {
			fprintf(fSimDescr, "\n----------------------------------------------------------------");
			fprintf(fSimDescr, "\n                    Condizioni di stabilita'");
			fprintf(fSimDescr, "\n----------------------------------------------------------------");
			fprintf(fSimDescr, "\n");
	        fprintf(fSimDescr, "\n               Velivolo dinamicamente instabile.");
			fprintf(fSimDescr, "\n                  Modificare i dati in input.");
			fprintf(fSimDescr, "\n");
		}
	}
	return;
}
