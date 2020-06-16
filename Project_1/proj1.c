//Datum : 3.11. 2017
//proj1.c
//Vyhladavaci algoritmus do navigacie
//Adam Hostin xhosti02


#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>



char konvertuj (char c){                            //DONE


    if (('a'<=c)&(c <='z')){           //pouziva sa vo funkciach nacitaja a zpracovanie_vstupu 
        c-= 'a'-'A';
    }
    return (c);
}
void zpracovanie_vstupu (int*p_znakov,char argv[],char *vstup) {
    for (int j = 0; argv[j] != 0; j++) {          //prechadza jednotlive znaky argumentu
        vstup[*p_znakov] = konvertuj(argv[j]);    //zmeni vsetky male pismena na velke
        (*p_znakov)++;                              //ked cyklus skonci v tejto premennej bude pocet znakov z argumentu
    }
    (*p_znakov)--;                                //aby som neskor v porovnavani nebral do uvahy "/0" znak
    if (vstup[*p_znakov]==0) printf("je tam null") ;
    return;
}

int nacitaj_adresy(char adresy[50][101], int *p_adries) {       //nacitava databazu adries
    int i = 0;
    int znak;

    while ((znak = getchar()) > 0) {                     //nacitavanie ukncuje koniec suboru (EOF)
        if (znak != '\n') {
           /* printf("pocas nacitavani adries %c", znak);*/
            adresy[*p_adries][i] = konvertuj((char) znak);   //priradi znak zo suboru do zoznamu adries
            i++;                                            //posuva sa o jden znak v pravo
            if (i>101){                                     //ak plati podmienka
                fprintf(stderr,"pretecenie adresy  ");
                exit(1);
            }
        } else {
            adresy[*p_adries][i] = 0;
            (*p_adries)++;                                     //posuva sa riadok
            i = 0;                                            //vynuluje sa premenna znaciaca znak riadku
            if ((*p_adries)>50) {
                fprintf(stderr, "pretecenie poctu adries  ");
                exit(2);
            }
        }
    }
    (*p_adries)++;                                          //niekde som sa prepocital ale takto to vychadza
    if (*p_adries==0){
        fprintf(stderr,"prazdna databaza adries");
        exit(5);
    }
    /*for (int k = 0; k < (*p_adries); k++) {
        for (int i = 0; i < (*p_adries); i++)
            printf("po nacitavani adries %c", adresy[k][i]);
        printf("\n");*/


        return 0;
}
void zorad_pole(char *vystup,int poradie_povoleneho_znaku){

    char pomocna_premenna;

    for (int i=0;i<(poradie_povoleneho_znaku-1);i++)
	    for (int j=0;j<(poradie_povoleneho_znaku-i-1);j++)
		    if (vystup[j]>vystup[j+1]){
			    pomocna_premenna=vystup[j];
			    vystup[j]=vystup[j+1];
			    vystup[j+1]=pomocna_premenna;
    		}
   /* for (int i=0;i<(poradie_povoleneho_znaku);i++)
        printf("zoradenie %c\n",vystup[i]);*/
}


int kolko_zhod(int p_adries,char *vstup,char adresy[50][101],int p_znakov){

    int p_zhod=0;

    for  (int j=0;j<=p_adries;j++){                                     //cyklus prechadza jednotlive adresy
        for (int i=0;(vstup[i]!=0)&&(adresy[j][i]!=0);i++) {            //cyklus prechadza jednotlive znaky ak narazi na NULL v prefixe alebo v adrese skonci
            if (vstup[i] != adresy[j][i]) {                         //porovnava konkretny znak prefixu a adresy ak sa nezhoduju ukonci cyklus
                break;
            }
            if (i == (p_znakov)) {                                    //ak sa zhoduju znaky a zaroven sa zhoduje poradie znaku a dlzka prefixu
                p_zhod++;                                           //p_zhod, pocita zhody adries a prefixu
            }
        }
    }
    return (p_zhod);
}
void jedna_zhoda(int p_znakov,int p_adries,char vstup[101],char adresy[50][101]){
    for  (int j=0;j<p_adries;j++){                                 //prehladava adresy
        for (int i=0;((vstup[i]!=0)||(adresy[j][i]!=0));i++) {
            if (vstup[i] != adresy[j][i]) {                     //ak sa konkretna adresa nezhoduje
                break;                                          //prejde na dalsiu
            }
            if (i == p_znakov) {                                // ak sa zhoduje vytlaci vystup
                printf("Found: %s\n",adresy[j]);
            }
        }
    }
    return;
}
void vypis_povolenych_znakov(int poradie_povoleneho_znaku,char vystup[101]) {
    bool medzera =false;

    if (poradie_povoleneho_znaku != 0) {
        printf("Enable: ");                                 //1. cast vystupu
        for (int k = 0; k < poradie_povoleneho_znaku; k++) {            //prechadza vystupne pole a vypisuje povolene znaky
            if (vystup[k] == ' ') {                        //medzere priradi podtrznik
                vystup[k] = '_';                         //konvertuje male znaky na velke ak je to nutne
                medzera=true;
            }
            printf("%c", vystup[k]);                                 //2. cast vystupu
        }
    }
    printf("\n");
    if (medzera)
        printf("Upozornenie: medzera sa vypisuje ako '_'\n");
    return;
}

void ukladanie_povolenych_znakov(char vystup[50],int *poradie_povoleneho_znaku, char adresa){//adresa je konkretny porovnavany znak

    for (int k = 0; k < *poradie_povoleneho_znaku ; ++k) {//prechadza vystupne pole
        if (adresa == vystup[k]) {              //kontroluje ci uz nie je v poli ak sa tam nachadza
            return;                                  //a presunie sa o adresu dalej
        }
    }
        vystup[*poradie_povoleneho_znaku] = adresa;     //prida sa do vystupneho pola
      //  printf("ukladanie povoleneho znaku %c",adresa);
        (*poradie_povoleneho_znaku)++;                 //pocet povolenych znakov sa zvysi o jedna
}


void zhoda(int p_adries,int p_znakov,char vstup[101],char adresy[50][101],char vystup[50]){

    int poradie_povoleneho_znaku=0;

    for  (int j=0;j<p_adries;j++){                                 //prechadza adresy
        for (int i=0;vstup[i]!=0;i++) {
            if (vstup[i] != adresy[j][i]) {                     //ak sa nenasla zhoda
                break;                                          //presunie sa na dalsiu
            }
            if (adresy[j][i+1]==0 && i == p_znakov )             //ak plati podmienka potom sa adresa adresy[j]==vstup (pozri funkcia zpracovanie_vstupu riadky 26-34)
                printf("Not found: %s\n",adresy[j]);
            else if (i == p_znakov) {                                //nasla sa zhoda
                ukladanie_povolenych_znakov(vystup,&poradie_povoleneho_znaku,adresy[j][i+1]);
            }
        }
    }
    zorad_pole(vystup,poradie_povoleneho_znaku);
    vypis_povolenych_znakov(poradie_povoleneho_znaku,vystup);
    return;
}


int main(int argc,char *argv[]) {

    int p_znakov=0,p_adries=0,p_zhod=0;  //pocet znakov zo vstupu (argumentu)
    char adresy[50][101],vstup[101],vystup[50];

    nacitaj_adresy(adresy, &p_adries);                           //nacitava databazu adries
                                                                //adresy su nacitavane na zaciatku pre pripad ze nebol zadany ziadny argument
    if (argc>2) {                                                   //ak je pocet argumentov vacsi ako 1 vypise chybu
        fprintf(stderr,"neplatny pocet argumentov %d \n",argc);        // skonci program
        exit (3);
    }
    else if (argc==1) {                                             //ak nema ziadny prefix vypise vsetky prve pismena
	int poradie_povoleneho_znaku=0;
 
        for (int j = 0; j < p_adries; j++)                                 //prechadza adresy
            ukladanie_povolenych_znakov(vystup, &poradie_povoleneho_znaku, adresy[j][0]);
        zorad_pole(vystup,poradie_povoleneho_znaku);
        vypis_povolenych_znakov(poradie_povoleneho_znaku,vystup);
        return (0);
    }

    zpracovanie_vstupu(&p_znakov,argv[1],vstup);

    p_zhod=kolko_zhod(p_adries,vstup,adresy,p_znakov);         // zisti pocet zhod

    if (p_zhod==0){                                                 //ak sa nenasla ziadna zhoda
        printf("not found\n");                                 //<==vystup je tu
    }
    else if (p_zhod==1) {                                                //ak sa prave jedna zhoda
        jedna_zhoda(p_znakov,p_adries,vstup,adresy);
    }
    else if (p_zhod>1){                                             //ak sa najde viac zhod
        zhoda(p_adries,p_znakov,vstup,adresy,vystup);
    }
    else {
        fprintf(stderr,"netusim co sa stalo ale nefungujem \n");
        exit (4);
    }
    printf("\n");
    return 0;
}
