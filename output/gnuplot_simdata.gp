# Gnuplot command file
# Questo e' un commento
# t, Aileron, Elevator, Rudder, Throttle, u, v, w, p, q, r, Phi, Theta, Psi, h, x, y
# viene specificato da linea di comando con l'opzione:
#    -e "datafile = 'output/gnuplot_simdata.dat'"
# altrimenti si toglie il commmento nella linea sottostante
#   datafile = 'output/gnuplot_simdata.dat'
#   datafile = 'D:/Simulatore_volo/Cessna/Cessna_v23_19/output/Impulso_alettone_20x0.05_rampa_manetta_80%x10_1000m_52ms_0.0deg_mFix_200s_000.dat'

x_size = 600 # dimensione orizzontale finestra in pixel
y_size = 600 # dimensione orizzontale finestra in pixel

          t_col = 1
       Mass_col = 2
   FuelMass_col = 3
    Aileron_col = 4
   Elevator_col = 5
     Rudder_col = 6
   Throttle_col = 7
      alpha_col = 8
       beta_col = 9
          V_col = 10
          u_col = 11
          v_col = 12
          w_col = 13
          p_col = 14
          q_col = 15
          r_col = 16
        Phi_col = 17
      Theta_col = 18
        Psi_col = 19
      Gamma_col = 20
          h_col = 21
     Thrust_col = 22
          x_col = 23
          y_col = 24
          z_col = 25       
      Power_col = 26
   PowerMax_col = 27                                                                    
                   
# COMANDI
reset                                                                      
set terminal wxt 1 size x_size,y_size # Dimensione della finestra
#set output 'temp.png'
set multiplot layout 4,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:Aileron_col  with lines linewidth 2 title "Aileron[°]" 
plot datafile using t_col:Elevator_col with lines linewidth 2 title "Elevator[°]"
plot datafile using t_col:Rudder_col   with lines linewidth 2 title "Rudder[°]"
plot datafile using t_col:Throttle_col with lines linewidth 2 title "Throttle[%]"
unset multiplot           
set output 
replot
#pause -1 

#VELOCITA' LINEARI
reset
set terminal wxt 2 size x_size,y_size # Dimensione della finestra
set multiplot layout 4,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:V_col         with lines linewidth 2 title "V [m/s]"        
plot datafile using t_col:u_col         with lines linewidth 2 title "u [m/s]"        
plot datafile using t_col:v_col         with lines linewidth 2 title "v [m/s]"        
plot datafile using t_col:w_col         with lines linewidth 2 title "w [m/s]"        
unset multiplot           
#pause -1            
                                                                      
#VELOCITA' ANGOLARI
reset
set terminal wxt 3 size x_size,y_size # Dimensione della finestra
set multiplot layout 4,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:p_col         with lines linewidth 2 title "p [°/s]"        
plot datafile using t_col:q_col         with lines linewidth 2 title "q [°/s]"        
plot datafile using t_col:r_col         with lines linewidth 2 title "r [°/s]"        
unset multiplot           
#pause -1            
                                                                      
                  
#QUOTA, SPINTA  
reset
set terminal wxt 4 size x_size,y_size # Dimensione della finestra
set multiplot layout 2,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:h_col         with lines linewidth 2 title "h [m]"        
plot datafile using t_col:Thrust_col    with lines linewidth 2 title "Thrust [N]"        
unset multiplot           
#pause -1            

# ANGOLI EULERO  
reset
set terminal wxt 5 size x_size,y_size # Dimensione della finestra
set multiplot layout 3,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:Phi_col       with lines linewidth 2 title "Phi [°]"      
plot datafile using t_col:Theta_col     with lines linewidth 2 title "Theta [°]"     
plot datafile using t_col:Psi_col       with lines linewidth 2 title "Psi [°]"      
unset multiplot           
#pause -1            

#ANGOLI AERODINAMICI
reset
set terminal wxt 6 size x_size,y_size # Dimensione della finestra
set multiplot layout 3,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:alpha_col     with lines linewidth 2 title "alpha [°]"    
plot datafile using t_col:beta_col      with lines linewidth 2 title "beta [°]"     
plot datafile using t_col:Gamma_col     with lines linewidth 2 title "Gamma [°]"    
unset multiplot           

#POTENZA
reset
set terminal wxt 7 size x_size,y_size # Dimensione della finestra
#set multiplot layout 3,1       # 4 grafici su una colonna 
set lmargin 10                 # margine sinistro comune per allineare correttamente i grafici
set rmargin 10                 # margine destro comune per allineare correttamente i grafici
plot datafile using t_col:Power_col    with lines linewidth 2 title "Requested Power [kW]", \
     datafile using t_col:PowerMax_col with lines linewidth 2 title "Available Power [kW]"     
#unset multiplot           


# TRAIETTORIA
reset                                                                      
set terminal wxt 8 size x_size,y_size # Dimensione della finestra
set parametric
set xyplane relative 0.2
set grid
set view 60,320
set xlabel "x [m]"
set ylabel "y [m]"
set zlabel "h [m]"
# Abilita le griglie su tutti i piani
set grid xtics ytics ztics
set grid mxtics mytics mztics
set ticslevel 0
splot datafile using x_col:y_col:h_col with lines linewidth 2 title "Trajectory" 
pause -1            
                                                       


#splot datafile using x_col:y_col:h_col with lines linewidth 2 title "Trajectory" \
#      datafile using x_col:y_col:(0*h_col) with lines linewidth 1 title "xy projection" \ 
#      datafile using x_col:(0*y_col):h_col with lines linewidth 1 title "xh projection" \ 
#      datafile using (0*x_col):y_col:h_col with lines linewidth 1 title "yh projection" 

#splot datafile using x_col:y_col:h_col with lines linewidth 2 title "Trajectory", 
#      datafile using x_col:y_col:(0*h_col) with lines linewidth 1 title "xy projection", 
#      datafile using x_col:(2500+0*y_col):h_col with lines linewidth 1 title "xh projection", 
#      datafile using (12000+0*x_col):y_col:h_col with lines linewidth 1 title "yh projection" 
       
#pause -1 



#linee di comando commentate per altri grafici         
#plot datafile using t_col:Mass_col      with lines linewidth 2 title "Mass [kg]"     
#plot datafile using t_col:FuelMass_col  with lines linewidth 2 title "FuelMass [kg]" 
#plot datafile using t_col:x_col         with lines linewidth 2 title "x [m]"        
#plot datafile using t_col:y_col         with lines linewidth 2 title "y [m]"        
#plot datafile using t_col:z_col         with lines linewidth 2 title "z [m]"          
