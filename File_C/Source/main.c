#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <limits.h>

#include "dati.h"
#include "engine.h"
#include "dba.h"
#include "propeller.h"
#include "user_input.h"
#include "Atmosphere.h"
#include "interpolazione.h"
#include "Trim.h"
#include "propel_original.h"
#include "Calcolo_RPM.h"
#include "Routh.h"
#include "integrazione.h"
#include "Manovre.h"
#include "Debug.h"
#include "Chart.h"

int main() {
	// Le variabili vengono impostate a valori invalidi per poter rilevare le mancate inizializzazioni in fase di debug
	double rho_h;
	USERVAL ScelteUtente;
	AC     Dernew; // Derivate aerodinamiche attuali
	CMD    comandi_trim;
	TEP    Tep;
	STATE  stato_deg; // Stato con angoli espressi in gradis
	FILE   *fSimDescript;

	// Inizializzazione variabili complesse a dati non validi
	AC_Initialize(&Dernew);
	CMD_Initialize(&comandi_trim);
	TEP_Initialize(&Tep);
	USERVAL_Initialize(&ScelteUtente);
	STATE_Initialize(&stato_deg);

    //************************************************************************
    // Debug (da rimuovere)
    //************************************************************************
	Debug_On = true; // Attiva stampa di debug
	Debug_Blocca_per_visualizzare = false; // true: Attiva la sospensione del programma in attesa di un tasto
	Debug_NextBreakCnt = 5;               // Prossimo valore di contatore per cui si blocca l'esecuzione
	Debug_Cnt_BreakIncrement = 5;  	      // Ogni quante stampe dsi ferma

    //************************************************************************
	// Inizializzazioni
    //************************************************************************
	printf("\t\t\t\t\tSIMULATORE CESSNA 172\n\n");

	Manovre_Inizializza(); // Inizializza il generatore dei comandi delle manovre

	// Leggi i file dei database parametri ed elimina i commenti
	dba_dati();         // Leggo input/dba.txt, scrivi dba_nuovo.txt
	propeller_dati();   //
	engine_dati();

	// Leggi i file dei database parametri privi di commentj e carica i dati
	PROP propeller = propeller_variabili(); //i vettori propeller hanno lunghezza 48
	ENG eng = engine_variabili();
	PLANE Cessna = dba_variabili();


    //************************************************************************
	// Scelte iniziali utente
    //************************************************************************
    user_input_ini_menu(&ScelteUtente); // Menu' iniziale di scelta dati
	Cessna.VarFuel = ScelteUtente.simMassaVariabile; // Scelta tipo di simulazione

	// Crea il file txt con la descrizione della simulazione
	chart_simulation_description_file_create(ScelteUtente.nomeFileSalvataggioDati);
	fSimDescript = chart_simulation_description_file_get_file_handler();

	if (fSimDescript != NULL)
	{
		// Scrivi le condizioni iniziali nel file
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n                 Simulatore Cessna 172 Skyhawk                  ");
		fprintf(fSimDescript, "\n                                                                ");
		fprintf(fSimDescript, "\n Gruppo di lavoro 2                                 Giugno 2024 ");
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n");
		fprintf(fSimDescript, "\n           Titolo simulazione: %s",ScelteUtente.titoloSim);
		fprintf(fSimDescript, "\n");
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n                      Condizioni iniziali");
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n");
		fprintf(fSimDescript, "\n                        Quota =   %4.0f [m]",  ScelteUtente.h);
		fprintf(fSimDescript, "\n                    Velocita' =  %5.2f [m/s]", ScelteUtente.V);
		fprintf(fSimDescript, "\n              Angolo di rampa =  %5.2f [°]", ScelteUtente.Gamma); // \370
		fprintf(fSimDescript, "\n           Durata simulazione = %6.0f [s]",    ScelteUtente.durataSimulazione);
		fprintf(fSimDescript, "\n        Passo di integrazione =  %.3f [s]",    ScelteUtente.deltaT);
		if (ScelteUtente.simMassaVariabile) {
			fprintf(fSimDescript, "\nSimulazione a massa variabile:  SI");
		}
		else {
			fprintf(fSimDescript, "\nSimulazione a massa variabile:  NO");
		}
		if (ScelteUtente.LimitazioneDiPotenzaAttiva) {
			fprintf(fSimDescript, "\n       Limitazione di potenza:  Attiva");
		}
		else {
			fprintf(fSimDescript, "\n       Limitazione di potenza:  NON Attiva");
		}
		fprintf(fSimDescript, "\n");
	}

    //************************************************************************
	// Scelta condizioni atmosferiche
    //************************************************************************
	AtmosphereChoiceAndCalc(ScelteUtente.h);
    rho_h = Atmosphere_Rho(ScelteUtente.h);

	printf("\n----------------------------------------------------------------");
	printf("\n   Dati atmosferici e di potenza per la quota di %.0f m", ScelteUtente.h);
	printf("\n----------------------------------------------------------------");
	printf("\n");
    printf("\n             Temperatura    T = %+3.1f [\370C]", KELVIN_TO_CENT(Atmosphere_temperature(ScelteUtente.h)));
    printf("\n               Pressione    P = %6.0f [Pa]",      Atmosphere_press(ScelteUtente.h));
    printf("\n                 Densita' rho = %4.3f [kg/m^3]", rho_h);
    printf("\n     Velocita' del Suono    a = %4.1f [m/s]",    Atmosphere_vsuono(ScelteUtente.h));
	printf("\n             Potenza max PMax = %5.0f [kW]",                    engineCalcPmax(&eng, ScelteUtente.h));
	printf("\n");
	user_input_StampaMsgAttendiTasto("");

	if (fSimDescript != NULL)
	{
		// Scrivi le condizioni iniziali nel file
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n      Dati atmosferici e di potenza per la quota di %.0f m", ScelteUtente.h);
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n");
		fprintf(fSimDescript, "\n             Temperatura    T = %+3.1f [°C]", KELVIN_TO_CENT(Atmosphere_temperature(ScelteUtente.h)));
		fprintf(fSimDescript, "\n               Pressione    P = %6.0f [Pa]",      Atmosphere_press(ScelteUtente.h));
		fprintf(fSimDescript, "\n                 Densita' rho = %4.3f [kg/m^3]", rho_h);
		fprintf(fSimDescript, "\n     Velocita' del Suono    a = %4.1f [m/s]",    Atmosphere_vsuono(ScelteUtente.h));
		fprintf(fSimDescript, "\n             Potenza max PMax = %5.0f [kW]",                    engineCalcPmax(&eng, ScelteUtente.h));
		fprintf(fSimDescript, "\n");
	}

    //************************************************************************
	// Creazione vettore di stato_deg iniziale
    //************************************************************************
	stato_deg.V     = ScelteUtente.V;
	stato_deg.h     = ScelteUtente.h;
	stato_deg.z     = -ScelteUtente.h;
	stato_deg.Gamma = ScelteUtente.Gamma;

    //************************************************************************
	//  Calcolo comandi di trim
    //************************************************************************
	comandi_trim = Trim(Cessna, &stato_deg, &Dernew, rho_h);

	printf("\n----------------------------------------------------------------");
	printf("\n                       Condizioni di trim");
	printf("\n----------------------------------------------------------------");
	printf("\n");
	printf("\n                            u = %.2f [m/s]", stato_deg.u);
	printf("\n                            v = %.2f [m/s]", stato_deg.v);
	printf("\n                            w = %.2f [m/s]", stato_deg.w);
	printf("\n                            p = %.2f [\370/s]", stato_deg.p);
	printf("\n                            q = %.2f [\370/s]", stato_deg.q);
	printf("\n                            r = %.2f [\370/s]", stato_deg.r);
	printf("\n                          phi = %.2f [\370]", stato_deg.Phi);
	printf("\n                        theta = %.2f [\370]", stato_deg.Theta);
	printf("\n                          psi = %.2f [\370]", stato_deg.Psi);
	printf("\n                        alpha = %.2f [\370]", Dernew.Alpha);
	printf("\n                      delta_e = %.2f [\370]", comandi_trim.DeltaEq);
	if (fSimDescript != NULL)
	{
		// Scrivi le condizioni di trim nel file
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n                       Condizioni di trim");
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n");
		fprintf(fSimDescript, "\n                            u = %.2f [m/s]", stato_deg.u);
		fprintf(fSimDescript, "\n                            v = %.2f [m/s]", stato_deg.v);
		fprintf(fSimDescript, "\n                            w = %.2f [m/s]", stato_deg.w);
		fprintf(fSimDescript, "\n                            p = %.2f [°/s]", stato_deg.p);
		fprintf(fSimDescript, "\n                            q = %.2f [°/s]", stato_deg.q);
		fprintf(fSimDescript, "\n                            r = %.2f [°/s]", stato_deg.r);
		fprintf(fSimDescript, "\n                          phi = %.2f [°]", stato_deg.Phi);
		fprintf(fSimDescript, "\n                        theta = %.2f [°]", stato_deg.Theta);
		fprintf(fSimDescript, "\n                          psi = %.2f [°]", stato_deg.Psi);
		fprintf(fSimDescript, "\n                        alpha = %.2f [°]", Dernew.Alpha);
		fprintf(fSimDescript, "\n                      delta_e = %.2f [°]", comandi_trim.DeltaEq);
	}

    //************************************************************************
	// Calcolo della spinta necessaria
    //************************************************************************
	Tep = Calcolo_RPM(eng, &Dernew, &comandi_trim, Cessna, &stato_deg, &propeller);
	{ // Converti rpm nel comando thrust di trim
		if (Tep.RPM < eng.RPMmin) {
			comandi_trim.EngPow = 0;
		}
		else if (Tep.RPM > eng.RPMmax) {
			comandi_trim.EngPow = 100;
		}
		else {
			comandi_trim.EngPow = (Tep.RPM - eng.RPMmin) / (eng.RPMmax - eng.RPMmin) * 100;
		}
	}
	printf("\n                          RPM = %f [rpm]", Tep.RPM);
	printf("\n                       Spinta = %.2f [N]", Tep.Thrust);
	printf("\n                      Manetta = %.2f [%]", comandi_trim.EngPow);
	printf("\n");

	if (fSimDescript != NULL)
	{
		fprintf(fSimDescript, "\n                          RPM = %f [rpm]", Tep.RPM);
		fprintf(fSimDescript, "\n                       Spinta = %.2f [N]", Tep.Thrust);
		fprintf(fSimDescript, "\n                      Manetta = %.2f [%]", comandi_trim.EngPow);
		fprintf(fSimDescript, "\n");
	}
	user_input_StampaMsgAttendiTasto("");


    //************************************************************************
	// Verifica stabilità con criterio di Routh
    //************************************************************************
	Routh(Cessna, Dernew, ScelteUtente.V, rho_h);
	user_input_StampaMsgAttendiTasto("");

    //************************************************************************
	// Immissione manovere
    //************************************************************************
	Manovre_lettura_da_tastiera(ScelteUtente.durataSimulazione);
	if (fSimDescript != NULL) {
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n                      Sequenza dei comandi");
		fprintf(fSimDescript, "\n----------------------------------------------------------------");
		fprintf(fSimDescript, "\n");
		fprintf(fSimDescript, "\n");

		Manovre_Print_All_Command(fSimDescript);
	}


    //************************************************************************
	// Attivazione salvataggio file
    //************************************************************************
	chart_enable_save_data_on_gnuplot_file(true); // Abilita il salvataggio dei dati di simulazione
	//chart_ask_filename();
	chart_start_collect_data_file(ScelteUtente.nomeFileSalvataggioDati);


    //************************************************************************
	// Integrazione equazioni del moto
    //************************************************************************
	integrazione(ScelteUtente.deltaT, stato_deg, Cessna, comandi_trim, Tep, eng, &propeller, ScelteUtente.LimitazioneDiPotenzaAttiva);

    //************************************************************************
	// Chiusura file
    //************************************************************************
	chart_stop_collect_data_file();
	chart_simulation_description_file_close();

    //************************************************************************
	// Apertura visualizzazione grafico
    //************************************************************************
	{
	    int scelta_sw_x_grafici;
		char command_string[200];
		char *simfilename;

		simfilename = chart_simulation_description_file_get_filename();

		if (strlen(simfilename) != 0) // Se esiste un file di simulazione
		{
			printf("\n----------------------------------------------------------------"
				   "\n"
				   "\n");
			scelta_sw_x_grafici = user_input_richiedi_int("\n"
													"\nVuoi visualizzare i grafici?"
													"\n [0] ---> No "
													"\n [1] ---> Visualizza con gnuplot"
													"\n          (piu' rapido, grafici meno rifiniti)"
													"\n [2] ---> Visualizza con Matlab"
													"\n          (piu' lento, grafici piu' rifiniti, consente il salvataggio dei grafici)"
													"\n",
													0,  // Val minimo
													2); // Val massimo
			switch (scelta_sw_x_grafici) {
				case 0:
					break;

				case 1:
						/* Crea una stringa del tipo:
						 * .\output\Visualizza_con_Gnuplot.bat <nomefile> */
					sprintf(command_string, ".\\output\\Visualizza_con_Gnuplot.bat %s", simfilename);
					printf("Richiamo gnuplot con la linea di comando: %s", command_string);
					system(command_string);	// Richiama gnuplot
					break;

				case 2:
						/* Crea una stringa del tipo:
						 * .\output\Visualizza_con_Matlab.bat <nomefile> */
					sprintf(command_string, ".\\output\\Visualizza_con_Matlab.bat %s", simfilename);
					printf("Richiamo gnuplot con la linea di comando: %s", command_string);
					system(command_string);	// Richiama matlab
					break;
			}
		}
	}
	user_input_StampaMsgAttendiTasto("");
	return 0;
}
