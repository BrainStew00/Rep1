#ifndef structdati_h
#define structdati_h
#include <stdbool.h>
#include <math.h>

#define INCLUDI_MANOVRE_DI_TEST                0

typedef struct
{
    double Alpha, CX, CY, CZ, Cl, Cm, Cn;
    double CXA, CXAP, CXU, CXQ, CXB, CXP, CXR;
    double CYB, CYBP, CYP, CYR, CYA, CYQ;
    double CZA, CZAP, CZU, CZQ, CZB, CZP, CZR;
    double ClB, ClBP, ClP, ClR, ClA, ClQ;
    double CmA, CmAP, CmU, CmQ, CmB, CmP, CmR;
    double CnB, CnBP, CnP, CnR, CnA, CnQ;
    double CXde, CXdle, CZde, CZdle, CYda, CYdr;
    double Clda, Cldr, Cmde, Cmdle, Cnda, Cndr;
    double CXom, CYom, CZom, Clom, Cmom, Cnom;
}AC;


typedef struct
{
    double Diametro, DiametroOgiva, J, ClA, Cl0, A0, CdA2, CdA, Cd0;
    int NPale, NumStazioni;
    double CSI[60], RD[60], CH[60], BA[60];
}PROP;

typedef struct
{
    double Potenza;
    double Esponente;
    double RPMmin;
    double RPMmax;
    double Eta;
    double SFC;
}ENG;



typedef struct
{
    double Mass, WSpan, WSurf, Chord, MDR, JX, JY, JZ, JXZ, EmpMass;
    double CG[3];
    double ElevatorMax, ElevatorMin, Ailerons, Rudder, FlapMin, FlapMax;
    double FuelMass, FuelFract;
    bool VarFuel;
    double Der[64][126];
    PROP Propeller;
} PLANE;

typedef struct
{
    double V;     // [m/s]
    double u;     // [m/s]
    double v;     // [m/s]
    double w;     // [m/s]
    double p;     // [rad/s]
    double q;     // [rad/s]
    double r;     // [rad/s]
    double Phi;   // [rad]
    double Theta; // [rad]
    double Psi;   // [rad]
    double Gamma; // [rad]
    double h;     // [m]
    double T;     // [N]
    double x;     // [m]
    double y;     // [m]
    double z;     // [m]
}STATE;

typedef struct
{
    float DeltaAil, DeltaEq, DeltaRud, EngPow;
}CMD;





typedef struct {
    double RPM, Thrust, Torque, J, q, t, Pmax, P_al, eff;
}TEP;



#define MAX_LUNGHEZZA_STRINGA		200
typedef struct {
	double V;
	double h;
	double Gamma;
	char   nomeFileSalvataggioDati[MAX_LUNGHEZZA_STRINGA];
	char   titoloSim[MAX_LUNGHEZZA_STRINGA];
	bool   simMassaVariabile;
	double durataSimulazione;
	double deltaT;
	bool   LimitazioneDiPotenzaAttiva;
}USERVAL;

#endif




#define PI	3.14159265358979323846
#define DEG_TO_RAD(DEG) ((DEG) * PI / 180.0)  /* Conversione da gradi sessagesimali a radianti */
#define RAD_TO_DEG(RAD) ((RAD) * 180.0 / PI) /* Conversione da radianti a gradi sessagesimali */
#define KELVIN_TO_CENT(K)  (K - 273.15)      /* Conversione da gradi centigradi a Kelvin */
#define CENT_TO_KELVIN(C)  (C + 273.15)      /* Conversione da gradi Kelvin a centigradi */

void AC_Initialize(AC *p);
void PROP_Initialize(PROP *p);
void ENG_Initialize(ENG *p);
void PLANE_Initialize(PLANE *p);
void STATE_Initialize(STATE *p);
void CMD_Initialize(CMD *p);

void TEP_Initialize(TEP *p);

void USERVAL_Initialize(USERVAL *p);

/**
 * Stampa su file il contenuto della struttura dati PLANE
 * Parametri:
 * plane: puntatore alla struttura che contiene i dati
 * nomefile: puntatore alla stringa con il nome del file da usare per il salvataggio
 * stampa_dati: scelta della parte da stampare
 *  0-> Stampa solo la parte iniziale
 *  1-> Stampa solo la tabella dei coefficienti
 */
void PLANE_save_on_file(PLANE *plane, char *nomefile, int stampa_dati);

/**
 * p: puntatore alla struttura dei dati da stampare
 * nomefile: file in cui scrivere i dati
 * stampa_dati: 0-> Stampa i titoli, 1->stampa i dati
 */
void AC_save_on_file(AC *p, char *nomefile, int stampa_dati);
