//The code below is from Professor Lucia Drummond's Distributed Systems class.

#include <stdio.h>
#include "mpi.h"

float funcao(float x); // função a integrar
float calcula(float local_a, float local_b, int local_n, float h);

int main(int argc, char **argv){
    int my_rank;
    int p;                  // número de processos
    float a = 0.0, b = 1.0; // intervalo a calcular
    int n = 4096;           // número de trapezóides
    float h;                // base do trapezóide
    float local_a, local_b; // intervalo local
    int local_n;            // número de trapezóides local
    float integral;         // integral no meu intervalo
    float total;            // integral total
    int source;             // remetente da integral
    int dest = 0;           // destino das integrais (nó 0)
    int tag = 200;          // tipo de mensagem (único)
    MPI_Status status;    
    
    int rem_n = 0; //Remaining trapezoids
    float rem_integral = 0;
    float rem_total = 0;
    float before_rem;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &p);    
       
    rem_n = n % p;
    //printf("Remaining trapezoids = %d\n", rem_n);

    h = (b - a) / n;
    //printf("Altura = %f\n", h);

    local_n = n / p;
    //printf("Local n = %d\n", local_n);

    local_a = a + my_rank * local_n * h;
    //printf("Local a = %f\n", local_a);

    local_b = local_a + local_n * h;
    //printf("Local b = %f\n\n", local_b);
    
    integral = calcula(local_a, local_b, local_n, h);

    if(rem_n && my_rank != 0){
        //Each process will calculate ONE trapezoid.
        rem_integral = calcula(local_a, local_b, 1, h);
        //printf("Remainder trapezoids integral (rank %d) = %f\n\n", my_rank, rem_integral);    
    }

    //Reduce all local integrals to the master process (rank 0) using MPI_SUM.
    total = integral;
    MPI_Reduce(&integral, &total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);
    
    before_rem = total; //Area before summing the remainder trapezoids. 

    //For debugging purposes.
    rem_total = rem_integral;
    MPI_Reduce(&rem_integral, &rem_total, 1, MPI_FLOAT, MPI_SUM, 0, MPI_COMM_WORLD);

    //Perform load balancing so each processor calculates only one trapezoid's area.
    //By the end, all remaining trapezoids will have their area calculated.
    if (rem_n){
        if (my_rank == 0){
            for (source = 1; source < p; source++){
                MPI_Recv(&rem_integral, 1, MPI_FLOAT, source, tag, MPI_COMM_WORLD, &status);
                total += rem_integral;
            }
        }
        else
            MPI_Send(&rem_integral, 1, MPI_FLOAT, dest, tag, MPI_COMM_WORLD);
    }
    
    //Print results
    if (my_rank == 0){
        printf("**********************************************************\n");
        printf("*Area excluindo trapezoides resultantes: %f\n", before_rem);
        printf("*Numero de trapezoides resultantes: %d\n", rem_n);
        printf("*Area dos trapezoides resultantes: %f\n", rem_total);
        printf("*Resultado final: %f\n", total);
    }
    MPI_Finalize();
}

float calcula(float local_a, float local_b, int local_n, float h){
    float integral;
    float x, i;
    integral = (funcao(local_a) + funcao(local_b)) / 2.0;

    x = local_a;
    for (i = 1; i <= local_n; i++){
        x += h;
        integral += funcao(x);
    }
    integral *= h;
    return integral;
}

float funcao(float x) {
 float fx; // valor de retorno

 // esta é a função a integrar
 // exemplo: função quadrática
 fx = x * x;
 return fx;
} 