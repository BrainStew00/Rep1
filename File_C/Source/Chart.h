/***
 * Funzioni per il salvataggio di file dati utilizzati per la reazione di grafici
 */
#include <stdbool.h>
#include "dati.h"

/**
 * Attiva il salvataggio su file dei dati di simulazione per gnuplot
 * parametro
 * true: abilita il salvataggio
 * false: disabilita il salvataggio
 *
 * NOta: va chiamata prima delle altre funzioni
 */
void chart_enable_save_data_on_gnuplot_file(bool enable);

/**
 * Imposta automaticamente il nome del file di salvataggio dati
 * in base ai parametri iniziali
 * aggiungendo un numero coda in modo che il file non esista
 * Il nome contiene in sequenza:
 *  - Quota
 *  - Velocita'
 *  - Angolo di rampa
 *  - tipo simulazione (massa fissa/variabile)
 *  - durata simulazione
 *  - numero incrementale
 *  Esempio: 1000m_52ms_0deg_mFix_300s_001
 *  Nota: '.dat' viene aggiunto automaticamente dalla funzione di salvataggio
 */
void chart_autoset_data_filename(USERVAL *pUserValues);


/**
 * Se il salvataggio dati su file e'stato attivato (con la funzione  chart_enable_save_data_on_gnuplot_file)
 * crea il file di comandi per gnuplot output\gnuplot_simdata.gp, poi nella cartella output crea ed apre il file di dati .dat
 * con nome nomefile
 */
void chart_start_collect_data_file(char nomefile[]);

/**
 * Chiude la fase di collezione dati
 */
void chart_stop_collect_data_file(void);

/**
 * Salva sul file specificato in gnuplot_data_filename i dati di simulazione relativi ad ogni istante
 */
void chart_save_data(double t, CMD *pcomando, double alfa, double beta, STATE *pstato_deg, PLANE *pplane, double Pow, double PowMax, double rpm);
/**
 * Verifica se il file .dat specificato esiste nella cartella .\output\
 */

bool chart_check_if_dat_file_exist(char nomefile[]);
/**
 * Chiede all'utente il nome del file in cui salvare i dati di simulazione
 **/
void chart_ask_filename(char nome[]);

/**
 * Verifica se gnuplot e' presente nella sottocartella omonima tentando di aprire in lettura il file 'Gnuplot\bin\gnuplot.exe'
 */
bool chart_check_if_gnuplot_available(void);

/**
 * Crea il file con il nome specificato e l'estensione txt nella cartella output
 */
void chart_simulation_description_file_create(char nomefile[]);
/**
 * restituisce il file handler del file di descrizione della simulazione
 */
FILE *chart_simulation_description_file_get_file_handler(void);
/**
 * Chiude il file di descrizione della simulazione
 */
void chart_simulation_description_file_close(void);


/**
 * restituisce il nome del file della simulazione compresa l'estensione .dat
 */
char *chart_simulation_description_file_get_filename(void);
