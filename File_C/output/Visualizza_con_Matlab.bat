@rem *************************************************************************************
@rem * File per eseguire il programma di visualizzzazione matlab
@rem *************************************************************************************
@rem visualizza la cartella corrente
@cd

cd %~dp0

matlab nodisplay -r "Postprocessing_V7('NomeFileDat', '%~nx1')"


pause

