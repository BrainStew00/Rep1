function Postprocessing_V7(varargin)
    
    %% Postprocessing V7.0
    % close all
    % clear all
    % clc
    p = inputParser;

    % Definisci il parametro atteso (stringa)
    addParameter(p, 'NomeFileDat', '', @ischar);

    % Analizza gli input
    parse(p, varargin{:});

    % Recupera la stringa
    NomeFileDat = p.Results.NomeFileDat;

    % Mostra la stringa
    disp(['La stringa passata è: ', NomeFileDat]);
    
    
    %% Apertura file contenente gli output della simulazione
    
    % Ottieni il percorso dello script corrente
    currentScriptPath = mfilename('fullpath');
    [currentScriptDir, ~, ~] = fileparts(currentScriptPath);

    if isempty(NomeFileDat)
         % Mostra la finestra di dialogo per selezionare il file .dat, aprendo la cartella dello script corrente
        [SYM_FILE, SYM_PATH] = uigetfile(fullfile(currentScriptDir, '*.dat'), 'Seleziona il file di dati');
    
        if isequal(SYM_FILE, 0)
            error('Nessun file selezionato.');
        else
            filePath = fullfile(SYM_PATH, SYM_FILE);
        end
    else
        % Costruisci il percorso completo del file direttamente
        filePath = fullfile(currentScriptDir, NomeFileDat);
    
        % Verifica se il file esiste
        if ~isfile(filePath)
            error(['Il file ', NomeFileDat, ' non esiste nel percorso ', currentScriptDir]);
        end
    end

    % Leggi i dati
    Dati = readtable(filePath);

     % Inizializza cella per figure e nomi
    Fig = {};  % Cella per le figure
    Name = {};  % Cella per i nomi delle figure
    
    %% Grafico della traiettodia 3D
    % Calcolo del massimo e minimo per ciascun asse
    min_x = min(Dati.x);
    max_x = max(Dati.x);
    min_y = min(Dati.y);
    max_y = max(Dati.y);
    min_h = min(Dati.h);
    max_h = max(Dati.h);
    
    
    % Calcolo dell'intervallo dei dati
    range_x = max_x - min_x;
    range_y = max_y - min_y;
    range_h = max_h - min_h;
    
    if(range_x < 1)
            range_x = 1;
        
    end
    if(range_y < 1)
            range_y = 1;
    end    
    if(range_h < 1)
       
            range_h = 1;
    end
    
    
    
    % Imposta l'offset come una frazione dell'intervallo dei dati
    offset_fraction = 0.10; % 5% dell'intervallo dei dati
    offset_x = range_x * offset_fraction;
    offset_y = range_y * offset_fraction;
    offset_h = range_h * offset_fraction;
   
    
    Fig{end+1} = figure;
    Name{end+1} = "Traiettoria_3D";
    hold on;
    
    % Tracciamento della traiettoria 3D
    Traiettoria = plot3(Dati.x, Dati.y, Dati.h, 'LineWidth', 2);
    
    % Proiezione sul piano XY (h=min(Dati.h) - offset_h)
    plot3(Dati.x, Dati.y, ((min_h+max_h)/2-range_h/2-offset_h) * ones(size(Dati.x)), 'r');
    
    % Proiezione sul piano XZ (y=min(Dati.y) - offset_y)
    plot3(Dati.x, ((min_y+max_y)/2+range_y/2+offset_y) * ones(size(Dati.y)), Dati.h, 'g');
    
    % Proiezione sul piano YZ (x=min(Dati.x) - offset_x)
    plot3(((min_x+max_x)/2-range_x/2-offset_x) * ones(size(Dati.x)), Dati.y, Dati.h, 'b');
    
    
    x_limits = [(min_x+max_x)/2-range_x/2-offset_x (min_x+max_x)/2+range_x/2+offset_x];
    y_limits = [(min_y+max_y)/2-range_y/2-offset_y (min_y+max_y)/2+range_y/2+offset_y];
    h_limits = [(min_h+max_h)/2-range_h/2-offset_h (min_h+max_h)/2+range_h/2+offset_h];
    
    % Imposta i limiti degli assi
    xlim(x_limits);
    ylim(y_limits);
    zlim(h_limits);
    
    
    xlabel("x_{NED} [m]");
    ylabel("y_{NED} [m]");
    zlabel("h(-z) [m]");
    grid on;
    legend('Traiettoria 3D', 'Proiezione XY', 'Proiezione XZ', 'Proiezione YZ');
    
    % Impostazione della vista di tre quarti
    view(45, 30);
    
    
    
    
    
    %% Grafico della traiettodia 2D - xy
    
    Fig{end+1} = figure;
    Name{end+1} = "Traiettoria_2D_xy";
    plot(Dati.x,Dati.y);
    
    xlim(x_limits);
    ylim(y_limits);
    
    xlabel("x_{NED} [m]");
    ylabel("y_{NED} [m]");
    grid on
    
    %% Grafico della traiettodia 2D - yz
    
    Fig{end+1} = figure;
    Name{end+1} = "Traiettoria_2D_yz";
    plot(Dati.y,Dati.h);
    
    
    xlabel("y_{NED} [m]");
    ylabel("h [m]");
    grid on
    
    %% Grafico della traiettodia 2D - xz
    
    Fig{end+1} = figure;
    Name{end+1} = "Traiettoria_2D_xz";
    plot(Dati.x,Dati.h);
    
    
    
    xlabel("x_{NED} [m]");
    ylabel("h [m]");
    grid on
    
    %% Grafico della quota
    
    Fig{end+1} = figure;
    Name{end+1} = "Quota";
    plot(Dati.t,Dati.h);
    xlabel("Time [s]");
    ylabel("h [m]");
    grid on
    
    %% Grafico di alpha, beta e gamma
    
    Fig{end+1} = figure;
    Name{end+1} = 'Alpha_Beta_Gamma';
    
    subplot(3,1,1)
    plot(Dati.t, Dati.alpha);
    ylabel('\alpha [deg]');
    xlabel('Time [s]');
    grid on;
    
    subplot(3,1,2)
    plot(Dati.t, Dati.beta);
    ylabel('\beta [deg]');
    xlabel('Time [s]');
    grid on;
    
    subplot(3,1,3)
    plot(Dati.t, Dati.Gamma);
    ylabel('\gamma [deg]');
    xlabel('Time [s]');
    grid on;
    
    %% Grafico di alpha e beta
    
    Fig{end+1} = figure;
    Name{end+1} = "Alpha_Beta";
    
    yyaxis left
    plot(Dati.t, Dati.alpha, 'b'); 
    ylabel('\alpha [deg]');
    xlabel('Time [s]');
    grid on;
    
    yyaxis right
    plot(Dati.t, Dati.beta, 'r');
    ylabel('\beta [deg]');
    xlabel('Time [s]');
    grid on;
    
    ax = gca;
    ax.YAxis(1).Color = 'b';
    ax.YAxis(2).Color = 'r';
    
    %% Grafico di theta, phi e psi
    
    Fig{end+1} = figure;
    Name{end+1} = "Theta_Phi_Psi";
    
    subplot(3,1,1)
    plot(Dati.t, Dati.Theta);
    ylabel("\theta [deg]");
    xlabel('Time [s]');
    grid on;
    
    subplot(3,1,2)
    plot(Dati.t, Dati.Phi);
    ylabel("\phi [deg]");
    xlabel('Time [s]');
    grid on;
    
    subplot(3,1,3)
    plot(Dati.t, Dati.Psi);
    ylabel("\psi [deg]");
    xlabel('Time [s]');
    grid on;
    
    %% Grafico di quota e spinta
    min_T = min(Dati.T);
    max_T = max(Dati.T);
    range_T = max_T - min_T;
    
    if(range_T < 0.1)
            range_T = 0.1;
    end
    T_limits = [(min_T+max_T)/2-range_T/2 (min_T+max_T)/2+range_T/2];



    Fig{end+1} = figure;
    Name{end+1} = "Quota_Spinta";
    
    subplot(2,1,1)
    plot(Dati.t, Dati.h);
    xlabel("Time [s]");
    ylabel("Altitude [m]");
    grid on;
    
    subplot(2,1,2)
    plot(Dati.t, Dati.T);
    
    ylim(T_limits);
    
    xlabel("Time [s]");
    ylabel("Thrust [N]");
    grid on;
    
    %% Grafico di spinta e potenza
    

    
    
    min_Power = min(Dati.Power);
    max_Power = max(Dati.Power);
    range_Power = max_Power - min_Power;
    
    if(range_Power < 0.1)
            range_Power = 0.1;
    end
    Power_limits = [(min_Power+max_Power)/2-range_Power/3 (min_Power+max_Power)/2+ 2*range_Power/3];





    Fig{end+1} = figure;
    Name{end+1} = "Spinta_Potenza";
    



    yyaxis left
    plot(Dati.t, Dati.T, 'b');
    
    ylim(T_limits);

    ylabel('Thrust [N]');
    xlabel('Time [s]');
    grid on;
    
    yyaxis right
    plot(Dati.t, Dati.Power, 'r');
    
    ylim(Power_limits);

    ylabel('Power [kW]');
    xlabel('Time [s]');
    grid on;
    
    ax = gca;
    ax.YAxis(1).Color = 'b';
    ax.YAxis(2).Color = 'r';
    
    % Grafico di potenza necessaria e disponibile
    Fig{end+1} = figure;
    Name{end+1} = "Potenza_Necessaria_Disponibile";
    
    plot(Dati.t, Dati.Power, 'b'); 
    hold on;
    plot(Dati.t, Dati.PowerMax, 'r');
    hold off;
    
    ylabel('Power [kW]');
    xlabel('Time [s]');
    grid on;
    
    legend('Requested Power', 'Available Power', 'Location', 'Best');
    
    % Impostazione colori degli assi
    ax = gca;
    ax.YColor = 'k'; % Colore degli assi Y impostato su nero
    ax.XColor = 'k'; % Colore degli assi X impostato su nero
    
    
    %% Grafico delle velocità angolari
    
    Fig{end+1} = figure;
    Name{end+1} = "Velocità_angolari";
    
    subplot(3,1,1)
    plot(Dati.t, Dati.p);
    ylabel("p [deg/s]");
    xlabel("Time [s]");
    grid on;
    
    subplot(3,1,2)
    plot(Dati.t, Dati.q);
    ylabel("q [deg/s]");
    xlabel("Time [s]");
    grid on;
    
    subplot(3,1,3)
    plot(Dati.t, Dati.r);
    ylabel("r [deg/s]");
    xlabel("Time [s]");
    grid on;
    
    %% Grafico delle velocità lineari
    
    Fig{end+1} = figure;
    Name{end+1} = "Velocità_lineari";
    
    subplot(4,1,1)
    plot(Dati.t, Dati.u);
    ylabel("u [m/s]");
    xlabel("Time [s]");
    grid on;
    
    subplot(4,1,2)
    plot(Dati.t, Dati.v);
    ylabel("v [m/s]");
    xlabel("Time [s]");
    grid on;
    
    subplot(4,1,3)
    plot(Dati.t, Dati.w);
    ylabel("w [m/s]");
    xlabel("Time [s]");
    grid on;
    
    subplot(4,1,4)
    plot(Dati.t, Dati.V);
    xlabel("Time [s]");
    ylabel("V_{tot} [m/s]");
    grid on;
    
    %% Grafico dei comandi
    
    Fig{end+1} = figure;
    Name{end+1} = "Comandi";
    
    subplot(4,1,1)
    plot(Dati.t, Dati.Aileron);
    xlabel("Time [s]");
    ylabel("\delta_a [deg]");
    grid on;
    
    subplot(4,1,2)
    plot(Dati.t, Dati.Elevator);
    xlabel("Time [s]");
    ylabel("\delta_e [deg]");
    grid on;
    
    subplot(4,1,3)
    plot(Dati.t, Dati.Rudder);
    ylabel("\delta_r [deg]");
    xlabel("Time [s]");
    grid on;
    
    subplot(4,1,4)
    plot(Dati.t, Dati.Throttle);
    ylabel("Throttle [%]");
    xlabel("Time [s]");
    grid on;
    
    %% Grafico delle masse
    
    Fig{end+1} = figure;
    Name{end+1} = "Masse";
    
    yyaxis left
    plot(Dati.t, Dati.Mass, 'b'); 
    ylabel('Aircraft mass [kg]');
    xlabel('Time [s]');
    grid on;
    
    yyaxis right
    plot(Dati.t, Dati.FuelMass, 'r');
    ylabel('Fuel mass [kg]');
    xlabel('Time [s]');
    grid on;
    
    ax = gca;
    ax.YAxis(1).Color = 'b';
    ax.YAxis(2).Color = 'r';
    
    %% Prompt per il salvataggio delle immagini
    
    savePrompt = input('Vuoi salvare le immagini in una cartella dedicata? [y/n]: ', 's');
    
    if strcmpi(savePrompt, 'y')
        % Crea una cartella con il nome "Immagini_" seguito dal nome del file
        folderName = "Immagini_" + SYM_FILE;
        if ~exist(folderName, 'dir')
            mkdir(folderName);
        end
    
        % Salva ciascuna figura nella cartella
        for i = 1:length(Fig)
            saveas(Fig{i}, fullfile(folderName, Name{i} + ".jpg"));
        end
        disp('Le immagini sono state salvate con successo.');
    else
        disp('Le immagini non sono state salvate.');
    end