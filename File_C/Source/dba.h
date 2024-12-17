#ifndef DBA_H_INCLUDED
#define DBA_H_INCLUDED
#include "dati.h"

#define DBA_NUM_DER_SAMPLE		126  // Numero di righe nella matrice Der[][] della struct PLANE
#define DBA_NUM_DER_PAR          64  // Numero di parametri (colonne) matrice Der[][] della struct PLANE
// Indici di riga dei parametri nella matrice Der[][] della struct PLANE

#define DBA_ALPHA_IX     0      // Alpha
#define DBA_CX_IX        1      // CX
#define DBA_CY_IX        2      // CY
#define DBA_CZ_IX        3      // CZ
#define DBA_CL_IX        4      // Cl
#define DBA_CM_IX        5      // Cm
#define DBA_CN_IX        6      // Cn
#define DBA_CXA_IX       7      // CXA
#define DBA_CXAP_IX      8      // CXAP
#define DBA_CXU_IX       9      // CXU
#define DBA_CXQ_IX       10     // CXQ
#define DBA_CXB_IX       11     // CXB
#define DBA_CXP_IX       12     // CXP
#define DBA_CXR_IX       13     // CXR
#define DBA_CYB_IX       14     // CYB
#define DBA_CYBP_IX      15     // CYBP
#define DBA_CYP_IX       16     // CYP
#define DBA_CYR_IX       17     // CYR
#define DBA_CYA_IX       18     // CYA
#define DBA_CYQ_IX       19     // CYQ
#define DBA_CZA_IX       20     // CZA
#define DBA_CZAP_IX      21     // CZAP
#define DBA_CZU_IX       22     // CZU
#define DBA_CZQ_IX       23     // CZQ
#define DBA_CZB_IX       24     // CZB
#define DBA_CZP_IX       25     // CZP
#define DBA_CZR_IX       26     // CZR
#define DBA_CLB_IX       27     // ClB
#define DBA_CLBP_IX      28     // ClBP
#define DBA_CLP_IX       29     // ClP
#define DBA_CLR_IX       30     // ClR
#define DBA_CLA_IX       31     // ClA
#define DBA_CLQ_IX       32     // ClQ
#define DBA_CMA_IX       33     // CmA
#define DBA_CMAP_IX      34     // CmAP
#define DBA_CMU_IX       35     // CmU
#define DBA_CMQ_IX       36     // CmQ
#define DBA_CMB_IX       37     // CmB
#define DBA_CMP_IX       38     // CmP
#define DBA_CMR_IX       39     // CmR
#define DBA_CNB_IX       40     // CnB
#define DBA_CNBP_IX      41     // CnBP
#define DBA_CNP_IX       42     // CnP
#define DBA_CNR_IX       43     // CnR
#define DBA_CNA_IX       44     // CnA
#define DBA_CNQ_IX       45     // CnQ
#define DBA_CXDE_IX      46     // CXde
#define DBA_CXDLE_IX     47     // CXdle
#define DBA_CZDE_IX      48     // CZde
#define DBA_CZDLE_IX     49     // CZdle
#define DBA_CYDA_IX      50     // CYda
#define DBA_CYDR_IX      51     // CYdr
#define DBA_CLDA_IX      52     // Clda
#define DBA_CLDR_IX      53     // Cldr
#define DBA_CMDE_IX      54     // Cmde
#define DBA_CMDLE_IX     55     // Cmdle
#define DBA_CNDA_IX      56     // Cnda
#define DBA_CNDR_IX      57     // Cndr
#define DBA_CXOM_IX      58     // CXom
#define DBA_CYOM_IX      59     // CYom
#define DBA_CZOM_IX      60     // CZom
#define DBA_CLOM_IX      61     // Clom
#define DBA_CMOM_IX      62     // Cmom
#define DBA_CNOM_IX      63     // Cnom





void dba_dati();
PLANE dba_variabili();

#endif // DBA_H_INCLUDED
