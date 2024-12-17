#include <stdio.h>
#include <stdlib.h>
#include "dati.h"
#include "ErrWarn.h"
#include "dba.h" // Controllo congruenza prototipi

void dba_dati(void) {
	FILE *input_file, *output_file;
	char line[100];
	int i = 1;

	input_file = fopen("input/dba.txt", "r"); // apertura file dba.txt
	if (input_file == NULL) {
		ErrWarn_DisplayErrMsgAndExit(E010, "Impossibile aprire il file dba.txt");
	}

	output_file = fopen("input/dba_nuovo.txt", "w");  // apertura file da scrivere
	if (output_file == NULL) {
		ErrWarn_DisplayErrMsgAndExit(E011, "Impossibile aprire il file dba_nuovo.txt in scrittura");
	}

	if ((input_file != NULL) && (output_file != NULL))
	{

		// Leggo il file di input riga per riga
		while (fgets(line, sizeof(line), input_file)) {
			// Controllo se la riga non inizia con un asterisco, partendo dalla 3 riga
			if (line[0] != '*' && line[1] != '*' && i > 3) {
				// Se la riga non inizia con un asterisco, la scrivo nel file di output
				fputs(line, output_file);
			}
			i++;
		}

		fclose(input_file);
		fclose(output_file);
	}

}

PLANE dba_variabili() {
	PLANE plane;
	int input;

	PLANE_Initialize(&plane);

	FILE *fw;
	char str[256];

	fw = fopen("input/dba_nuovo.txt", "r");

	if (fw == NULL) {
		ErrWarn_DisplayErrMsgAndExit(E012, "Impossibile aprire il file input\\dba_nuovo.txt in lettura");
	}
	else {
		fscanf(fw, "%lf %*s\n", &plane.EmpMass);
		plane.Mass = plane.EmpMass;
		fscanf(fw, "%lf %*s %*s\n", &plane.WSpan);
		fscanf(fw, "%lf %*s %*s\n", &plane.WSurf);
		fscanf(fw, "%lf %*s\n", &plane.Chord);
		fscanf(fw, "%lf %*s %*s %*s\n", &plane.MDR);
		fgets(str, 256, fw);

		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);

		fscanf(fw, "%lf %*s\n", &plane.JX);
		fscanf(fw, "%lf %*s\n", &plane.JY);
		fscanf(fw, "%lf %*s\n", &plane.JZ);
		fscanf(fw, "%lf %*s\n", &plane.JXZ);

		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);
		fgets(str, 256, fw);

		fscanf(fw, "%lf %*s %*s\n", &plane.ElevatorMax);
		fscanf(fw, "%lf %*s %*s\n", &plane.ElevatorMin);
		fscanf(fw, "%lf %*s %*s\n", &plane.Ailerons);
		fscanf(fw, "%lf %*s %*s\n", &plane.Rudder);
		fscanf(fw, "%lf %*s %*s %*s\n", &plane.FlapMin);
		fscanf(fw, "%lf %*s %*s %*s\n", &plane.FlapMax);

		fscanf(fw, "%d %*s %*s %*s %*s\n", &input);
		if (input == 1) {
			plane.VarFuel = true;
		}
		else {
			plane.VarFuel = false;
		}
		fscanf(fw, "%lf %*s %*s %*s %*s %*s %*s %*s\n", &plane.FuelFract);
		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[1][i], &(plane.Der)[2][i], &(plane.Der)[3][i], &(plane.Der)[4][i], &(plane.Der)[5][i], &(plane.Der)[6][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[7][i], &(plane.Der)[8][i], &(plane.Der)[9][i], &(plane.Der)[10][i], &(plane.Der)[11][i],
					&(plane.Der)[12][i], &(plane.Der)[13][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[14][i], &(plane.Der)[15][i], &(plane.Der)[16][i], &(plane.Der)[17][i], &(plane.Der)[18][i],
					&(plane.Der)[19][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[20][i], &(plane.Der)[21][i], &(plane.Der)[22][i], &(plane.Der)[23][i], &(plane.Der)[24][i],
					&(plane.Der)[25][i], &(plane.Der)[26][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[27][i], &(plane.Der)[28][i], &(plane.Der)[29][i], &(plane.Der)[30][i], &(plane.Der)[31][i],
					&(plane.Der)[32][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[33][i], &(plane.Der)[34][i], &(plane.Der)[35][i], &(plane.Der)[36][i], &(plane.Der)[37][i],
					&(plane.Der)[38][i], &(plane.Der)[39][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[40][i], &(plane.Der)[41][i], &(plane.Der)[42][i], &(plane.Der)[43][i], &(plane.Der)[44][i],
					&(plane.Der)[45][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[46][i], &(plane.Der)[47][i], &(plane.Der)[48][i], &(plane.Der)[49][i], &(plane.Der)[50][i],
					&(plane.Der)[51][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[52][i], &(plane.Der)[53][i], &(plane.Der)[54][i], &(plane.Der)[55][i], &(plane.Der)[56][i],
					&(plane.Der)[57][i]);
		}

		fgets(str, 256, fw);

		for (int i = 0; i < 126; i++) {
			fscanf(fw, "%lf %lf %lf %lf %lf %lf %lf\n", &(plane.Der)[0][i], &(plane.Der)[58][i], &(plane.Der)[59][i], &(plane.Der)[60][i], &(plane.Der)[61][i], &(plane.Der)[62][i],
					&(plane.Der)[63][i]);
		}

	}
	fclose(fw);
	printf("Il salvataggio dei dati aerodinamici e' avvenuto correttamente\n\n");
	return plane;
}

