#include <stdio.h>
#include <string.h>
#include "dati.h"
#include "limits.h"
#include "math.h"


  /* Possibili valori per INITIALIZE_MODE */
#define INITIALIZE_WITH_ZERO           (1) // Imposta tutti i campi a zero
#define INITIALIZE_WITH_NOT_A_NUMBER   (2) // Imposta tutti i float e double a NAN (Not A Number), gli int al minimo INT_MIN

#define INITIALIZE_MODE  INITIALIZE_WITH_NOT_A_NUMBER




void AC_Initialize(AC *p)
{
	double init_value;


	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value = 0.0;
  	}
	else {
		init_value = NAN; // Not a number
  	}

	p->Alpha = init_value;
	p->CX    = init_value;
	p->CY    = init_value;
	p->CZ    = init_value;
	p->Cl    = init_value;
	p->Cm    = init_value;
	p->Cn    = init_value;
	p->CXA   = init_value;
	p->CXAP  = init_value;
	p->CXU   = init_value;
	p->CXQ   = init_value;
	p->CXB   = init_value;
	p->CXP   = init_value;
	p->CXR   = init_value;
	p->CYB   = init_value;
	p->CYBP  = init_value;
	p->CYP   = init_value;
	p->CYR   = init_value;
	p->CYA   = init_value;
	p->CYQ   = init_value;
	p->CZA   = init_value;
	p->CZAP  = init_value;
	p->CZU   = init_value;
	p->CZQ   = init_value;
	p->CZB   = init_value;
	p->CZP   = init_value;
	p->CZR   = init_value;
	p->ClB   = init_value;
	p->ClBP  = init_value;
	p->ClP   = init_value;
	p->ClR   = init_value;
	p->ClA   = init_value;
	p->ClQ   = init_value;
	p->CmA   = init_value;
	p->CmAP  = init_value;
	p->CmU   = init_value;
	p->CmQ   = init_value;
	p->CmB   = init_value;
	p->CmP   = init_value;
	p->CmR   = init_value;
	p->CnB   = init_value;
	p->CnBP  = init_value;
	p->CnP   = init_value;
	p->CnR   = init_value;
	p->CnA   = init_value;
	p->CnQ   = init_value;
	p->CXde  = init_value;
	p->CXdle = init_value;
	p->CZde  = init_value;
	p->CZdle = init_value;
	p->CYda  = init_value;
	p->CYdr  = init_value;
	p->Clda  = init_value;
	p->Cldr  = init_value;
	p->Cmde  = init_value;
	p->Cmdle = init_value;
	p->Cnda  = init_value;
	p->Cndr  = init_value;
	p->CXom  = init_value;
	p->CYom  = init_value;
	p->CZom  = init_value;
	p->Clom  = init_value;
	p->Cmom  = init_value;
	p->Cnom  = init_value;
}

/**
 * p: puntatore alla struttura dei dati da stampare
 * nomefile: file in cui scrivere i dati
 * stampa_dati: 0-> Stampa i titoli, 1->stampa i dati
 */
void AC_save_on_file(AC *p, char *nomefile, int stampa_dati)
{
	FILE *fw;

	fw = fopen(nomefile, "a"); // Apri file in "append"
	if (fw != NULL)
	{
		if (stampa_dati != 1) // Se devo stampare solo intestazione
		{
			fprintf(fw," Alpha   ;   CX      ;   CY      ;   CZ      ;   Cl      ;   Cm      ;   Cn      ;   CXA     ;   "
					    "CXAP    ;   CXU     ;   CXQ     ;   CXB     ;   CXP     ;   CXR     ;   CYB     ;   CYBP    ;   "
					    "CYP     ;   CYR     ;   CYA     ;   CYQ     ;   CZA     ;   CZAP    ;   CZU     ;   CZQ     ;   "
					    "CZB     ;   CZP     ;   CZR     ;   ClB     ;   ClBP    ;   ClP     ;   ClR     ;   ClA     ;   "
					    "ClQ     ;   CmA     ;   CmAP    ;   CmU     ;   CmQ     ;   CmB     ;   CmP     ;   CmR     ;   "
					    "CnB     ;   CnBP    ;   CnP     ;   CnR     ;   CnA     ;   CnQ     ;   CXde    ;   CXdle   ;   "
					    "CZde    ;   CZdle   ;   CYda    ;   CYdr    ;   Clda    ;   Cldr    ;   Cmde    ;   Cmdle   ;   "
					    "Cnda    ;   Cndr    ;   CXom    ;   CYom    ;   CZom    ;   Clom    ;   Cmom    ;   Cnom    ;   \n");
		}
		else // Se devo stampare i dati
		{
			fprintf(fw, "%10.6f; ",p->Alpha );
			fprintf(fw, "%10.6f; ",p->CX    );
			fprintf(fw, "%10.6f; ",p->CY    );
			fprintf(fw, "%10.6f; ",p->CZ    );
			fprintf(fw, "%10.6f; ",p->Cl    );
			fprintf(fw, "%10.6f; ",p->Cm    );
			fprintf(fw, "%10.6f; ",p->Cn    );
			fprintf(fw, "%10.6f; ",p->CXA   );
			fprintf(fw, "%10.6f; ",p->CXAP  );
			fprintf(fw, "%10.6f; ",p->CXU   );
			fprintf(fw, "%10.6f; ",p->CXQ   );
			fprintf(fw, "%10.6f; ",p->CXB   );
			fprintf(fw, "%10.6f; ",p->CXP   );
			fprintf(fw, "%10.6f; ",p->CXR   );
			fprintf(fw, "%10.6f; ",p->CYB   );
			fprintf(fw, "%10.6f; ",p->CYBP  );
			fprintf(fw, "%10.6f; ",p->CYP   );
			fprintf(fw, "%10.6f; ",p->CYR   );
			fprintf(fw, "%10.6f; ",p->CYA   );
			fprintf(fw, "%10.6f; ",p->CYQ   );
			fprintf(fw, "%10.6f; ",p->CZA   );
			fprintf(fw, "%10.6f; ",p->CZAP  );
			fprintf(fw, "%10.6f; ",p->CZU   );
			fprintf(fw, "%10.6f; ",p->CZQ   );
			fprintf(fw, "%10.6f; ",p->CZB   );
			fprintf(fw, "%10.6f; ",p->CZP   );
			fprintf(fw, "%10.6f; ",p->CZR   );
			fprintf(fw, "%10.6f; ",p->ClB   );
			fprintf(fw, "%10.6f; ",p->ClBP  );
			fprintf(fw, "%10.6f; ",p->ClP   );
			fprintf(fw, "%10.6f; ",p->ClR   );
			fprintf(fw, "%10.6f; ",p->ClA   );
			fprintf(fw, "%10.6f; ",p->ClQ   );
			fprintf(fw, "%10.6f; ",p->CmA   );
			fprintf(fw, "%10.6f; ",p->CmAP  );
			fprintf(fw, "%10.6f; ",p->CmU   );
			fprintf(fw, "%10.6f; ",p->CmQ   );
			fprintf(fw, "%10.6f; ",p->CmB   );
			fprintf(fw, "%10.6f; ",p->CmP   );
			fprintf(fw, "%10.6f; ",p->CmR   );
			fprintf(fw, "%10.6f; ",p->CnB   );
			fprintf(fw, "%10.6f; ",p->CnBP  );
			fprintf(fw, "%10.6f; ",p->CnP   );
			fprintf(fw, "%10.6f; ",p->CnR   );
			fprintf(fw, "%10.6f; ",p->CnA   );
			fprintf(fw, "%10.6f; ",p->CnQ   );
			fprintf(fw, "%10.6f; ",p->CXde  );
			fprintf(fw, "%10.6f; ",p->CXdle );
			fprintf(fw, "%10.6f; ",p->CZde  );
			fprintf(fw, "%10.6f; ",p->CZdle );
			fprintf(fw, "%10.6f; ",p->CYda  );
			fprintf(fw, "%10.6f; ",p->CYdr  );
			fprintf(fw, "%10.6f; ",p->Clda  );
			fprintf(fw, "%10.6f; ",p->Cldr  );
			fprintf(fw, "%10.6f; ",p->Cmde  );
			fprintf(fw, "%10.6f; ",p->Cmdle );
			fprintf(fw, "%10.6f; ",p->Cnda  );
			fprintf(fw, "%10.6f; ",p->Cndr  );
			fprintf(fw, "%10.6f; ",p->CXom  );
			fprintf(fw, "%10.6f; ",p->CYom  );
			fprintf(fw, "%10.6f; ",p->CZom  );
			fprintf(fw, "%10.6f; ",p->Clom  );
			fprintf(fw, "%10.6f; ",p->Cmom  );
			fprintf(fw, "%10.6f; ",p->Cnom  );
			fprintf(fw, "\n");
		}
		fflush(fw);
		fclose(fw);
	}
}

void PROP_Initialize(PROP *p)
{
	float init_value_float;
	int init_value_int;
	int i;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_float = 0.0;
		init_value_int = 0;
  	}
	else {
		init_value_float = NAN; // Not a number
		init_value_int = INT_MIN;
  	}

    p->Diametro      = init_value_float;
	p->DiametroOgiva = init_value_float;
	p->J             = init_value_float;
	p->ClA           = init_value_float;
	p->Cl0           = init_value_float;
	p->A0            = init_value_float;
	p->CdA2          = init_value_float;
	p->CdA           = init_value_float;
	p->Cd0           = init_value_float;
    p->NPale         = init_value_int;
    p->NumStazioni   = init_value_int;
    for (i = 0; i < 60; i++) {
      p->CSI[i] = init_value_float;
	  p->RD[i]  = init_value_float;
	  p->CH[i]  = init_value_float;
	  p->BA[i]  = init_value_float;
    }
}

void ENG_Initialize(ENG *p)
{
	double init_value_double;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_double = 0.0;
  	}
	else {
		init_value_double = NAN; // Not a number
  	}

    p->Potenza   = init_value_double;
    p->Esponente = init_value_double;
    p->RPMmin    = init_value_double;
    p->RPMmax    = init_value_double;
    p->Eta       = init_value_double;
    p->SFC       = init_value_double;
}

/*
void ENG_Initialize(ENG *p)
{
	double init_value_double;
	float init_value_float;
	int init_value_int;
	int i;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_double = 0.0;
		init_value_float = 0.0;
		init_value_int = 0;
  	}
	else {
		init_value_double = NAN; // Not a number
		init_value_float = NAN; // Not a number
		init_value_int = INT_MIN;
  	}

}
*/

void PLANE_Initialize(PLANE *p)
{
	double init_value_double;
	int init_value_int;
	int i,j;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_double = 0.0;
		init_value_int = 0;
  	}
	else {
		init_value_double = NAN; // Not a number
		init_value_int = INT_MIN;
  	}

    p->Mass          = init_value_double;
    p->WSpan         = init_value_double;
    p->WSurf         = init_value_double;
    p->Chord         = init_value_double;
    p->MDR           = init_value_double;
    p->JX            = init_value_double;
    p->JY            = init_value_double;
    p->JZ            = init_value_double;
    p->JXZ           = init_value_double;
    p->EmpMass       = init_value_double;
    p->CG[0]         = init_value_double;
    p->CG[1]         = init_value_double;
    p->CG[2]         = init_value_double;
    p->ElevatorMax   = init_value_double;
    p->ElevatorMin   = init_value_double;
    p->Ailerons      = init_value_double;
    p->Rudder        = init_value_double;
    p->FlapMin       = init_value_double;
    p->FlapMax       = init_value_double;
    p->FuelMass      = init_value_double;
    p->FuelFract     = init_value_double;
    p->VarFuel       = init_value_int;
    for (i = 0; i < 64; i++) {
    	for (j = 0; j < 126; j++) {
    		p->Der[i][j] = init_value_double;
    	}
    }
    PROP_Initialize(&p->Propeller);
}

/**
 * Stampa su file il contenuto della struttura dati PLANE
 * Parametri:
 * plane: puntatore alla struttura che contiene i dati
 * nomefile: puntatore alla stringa con il nome del file da usare per il salvataggio
 * parte: scelta della parte da stampare
 *  0-> Stampa solo la parte iniziale
 *  1-> Stampa solo la tabella dei coefficienti
 */
void PLANE_save_on_file(PLANE *plane, char *nomefile, int stampa_dati)
{
	FILE *fw;
	int i,j;

	fw = fopen(nomefile, "a");// Apri file in "append"
	if (fw != NULL)
	{
		if (stampa_dati == 0)
		{
			fprintf(fw, "Mass        = %f\n", plane->Mass       );
			fprintf(fw, "WSpan       = %f\n", plane->WSpan      );
			fprintf(fw, "WSurf       = %f\n", plane->WSurf      );
			fprintf(fw, "Chord       = %f\n", plane->Chord      );
			fprintf(fw, "MDR         = %f\n", plane->MDR        );
			fprintf(fw, "JX          = %f\n", plane->JX         );
			fprintf(fw, "JY          = %f\n", plane->JY         );
			fprintf(fw, "JZ          = %f\n", plane->JZ         );
			fprintf(fw, "JXZ         = %f\n", plane->JXZ        );
			fprintf(fw, "EmpMass     = %f\n", plane->EmpMass    );
			fprintf(fw, "CG[0]       = %f\n", plane->CG[0]      );
			fprintf(fw, "CG[1]       = %f\n", plane->CG[1]      );
			fprintf(fw, "CG[2]       = %f\n", plane->CG[2]      );
			fprintf(fw, "ElevatorMax = %f\n", plane->ElevatorMax);
			fprintf(fw, "ElevatorMin = %f\n", plane->ElevatorMin);
			fprintf(fw, "Ailerons    = %f\n", plane->Ailerons   );
			fprintf(fw, "Rudder      = %f\n", plane->Rudder     );
			fprintf(fw, "FlapMin     = %f\n", plane->FlapMin    );
			fprintf(fw, "FlapMax     = %f\n", plane->FlapMax    );
			fprintf(fw, "FuelMass    = %f\n", plane->FuelMass   );
			fprintf(fw, "FuelFract   = %f\n", plane->FuelFract  );
			fprintf(fw, "VarFuel     = %f\n", plane->VarFuel    );
		}

		if (stampa_dati == 1)
		{
			fprintf(fw,"   Alpha   ;   CX      ;   CY      ;   CZ      ;   Cl      ;   Cm      ;   Cn      ;   CXA     ;   "
						  "CXAP    ;   CXU     ;   CXQ     ;   CXB     ;   CXP     ;   CXR     ;   CYB     ;   CYBP    ;   "
						  "CYP     ;   CYR     ;   CYA     ;   CYQ     ;   CZA     ;   CZAP    ;   CZU     ;   CZQ     ;   "
						  "CZB     ;   CZP     ;   CZR     ;   ClB     ;   ClBP    ;   ClP     ;   ClR     ;   ClA     ;   "
						  "ClQ     ;   CmA     ;   CmAP    ;   CmU     ;   CmQ     ;   CmB     ;   CmP     ;   CmR     ;   "
						  "CnB     ;   CnBP    ;   CnP     ;   CnR     ;   CnA     ;   CnQ     ;   CXde    ;   CXdle   ;   "
						  "CZde    ;   CZdle   ;   CYda    ;   CYdr    ;   Clda    ;   Cldr    ;   Cmde    ;   Cmdle   ;   "
						  "Cnda    ;   Cndr    ;   CXom    ;   CYom    ;   CZom    ;   Clom    ;   Cmom    ;   Cnom    ;   \n");
			for (j = 0; j < 126; j++) {
				for (i = 0; i < 64; i++) {
					fprintf(fw, "%10.6f; ", plane->Der[i][j]);
				}
				fprintf(fw, "\n ");
			}
		}
        fflush(fw);
        fclose (fw);
	}
}

void STATE_Initialize(STATE *p)
{
	double init_value_double;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_double = 0.0;
  	}
	else {
		init_value_double = NAN; // Not a number
  	}

	p->V     = init_value_double;
	p->u     = init_value_double;
	p->v     = init_value_double;
	p->w     = init_value_double;
	p->p     = init_value_double;
	p->q     = init_value_double;
	p->r     = init_value_double;
	//NON_USATO p->Xv    = init_value_double;
	//NON_USATO p->Yv    = init_value_double;
	//NON_USATO p->Zv    = init_value_double;
	p->Phi   = init_value_double;
	p->Theta = init_value_double;
	p->Psi   = init_value_double;
	p->Gamma = init_value_double;
	p->h     = init_value_double;
	p->T     = init_value_double;
	p->x     = init_value_double;
	p->y     = init_value_double;
	p->z     = init_value_double;
}


void CMD_Initialize(CMD *p)
{
	float init_value_float;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_float = 0.0;
  	}
	else {
		init_value_float = NAN; // Not a number
  	}
	p->DeltaAil = init_value_float;
	p->DeltaEq  = init_value_float;
	p->DeltaRud = init_value_float;
	p->EngPow   = init_value_float;
}


void TEP_Initialize(TEP *p)
{
	double init_value_double;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_double = 0.0;
  	}
	else {
		init_value_double = NAN; // Not a number
  	}

	p->RPM       = init_value_double;
	p->Thrust    = init_value_double;
	p->Torque    = init_value_double;
	p->J         = init_value_double;
	p->q         = init_value_double;
	p->t         = init_value_double;
	p->Pmax      = init_value_double;
	p->P_al      = init_value_double;
	p->eff       = init_value_double;
}


void USERVAL_Initialize(USERVAL *p)
{
	double init_value_double;
	int init_value_int;

	if (INITIALIZE_MODE == INITIALIZE_WITH_ZERO){
		init_value_double = 0.0;
		init_value_int = 0;
  	}
	else {
		init_value_double = NAN; // Not a number
		init_value_int = INT_MIN;
  	}


	p->V     = init_value_double;
	p->h     = init_value_double;
	p->Gamma = init_value_double;
	strcpy(p->nomeFileSalvataggioDati, "");
	strcpy(p->titoloSim, "");
	p->simMassaVariabile = init_value_int;
	p->durataSimulazione = init_value_double;
	p->deltaT = init_value_double;
	p->LimitazioneDiPotenzaAttiva = init_value_int;
}

