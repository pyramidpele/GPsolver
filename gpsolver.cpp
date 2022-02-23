//Source code for the FDM and Runge functions which discretise the 2-component coupled GP eqns 

#include "gpsolver.h"
#include <stdlib.h>
#include <cmath>
#include <iostream>
#include <fstream>

dcomp I=dcomp(0.,1.); //defining i


void GPsolver::System_generator(double x[], double omega[]){
    //generating spatial grid 
    for (int i=1; i<N; i++){
        if(i%2==0){
            x[i]=x[i-1]+dx;
        }else{
            x[i]=x[i-1];
        }
    }

    //generating coherent coupling for modulation of system
    for (int i=0; i<N; i++){
        if(i<N/2){
            omega[i]=omegaLHS;
        }else{
            omega[i]=omegaRHS;
        }
    }
}

//generates initial psi and excitation
void GPsolver::Init_psi_generator(dcomp psi[], bool excitation, double x[]){

    //opening up results file
    std::ofstream output;
    output.open("results/results.txt");

    //generating initial psi for each gridpoint and saving result
    for (int i=0; i<N; i++){
        psi[i]=sqrt(n_0/2); //since norm(psi)=n
        
        if(excitation){ //add excitation at x_0
            if(i%2==0){ //condensate a
                psi[i]+=0.0001*exp(I*(k_0*x[i])-pow((x[i]-x_0)/width,2)/2); 
            }else{ //condensate b
                psi[i]-=0.0001*exp(I*(k_0*x[i])-pow((x[i]-x_0)/width,2)/2);
            }
        }

        output<<norm(psi[i]); 
        if(i<N-1){
            output<<",";
        }
    }

    output<<"\r\n";
    output.close();
}

//solves eigenproblem (resulting from discretisation) using RK4 method to get psi(a0,b0,a1,b1,...,aN-1,bN-1) at +dt
void GPsolver::RK4(dcomp psi[], double omega[]){ //remember to multiply Mk's by I=-i

    //declaring variables for RK4
    dcomp k[N];
    dcomp k_1[N], Mk_1[N];
    dcomp k_2[N], Mk_2[N];
    dcomp k_3[N], Mk_3[N];
    dcomp k_4[N], Mk_4[N];

    //1st RK4 iteration
    for (int i=0; i<N; i++){
        k_1[i]=psi[i];
    }
    Spatial_discretiser(k_1, Mk_1, omega); //calculating Mk_1

    //2nd RK4 iteration
    for (int i=0; i<N; i++){
        k_2[i]=psi[i]+dt*Mk_1[i]/2.;
    }
    Spatial_discretiser(k_2, Mk_2, omega); //calculating Mk_2

    //3rd RK4 iteration
    for (int i=0; i<N; i++){
        k_3[i]=psi[i]+dt*Mk_2[i]/2.;
    }
    Spatial_discretiser(k_3, Mk_3, omega); //calculating Mk_3
    
    //4th RK4 iteration
    for (int i=0; i<N; i++){
        k_4[i]=psi[i]+dt*Mk_3[i];
    }
    Spatial_discretiser(k_4, Mk_4, omega); //calculating Mk_4

    //calculating new psi after dt time increment
    for (int i=0; i<N; i++){
        psi[i]+=dt/6.*(Mk_1[i]+2.*Mk_2[i]+2.*Mk_3[i]+Mk_4[i]);
    }
}

//spatially discretises RHS of coupled GP eqn in 1D using FDM and calculates -iMk(a0,b0,a1,b1,...,aN-1,bN-1) 
void GPsolver::Spatial_discretiser(dcomp k[], dcomp Mk[], double omega[]){

    dcomp C[N]; //constant introduced for convenience 
    Const_calc(k, C, omega); //calculates constant for each component at each gridpoint

    //calculating -iMk for each gridpoint, (N+i)%N to make grid loop 
    for (int i=0; i<N; i++){
        if (i%2==0){ //even entries are for condensate a
            Mk[i]=-I*(-k[(N+i-2)%N]/pow(dx,2)+C[i]*k[i]+omega[i+1]*k[i+1]-k[(N+i+2)%N]/pow(dx,2));
        }else{ //odd entries for condensate b
            Mk[i]=-I*(-k[(N+i-2)%N]/pow(dx,2)+omega[i-1]*k[i-1]+C[i]*k[i]-k[(N+i+2)%N]/pow(dx,2));
        }
    }
}

//Calculates convenient constant for RHS of discretised coupled GP eqns C(a0,b0,a1,b1,...,aN-1,bN-1)
void GPsolver::Const_calc(dcomp k[], dcomp C[], double omega[]){
    
    double mu[N]; //chemical potential in units of gn
    Chem_potential(mu, omega); //calculates mu

    for (int i=0; i<N; i++){
        if (i%2==0){ //even entries are for condensate a
            C[i]=2/pow(dx,2)+V_a/(g*n_0)+norm(k[i])/n_0+g_ab*norm(k[i+1])/(g*n_0)-mu[i];
        }else{ //odd entries for condensate b
            C[i]=2/pow(dx,2)+V_b/(g*n_0)+norm(k[i])/n_0+g_ab*norm(k[i-1])/(g*n_0)-mu[i];
        }
    }
}

//calculates dimensionless chemical potential mu, to make GP eqns timeless
void GPsolver::Chem_potential(double mu[], double omega[]){
    
    for (int i=0; i<N; i++){
        mu[i]=(1+g_ab/g)/2-abs(omega[i]);
    }
}





