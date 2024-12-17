#ifndef INTEGRAZIONE_H_INCLUDED
#define INTEGRAZIONE_H_INCLUDED

void integrazione(double dt, STATE stati_deg, PLANE velivolo, CMD comandi_trim, TEP tep, ENG eng, PROP *ppropeller, bool LimitaPotenza);

#endif // INTEGRAZIONE_H_INCLUDED
