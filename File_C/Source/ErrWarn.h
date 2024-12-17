#ifndef ERRWARN_H_INCLUDED
#define ERRWARN_H_INCLUDED

#define E001   1
#define E002   2
#define E003   3
#define E004   4
#define E005   5
#define E006   6
#define E007   7
#define E008   8
#define E009   9

#define E010   10
#define E011   11
#define E012   12
#define E013   13
#define E014   14
#define E015   15
#define E016   16
#define E017   17
#define E018   18
#define E019   19

#define E020   20
#define E021   21
#define E022   22
#define E023   23
#define E024   24
#define E025   25
#define E026   26
#define E027   27
#define E028   28
#define E029   29

#define E030   30
#define E031   31
#define E032   32
#define E033   33
#define E034   34
#define E035   35
#define E036   36
#define E037   37
#define E038   38
#define E039   39

#define W001    1
#define W002    2
#define W003    3
#define W004    4
#define W005    5
#define W006    6
#define W007    7
#define W008    8
#define W009    9

#define W010   10
#define W011   11
#define W012   12
#define W013   13
#define W014   14
#define W015   15
#define W016   16
#define W017   17
#define W018   18
#define W019   19

#define W020   20
#define W021   21
#define W022   22
#define W023   23
#define W024   24
#define W025   25
#define W026   26
#define W027   27
#define W028   28
#define W029   29

#define W030   30
#define W031   31
#define W032   32
#define W033   33
#define W034   34
#define W035   35
#define W036   36
#define W037   37
#define W038   38
#define W039   39
 void ErrWarn_Initialize(void);

 void ErrWarn_DisplayErrMsgAndExit(int ErrNum, char *msg);

 void ErrWarn_DisplayWarnMsgAndContinue(int WarnNum, char *msg);

#endif // ERRWARN_H_INCLUDED
