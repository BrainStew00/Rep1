/***
 * Funzioni per il salvataggio di file dati utilizzati per la reazione di grafici
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "Chart.h"
#include "dati.h"
#include "user_input.h"

FILE *fOutSimDataFile;
FILE *fSimDescrFile;
char saveDataFullFilename[MAX_LUNGHEZZA_STRINGA];
char saveDataFilename[MAX_LUNGHEZZA_STRINGA];

bool bEnableSaveDataFile = false;        // Se true abilita il salvataggio del file dati per fare i grafici con gnuplot


/**
 * Crea il file con il nome specificato e l'estensione txt nella cartella output
 */
void chart_simulation_description_file_create(char nomefile[]) {
	char simDescrFilename[MAX_LUNGHEZZA_STRINGA];


	strcpy(simDescrFilename, "output\\");
	strcat(simDescrFilename, nomefile);
	strcat(simDescrFilename, ".txt");

	fSimDescrFile = fopen(simDescrFilename, "w");
}




/**
 * restituisce il file handler del file di descrizione della simulazione
 */
FILE *chart_simulation_description_file_get_file_handler(void) {
	return fSimDescrFile;
}

/**
 * Chiude il file di descrizione della simulazione
 */
void chart_simulation_description_file_close(void) {

	if (fSimDescrFile != NULL) {
		fclose(fSimDescrFile);
	}
}

/**
 * restituisce il nome del file della simulazione compresa l'estensione .dat
 */
char *chart_simulation_description_file_get_filename(void) {
	return saveDataFilename;
}


/**
 * Chiede all'utente il nome del file in cui salvare i dati di simulazione
 **/
void chart_ask_filename(char nome[])
{
	bool finito;
	int scelta;
	char FullFilename[MAX_LUNGHEZZA_STRINGA];
	char nuovonome[MAX_LUNGHEZZA_STRINGA];

	finito = false;
	strcpy(FullFilename, "output\\");
	strcat(FullFilename, nome);
	strcat(FullFilename, ".dat");

	while (!finito)  // Creo loop per poter fare eventuali check di validita' del nome file
	{
		// Immetti nuovo nome del file
		strcpy(nuovonome, nome);
		user_input_richiedi_stringa("\nNome attuale del file dei risultati di simulazione",  nuovonome);
        // Crea nome completo
		strcpy(FullFilename, "output\\");
		strcat(FullFilename, nuovonome);
		strcat(FullFilename, ".dat");
		if (!chart_check_if_dat_file_exist(nuovonome)) // Se il file non esiste
		{
    		strcpy(nome, nuovonome);            // Aggiorna il nome file ricevuto con quello appena immesso
    		finito = true;                      // Il file non esiste, ok
		}
		else                                    // Se il file esiste
		{
    		scelta = user_input_richiedi_int("\nIl file esiste, vuoi sovrascriverlo?"
                                             "\n [0] ---> No "
                                             "\n [1] ---> Si "
                                             "\n",
					                           0,  // Val minimo
					                           1); // Val massimo
    		if (scelta == 1) // Se si vuole sovrascrivere
    		{
				strcpy(nome, nuovonome);            // Aggiorna il nome file ricevuto con quello appena immesso
    			finito = true; // Fine immissione
    		}
    		else             // Se non si vuole sovrascrivere
    		{
    			finito = false; // fai un altro giro
    		}
		}
	}

	printf("\n"
			"\nIl file verra' salvato in %s\n\n", FullFilename);
}


/**
 * Imposta automaticamente il nome del file di salvataggio dati
 * in base ai parametri iniziali
 * aggiungendo un numero coda in modo che il file non esista
 * Il nome contiene in sequenza:
 *  - Titolo
 *  - Quota
 *  - Velocita'
 *  - Angolo di rampa
 *  - tipo simulazione (massa fissa/variabile)
 *  - Limitazione di potenza
 *  - durata simulazione
 *  - numero incrementale
 *  Esempio: 1000m_52ms_0deg_mFix_300s_001
 *  Nota: '.dat' viene aggiunto automaticamente dalla funzione di salvataggio
 */
void chart_autoset_data_filename(USERVAL *pUserValues)
{
	char nuovonome[MAX_LUNGHEZZA_STRINGA];
	int count;
	char massavar[MAX_LUNGHEZZA_STRINGA];
	char limitpot[MAX_LUNGHEZZA_STRINGA];
	char titoloMod[MAX_LUNGHEZZA_STRINGA];
	int len;
	int i;
	bool file_esiste;

	if (pUserValues->simMassaVariabile) {
		strcpy(massavar, "mVar");
	}
	else {
		strcpy(massavar, "mFix");
	}

	if (pUserValues->LimitazioneDiPotenzaAttiva) {
		strcpy(limitpot, "PowLim");
	}
	else {
		strcpy(limitpot, "NoPowLim");
	}

	// Sostituisci gli spazi e i caratteri ' oppure " del titolo con '_' per evitare problemi con il nome del file
	strcpy(titoloMod, pUserValues->titoloSim); // Copia locale
	len = strlen(titoloMod);
	for (i = 0; i < len; i++) {
		if (isspace(titoloMod[i])) {// Se il carattere e' uno spazio
			titoloMod[i] = '_'; // Sostituiscilo con '_'
		}
		else if (titoloMod[i] == '\''){
			titoloMod[i] = '_'; // Sostituiscilo con '_'
		}
		else if (titoloMod[i] == '"'){
			titoloMod[i] = '_'; // Sostituiscilo con '_'
		}
		else if (titoloMod[i] == ','){
			titoloMod[i] = '_'; // Sostituiscilo con '_'
		}
		else {

		}

	}

	// Crea
	count = 0;
	do
	{
		// Esempio: Trim_1000m_52ms_0deg_mFix_PowLim_300s_001
		sprintf(nuovonome, "%s_%4.0fm_%2.0fms_%.1fdeg_%s_%s_%.0fs_%03d", // Come printf ma scrive in una stringa
				titoloMod,
				pUserValues->h,
				pUserValues->V,
				pUserValues->Gamma,
				massavar,
				limitpot,
				pUserValues->durataSimulazione,
				count);
		file_esiste = chart_check_if_dat_file_exist(nuovonome);
		if (file_esiste){
			count++;
		}

	} while (file_esiste);
	strcpy(pUserValues->nomeFileSalvataggioDati, nuovonome);
}


/**
 * Verifica se il file .dat specificato esiste nella cartella .\output\
 */
bool chart_check_if_dat_file_exist(char nomefile[])
{
	bool file_esiste;
	char FullFilename[MAX_LUNGHEZZA_STRINGA] = "";
	FILE *fDataFile;

	                                                   // Crea il nome completo del fiile
	strcpy(FullFilename, "output\\");
	strcat(FullFilename, nomefile);
	strcat(FullFilename, ".dat");

	fDataFile = fopen(FullFilename, "r");	// Apri il file in lettura
	if (fDataFile == NULL)                  // Se non sono riuscito ad aprire il file in lettura
	{
		file_esiste = false;
	}
	else									// Se sono riuscito ad aprire il file in lettura il file esiste
	{
		file_esiste = true;
		fclose(fDataFile);                  // Chiudi file
	}

	return file_esiste;
}

/**
 * Attiva il salvataggio su file dei dati di simulazione per gnuplot
 * parametro
 * true: abilita il salvataggio
 * false: disabilita il salvataggio
 *
 * NOta: va chiamata prima delle altre funzioni
 */
void chart_enable_save_data_on_gnuplot_file(bool enable) {
        bEnableSaveDataFile = enable;
}


/**
 * Se il salvataggio dati su file e'stato attivato (con la funzione  chart_enable_save_data_on_gnuplot_file)
 * crea il file di comandi per gnuplot output\gnuplot_simdata.gp, poi nella cartella output crea ed apre il file di dati .dat
 * con nome nomefile
 */
void chart_start_collect_data_file(char nomefile[]){
        bool file_opened = false;

    strcat(saveDataFilename, nomefile);
    strcat(saveDataFilename, ".dat");

	strcpy(saveDataFullFilename, "output\\");
	strcat(saveDataFullFilename, saveDataFilename);


    	/* apri il file dei dati per gnuplot */
    if (bEnableSaveDataFile) {
		fOutSimDataFile = fopen(saveDataFullFilename, "w");
		if (fOutSimDataFile == NULL){
				file_opened = false;
				bEnableSaveDataFile = false; // Disattiva salvataggi successuìvi
		}
		else {
				file_opened = true;
		}

    }

	/* Crea intestazione del file dei dati per gnuplot */
    if (file_opened) {

                               //"+1.04300e+003, +5.21500e+001, +0.00000e+000, -4.13404e+000, +0.00000e+000,
        fprintf(fOutSimDataFile, "             t,          Mass,      FuelMass,       Aileron,      Elevator,"
        		                 "        Rudder,        Throttle,         alpha,          beta,             V,"
        		                 "             u,             v,             w,             p,             q,"
        		                 "             r,           Phi,         Theta,           Psi,         Gamma,"
        		                 "             h,             T,             x,             y,             z,"
        		                 "         Power,      PowerMax,           rpm\n");
    }
}

/**
 * Chiude la fase di collezione dati
 */
void chart_stop_collect_data_file(void){
	if (bEnableSaveDataFile)
	{
	    fflush(fOutSimDataFile);
		fclose (fOutSimDataFile);
		printf("Dati di simulazione salvati in %s\n", saveDataFullFilename);
	}
}

/**
 * Salva sul file specificato in saveDataFullFilename i dati di simulazione relativi ad ogni istante
 */
void chart_save_data(double t, CMD *pcomando, double alfa, double beta, STATE *pstato_deg, PLANE *pplane, double Pow, double PowMax, double rpm) {
	if (bEnableSaveDataFile) {



			                    //    t     Mass     FuelMass Aileron  Elevator
		fprintf(fOutSimDataFile,  "%+1.10e, %+1.10e, %+1.10e, %+1.10e, %+1.10e, "
                                //  Rudder   Throttle   alpha     beta      V
				                  "%+1.10e, %+1.12e, %+1.10e, %+1.10e, %+1.10e, "
                                //   u        v        w        p        q
				                  "%+1.10e, %+1.10e, %+1.10e, %+1.10e, %+1.10e, "
                                //   r        Phi     Theta     Psi    Gamma
				                  "%+1.10e, %+1.10e, %+1.10e, %+1.10e, %+1.10e, "
                                //   h        T        x        y        z
                                  "%+1.10e, %+1.10e, %+1.10e, %+1.10e, %+1.10e, "
                                //   Power,PowerMax, rpm
                                  "%+1.12e, %+1.12e, %+1.10e\n",
				t,
				pplane->Mass,
				pplane->FuelMass,
				pcomando->DeltaAil,
				pcomando->DeltaEq,
				pcomando->DeltaRud,
				pcomando->EngPow,
				alfa,
				beta,
				pstato_deg->V,
				pstato_deg->u,
				pstato_deg->v,
				pstato_deg->w,
				pstato_deg->p, //
				pstato_deg->q,
				pstato_deg->r,
				//NON_USATO pstato_deg->Xv,
				//NON_USATO pstato_deg->Yv,
				//NON_USATO pstato_deg->Zv,
				pstato_deg->Phi,
				pstato_deg->Theta,
				pstato_deg->Psi,
				pstato_deg->Gamma,
				pstato_deg->h,
				pstato_deg->T,
				pstato_deg->x,
				pstato_deg->y,
				pstato_deg->z,
				Pow,
				PowMax);
	}
}


/**
 * Verifica se gnuplot e' presente nella sottocartella omonima tentando di aprire in lettura il file 'Gnuplot\bin\gnuplot.exe'
 */
 /*
bool chart_check_if_gnuplot_available(void)
{
	bool presente = false;
	FILE *fgnuplot;

	printf("\n\nVerifica se Gnuplot e' disponibile:\n");

	fgnuplot = fopen("Gnuplot\\bin\\gnuplot.exe", "r"); // Tenta di aprire il file in lettura
	if (fgnuplot != NULL)                             // Se tentativo di apertura ha funzionato
	{
		printf("Gnuplot disponibile\n");
		presente = true;                              // Segnala gnuplot presente
		fclose(fgnuplot);                             // chiudi file
	}
	else                                              // Se tentativo di apertura non ha funzionato
	{
		printf("gnuplot.exe non trovato nella cartella gnuplot\\bin\n");
		presente = false;                             // Segnala gnuplot non presente
	}

	printf("\n\nVerifica se gnuplot_simdata.gp e' disponibile:\n");

	fgnuplot = fopen(".\\output\\gnuplot_simdata.gp", "r"); // Tenta di aprire il file in lettura
	if (fgnuplot != NULL)                             // Se tentativo di apertura ha funzionato
	{
		printf("gnuplot_simdata.gp disponibile\n");
		fclose(fgnuplot);                             // chiudi file
	}
	else                                              // Se il file non esiste prova a crearlo
	{
		printf("gnuplot_simdata.gp non trovato nella cartella output\n");
		fgnuplot = fopen(".\\output\\gnuplot_simdata.gp", "w"); // Tenta di creare il file gnuplot_simdata.gp
		if (fgnuplot == NULL)
                                        // Se tentativo di apertura non ha funzionato
		{
			printf("\nImpossibile creare il file output\\gnuplot_simdata.gp"); // Segnalalo
		}
		else                                              // Se tentativo di apertura ha funzionato
		{                                                 // Ricrea il file gnuplot_simdata.gp
			fprintf(fgnuplot, "# Gnuplot command file                                                                         \n");
			fprintf(fgnuplot, "# Questo e' un commento                                                                        \n");
			fprintf(fgnuplot, "# t, Aileron, Elevator, Rudder, Thrust, u, v, w, p, q, r, Phi, Theta, Psi, h, x, y             \n");
			fprintf(fgnuplot, "# viene specificato da linea di comando con l'opzione:                                         \n");
			fprintf(fgnuplot, "#    -e \"datafile = 'output/gnuplot_simdata.dat'\"                                            \n");
			fprintf(fgnuplot, "# altrimenti si toglie il commmento nella linea sottostante                                    \n");
			fprintf(fgnuplot, "#   datafile = 'output/gnuplot_simdata.dat'                                                    \n");
			fprintf(fgnuplot, "          t_col = 1                                                                            \n");
			fprintf(fgnuplot, "       Mass_col = 2                                                                            \n");
			fprintf(fgnuplot, "   FuelMass_col = 3                                                                            \n");
			fprintf(fgnuplot, "    Aileron_col = 4                                                                            \n");
			fprintf(fgnuplot, "   Elevator_col = 5                                                                            \n");
			fprintf(fgnuplot, "     Rudder_col = 6                                                                            \n");
			fprintf(fgnuplot, "     Thrust_col = 7                                                                            \n");
			fprintf(fgnuplot, "      alpha_col = 8                                                                            \n");
			fprintf(fgnuplot, "       beta_col = 9                                                                            \n");
			fprintf(fgnuplot, "          V_col = 10                                                                           \n");
			fprintf(fgnuplot, "          u_col = 11                                                                           \n");
			fprintf(fgnuplot, "          v_col = 12                                                                           \n");
			fprintf(fgnuplot, "          w_col = 13                                                                           \n");
			fprintf(fgnuplot, "          p_col = 14                                                                           \n");
			fprintf(fgnuplot, "          q_col = 15                                                                           \n");
			fprintf(fgnuplot, "          r_col = 16                                                                           \n");
			fprintf(fgnuplot, "        Phi_col = 17                                                                           \n");
			fprintf(fgnuplot, "      Theta_col = 18                                                                           \n");
			fprintf(fgnuplot, "        Psi_col = 19                                                                           \n");
			fprintf(fgnuplot, "      Gamma_col = 20                                                                           \n");
			fprintf(fgnuplot, "          h_col = 21                                                                           \n");
			fprintf(fgnuplot, "          T_col = 22                                                                           \n");
			fprintf(fgnuplot, "          x_col = 23                                                                           \n");
			fprintf(fgnuplot, "          y_col = 24                                                                           \n");
			fprintf(fgnuplot, "          z_col = 25                                                                           \n");
			fprintf(fgnuplot, "                                                                                               \n");
			fprintf(fgnuplot, "set terminal wxt size 1400,900 # Dimensione della finestra                                     \n");
			fprintf(fgnuplot, "set multiplot layout 5,1       # 5 grafici su una colonna                                      \n");
			fprintf(fgnuplot, "set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici \n");
			fprintf(fgnuplot, "set rmargin 10                 # margine destro comune per allineare correttamente i grafici   \n");
			fprintf(fgnuplot, "#set size 1, 0.2               #dimensione singoli grafici, alternativo a layout 5,1. non usato\n");
			fprintf(fgnuplot, "set style line 1 linewidth 2   # stile linea, non usato                                        \n");
			fprintf(fgnuplot, "#set origin 0.0,0.8            #posizione grafico. Usare con 'set size 1, 0.2 '      \n");
			fprintf(fgnuplot, "plot datafile using t_col:Aileron_col  with lines linewidth 2 title \"Aileron\" , \\\n");
			fprintf(fgnuplot, "     datafile using t_col:Elevator_col with lines linewidth 2 title \"Elevator\" , \\\n");
			fprintf(fgnuplot, "     datafile using t_col:Rudder_col   with lines linewidth 2 title \"Rudder\" , \\\n");
			fprintf(fgnuplot, "     #datafile using t_col:Thrust_col  with lines linewidth 2 title \"Thrust\"       \n");
			fprintf(fgnuplot, "                                                                                     \n");
			fprintf(fgnuplot, "#set origin 0.0,0.6            #posizione grafico. Usare con 'set size 1, 0.2 '      \n");
			fprintf(fgnuplot, "plot  datafile using t_col:h_col         with lines linewidth 2 title \"h\"          \n");
			fprintf(fgnuplot, "                                                                                     \n");
			fprintf(fgnuplot, "#set origin 0.0,0.4            #posizione grafico. Usare con 'set size 1, 0.2 '      \n");
			fprintf(fgnuplot, "plot  datafile using t_col:u_col         with lines linewidth 2 title \"u\"          \n");
			fprintf(fgnuplot, "                                                                                     \n");
			fprintf(fgnuplot, "#set origin 0.0,0.2            #posizione grafico. Usare con 'set size 1, 0.2 '      \n");
			fprintf(fgnuplot, "plot  datafile using t_col:v_col         with lines linewidth 2 title \"v\"          \n");
			fprintf(fgnuplot, "                                                                                     \n");
			fprintf(fgnuplot, "#set origin 0.0,0.0            #posizione grafico. Usare con 'set size 1, 0.2 '      \n");
			fprintf(fgnuplot, "plot  datafile using t_col:w_col         with lines linewidth 2 title \"w\"          \n");
			fprintf(fgnuplot, "                                                                                     \n");
			fprintf(fgnuplot, "set key right  # visualizza legenda a destra                                         \n");
			fprintf(fgnuplot, "pause -1                                                                             \n");
			fprintf(fgnuplot, "                                                                                     \n");
			fprintf(fgnuplot, "#linee di comando commentate per altri grafici                                       \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Mass_col      with lines linewidth 2 title \"Mass\"       \n");
			fprintf(fgnuplot, "#plot datafile using t_col:FuelMass_col  with lines linewidth 2 title \"FuelMass\"   \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Aileron_col   with lines linewidth 2 title \"Aileron\"    \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Elevator_col  with lines linewidth 2 title \"Elevator\"   \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Rudder_col    with lines linewidth 2 title \"Rudder\"     \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Thrust_col    with lines linewidth 2 title \"Thrust\"     \n");
			fprintf(fgnuplot, "#plot datafile using t_col:alpha_col     with lines linewidth 2 title \"alpha\"      \n");
			fprintf(fgnuplot, "#plot datafile using t_col:beta_col      with lines linewidth 2 title \"beta\"       \n");
			fprintf(fgnuplot, "#plot datafile using t_col:V_col         with lines linewidth 2 title \"V\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:u_col         with lines linewidth 2 title \"u\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:v_col         with lines linewidth 2 title \"v\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:w_col         with lines linewidth 2 title \"w\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:p_col         with lines linewidth 2 title \"p\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:q_col         with lines linewidth 2 title \"q\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:r_col         with lines linewidth 2 title \"r\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Phi_col       with lines linewidth 2 title \"Phi\"        \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Theta_col     with lines linewidth 2 title \"Theta        \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Psi_col       with lines linewidth 2 title \"Psi\"        \n");
			fprintf(fgnuplot, "#plot datafile using t_col:Gamma_col     with lines linewidth 2 title \"Gamma\"      \n");
			fprintf(fgnuplot, "#plot datafile using t_col:T_col         with lines linewidth 2 title \"T\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:h_col         with lines linewidth 2 title \"h\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:x_col         with lines linewidth 2 title \"x\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:y_col         with lines linewidth 2 title \"y\"          \n");
			fprintf(fgnuplot, "#plot datafile using t_col:z_col         with lines linewidth 2 title \"z\"          \n");
			fflush(fgnuplot);
			fclose(fgnuplot);
		}
	}
	return presente;
}
*/
