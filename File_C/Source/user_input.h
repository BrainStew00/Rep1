#ifndef USER_INPUT_H_INCLUDED
#define USER_INPUT_H_INCLUDED
#include "dati.h"





STATE user_input();

void user_input_ini_menu(USERVAL *pUserValues);
bool user_input_leggi_dati_iniziali(char *nomefile, USERVAL *pUserVal);
void user_input_salva_dati_iniziali(char *nomefile, USERVAL *pUserVal);
/**
 * Chiede all'utente di inserire un intero
 * Parametri:
 * prompt: messaggio da visualizzare
 * min: valore minimo accettabile
 * max: valore max accettabile
 */
int user_input_richiedi_int(char *prompt, int min, int max);

/**
 * Chiede all'utente di inserire un intero positivo
 * Parametri:
 * prompt: messaggio da visualizzare
 */
int user_input_richiedi_int_positivo(char *prompt);

/**
 * Chiede all'utente di inserire un double
 * Parametri:
 * prompt: messaggio da visualizzare
 * min: valore minimo accettabile
 * max: valore max accettabile
 */
double user_input_richiedi_double(char *prompt, double dmin, double dmax);

/**
 * Chiede all'utente di inserire un double
 * premendo direttamente enter si mantiene il valore attuale
 * Parametri:
 * prompt: messaggio da visualizzare
 * unita_misura: stringa con unita' di misura da stampare a fianco del valore attuale. Inserire "" se adimensionale
 * attuale: valore attuale
 * min: valore minimo accettabile
 * max: valore max accettabile
 */
double user_input_richiedi_double_con_default(char *prompt, char *unita_misura, double attuale, double dmin, double dmax);

/**
 * Chiede all'utente di inserire un double positivo
 * Parametri:
 * prompt: messaggio da visualizzare
 */
double user_input_richiedi_double_positivo(char *prompt);

/**
 * Chiede all'utente di inserire una stringa.
 * Propone il valore attuale della stringa e non lo modifica se l'utente preme il tasto "enter"
 * Parametri:
 * prompt: messaggio da visualizzare
 * stringa: puntatore alla stringa da inserire
 */
void user_input_richiedi_stringa(char prompt[], char stringa[]);

/**
 * Elimina i caratteri di spaziatura a fine inizio stringa
 */
void user_input_elimina_spazi_ini_fin(char stringa[]);

/**
 * Visualizza il messaggio msg, attende la pressione di un tasto per proseguire
 */
void user_input_StampaMsgAttendiTasto(char *msg);
#endif // USER_INPUT_H_INCLUDED
