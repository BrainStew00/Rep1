#ifndef SIMULATORE_ATMOSPHERE_H
#define SIMULATORE_ATMOSPHERE_H


	/* Dati per il calcolo dei parametri atmosferici */
typedef struct
{
	double temp_0;    // Temperatura a quota 0
	double press_0;   // Pressione a quota 0
	double vsuono_0;  // Velocita' del suono a quota 0
	double rho_0;     // Densita' dell'aria a quota 0
	double h_ref;     // Altitudine alla quale sono stati immessi i dati di riferimento
} ATMOSPAR;

extern ATMOSPAR AtmosPar;
void AtmosphereChoiceAndCalc(double h);


/**
 * Calcola la temperatura dell'aria alla quota specificata
 */
double Atmosphere_temperature(double h);

/**
 * Calcola la pressione dell'aria alla quota specificata
 * P(h) = P(h=0) * (T(h) / T(h=0))^5.2561
 */
double Atmosphere_press(double h);

/*
 * Calcola la densità dell'aria in base alla quota specificata
 * rho(h) = rho(h=0) * (T(h) / T(h=0))^4.2561
 */
double Atmosphere_Rho(double h);

/*
 * Calcola la velocita' del suono in base alla quota specificata
 * vsuono(h) = sqrt(gamma * R * T(h)) = vsuono(0) * sqrt( T(h) / T(h=0))
 */
double Atmosphere_vsuono(double h);

/**
 * Imposta i parametri atmosferici arbitrari
 */
void Atmosphere_imposta_parametri_modello_personalizzato(double h, double temp_h, double press_h, double rho_h, double vsuono_h);
/**
 * Imposta i parametri atmosferici secondo il modello ISA consentendo di scegliere pressione e temperatura
 */
void Atmosphere_imposta_parametri_modello_standard(double h, double temp_h, double press_h);

#endif //SIMULATORE_ATMOSPHERE_H
