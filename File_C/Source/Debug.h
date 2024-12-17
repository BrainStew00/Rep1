#include <stdbool.h>
extern bool Debug_On;                          // true: Attiva l visualizzazione
extern bool Debug_Blocca_per_visualizzare;     // true: attiva la sospensione dell'esecuzione ogni Debug_Cnt_BreakIncrement
extern int  Debug_Cnt_BreakIncrement;          // Ogni quante stampe di debug si ferma
extern int  Debug_NextBreakCnt;                // Prossimo valore di contatore per cui si blocca l'esecuzione

/**
 * Stampa il testo msg seguito dal valore value
 * La stampa avviene solo se la variabile globale  Debug_On e' impostata a true
 */
void debug_print_double(char *msg, double value);

void debug_integrazione_abilita_visualizzazione(void); // Chiede all'utente se stampare le informazioni di debug relative a integrazione.c
void debug_integrazione_aggiorna_contatore_cicli(void);
bool debug_integrazione_stampa_in_questo_ciclo(void);
void debug_integrazione_stampa_residui(double t, double residui[]); // Stampa dei residui come da impostazioni utente
bool debug_integrazione_visualizzazione_abilitata(void);
