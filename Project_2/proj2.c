//Autor: Adam Hostin
//Datum: 14.11. 2017

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>


double taylor_tan(double x, unsigned int n);                    //pocita tangens cez tailorov rad
double cfrac_tan(double x, unsigned int n);                     //pocita tangens cez zretazený zlomok
void help();                                                    //vypise napovedu
void porovnaj_tangens(int argc, char *argv[]);                  //funkcia --tan porovnáva vzsledky tangensu z matematickej kniznice taylora a zretazeneho zlomku
double kontrola_argumentu (char *argument, int je_to_vyska);    //kontroluje ci je argument double
void  vypocet (int argc, char *argv[]);                         //pocita vzdialenost v pripade yadanej bety aj vzsku

int main(int argc, char *argv[]) {

    if (argc==1) {
        fprintf(stderr,"nebol zadany argument");
        exit (EXIT_FAILURE);
    }
    if (strcmp(argv[1],"--help")==0)
        help();
    else if (strcmp(argv[1],"--tan")==0){
        porovnaj_tangens(argc, argv);
    }
    else if ((strcmp(argv[1],"-m")==0) || (strcmp(argv[1],"-c")==0)){
        vypocet (argc, argv);
    }
    else printf("spustil si ma zle skus to takto ./proj2 --help");
    return (0);
}
void help(){
    printf("vypocet tangensu\n\n"
    "spustenie: ./proj2 --tan A N M\n"
    "popis: porovná přesnosti výpočtu tangens úhlu A (v radiánech) mezi voláním tan z matematické\n"
    "     knihovny, a výpočtu tangens pomocí Taylorova polynomu a zřetězeného zlomku.\n"
    "     Argumenty N a M udávají, ve kterých iteracích iteračního výpočtu má porovnání probíhat.\n"
    "     0 < N <= M < 14\n"
    "format vystupu: I M T TE C CE\n"
    "                I značí počet iterací iteračního výpočtu\n"
    "                M výsledek z funkce tan matematické knihovny\n"
    "                T výsledek z funkce taylor_tan (taylorov polynom)\n"
    "                TE absolutní chybu mezi výpočtem pomocí Taylorova polynomu a matematickou knihovnou\n"
    "                C výsledek z funkce cfrac_tan a(zretazeny zlomok)\n"
    "                CE absolutní chybu mezi výpočtem pomocí zřetězených zlomků a matematickou knihovnou.\n"
    "vypocet vzdialenosti a vysky\n\n"
    "spustenie: ./proj2 [-c X] -m A [B]\n"
    "popis: Úhel α je dán argumentem A v radiánech. Program vypočítá a vypíše vzdálenost měřeného objektu.\n"
    "       0 < A <= 1.4 < π/2.\n"
    "       Pokud je zadán, úhel β udává argument B v radiánech. Program vypočítá a vypíše i výšku měřeného objektu.\n "
    "       0 < B <= 1.4 < π/2\n"
    "       Argument -c nastavuje výšku měřicího přístroje c pro výpočet. Výška c je dána argumentem X (0 < X <= 100).\n"
    "       Argument je volitelný - implicitní výška je 1.5 metrů."
    "vystup: V pripade zadania oboch uhlov sa vypisu 2 cisla kazde na jeden riadok najprv vzdialenost a potom vysku\n\n");

    return;
}

double taylor_tan(double x, unsigned int n){                                        //taylor
    double citatel [12]= {1, 2, 17, 62, 1382, 21844, 929569, 6404582, 443861162, 18888466084, 113927491862, 58870668456604}; //citatele pre taylorov rad od 2 po 13 člen
    double menovatel [12]={3, 15, 315, 2835, 155925, 6081075, 638512875, 10854718875, 1856156927625, 194896477400625, 49308808782358125, 3698160658676859375};//menovatele pre taylorov rad od 2 po 13 člen
    double sum=x;                                                                   //do suctu priradi prvy clen postupnosti
    double x0=x;

    for (int i=1;i<((int)n);i++) {
        x *= x0*x0;
        sum += (citatel[i - 1] * x)/menovatel[i-1];
    }
return (sum);
}
double cfrac_tan(double x, unsigned int n){

    if (n==1) return (x);
    double menovatel=(n*2)-1;
    double vnoreny_zlomok=(x*x)/menovatel;

    for (int i=((int)n)-1;i>0;i--){
        menovatel-=2;
        if (i==1){
            vnoreny_zlomok=x/(menovatel-vnoreny_zlomok);
            return (vnoreny_zlomok);
        }
        vnoreny_zlomok= (x*x)/(menovatel-vnoreny_zlomok);
    }
    return (vnoreny_zlomok);
}

void porovnaj_tangens(int argc, char *argv[]){
    if (argc!=5){
        fprintf(stderr,"chyba pri zadavani argumentu");
        exit (EXIT_FAILURE);
    }
    double x=kontrola_argumentu (argv[2],0);
    int n=atoi(argv[3]);
    int m=(atoi(argv[4]));
    if (!(0 < n && n<= m && m < 14)) {
        fprintf(stderr, "chyba prizadavani argumentu pozri help\n");
        exit (EXIT_FAILURE);
    }
    for(;n<=m;n++){
        printf("%d %e %e %e %e %e\n",n,tan(x),taylor_tan(x,(unsigned int) n),(fabs(tan(x) - taylor_tan(x,(unsigned int) n))),cfrac_tan(x,(unsigned int) n),(fabs(tan(x) - cfrac_tan(x,(unsigned int) n))));
    }
    exit(0);
}

double kontrola_argumentu (char *argument, int je_to_vyska){
    char *ostatne = '\0';
    double vystup = strtod(argument,&ostatne);

    if (*ostatne!='\0'){
        fprintf(stderr,"neplatny argument %s",argument);
        exit (EXIT_FAILURE);
    }
    if (je_to_vyska==0) {
        if (!(0 < vystup && vystup <= 1.4)) {
            fprintf(stderr, "argument mimo rozsah pozri --help %s", argument);
            exit(EXIT_FAILURE);
        }
    }
    else if (je_to_vyska==1)
        if (!(0 < vystup && vystup <= 100)) {
            fprintf(stderr, "argument mimo rozsah pozri --help %s", argument);
            exit(EXIT_FAILURE);
        }
    return (vystup);
}
void  vypocet (int argc, char *argv[]){
    double c;
    double alfa;
    double beta;
    double dlzka;
    double vyska;

    if ((strcmp(argv[1],"-c")==0)) {
        if (argc !=6 && argc!=5) {
            fprintf(stderr, "chyba pri zadavani argumentu");
            exit (EXIT_FAILURE);
        }
        c = kontrola_argumentu(argv[2],1);
        alfa = kontrola_argumentu(argv[4],0);
        if (argc==6)
            beta = kontrola_argumentu(argv[5],0);
    }
    else if ((strcmp(argv[1],"-m")==0)) {
        if (argc != 4 && argc != 3) {
            fprintf(stderr, "chyba pri zadavani argumentu");
            exit (EXIT_FAILURE);

        }
        c = 1.5;
        alfa = kontrola_argumentu(argv[2],0);
        if (argc == 4) {
            beta = kontrola_argumentu(argv[3],0);
        }
    }
    else {
        fprintf(stderr, "chyba pri zadavani argumentu");
        exit (EXIT_FAILURE);
    }
    dlzka= c/cfrac_tan(alfa,10);
    if (((strcmp(argv[1],"-m")==0)&&(argc == 4))||((strcmp(argv[1],"-c")==0)&&(argc==6))){ //ak tam je beta
        vyska= (dlzka*cfrac_tan(beta,10))+c;
        printf("%.10e\n%.10e\n",dlzka,vyska);
    }
    else
        printf("%.10e\n",dlzka);
    exit(0);
}
