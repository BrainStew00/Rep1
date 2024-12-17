#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include "dati.h"
#include "interpolazione.h"
#include "Manovre.h"
#include "time.h"
#include "Debug.h"
#include "Chart.h"
#include "user_input.h"
#include "propel_original.h"
#include "integrazione.h"
#include "engine.h"
#include "Check_Limits.h"
#include "Atmosphere.h"
#include "ErrWarn.h"

#define g 9.80665
#define POW_ERR 0.2


	/* Indici del vettore resudui */
#define U_IX     0
#define V_IX     1
#define W_IX     2
#define P_IX     3
#define Q_IX     4
#define R_IX     5
#define PHI_IX   6
#define THETA_IX 7
#define PSI_IX   8
#define H_IX     9
#define X_IX     10
#define Y_IX     11
#define Z_IX     12

TEP PowerLimiter(PROP *ppropeller, double rpm, float Vel, double rho_quotah, TEP tep, double PowEngineMax);

double Debug_DeltaTBreak = 0.1;
double Debug_TBreak = 100;
bool   Debug_bCalcCoeffModoOriginale = false;

/**
 * Routine di integrazione con il metodo di Eulero esplicito.
 * I calcoli dell'integrazione usano gli angoli in radianti, mentre gli input e gli output sono in gradi.
 * la variabile struct stati_rad e' quella che evolve con Eulero, da questa si ricava la stati_deg che e' invece in gradi
 */
void integrazione(double dt, STATE stati_deg, PLANE velivolo, CMD comandi_trim, TEP tep, ENG eng, PROP *ppropeller, bool LimitaPotenza) {
	double deltaFuel = NAN;
	double residui[13] = { NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN, NAN };
	double cx1 = NAN, cy1 = NAN, cz1 = NAN, cl1 = NAN, cm1 = NAN, cn1 = NAN;
	double u1 = NAN, v1 = NAN, w1 = NAN,
		   p1 = NAN, q1 = NAN, r1 = NAN,
		   fi1 = NAN, theta1 = NAN, psi1 = NAN,
		   h1 = NAN;
	double cx_ss = NAN, cx_alpha = NAN, cx_deltae = NAN,
		   cy_ss = NAN, cy_alpha = NAN, cy_deltaa = NAN, cy_deltar = NAN,
		   cz_ss = NAN, cz_alpha = NAN, cz_deltae = NAN,
		   cl_ss = NAN, cl_alpha = NAN, cl_deltaa = NAN, cl_deltar = NAN,
		   cm_ss = NAN, cm_alpha = NAN, cm_deltae = NAN,
		   cn_ss = NAN, cn_alpha = NAN, cn_deltaa = NAN, cn_deltar = NAN;

	double r_dot = NAN, p_dot = NAN;
	double rho_quotah = NAN;
	double cx_beta = NAN, cy_beta = NAN, cz_beta = NAN,
		   cl_beta = NAN, cm_beta = NAN, cn_beta = NAN;
	double T = NAN, V = NAN;
	double PowEngineMax = NAN, rpm_ciclo = NAN;
	double cxp = NAN, cxq = NAN, cxr = NAN,
		   cyp = NAN, cyq = NAN, cyr = NAN,
		   czp = NAN, czq = NAN, czr = NAN,
		   clp = NAN, clq = NAN, clr = NAN,
		   cmp = NAN, cmq = NAN, cmr = NAN,
		   cnp = NAN, cnq = NAN, cnr = NAN;
	double massa_vel = NAN, t = NAN, alfa_deg = NAN, beta_deg = NAN;
	double m_fuel = NAN;
	double p_cap = NAN, q_cap = NAN, r_cap = NAN;
	AC der;
	double X, Y, Z, L, M, N;
	CMD comandi_manovre, comandi_tot;
	double cmd_tot_DeltaAil_rad = NAN;   // Comando complessivo Aileron in radianti
	double cmd_tot_DeltaEq_rad = NAN;    // Comando complessivo Equilibrator in radianti
	double cmd_tot_DeltaRud_rad = NAN;   // Comando complessivo Rudder in radianti
	double alfa_rad = NAN;               // Alfa in radianti
	double beta_rad = NAN;               // Beta in radianti
	double gamma_rad = NAN;              // Gamma in radianti
	STATE stati_rad;                     // Stato con angoli espressi in radianti

	AC_Initialize(&der);
	CMD_Initialize(&comandi_manovre);
	CMD_Initialize(&comandi_tot);
	double time_max = Manovre_getDurata(); // la durata dovrebbe dipendere dalle manovre, basta ogliere il commento la funzione e' già implementata
	printf("\n time_max %f \n", time_max);

	T = tep.Thrust;
	residui[R_IX] = 0; // primo valore utilizzto per r_dot e p_dot
	residui[P_IX] = 0;
	stati_deg.x = 0;
	stati_deg.y = 0;

		// Crea una copia dello stato convertendo gli angoli da gradi a radianti
	stati_rad.V      = stati_deg.V;                 // [m/s]     --> [m/s]
	stati_rad.u      = stati_deg.u;                 // [m/s]     --> [m/s]
	stati_rad.v      = stati_deg.v;                 // [m/s]     --> [m/s]
	stati_rad.w      = stati_deg.w;                 // [m/s]     --> [m/s]
	stati_rad.p      = DEG_TO_RAD(stati_deg.p);     // [gradi/s] --> [rad/s]
	stati_rad.q      = DEG_TO_RAD(stati_deg.q);     // [gradi/s] --> [rad/s]
	stati_rad.r      = DEG_TO_RAD(stati_deg.r);     // [gradi/s] --> [rad/s]
	stati_rad.Phi    = DEG_TO_RAD(stati_deg.Phi);   // [gradi]   --> [rad]
	stati_rad.Theta  = DEG_TO_RAD(stati_deg.Theta); // [gradi]   --> [rad]
	stati_rad.Psi    = DEG_TO_RAD(stati_deg.Psi);   // [gradi]   --> [rad]
	stati_rad.Gamma  = DEG_TO_RAD(stati_deg.Gamma); // [gradi]   --> [rad]
	stati_rad.h      = stati_deg.h    ;             // [m]       --> [m]
	stati_rad.T      = stati_deg.T    ;             // [N]       --> [N]
	stati_rad.x      = stati_deg.x    ;             // [m]       --> [m]
	stati_rad.y      = stati_deg.y    ;             // [m]       --> [m]
	stati_rad.z      = stati_deg.z    ;             // [m]       --> [m]


 	m_fuel = velivolo.FuelFract * velivolo.Mass;
	velivolo.FuelMass = m_fuel; // massa iniziale di carburante

	debug_integrazione_abilita_visualizzazione(); // Chiede all'utente se stampare le informazioni di debug relative a integrazione.c

	t = 0;

	while (t <= time_max) {

		debug_integrazione_aggiorna_contatore_cicli();

		if (t >= Debug_TBreak)
		{
			Debug_TBreak += Debug_DeltaTBreak;
		}

        //************************************************************************
        // Calcolo dei comandi
        //************************************************************************
		{
			// Lettura comandi relativi al trim
			comandi_manovre = Manovre_getCommand(t);
			// Il comando complessivo e' dato dalla somma del trim e del comando di manovra
			comandi_tot.DeltaAil = comandi_trim.DeltaAil + comandi_manovre.DeltaAil;
			comandi_tot.DeltaEq = comandi_trim.DeltaEq + comandi_manovre.DeltaEq;
			comandi_tot.DeltaRud = comandi_trim.DeltaRud + comandi_manovre.DeltaRud;
			comandi_tot.EngPow = comandi_trim.EngPow + comandi_manovre.EngPow;

			Check_Limits_Keep_Cmd_In_Range(&comandi_tot, &velivolo); // Mantiene i comandi entro il range consentito

			// Conversione comandi da gradi in radianti;
			cmd_tot_DeltaAil_rad = DEG_TO_RAD(comandi_tot.DeltaAil);
			cmd_tot_DeltaEq_rad  = DEG_TO_RAD(comandi_tot.DeltaEq );
			cmd_tot_DeltaRud_rad = DEG_TO_RAD(comandi_tot.DeltaRud);

			// Potenza max disponibile dal motore alla quota attuales
			PowEngineMax = engineCalcPmax(&eng, stati_rad.h);


			// Conversione manetta --> rpm
			rpm_ciclo = eng.RPMmin + comandi_tot.EngPow * (eng.RPMmax - eng.RPMmin) / 100;
		}

        //************************************************************************
		// Fotografia dello stato prima di inizare i calcoli
        //************************************************************************
		// massa aggiornata
		massa_vel = velivolo.Mass;

		// stato
		u1 = stati_rad.u;
		v1 = stati_rad.v;
		w1 = stati_rad.w;
		p1 = stati_rad.p;
		q1 = stati_rad.q;
		r1 = stati_rad.r;
		fi1    = stati_rad.Phi;   // sono gia' radianti
		theta1 = stati_rad.Theta; // sono gia' radianti
		psi1   = stati_rad.Psi;   // sono gia' radianti
		h1 = stati_rad.h;
		/*x1 = statradeg.x; */
		/*y1 = stati_deg.y; */
		r_dot = residui[R_IX]; // Derivata di r al punto di integrazione precedente
		p_dot = residui[P_IX]; // Derivata di p al punto di integrazione precedente

        //************************************************************************
		//Calcolo velocita', beta_deg, alfa_deg, densita'
        //************************************************************************
		V = sqrt(u1 * u1 + v1 * v1 + w1 * w1); // [m/s]
		stati_rad.V = V;
		//printf("\nV integrazione: %f",V);
		alfa_rad = atan2(w1, u1);    // radianti
		alfa_deg = RAD_TO_DEG(alfa_rad); // gradi
		//alfa_deg=alpha_trim;
		beta_rad = asin(v1 / V);     // radianti
		beta_deg = RAD_TO_DEG(beta_rad); //gradi

		gamma_rad = stati_rad.Theta - alfa_rad;
		rho_quotah = Atmosphere_Rho(h1);

		if (debug_integrazione_stampa_in_questo_ciclo() == true) {
			double t_quota;

			t_quota = Atmosphere_temperature(h1);
			printf("integrazione: %f ", V);
			printf("Valore di T_quota :%f ", t_quota);
			printf("Valore di rhoh: %f ", rho_quotah);
			printf("\n");
		}

        //************************************************************************
		//Calcolo coefficienti aerodinamici
        //************************************************************************
		//interpol per il calcolo dei valori intermedi:
		der = interpolazione(velivolo.Der, alfa_deg);

		cx_ss = der.CX;
		cy_ss = der.CY;
		cz_ss = der.CZ;
		cm_ss = der.Cm;
		cl_ss = der.Cl;
		cn_ss = der.Cn;

		cx_alpha  = der.CXA;

		cx_deltae = der.CXde;

		cy_alpha  = der.CYA;
		cy_deltaa = der.CYda;
		cy_deltar = der.CYdr;

		cz_alpha  = der.CZA;
		cz_deltae = der.CZde;
		cl_alpha  = der.ClA;
		cl_deltaa = der.Clda;
		cl_deltar = der.Cldr;
		cm_alpha  = der.CmA;
		cm_deltae = der.Cmde;
		cn_alpha  = der.CnA;
		cn_deltaa = der.Cnda;
		cn_deltar = der.Cndr;

		cx_beta = der.CXB;
		cy_beta = der.CYB;
		cz_beta = der.CZB;
		cl_beta = der.ClB;
		cm_beta = der.CmB;
		cn_beta = der.CnB;

		cxp = der.CXP;
		cxq = der.CXQ;
		cxr = der.CXR;
		cyp = der.CYP;
		cyq = der.CYQ;
		cyr = der.CYR;
		czp = der.CZP;
		czq = der.CZQ;
		czr = der.CZR;
		clp = der.ClP;
		clq = der.ClQ;
		clr = der.ClR;
		cmp = der.CmP;
		cmq = der.CmQ;
		cmr = der.CmR;
		cnp = der.CnP;
		cnq = der.CnQ;
		cnr = der.CnR;

		if (Debug_bCalcCoeffModoOriginale)
		{
			p_cap = p1 * velivolo.WSpan / (2 * V);
			q_cap = q1 * velivolo.Chord / (2 * V);
			r_cap = r1 * velivolo.WSpan / (2 * V);

			//calcolo coefficienti
			cx1 = cx_ss + cx_alpha * alfa_rad +                             cx_deltae * cmd_tot_DeltaEq_rad;
			cy1 =         cy_beta  * beta_rad + cyp * p_cap + cyr * r_cap + cy_deltar * cmd_tot_DeltaRud_rad;
			cz1 = cz_ss + cz_alpha * alfa_rad + czq * q_cap +               cz_deltae * cmd_tot_DeltaEq_rad;
			cl1 =         cl_beta  * beta_rad + clp * p_cap + clr * r_cap + cl_deltaa * cmd_tot_DeltaAil_rad + cl_deltar * cmd_tot_DeltaRud_rad;
			cm1 = cm_ss + cm_alpha * alfa_rad + cmq * q_cap +               cm_deltae * cmd_tot_DeltaEq_rad;
			cn1 =         cn_beta  * beta_rad + cnp * p_cap + cnr * r_cap + cn_deltaa * cmd_tot_DeltaAil_rad + cn_deltar * cmd_tot_DeltaRud_rad;
		}
		else
		{
			double p_capWS = p1 * (velivolo.WSpan / (2 * V));
			double q_capWS = q1 * (velivolo.WSpan / (2 * V));
			double r_capWS = r1 * (velivolo.WSpan / (2 * V));
			double p_capC  = p1 * (velivolo.Chord / (2 * V));
			double q_capC  = q1 * (velivolo.Chord / (2 * V));
			double r_capC  = r1 * (velivolo.Chord / (2 * V));

			cx1 = cx_ss + cx_alpha  * alfa_rad + cx_beta * beta_rad + cxp * p_capC  + cxq * q_capC  + cxr * r_capC  + cx_deltae * cmd_tot_DeltaEq_rad;
			cy1 = cy_ss + cy_alpha  * alfa_rad + cy_beta * beta_rad + cyp * p_capWS + cyq * q_capWS + cyr * r_capWS + cy_deltaa * cmd_tot_DeltaAil_rad + cy_deltar * cmd_tot_DeltaRud_rad;
			cz1 = cz_ss + cz_alpha  * alfa_rad + cz_beta * beta_rad + czp * p_capC  + czq * q_capC  + czr * r_capC  + cz_deltae * cmd_tot_DeltaEq_rad;
			cl1 = cl_ss + cl_alpha  * alfa_rad + cl_beta * beta_rad + clp * p_capWS + clq * q_capWS + clr * r_capWS + cl_deltaa * cmd_tot_DeltaAil_rad + cl_deltar * cmd_tot_DeltaRud_rad;
			cm1 = cm_ss + cm_alpha  * alfa_rad + cm_beta * beta_rad + cmp * p_capC  + cmq * q_capC  + cmr * r_capC  + cm_deltae * cmd_tot_DeltaEq_rad;
			cn1 = cn_ss + cn_alpha  * alfa_rad + cn_beta * beta_rad + cnp * p_capWS + cnq * q_capWS + cnr * r_capWS + cn_deltaa * cmd_tot_DeltaAil_rad + cn_deltar * cmd_tot_DeltaRud_rad;
		}

        //************************************************************************
		// Calcolo forze e momenti
        //************************************************************************
		X = 0.5 * rho_quotah * pow(V, 2) * velivolo.WSurf * cx1;
		Y = 0.5 * rho_quotah * pow(V, 2) * velivolo.WSurf * cy1;
		Z = 0.5 * rho_quotah * pow(V, 2) * velivolo.WSurf * cz1;
		L = 0.5 * rho_quotah * pow(V, 2) * velivolo.WSurf * velivolo.WSpan * cl1;
		M = 0.5 * rho_quotah * pow(V, 2) * velivolo.WSurf * velivolo.Chord * cm1;
		N = 0.5 * rho_quotah * pow(V, 2) * velivolo.WSurf * velivolo.WSpan * cn1;

        //************************************************************************
		// Calcolo thrust
        //************************************************************************
		tep = propel_original(ppropeller, rpm_ciclo, stati_rad.V, rho_quotah);
		// Controllo potenza motore
		{
			static int conta_x_stampa = 0; // Ultimo istante di stampa del messaggio di errore
			static int cnt_prossima_stampa = 0;
			char msg[MAX_LUNGHEZZA_STRINGA];

			if (tep.P_al > PowEngineMax) {	// Se la potenza assorbita dall'elica e' superiore a quella disponibile dal motore
				conta_x_stampa++;
				// La stampa rallenta molto, si stampa il messaggo solo ogni 100 cicli cioè 1000 ms di simulazione
				if (conta_x_stampa >= cnt_prossima_stampa)
				{
					cnt_prossima_stampa += 100; // Prossima stampa tra 1000 ms
					sprintf(msg, "[t=%6.2f] La potenza richiesta dall'elica (%.0fkW) supera quella disponibile dal motore (%.0fkW).\n",t,  tep.P_al, PowEngineMax);
					ErrWarn_DisplayWarnMsgAndContinue(W022, msg);
				}
			}
			else
			{
				cnt_prossima_stampa = 0;
			}
		}
		if (LimitaPotenza) // Se è attiva la limitazione della potenza
		{
			tep = PowerLimiter(ppropeller, rpm_ciclo, stati_rad.V, rho_quotah, tep, PowEngineMax); // Limitatore di potenza
			rpm_ciclo = tep.RPM; // Aggiorna rpm con il valore eventualmente limitato da PowerLimiter
		}
		T = tep.Thrust;
		//NON_USATO manetta = (tep.RPM - eng.RPMmin) * 100 / (eng.RPMmax - eng.RPMmin);

        //************************************************************************
		// Aggiornamento masse per consumo di carburante se la simulazione e' stata richiesta a massa variabile
        //************************************************************************
		if (velivolo.VarFuel)	// Se simulazione a massa variabile
		{
			deltaFuel = engineCalcFuelConsumption(tep.P_al, eng.SFC, dt); // Consumo di carburante nel tempo dt
            if (debug_integrazione_stampa_in_questo_ciclo() == true) {
                printf("deltaFuel= %f\n", deltaFuel);
            }
			velivolo.Mass -= deltaFuel; // Aggiornamento massa valivolo dovuto al consumo di carburante
			velivolo.FuelMass -= deltaFuel; // Aggiornamento massa carburante
		}

        //************************************************************************
		// Calcolo residui
        //************************************************************************
		residui[U_IX    ] = (r1 * v1 - q1 * w1) - g * sin(theta1) +            X / massa_vel + T / massa_vel;
		residui[V_IX    ] = (p1 * w1 - r1 * u1) + g * sin(fi1) * cos(theta1) + Y / massa_vel;
		residui[W_IX    ] = (q1 * u1 - p1 * v1) + g * cos(fi1) * cos(theta1) + Z / massa_vel;
		residui[P_IX    ] = -(velivolo.JZ - velivolo.JY) * q1 * r1 / velivolo.JX + (p1 * q1 + r_dot)         * velivolo.JXZ / velivolo.JX + L / velivolo.JX;
		residui[Q_IX    ] = -(velivolo.JX - velivolo.JZ) * p1 * r1 / velivolo.JY - (pow(p1, 2) - pow(r1, 2)) * velivolo.JXZ / velivolo.JY + M / velivolo.JY;
		residui[R_IX    ] = -(velivolo.JY - velivolo.JX) * p1 * q1 / velivolo.JZ - (q1 * r1 - p_dot)         * velivolo.JXZ / velivolo.JZ + N / velivolo.JZ;
		residui[PHI_IX  ] = (p1 + q1 * sin(fi1) * tan(theta1) + r1 * cos(fi1) * tan(theta1));
		residui[THETA_IX] = (q1 * cos(fi1) - r1 * sin(fi1));
		residui[PSI_IX  ] = (q1 * sin(fi1) / cos(theta1) + r1 * cos(fi1) / cos(theta1));
		residui[X_IX    ] = u1 * cos(psi1) * cos(theta1)
                          + v1 * (cos(psi1) * sin(theta1) * sin(fi1) - sin(psi1) * cos(fi1))
                          + w1 * (cos(psi1) * sin(theta1) * cos(fi1) + sin(psi1) * sin(fi1));
        residui[Y_IX    ] = u1 * sin(psi1) * cos(theta1)
                          + v1 * (sin(psi1) * sin(theta1) * sin(fi1) + cos(psi1) * cos(fi1))
                          + w1 * (sin(psi1) * sin(theta1) * cos(fi1) - cos(psi1) * sin(fi1));
		residui[Z_IX    ] = -u1 * sin(theta1) + v1 * cos(theta1) * sin(fi1) + w1 * cos(theta1) * cos(fi1);
		residui[H_IX    ] = -residui[Z_IX    ];

		if (debug_integrazione_stampa_in_questo_ciclo() == true) {
			debug_integrazione_stampa_residui(t, residui); // Stampa dei residui come da impostazioni utente
		}

        //************************************************************************
		// Aggiornamento stato (Eulero)
        //************************************************************************
		stati_rad.u     =  stati_rad.u     + dt * residui[U_IX    ];
		stati_rad.v     =  stati_rad.v     + dt * residui[V_IX    ];
		stati_rad.w     =  stati_rad.w     + dt * residui[W_IX    ];
		stati_rad.p     =  stati_rad.p     + dt * residui[P_IX    ];
		stati_rad.q     =  stati_rad.q     + dt * residui[Q_IX    ];
		stati_rad.r     =  stati_rad.r     + dt * residui[R_IX    ];
		stati_rad.Phi   =  stati_rad.Phi   + dt * residui[PHI_IX  ];
		stati_rad.Theta =  stati_rad.Theta + dt * residui[THETA_IX];
		stati_rad.Psi   =  stati_rad.Psi   + dt * residui[PSI_IX  ];
		stati_rad.x     =  stati_rad.x     + dt * residui[X_IX    ];
		stati_rad.y     =  stati_rad.y     + dt * residui[Y_IX    ];
		stati_rad.z     =  stati_rad.z     + dt * residui[Z_IX    ];
		stati_rad.h     = -stati_rad.z;
		stati_rad.T     = T;
		stati_rad.Gamma = gamma_rad;



        //************************************************************************
		// Crea una copia dello stato convertendo gli angoli da radianti a gradi
        //************************************************************************
	    stati_deg.V      = stati_rad.V;
	    stati_deg.u      = stati_rad.u;
	    stati_deg.v      = stati_rad.v;
	    stati_deg.w      = stati_rad.w;
	    stati_deg.p      = RAD_TO_DEG(stati_rad.p);
	    stati_deg.q      = RAD_TO_DEG(stati_rad.q);
	    stati_deg.r      = RAD_TO_DEG(stati_rad.r);
	    stati_deg.Phi    = RAD_TO_DEG(stati_rad.Phi);
	    stati_deg.Theta  = RAD_TO_DEG(stati_rad.Theta);
	    stati_deg.Psi    = RAD_TO_DEG(stati_rad.Psi);
	    stati_deg.Gamma  = RAD_TO_DEG(stati_rad.Gamma);
	    stati_deg.h      = stati_rad.h;
	    stati_deg.T      = stati_rad.T;
	    stati_deg.x      = stati_rad.x;
	    stati_deg.y      = stati_rad.y;
	    stati_deg.z      = stati_rad.z;

        //************************************************************************
		// Salvataggio dati
        //************************************************************************
        chart_save_data(t,
        		        &comandi_tot,
						alfa_deg,
						beta_deg,
						&stati_deg,
						&velivolo,
        		        tep.P_al,      // Potenza all'elica
				        PowEngineMax,  // Max potenza disponibile dal motore
						rpm_ciclo);

        //************************************************************************
		// Verifica limiti (in gradi)
        //************************************************************************
        Check_Limits_set_error_and_warning(t,            // Istante attuale
        		                          &stati_deg,   // Stato del sistema (angoli in gradi)
										  tep.P_al,     // Potenza all'elica
										  PowEngineMax, // Max potenza disponibile dal motore
										  &velivolo);

        if (!debug_integrazione_visualizzazione_abilitata())	// Se non vengon stampate informazioni di debug
        {                                                       // Stampa la percntuale di comapletamento
        	int percentualeCompletamento;
        	static int percentualeCompletamentoPrevious = -1;

        	percentualeCompletamento = t * 100.0 / time_max;

        	if (percentualeCompletamentoPrevious != percentualeCompletamento)
            {
        		percentualeCompletamentoPrevious = percentualeCompletamento;
        		printf("\rCompletamento: %3d%%", percentualeCompletamento);
            }
        }

        //************************************************************************
		// Aggiornamento tempo di simulazione
        //************************************************************************
		t = t + dt;

	}
    if (!debug_integrazione_visualizzazione_abilitata())	// Se non vengon stampate informazioni di debug
    {
		printf("\r Completamento: %5.1f%%\n\n", 100.0);
    }
	printf("Residuo u:        %f\n", residui[U_IX    ]);
	printf("Residuo v:        %f\n", residui[V_IX    ]);
	printf("Residuo w:        %f\n", residui[W_IX    ]);
	printf("Residuo p:        %f\n", residui[P_IX    ]);
	printf("Residuo q:        %f\n", residui[Q_IX    ]);
	printf("Residuo r:        %f\n", residui[R_IX    ]);
	printf("Residuo fi:       %f\n", residui[PHI_IX  ]);
	printf("Residuo theta:    %f\n", residui[THETA_IX]);
	printf("Residuo psi:      %f\n", residui[PSI_IX  ]);
	printf("Residuo h:        %f\n", residui[H_IX    ]);

	/* printf("\nu:        %f",stati_deg.u);
	 printf("\nv:        %f",stati_deg.v);
	 printf("\nw:        %f",stati_deg.w);
	 printf("\np:        %f",stati_deg.p);
	 printf("\nq:        %f",stati_deg.q);
	 printf("\nr:        %f",stati_deg.r);
	 printf("\nfi:       %f",stati_deg.Phi);
	 printf("\ntheta:    %f",stati_deg.Theta);
	 printf("\npsi:      %f",stati_deg.Psi);
	 printf("\nh:        %f",stati_deg.h);*/

	return;
}


/**
 * Verifica che la potenza richiesta dall'elica tep.P_al non superi la
 * potenza disponibile dal motore PowEngineMax. Nel caso limita la potenza
 * assorbita a quella disponibile riducendo il numero di giri del motore.
 */
TEP PowerLimiter(PROP *ppropeller, double rpm, float Vel, double rho_quotah, TEP tep, double PowEngineMax)
{
	static TEP tepSave;
	TEP tep_test, tep_left, tep_right, tep_result;
	double rpm_left, rpm_right, rpm_test;
	int count;
	double search_rpm_step = 50.0;


	tep_result = tep; // assumi di restituire il dato ricevuto
	if (tep.P_al <= PowEngineMax)  // Se la potenza attuale è disponibile dal motore
	{
		tepSave = tep; // Salva una copia
	}
	else // Se la potenza attualmente assorbita dall'elica supera quella del motore
	{    // Cerca il max valore di rpm con potenza entro il limite. Usa la bisezione.


		rpm_left = tepSave.RPM;	// Questo valore era entro il limite

		// Assicurati che rpm_left corrisponda ad una potenza entro i limiti
		count = 0;
		do
		{
			tep_left = propel_original(ppropeller, rpm_left, Vel, rho_quotah); // Ricalcola cpn i valori attuali
			if (tep_left.P_al > PowEngineMax) // Se potenza oltre i limiti
			{
				rpm_left -= search_rpm_step; // Prova con un numero di rpm inferiore
			}
			if (++count > 100) // Evita eventuali blocchi
			{
				break;
			}
		} while (tep_left.P_al > PowEngineMax); // Continua a cercare se potenza ancora oltre il limite

		// Cerca un valore di rpm vicino che sia fuori dei limiti
		count = 0;
		rpm_right = rpm_left + search_rpm_step;
		do
		{
			tep_right = propel_original(ppropeller, rpm_right, Vel, rho_quotah); // Ricalcola cpn i valori attuali
			if (tep_right.P_al < PowEngineMax) // Se potenza oltre i limiti
			{
				rpm_left = rpm_right;
				tep_left = tep_right;
				rpm_right += search_rpm_step; // Prova con un numero di rpm superiore
			}
			if (++count > 100) // Evita eventuali blocchi
			{
				break;
			}
		} while (tep_right.P_al < PowEngineMax); // Continua a cercare se potenza ancora sotto il limite

		// Ricerca con bisezione
		count = 0;
		do
		{
			rpm_test = (rpm_left + rpm_right) / 2;
			tep_test = propel_original(ppropeller, rpm_test, Vel, rho_quotah); // Ricalcola cpn i valori attuali
			if (fabs(tep_test.P_al - PowEngineMax) < POW_ERR) // Se potenza assorbita uguale alla max disponibile entro l'errore accettabile
			{
				break; // Interrompi ricerca
			}
			else if (tep_test.P_al < PowEngineMax)
			{
				rpm_left = rpm_test;
			}
			else
			{
				rpm_right = rpm_test;
			}
		} while (count++ < 100); // Max 100 tentativi
		tep_result = tep_test;
		if (tep_result.P_al <= PowEngineMax) // Se potenza entro i limiti
		{
			tepSave = tep_result; // Salva una copia
		}

	}

	return tep_result;
}
