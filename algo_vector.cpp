# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>
# include <vector>
# include "mpi.h"

using namespace std;

# define DEBUG             0
# define MAX_CH 2000
# define RANDOM_SEED       0
// 
//  Message tags
//
# define NEED_CHROMOSOME      1
# define RANDOM_NUMBERS    2

/** Solution **/
struct solution{
	int chromosome[MAX_CH];
	float eval;
	int volumeCons;
};

/********************************************************************************/
solution genererSolution(int indice);
int normaliser(int random, int max);
int main ( int argc, char *argv[] );
/********************************************************************************/

solution genererSolution(int indice){
	solution sol;
	sol.eval = 2.0;
	sol.volumeCons = 20;
	for(int i=0;i<MAX_CH;i++){
		sol.chromosome[i]=normaliser(random(),1000);

	}
	return sol;
}

/********************************************************************************/
int normaliser(int random, int max){
	return (random % max );
}


/********************************************************************************/

int main ( int argc, char *argv[] )

{
/********************* begin declaration *************************************/
	double start_time, end_time;
	double execution_time;
	solution sol;
	int dest;
	int done;
	int j;
	int id;
	int indice=0;
	MPI::Status mesgStatus;
	int num_procs;
	int request;
	int temp;
	int population_size=15000;
	int N=0;
	int sous_population_size;
	int master=0;
	vector <solution> sous_population;
	int ranks[1];
	MPI::Group worker_group;
	MPI::Intracomm worker_comm;
	MPI::Group world_group;

/********************* end declaration *************************************/
	
	MPI::Init ( argc, argv ); // initialiser MPI
	num_procs = MPI::COMM_WORLD.Get_size (); // nombre de processeurs
	sous_population_size = population_size/(num_procs-1); // taille de la sous population
	id = MPI::COMM_WORLD.Get_rank (); // id du processeur
	start_time= MPI::Wtime();

/************************creer un communicator ***************************/


	world_group = MPI::COMM_WORLD.Get_group ( );  
	ranks[0] = master;
	worker_group = world_group.Excl ( 1, ranks );
	worker_comm = MPI::COMM_WORLD.Create ( worker_group );
	worker_group.Free ( );

/************************ fin creation un communicator ***************************/

	if ( id == 0 ) 
  	{
# if RANDOM_SEED
		struct timeval time;
		gettimeofday ( &time, 0 );
		srandom ( (int)(time.tv_usec*1000000+time.tv_sec) );    
# endif

		do{
			MPI::COMM_WORLD.Recv ( &request, 1, MPI::INT, MPI::ANY_SOURCE,NEED_CHROMOSOME, mesgStatus );
			if ( request ) {
				j=normaliser(random(),population_size);// generer l'indice aleatoirement
				sol=genererSolution(j);// recuperer le chromosome
				dest = mesgStatus.Get_source ();
				MPI::COMM_WORLD.Send(&sol, sizeof(struct solution), MPI::BYTE, dest, RANDOM_NUMBERS);
			}
    	}while ( request > 0 );
	}
	else { // les autres processeurs
			indice = 0;
			MPI::COMM_WORLD.Send ( &request, 1, MPI::INT, master, NEED_CHROMOSOME );
			do{
				request = 1;
				MPI::COMM_WORLD.Recv(&sol, sizeof(struct solution), MPI::BYTE, master, RANDOM_NUMBERS,mesgStatus);
				sous_population.push_back(sol);						
				indice++;
				//cout << "Processeur : "<<id<< "  solution num: "<<indice<<endl;	

      			
				if (indice==sous_population_size)
				{
					temp = indice;
					worker_comm.Reduce ( &temp, &N, 1, MPI::INT, MPI::SUM, 0 );
				 	request = 0;
					cout<<"id "<<id << "N: "<<N<<endl;
					if(N==population_size) {

						MPI::COMM_WORLD.Send ( &request, 1, MPI::INT, master, NEED_CHROMOSOME );
					}
				}
				else	
				{
					request = 1;	
					MPI::COMM_WORLD.Send ( &request, 1, MPI::INT, master, NEED_CHROMOSOME);			  
				}
			}while (indice<sous_population_size);
	}
	MPI::Finalize ( );
	end_time= MPI::Wtime();
	execution_time = end_time-start_time;
	if (id==0){
	 	cout << "  Temps d'execution:   " << execution_time << " secondes\n";
	}
	else{
		cout << "  work done from process "<<id<<" individuals collected: "<<indice<<"\n";
	}
	return 0;
}

