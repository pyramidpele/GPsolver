//ihd/dt(x_a)=((-1/2m)(hbar^2)(Del^2) + g_a(x_a)^2 +g_ab(x_b)^2)x_a + omega(x_b)
//ihd/dt(x_b)=((-1/2m)(hbar^2)(Del^2) + g_b(x_b)^2 +g_ab(x_a)^2)x_b + omega^*(x_a)
//step time dt?
//generating psi_init?

#include "gpsolver.h"
#include <iostream>

int main(){

//declaring objects to store BEC parameters and functions
BEC_parameters parameter;
GPsolver GPsolver;

    //input discretisation parameters
    std::cout<<"Input gridsize:"; 
    std::cin>>parameter.gridsize;
        if (parameter.gridsize%2!=0){
            std::cout<<"Please enter an even grid size N"<<std::endl;
            exit(1);                    
        }
    std::cout<<"Input grid spacing:";
    std::cin>>parameter.dx;
    std::cout<<"Input runtime:";
    std::cin>>parameter.runtime;
    //std::cout<<"Input timestep:"; set to 1 atm
    //std::cin>>parameter.dt;

    //input BEC parameters
    std::cout<<"Input initial density of condensate n_0:"; 
    std::cin>>parameter.n_0;
    std::cout<<"Input intraspecies (a on a, b on b) contact interaction g:"; 
    std::cin>>parameter.g;
    std::cout<<"Input interspecies (a on b, b on a) contact interaction g_ab:"; 
    std::cin>>parameter.g_ab;
    std::cout<<"Input coherent coupling:"; 
    std::cin>>parameter.omega;

    //initial size of wavefunction (has to accomodate both a and b so 2*N)
    //generating initial wavefunction of condensate
    double psi_init[parameter.N];
    double psi[parameter.N];

    for (int i=0; i<parameter.N; i++){
        psi[i]=psi_init[i];
    }

    //evaluating psi in time increments using RK4 using initial condensate wavefunction psi_init
    //step time set to 1
    for(int t=0; t<parameter.runtime; t++ ){
        GPsolver.RK4(psi); //returns new psi after one time step
        //piping result to file
    }

    return 0;
}