/**
 * Si usano le seguenti formule
 *
 * T(h) = T(h=0) - 0.0065 h
 *
 * P(h) = P(h=0) * (T(h) / T(h=0))^5.2561
 *
 * rho(h) = P(h) / (R * T(h))
 *
 * vsuono(h) = sqrt(gamma * R * T(h))
 */

#include "Atmosphere.h"

#include <ctype.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dati.h"
#include "user_input.h"

#define RHO0    1.225
#define T0 	(273.15 + 15)
#define R 287.05
#define GAMMA_ARIA 1.4
#define QUOTA_DEFAULT       0
#define PRESS_DEFAULT  101325
#define TEMP_DEFAULT_C     15 // Gradi centigradi

#define K_LIM_RANGE         2  // Il range accettabile va dal valore attuale diviso per K_LIM_RANGE al valore attuale moltiplicato per K_LIM_RANGE

/**
 * L'utente puo' scegliere arbitrariamente le condizioni atmosferiche alla quota iniziale di simulazione.
 * Vengono calcolati e memorizzati i valori di temperatura e pressione ad h=0 in modo che alla quota specificata
 * corrispondano con i valori immessi.
 *
 * Temperatura
 *     		T(h) = T(h=0) - 0.0065 h
 *
 *     	Inserendo h e T(h) viene calcolato
 *
 *     		T(h=0) = T(h) + 0.0065h
 *
 * Pressione
 *
 * 			P(h) = P(h=0) * (T(h) / T(h=0))^5.2561
 *
 *      Inserendo h e P(h) e T(h) si calcola
 *
 *      	P(h=0) = P(h)  / (T(h) / T(h=0))^5.2561
 *
 * Densita'
 *      	rho(h) = P(h) / (R * T(h))
 *      	P(h) = P(h=0) * (T(h) / T(h=0))^5.2561
 *
 *      Combinandole si ottiene
 *
 * 			rho(h) = rho(h=0) * (T(h) / T(h=0))^4.2561
 *
 *      Inserendo rho(h) si calcola:
 *
 *      	rho(h=0) = rho(h) / (T(h) / T(h=0))^4.2561
 *
 * Velocita' del suono
 *
 * 			vsuono(h) = sqrt(gamma * R * T(h)) = vsuono(0) * sqrt( T(h) / T(h=0))
 *
 *      Inserendo vsuono(h) si calcola:
 *
 *      	vsuono(0) = vsuono(h) / sqrt( T(h) / T(h=0))
 *
 */



ATMOSPAR AtmosPar;



/**
 * Imposta i parametri atmosferici arbitrari
 */
void Atmosphere_imposta_parametri_modello_personalizzato(double h, double temp_h, double press_h, double rho_h, double vsuono_h)
{
	//T(h=0) = T(h) + 0.0065h
	AtmosPar.temp_0 = temp_h + 0.0065 * h;
	//P(h=0) = P(h)  / (T(h) / T(h=0))^5.2561
	AtmosPar.press_0 = press_h / pow((temp_h / AtmosPar.temp_0), 5.2561);
	//  vsuono(0) = vsuono(h) / sqrt( T(h) / T(h=0))
	AtmosPar.vsuono_0 = vsuono_h / sqrt(temp_h / AtmosPar.temp_0);
	// rho(h=0) = rho(h) / (T(h) / T(h=0))^4.2561
	AtmosPar.rho_0 = rho_h / pow(temp_h / AtmosPar.temp_0, 4.2561);
	AtmosPar.h_ref = h;
}



/**
 * Imposta i parametri atmosferici secondo il modello ISA consentendo di scegliere pressione e temperatura
 */
void Atmosphere_imposta_parametri_modello_standard(double h, double temp_h, double press_h)
{
	//T(h=0) = T(h) + 0.0065h
	AtmosPar.temp_0 = temp_h + 0.0065 * h;
	//P(h=0) = P(h)  / (T(h) / T(h=0))^5.2561
	AtmosPar.press_0 = press_h / pow((temp_h / AtmosPar.temp_0), 5.2561);

	// vsuono(h) = sqrt(gamma * R * T(h))
	AtmosPar.vsuono_0 = sqrt(GAMMA_ARIA * R * AtmosPar.temp_0);
	// rho(h) = P(h) / (R * T(h))
	AtmosPar.rho_0 = AtmosPar.press_0 / (R * AtmosPar.temp_0);
	AtmosPar.h_ref = h;

}


/**
 * Calcola la temperatura dell'aria alla quota specificata
 */
double Atmosphere_temperature(double h)
{
	return (AtmosPar.temp_0 - 0.0065 * h);
}

/**
 * Calcola la pressione dell'aria alla quota specificata
 * P(h) = P(h=0) * (T(h) / T(h=0))^5.2561
 */
double Atmosphere_press(double h)
{
	double t_h, press;


	t_h = Atmosphere_temperature(h);
	press = AtmosPar.press_0 * pow((t_h / AtmosPar.temp_0), 5.2561);

	return (press);
}

/*
 * Calcola la densità dell'aria in base alla quota specificata
 * rho(h) = rho(h=0) * (T(h) / T(h=0))^4.2561
 */
double Atmosphere_Rho(double h)
{
	double t_h, rho;


	t_h = Atmosphere_temperature(h);
	rho = AtmosPar.rho_0 * pow((t_h / AtmosPar.temp_0), 4.2561);
	return rho;
}

/*
 * Calcola la velocita' del suono in base alla quota specificata
 * vsuono(h) = sqrt(gamma * R * T(h)) = vsuono(0) * sqrt( T(h) / T(h=0))
 */
double Atmosphere_vsuono(double h)
{
	double t_h, vsuono;

	t_h = Atmosphere_temperature(h);
	vsuono = AtmosPar.vsuono_0 * sqrt(t_h / AtmosPar.temp_0);
	return vsuono;
}

// il codice mostra i dati atmosferici di default a video e permette di modificarli.

void AtmosphereChoiceAndCalc(double h) {
	int input;
	double press, vsuono, rho, temp_c;
	double press_ini, vsuono_ini, rho_ini, temp_c_ini;

	char msg[1000];


	/**
	 * Inizializzazione con i parametri standard
	 * Quota 0
	 * Pressione 101325
	 * Temperatura 15°C
	 * Conseguenza:
	 * Rho 1.225
	 * Vsuono 340
	 *
	 */
	Atmosphere_imposta_parametri_modello_standard(QUOTA_DEFAULT, CENT_TO_KELVIN(TEMP_DEFAULT_C), PRESS_DEFAULT);


	printf("\n\\\\\\\\\\\\\\\\\\\\\\"
			"\n\\\\\\\\ATMOSFERA ISA"
			"\n\\\\\\\\\\\\\\\\\\\\\\\n");
	printf("\nLa simulazione fa riferimento al modello atmosferico ISA, avente i seguenti valori per quota h=0 m (sea level):\n\n");
	printf("        Temperatura    T = %+3.1f [\370C]\n", KELVIN_TO_CENT(AtmosPar.temp_0));
	printf("          Pressione   P = %6.0f [Pa]\n", AtmosPar.press_0);
	printf("            Densita' rho = %4.3f [kg/m^3]\n", AtmosPar.rho_0);
	printf("Velocita' del Suono    a = %4.1f [m/s]\n", AtmosPar.vsuono_0);

	printf("\nSe non si desidera procedere con i suddetti parametri e' possibile modificarli, reinserendoli manualmente o scegliendo una quota differente.\n");
		// Crea il messaggio inserendo la quota iniziale
	sprintf(msg,
			"\n"
			"Premere:\n"
			"\t[1] se si desidera procedere con i valori iniziali precedentemente indicati\n"
			"\t[2] se si desidera modificare i valori iniziali al livello del mare\n"
			"\t[3] se di desidera immettere i valori manualmente alla quota iniziale (%.0f m)\n",
			h);
	input = user_input_richiedi_int(msg,
									1,  // Valore minimo accettabile
									3); // Valore massimo accettabile
	switch (input) {
		case 1: // mantiene i valori iniziali di default
			// Non fare nulla, parametri già impostati
			break;

		case 2: //modifica i valori iniziali slm
			press_ini = AtmosPar.press_0;
			press = user_input_richiedi_double_con_default("Pressione al livello del mare",
					                                       "Pa",             // unita di misura
					                                       press_ini,        // Valore attuale
														   press_ini / K_LIM_RANGE,  // Min
														   press_ini * K_LIM_RANGE); // Max
			temp_c_ini = KELVIN_TO_CENT(AtmosPar.temp_0);
			temp_c = user_input_richiedi_double_con_default("Temperatura al livello del mare",
					                                       "\370C",         // unita di misura °C
					                                       temp_c_ini, // Valore attuale
                                                           temp_c_ini - 105,             // -90 slm Min Stazione Vostok, Antartide 21 luglio 1983
														   temp_c_ini + 45);             // +60 Max Furnace Creek Ranch (Death Valley) 10 luglio 1913

			Atmosphere_imposta_parametri_modello_standard(0, CENT_TO_KELVIN(temp_c), press); // Ricalcola rho e vsuono a quota 0

			rho_ini = AtmosPar.rho_0;
			rho = user_input_richiedi_double_con_default("Densita' dell'aria al livello del mare",
					                                       "kg/m^3",       // unita di misura
					                                       rho_ini,        // Valore attuale
                                                           rho_ini / K_LIM_RANGE,  // Min
														   rho_ini * K_LIM_RANGE); // Max

			vsuono_ini = AtmosPar.vsuono_0;
			vsuono = user_input_richiedi_double_con_default("Velocita' del suono al livello del mare",
					                                       "m/s",       // unita di misura
					                                       vsuono_ini,        // Valore attuale
                                                           vsuono_ini / K_LIM_RANGE,  // Min
														   vsuono_ini * K_LIM_RANGE); // Max

			Atmosphere_imposta_parametri_modello_personalizzato(0, CENT_TO_KELVIN(temp_c), press, rho, vsuono); // Ricalcola rho e vsuono a quota 0
			break;

		case 3: // immette i valori manualmente alla quota iniziale
			press_ini = Atmosphere_press(h); // Calcola la pressione con i parametri standard
			sprintf(msg, "Pressione a %.0f m", h);
			press = user_input_richiedi_double_con_default(msg,              // Messaggio
					                                       "Pa",              // unita di misura
					                                       press_ini,        // Valore attuale
                                                           press_ini / K_LIM_RANGE,  // Min
														   press_ini * K_LIM_RANGE); // Max
			temp_c_ini = KELVIN_TO_CENT(Atmosphere_temperature(h)); // Calcola la temperatura con i parametri standard
			sprintf(msg, "Temperatura a %.0f m", h);
			temp_c = user_input_richiedi_double_con_default(msg,
					                                       "\370C",         // unita di misura °C
					                                       temp_c_ini, // Valore attuale
                                                           temp_c_ini - 105,             // -90 slm Min Stazione Vostok, Antartide 21 luglio 1983
														   temp_c_ini + 45);             // +60 Max Furnace Creek Ranch (Death Valley) 10 luglio 1913

			Atmosphere_imposta_parametri_modello_standard(h, CENT_TO_KELVIN(temp_c), press); // Ricalcola rho e vsuono

			rho_ini = Atmosphere_Rho(h);
			sprintf(msg, "Densita' dell'aria a %.0f m", h);
			rho = user_input_richiedi_double_con_default(msg,
					                                    "kg/m^3",       // unita di misura
					                                     rho_ini,        // Valore attuale
                                                         rho_ini / K_LIM_RANGE,  // Min
														 rho_ini * K_LIM_RANGE); // Max

			vsuono_ini = Atmosphere_vsuono(h);
			sprintf(msg, "Velocita' del suono a %.0f m", h);
			vsuono = user_input_richiedi_double_con_default(msg,
					                                       "m/s",       // unita di misura
					                                       vsuono_ini,        // Valore attuale
                                                           vsuono_ini / K_LIM_RANGE,  // Min
														   vsuono_ini * K_LIM_RANGE); // Max

			Atmosphere_imposta_parametri_modello_personalizzato(h, CENT_TO_KELVIN(temp_c), press, rho, vsuono); // Ricalcola rho e vsuono
			break;
	}



}
