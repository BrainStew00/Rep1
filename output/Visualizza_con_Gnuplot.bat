@echo off
rem *************************************************************************************
rem * File di comandi per windows cmd window, richiamato a fine esecuzione di Cessna.exe
rem *************************************************************************************
@echo %~nx0 %1

rem visualizza la cartella corrente
cd
cd %~dp0
cd
echo %0

rem
rem     Simulatore          
rem     ¦                   
rem     +---File_C          
rem     ¦   +---bin         
rem     ¦   ¦   +---Debug   
rem     ¦   ¦   +---Release 
rem     ¦   +---input       
rem     ¦   +---obj         
rem     ¦   ¦   +---Debug   
rem     ¦   ¦   +---Release 
rem     ¦   +---output      
rem     ¦   +---Source      
rem     +---Gnuplot         
rem     ¦   +---bin         
rem     +---input           
rem     +---output          
rem
     
if exist "..\gnuplot\bin\gnuplot.exe" (
    ..\gnuplot\bin\gnuplot.exe -e "datafile = '%~n1.dat'" .\gnuplot_simdata.gp
) else (
    if exist "..\..\gnuplot\bin\gnuplot.exe" (
        ..\..\gnuplot\bin\gnuplot.exe -e "datafile = '%~n1.dat'" .\gnuplot_simdata.gp
    ) else (
        if exist "..\..\..\gnuplot\bin\gnuplot.exe" (
            ..\..\..\gnuplot\bin\gnuplot.exe -e "datafile = '%~n1.dat'" .\gnuplot_simdata.gp
        ) else (
            echo Gnuplot non trovato 
        )
    )
) 


pause
