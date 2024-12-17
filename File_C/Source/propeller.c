#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "propeller.h"
#include "dati.h"

/********************************************//**
 * \brief
 *
 * \return void
 *
 ***********************************************/
void propeller_dati(void) {
	FILE *input_file, *output_file;
	char line[100];
	int i = 0;

	input_file = fopen("input/propeller.txt", "r"); // apertura file dba.txt
	output_file = fopen("input/propeller_nuovo.txt", "w");  // apertura file da scrivere

	// Leggo il file di input riga per riga
	while (fgets(line, sizeof(line), input_file) != NULL) {
		i++;
		// Controllo se la riga non inizia con un asterisco, partendo dalla 3 riga
		if (line[0] != '*' && line[1] != '*') {
			// Se la riga non inizia con un asterisco, la scrivo nel file di output
			fputs(line, output_file);
		}
	}
	fclose(input_file);
	fclose(output_file);
}


PROP propeller_variabili() {

	FILE *input_file;
	PROP prop; // struttura su cui inserire i dati letti da file

	char str[256];

	int i = 0;

	input_file = fopen("input/propeller_nuovo.txt", "r");

	if (input_file == NULL)
		printf("Errore");
	else {
		fscanf(input_file, "%lf %*s %*s\n", &prop.Diametro);
		fscanf(input_file, "%lf %*s %*s %*s\n", &prop.DiametroOgiva);
		fscanf(input_file, "%d %*s %*s %*s\n", &prop.NPale);
		fscanf(input_file, "%lf %*s %*s\n", &prop.J);
		fscanf(input_file, "%d %*s %*s %*s\n", &prop.NumStazioni);
		fscanf(input_file, "%lf %*s %*s\n ", &prop.ClA);
		fscanf(input_file, "%lf %*s\n ", &prop.Cl0);
		fscanf(input_file, "%lf %*s %*s\n ", &prop.A0);
		fscanf(input_file, "%lf %*s %*s\n ", &prop.CdA2);
		fscanf(input_file, "%lf %*s %*s\n ", &prop.CdA);
		fscanf(input_file, "%lf %*s\n ", &prop.Cd0);

		fgets(str, 256, input_file);

		while (!feof(input_file)) {

			fscanf(input_file, "%lf %lf %lf %lf\n", &prop.CSI[i], &prop.RD[i], &prop.CH[i], &prop.BA[i]);
			i++;

		}
	}
	fclose(input_file);
	printf("Il salvataggio dei dati propeller e' avvenuto correttamente\n");
	return prop;
}

