# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>
# include <vector>
# include "mpi.h"
#include <time.h>
#include "strategies.h"
#include <ctime>
#include <fstream>

using namespace std;

# define DEBUG             0
# define RANDOM_SEED       0
// 
//  Message tags
//
# define NEED_CHROMOSOME      1
# define RANDOM_NUMBERS    2

// var global

/********************************************************************************/
solution genererSolution(int indice);
int normaliser(int random, int max);
vector<solution> initAg();
vector<solution> finaliserAg(vector <solution> sous_population);
void AlgoGen();
int main ( int argc, char *argv[] );

/********************************************************************************/
int main ( int argc, char *argv[] )
{

		double begin, end , timefinpgm;
		MPI::Init ( argc, argv ); // initialiser MPI

		int num_procs = MPI::COMM_WORLD.Get_size (); // nombre de processeurs
	
		int id = MPI::COMM_WORLD.Get_rank (); // id du processeur
		char* paramfile="param.txt";
		char* datafile="Exo_TD.txt";
		char* parampopulation="population.txt";
		

		begin= MPI::Wtime();

		


		recup_AGparam(paramfile);
    param.ratio=50;
		//dakhel ag_param, ecrire les parametre dans l'ordre
		init_tableau(datafile);
		tri_tableau();
		srand(time(NULL));
		if(id==0){
					
					//fichier csv output matensahche
				/*	time_t t0=time(NULL);
					clock_t begin = clock();*/

					//à changer dans le fichier et dans le main
					initialeMixte(param.N*(num_procs-1));


					 //dakhel ag param, concatèner volume et fitness de la meilleure solution
					//puis concaténer exec time dans main
					/*cout << "\n Temps d'execution " << difftime(time(NULL),t0) <<"s";*/
					//clock_t end = clock();
					/*double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;*/

					/*std::ofstream csv;
					getchar();
					csv.open("result.csv",std::ios_base::app);
					csv<<elapsed_secs<<"\n";
					csv.close();*/
}
		MPI_Barrier(MPI::COMM_WORLD);
	

		population_act = initAg();

		cout <<"taille sous pop apres affection  lovyouhhhhh "<<population_act.size() << endl;
		// executer l'AG sur les noeuds sauf le maitre 
		if (id != 0 ) {
		cout <<"ana " <<id <<"rani rayeh nebda f l ag"<<endl;

		AG(param,parampopulation);

		cout <<"ana " <<id <<"rani kamalet"<<endl;

		unsigned len=population_act.size();
		cout << "id: "<<id <<" size : "<<len<<endl;
}
		MPI_Barrier(MPI::COMM_WORLD);
	


		cout << "end barrier"<<endl;
		population_glob.clear();
		population_glob = finaliserAg(population_act);

		

		if(id == 0) {
    cout << " taille population glob : "<<population_glob.size()<<endl;
		std::sort(population_glob.begin(),population_glob.end(),IndividuCompare());

		end= MPI::Wtime();
		cout << "fin de l'AG binajah ^_^  time :" << end-begin <<" secondes"<<endl;
    cout << "\n ANA LEMKHEYYER FIKOM " << endl;
    print_individu(population_glob.at(0));
    }
		MPI::Finalize ( ); 
		return 0;
}

/******************************************************************************/
solution genererSolution(int indice){
	
	return population_glob.at(indice);
}

/********************************************************************************/
int normaliser(int random, int max){
	return (random % max );
}


/********************************************************************************/

vector<solution> initAg(){
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
	int population_size;
	int N=0;
	int len=0;
	int sous_population_size;
	int master=0;
	vector <solution> sous_population;
	int ranks[1];
	MPI::Group worker_group;
	MPI::Intracomm worker_comm;
	MPI::Group world_group;

/********************* end declaration *************************************/
	
	num_procs = MPI::COMM_WORLD.Get_size (); // nombre de processeurs
	sous_population_size = param.N; // taille de la sous population
	population_size= param.N* (num_procs-1);
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
/*# if RANDOM_SEED
		struct timeval time;
		gettimeofday ( &time, 0 );
		srandom ( (int)(time.tv_usec*1000000+time.tv_sec) );    
# endif
*/

		do{
			MPI::COMM_WORLD.Recv ( &request, 1, MPI::INT, MPI::ANY_SOURCE,NEED_CHROMOSOME, mesgStatus );
			if ( request ) {
				sous_population.clear();
				for(int k=0;k<sous_population_size;k++){
					j=normaliser(random(),sous_population_size*(num_procs-1));// generer l'indice aleatoirement
					sol=genererSolution(j);// recuperer le chromosome
					sous_population.push_back(sol);				
				}

				dest = mesgStatus.Get_source ();

				len=sous_population.size();
				MPI::COMM_WORLD.Send(&len,1,MPI::INT,dest,NEED_CHROMOSOME);
				   
				if(len!=0) 
				{
				  MPI::COMM_WORLD.Send(sous_population.data(),len*sizeof(solution),MPI::BYTE,dest,NEED_CHROMOSOME);
				}
			}
    	}while ( request > 0 );
	}
	else { // les autres processeurs
		request=1;
		MPI::COMM_WORLD.Send ( &request, 1, MPI::INT, master, NEED_CHROMOSOME );

		MPI::COMM_WORLD.Recv(&len,1,MPI::UNSIGNED,master,  NEED_CHROMOSOME,mesgStatus);
		sous_population.resize(len);
		if(len!=0) 
		{
			MPI::COMM_WORLD.Recv(sous_population.data(),len*sizeof(solution),MPI::BYTE,master,  NEED_CHROMOSOME,mesgStatus);
			cout << "id: "<<id<< "sous_population.at(100).chromosome[100]"<<sous_population.at(100).eval << endl << endl;
			temp = sous_population_size;
			worker_comm.Reduce ( &temp, &N, 1, MPI::INT, MPI::SUM, 0 );
			if(N==population_size) {
				request=0;
				MPI::COMM_WORLD.Send ( &request, 1, MPI::INT, master, NEED_CHROMOSOME );
			}
			}
			else sous_population.clear();
	}
	end_time= MPI::Wtime();
	execution_time = end_time-start_time;
	if (id==0){
	 	cout << "  Temps d'execution distrbution:   " << execution_time << " secondes\n" << endl << endl;
	}
	else{
		cout << "  work done from process "<<id<<"\n" << "la taille " << sous_population.size() << endl << endl;

	}

	return sous_population;
}


/********************************************************************************/


 vector <solution> finaliserAg(vector <solution> sous_population){
/********************* begin declaration *************************************/
	double start_time, end_time;
	double execution_time;
	int dest;
	int j;
	int id;
	MPI::Status mesgStatus;
	int num_procs;
	int len=0;
	int sous_population_size;	
	int population_size=6000;
	int master=0;
	vector <solution> population;

	int recv_num=0;

/********************* end declaration *************************************/
	
	num_procs = MPI::COMM_WORLD.Get_size (); // nombre de processeurs
	sous_population_size = population_size/(num_procs-1); // taille de la sous population
	id = MPI::COMM_WORLD.Get_rank (); // id du processeur
	start_time= MPI::Wtime();

	if ( id == 0 ) 
  	{
		do{
				MPI::COMM_WORLD.Recv(&len,1,MPI::UNSIGNED,MPI::ANY_SOURCE,  NEED_CHROMOSOME,mesgStatus);
				sous_population.resize(len);
				dest = mesgStatus.Get_source ();
				if(len!=0) 
				{
					MPI::COMM_WORLD.Recv(sous_population.data(),len*sizeof(solution),MPI::BYTE,dest,  NEED_CHROMOSOME,mesgStatus);
					// regrouper dans une liste globale
					population.insert(population.end(),sous_population.begin(),sous_population.end());
					cout << "sous_population.at(100).chromosome[100]"<<sous_population.at(100).chromosome[100] << endl;
					recv_num++;
				}
				else sous_population.clear();
				
    	}while ( recv_num< num_procs-1);
	}
	else { // les autres processeurs
				len=sous_population.size();
				MPI::COMM_WORLD.Send(&len,1,MPI::INT,master,NEED_CHROMOSOME);
				   
				if(len!=0) 
				{
				  MPI::COMM_WORLD.Send(sous_population.data(),len*sizeof(solution),MPI::BYTE,master,NEED_CHROMOSOME);
				}
	}
	end_time= MPI::Wtime();
	execution_time = end_time-start_time;
	if (id==0){
	 	cout << "  Temps d'execution collect d'individus:   " << execution_time << " secondes\n" << endl << endl;
	}
	return population;
}

/*************************************************************************/

