#include "stdio.h"
#include "Manovre.h"
#include "ManovreTest.h"


void Manovre_debug_Immetti_Comandi_debug_2(void){
    int i;

	/*********************/
	/*       DEBUG       */
	/*********************/

	/***
	 * Debug: Inserisci sequenza di manovre di esempio:
	 *
	 *
	 * Esempio: Impulso aileron di durata 0.1s
     *
     *
     *
     *
     *
	 *11 _                ___________
	 *                   |           |
	 *                   |           |
	 * 0 _  _____________|           |________________   AILERON
	 *
	 *      |            |           |
	 *      0           29.9        30.1             100
     *
	 */


	{
		i = 0;

		sequenza_manovre[i].istante_inizio = 29.9;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = IMPULSO;
		sequenza_manovre[i].param_manovra.impulso.ampiezza = 11;
		sequenza_manovre[i].param_manovra.impulso.deltaT = 0.2;

		i++;
		sequenza_manovre[i].istante_inizio = 100;
		sequenza_manovre[i].manovra = FINE_MANOVRE;
	}

	Manovre_Converti_sequenza_manovre_in_sequenze_comandi(0.1); // Converti la sequenza di manovre in sequenza di comandi di durata  0.1s
	Manovre_Print_All_Command(stdout); // Stampa a video i comandi convertiti
   // /***
    //  * Debug: Inserisci una sequenza di comandi di esempio
    //  *
    //  * 5 _
    //  *                       |----|
    //  *                       |    |
    //  * 0 _  _________________|    |________________   DeltaAil
    //  *
    //  *      |                |    |               |
    //  *      0               30    30.1             100
    //  *
    //  */
    // {
    // 	i = 0;
    // 	ManovraSpezzata_DeltaAil[i].time = 0;
    // 	ManovraSpezzata_DeltaAil[i].value = 0;
    //
    // 	i++;
    // 	ManovraSpezzata_DeltaAil[i].time = 30; // 30 s
    // 	ManovraSpezzata_DeltaAil[i].value = 0;
    //
    // 	i++;
    // 	ManovraSpezzata_DeltaAil[i].time = 30; // 30 s
    // 	ManovraSpezzata_DeltaAil[i].value = 11;
    //
    // 	i++;
    // 	ManovraSpezzata_DeltaAil[i].time = 30.1; // 30.1 s
    // 	ManovraSpezzata_DeltaAil[i].value = 11;
    //
    // 	i++;
    // 	ManovraSpezzata_DeltaAil[i].time = 30.1; // 30.1 s
    // 	ManovraSpezzata_DeltaAil[i].value = 0;
    //
    //
    // 	i++;
    // 	ManovraSpezzata_DeltaAil[i].time = 100; // 100 s
    // 	ManovraSpezzata_DeltaAil[i].value = 0;
    //
    // 	i++;
    // 	ManovraSpezzata_DeltaAil[i].time =  0; // Fine comando: l'istante di tempo e' antecedente all'ultimo punto definito
    // 	ManovraSpezzata_DeltaAil[i].value = 0;
    // }
    // ManovraSpezzata_DeltaEq[0].time   = 0;
    // ManovraSpezzata_DeltaEq[0].value  = 0;
    // ManovraSpezzata_DeltaRud[0].time  = 0;
    // ManovraSpezzata_DeltaRud[0].value = 0;
    // ManovraSpezzata_EngPow[0].time    = 0;
    // ManovraSpezzata_EngPow[0].value   = 0;
}







void Manovre_debug_Immetti_Comandi_debug(void){
    int i;

	/*********************/
	/*       DEBUG       */
	/*********************/

	/***
	 * Debug: Inserisci sequenza di manovre di esempio:
	 *
	 *
	 * Esempio: rampa+gradino seguita da impulso di durata 0.1s
     *
     *       ^
     * value |
     *       |
     *       |
     *    21 |                  X*X
     *       |                  | |
     *       |                  | |                       AILERON
     *       |                  | |
     *       |                  | |
     *    10 |            X*****X X********************
     *       |          *
     *       |        *
     *       |      *
     *       *****X---------------------------------------->
     *           10      15    18                      time
     *                           18.01
     *
     *
     *
     *
	 *10 _                  _______
	 *                     /       \
	 *                    /         \
	 * 0 _  _____________/           \________________   EngPow
	 *
	 *      |            |  |      |  |
	 *      0            5 10     25 30              70
     *
	 */


	{
		i = 0;
		sequenza_manovre[i].istante_inizio = 5;
		sequenza_manovre[i].tipocomando = THROTTLE;
		sequenza_manovre[i].manovra = TRAPEZIO;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_salita = 10;
		sequenza_manovre[i].param_manovra.trapezio.tempo_salita = 5;
		sequenza_manovre[i].param_manovra.trapezio.tempo_regime = 15;
		sequenza_manovre[i].param_manovra.trapezio.tempo_discesa = 5;
		sequenza_manovre[i].param_manovra.trapezio.ampiezza_discesa = -10;

		i++;
		sequenza_manovre[i].istante_inizio = 5;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = RAMPA_GRADINO;
		sequenza_manovre[i].param_manovra.rampa_gradino.ampiezza = 10;
		sequenza_manovre[i].param_manovra.rampa_gradino.durata = 5;

		i++;
		sequenza_manovre[i].istante_inizio = 18;
		sequenza_manovre[i].tipocomando = AILERON;
		sequenza_manovre[i].manovra = IMPULSO;
		sequenza_manovre[i].param_manovra.impulso.ampiezza = 11;
		sequenza_manovre[i].param_manovra.impulso.deltaT = 0.1;

		i++;
		sequenza_manovre[i].istante_inizio = 20;
		sequenza_manovre[i].tipocomando = ELEVATOR;
		sequenza_manovre[i].manovra = RAMPA;
		sequenza_manovre[i].param_manovra.rampa.ampiezza = 11;
		sequenza_manovre[i].param_manovra.rampa.durata = 0.1;

		i++;
		sequenza_manovre[i].istante_inizio = 70;
		sequenza_manovre[i].manovra = FINE_MANOVRE;
	}


	/***
	 * Debug: Inserisci una sequenza di comandi di esempio
	 *
	 * 5 _
	 *                         /\
	 *                        /  \
	 * 0 _  _________________/    \________________   DeltaAil
	 *
	 *      |                |  |  |               |
	 *      0               30 35 40              70
	 *
	 */
	{
		i = 0;
		ManovraSpezzata_DeltaAil[i].time = 0;
		ManovraSpezzata_DeltaAil[i].value = 0;

		i++;
		ManovraSpezzata_DeltaAil[i].time = 30; // 30 s
		ManovraSpezzata_DeltaAil[i].value = 0;

		i++;
		ManovraSpezzata_DeltaAil[i].time = 35; // 35 s
		ManovraSpezzata_DeltaAil[i].value = 5;

		i++;
		ManovraSpezzata_DeltaAil[i].time = 40; // 35 s
		ManovraSpezzata_DeltaAil[i].value = 0;

		i++;
		ManovraSpezzata_DeltaAil[i].time = 70; // 70 s
		ManovraSpezzata_DeltaAil[i].value = 0;

		i++;
		ManovraSpezzata_DeltaAil[i].time =  0; // Fine comando: l'istante di tempo e' antecedente all'ultimo punto definito
		ManovraSpezzata_DeltaAil[i].value = 0;
	}

	/***
	 * Debug: Inserisci una sequenza di comandi di esempio
	 *
	 *10 _                  _______
	 *                     /       \
	 *                    /         \
	 * 0 _  _____________/           \________________   EngPow
	 *
	 *      |            |  |      |  |
	 *      0           25 30     40 45              70
	 *
	 */
	{
		i = 0;
		ManovraSpezzata_EngPow[i].time  =  0;
		ManovraSpezzata_EngPow[i].value =  0;

		i++;
		ManovraSpezzata_EngPow[i].time  = 25; // 30 s
		ManovraSpezzata_EngPow[i].value =  0;

		i++;
		ManovraSpezzata_EngPow[i].time  = 30; // 35 s
		ManovraSpezzata_EngPow[i].value = 10;

		i++;
		ManovraSpezzata_EngPow[i].time  = 40; // 35 s
		ManovraSpezzata_EngPow[i].value = 10;

		i++;
		ManovraSpezzata_EngPow[i].time  = 45; // 35 s
		ManovraSpezzata_EngPow[i].value =  0;

		i++;
		ManovraSpezzata_EngPow[i].time  = 70; // 70 s
		ManovraSpezzata_EngPow[i].value =  0;

		i++;
		ManovraSpezzata_EngPow[i].time  = 0; // Fine comando: l'istante di tempo e' antecedente all'ultimo punto definito
		ManovraSpezzata_EngPow[i].value = 0;
	}

	ManovraSpezzata_DeltaEq[0].time   = 0;
	ManovraSpezzata_DeltaEq[0].value  = 0;
	ManovraSpezzata_DeltaRud[0].time  = 0;
	ManovraSpezzata_DeltaRud[0].value = 0;
	ManovraSpezzata_EngPow[0].time    = 0;
	ManovraSpezzata_EngPow[0].value   = 0;
}



