#include <iostream>
#include <time.h>
#include "strategies.h"
#include <ctime>
#include <fstream>
int main(int argc, char *argv[])
{


    recup_AGparam("/home/koopa/param.txt");
    //dakhel ag_param, ecrire les parametre dans l'ordre

    init_tableau("/home/koopa/Exo_TD.txt");
    tri_tableau();
    //fichier csv output matensahche

    srand(time(NULL));
    time_t t0=time(NULL);
    clock_t begin = clock();

    //à changer dans le fichier et dans le main
    initialeMixte(10);
    AG(param,"/home/koopa/population.txt");
     //dakhel ag param, concatèner volume et fitness de la meilleure solution
    //puis concaténer exec time dans main
    cout << "\n Temps d'execution " << difftime(time(NULL),t0) <<"s";
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;

    std::ofstream csv;
    getchar();
    csv.open("/home/koopa/result.csv",std::ios_base::app);
    csv<<elapsed_secs<<"\n";
    csv.close();
    return 0;
}
