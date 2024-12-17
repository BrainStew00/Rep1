#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dati.h"
#include "ErrWarn.h"
#include "Atmosphere.h"
#include "engine.h"

void engine_dati() {
	FILE *input_file, *output_file;
	char line[100];
	char parola[] = "RIDUZIONE";

	input_file = fopen("input/engine.txt", "r"); // apertura file dba.txt
	if (input_file == NULL)
	{
		ErrWarn_DisplayErrMsgAndExit(E013, "Impossibile aprire il file engine.txt in lettura");
	}

	output_file = fopen("input/engine_nuovo.txt", "w");  // apertura file da scrivere
	if (output_file == NULL)
	{
		ErrWarn_DisplayErrMsgAndExit(E014, "Impossibile aprire il file engine_nuovo.txt in scrittura");
	}

	// Leggo il file di input riga per riga
	while (fgets(line, sizeof(line), input_file) != NULL) {
		if (strstr(line, parola) == NULL) {
			// Controllo se la riga non inizia con un asterisco
			if (line[0] != '*' && line[1] != '*') {
				// Se la riga non inizia con un asterisco, la scrivo nel file di output
				fputs(line, output_file);
			}
		}
	}
	fclose(input_file);
	fclose(output_file);
}

/**
 * Calcolo del consumo di carburante nel tempo specificato
 * Parametri:
 * 	power_kw: potenza in kw
 * 	sfc_kgs_w: consumo specifico in kg/s/w
 * 	delta_t_s: Intervallo di tempo in secondi
 */
double engineCalcFuelConsumption(double power_kw, double sfc_kgs_w, double delta_t_s) {
	double deltaFuel;

	deltaFuel = power_kw * 1000.0 * sfc_kgs_w * delta_t_s;

	return deltaFuel;
}

/**
 * Calcola la potenza max erogabile dal motore alla quota specificata
 */
double engineCalcPmax(ENG *peng, double h) {
	double Pmax_h, rho_h, rho_0;

	rho_h = Atmosphere_Rho(h);
	rho_0 = Atmosphere_Rho(0);
	Pmax_h = peng->Potenza * pow(rho_h / rho_0, peng->Esponente);
	return Pmax_h;
}


ENG engine_variabili() {
	ENG eng;
	FILE *input_file;
	char buff[100];

	ENG_Initialize(&eng);

	int i = 1;

	input_file = fopen("input/engine_nuovo.txt", "r");

	if (input_file == NULL){
		ErrWarn_DisplayErrMsgAndExit(E015, "Impossibile aprire il file engine_nuovo.txt in lettura");
	}
	else {
		while (i <= 1) {
			fscanf(input_file, "%lf", &eng.Potenza);
			//printf("sizeof(buff)=%d", sizeof(buff));
			fgets(buff, sizeof(buff), input_file);
			i++;
		}
		while (i <= 2) {
			fscanf(input_file, "%lf", &eng.Esponente);
			fgets(buff, sizeof(buff), input_file);
			i++;
		}
		while (i <= 3) {
			fscanf(input_file, "%lf", &eng.RPMmin);
			fgets(buff, sizeof(buff), input_file);
			i++;
		}
		while (i <= 4) {
			fscanf(input_file, "%lf", &eng.RPMmax);
			fgets(buff, sizeof(buff), input_file);
			i++;
		}
		while (i <= 5) {
			fscanf(input_file, "%lf", &eng.Eta);
			fgets(buff, sizeof(buff), input_file);
			i++;
		}
		while (i <= 6) {
			fscanf(input_file, "%lf", &eng.SFC);
			fgets(buff, sizeof(buff), input_file);
			i++;
		}
	}

	fclose(input_file);
	printf("Il salvataggio dei dati engine e' avvenuto correttamente\n");

	return eng;
}
