/**
 * 
 * Datum : 6.12.2017
 *
 * Kostra programu pro 3. projekt IZP 2017/18
 *
 * Jednoducha shlukova analyza
 * Unweighted pair-group average
 * https://is.muni.cz/th/172767/fi_b/5739129/web/web/usrov.html
 */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h> // sqrtf
#include <limits.h> // INT_MAX
#include <ctype.h>


/*****************************************************************
 * Ladici makra. Vypnout jejich efekt lze definici makra
 * NDEBUG, napr.:
 *   a) pri prekladu argumentem prekladaci -DNDEBUG
 *   b) v souboru (na radek pred #include <assert.h>
 *      #define NDEBUG
 */
#ifdef NDEBUG
#define debug(s)
#define dfmt(s, ...)
#define dint(i)
#define dfloat(f)
#else

// vypise ladici retezec
#define debug(s) printf("- %s\n", s)

// vypise formatovany ladici vystup - pouziti podobne jako printf
#define dfmt(s, ...) printf(" - "__FILE__":%u: "s"\n",__LINE__,__VA_ARGS__)

// vypise ladici informaci o promenne - pouziti dint(identifikator_promenne)
#define dint(i) printf(" - " __FILE__ ":%u: " #i " = %d\n", __LINE__, i)

// vypise ladici informaci o promenne typu float - pouziti
// dfloat(identifikator_promenne)
#define dfloat(f) printf(" - " __FILE__ ":%u: " #f " = %g\n", __LINE__, f)

#endif

/*****************************************************************
 * Deklarace potrebnych datovych typu:
 *
 * TYTO DEKLARACE NEMENTE
 *
 *   struct obj_t - struktura objektu: identifikator a souradnice
 *   struct cluster_t - shluk objektu:
 *      pocet objektu ve shluku,
 *      kapacita shluku (pocet objektu, pro ktere je rezervovano
 *          misto v poli),
 *      ukazatel na pole shluku.
 */

struct obj_t {
    int id;
    float x;
    float y;
};

struct cluster_t {
    int size;
    int capacity;
    struct obj_t *obj;
};

/*****************************************************************
 * Deklarace potrebnych funkci.
 *
 * PROTOTYPY FUNKCI NEMENTE
 *
 * IMPLEMENTUJTE POUZE FUNKCE NA MISTECH OZNACENYCH '
 *
 */

/*
 Inicializace shluku 'c'. Alokuje pamet pro cap objektu (kapacitu).
 Ukazatel NULL u pole objektu znamena kapacitu 0.
*/
void init_cluster(struct cluster_t *c, int cap) {

    assert(c != NULL);
    assert(cap >= 0);


    c->capacity=cap;
    c->size=0;
    c->obj=(struct obj_t*)malloc(cap*sizeof(struct obj_t));
    if (c->obj==NULL) {
        fprintf(stderr, "chyba pri alokacii objektov");
        exit(EXIT_FAILURE);
    }

}

/*
 Odstraneni vsech objektu shluku a inicializace na prazdny shluk.
 */
void clear_cluster(struct cluster_t *c)
{
    free(c->obj);           //uvoľní pamäť vyhradenú na objekty
    c->capacity=0;
    c->size=0;

    //free(c);                //uvoľní pamäť vyhradenú na clusteri
    // init_cluster(c, 0);     //inicializuje prázdny zhluk    init_cluster(c, NULL);
}

/// Chunk of cluster objects. Value recommended for reallocation.
const int CLUSTER_CHUNK = 10;

/*
 Zmena kapacity shluku 'c' na kapacitu 'new_cap'.
 */
struct cluster_t *resize_cluster(struct cluster_t *c, int new_cap)
{
    // TUTO FUNKCI NEMENTE
    assert(c);
    assert(c->capacity >= 0);
    assert(new_cap >= 0);

    if (c->capacity >= new_cap)
        return c;

    size_t size = sizeof(struct obj_t) * new_cap;

    void *arr = realloc(c->obj, size);
    if (arr == NULL)
        return NULL;

    c->obj = (struct obj_t*)arr;
    c->capacity = new_cap;
    return c;
}

/*
 Prida objekt 'obj' na konec shluku 'c'. Rozsiri shluk, pokud se do nej objekt
 nevejde.
 */
void append_cluster(struct cluster_t *c, struct obj_t obj)
{
    if (c->capacity==c->size) {
        c= resize_cluster(c,c->capacity+CLUSTER_CHUNK);
        if (c==NULL){
            fprintf(stderr,"chyba pri realokacii");
            exit(EXIT_FAILURE);
        }

    }

    c->obj[c->size]=obj;
    c->size++;
}

/*
 Seradi objekty ve shluku 'c' vzestupne podle jejich identifikacniho cisla.
 */
void sort_cluster(struct cluster_t *c);

/*
 Do shluku 'c1' prida objekty 'c2'. Shluk 'c1' bude v pripade nutnosti rozsiren.
 Objekty ve shluku 'c1' budou serazeny vzestupne podle identifikacniho cisla.
 Shluk 'c2' bude nezmenen.
 */
void merge_clusters(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c2 != NULL);

    if (c2->size != 0) assert(c2->obj);

    int previous_c1_size = c1->size;
    // prida objekty zhluku `c2` do zhluku `c1`
    for (int i = 0; i < c2->size; i++) {
        append_cluster(c1, c2->obj[i]);
    }

    if (c2->size > 0 && c1->size == previous_c1_size + c2->size) {
        // zoradi objekty v zhluku
        sort_cluster(c1);
    }
}

/**********************************************************************/
/* Prace s polem shluku */

/*
 Odstrani shluk z pole shluku 'carr'. Pole shluku obsahuje 'narr' polozek
 (shluku). Shluk pro odstraneni se nachazi na indexu 'idx'. Funkce vraci novy
 pocet shluku v poli.
*/
int remove_cluster(struct cluster_t *carr, int narr, int idx)
{
    assert(idx < narr);
    assert(narr > 0);
    struct cluster_t tmp;   //pomocna premenna na vymenu posledneho zhluku a zhluku ktory idem zahodit

    tmp=carr[narr-1];         //swap
    carr[narr-1]=carr[idx];
    carr[idx]=tmp;

    clear_cluster(&carr[narr - 1]);
    narr--;

    return narr;
}

/*
 Pocita Euklidovskou vzdalenost mezi dvema objekty.
 */
float obj_distance(struct obj_t *o1, struct obj_t *o2)
{
    assert(o1 != NULL);
    assert(o2 != NULL);

    return sqrtf(powf(o1->x - o2->x, 2.0) + powf(o1->y - o2->y, 2.0)); // to je vzorec na vypocet vzdialenosti  nechce sa mi to prepisovat zlozito
}

/*
 Pocita vzdialenost dvoch zhlukov.
*/
float cluster_distance(struct cluster_t *c1, struct cluster_t *c2)
{
    assert(c1 != NULL);
    assert(c1->size > 0);
    assert(c2 != NULL);
    assert(c2->size > 0);

    float sumx1=0.0;
    float sumy1=0.0;
    float sumx2=0.0;
    float sumy2=0.0;
    struct obj_t c11;
    struct obj_t c22;

    c11.id=11;
    c22.id=22;

    for (int i=0;i<c1->size;i++){
        sumx1+=c1->obj[i].x;
        sumy1+=c1->obj[i].y;
    }
    c11.x=sumx1=sumx1/c1->size;
    c11.y=sumy1=sumy1/c1->size;


    for (int i=0;i<c2->size;i++){
        sumx2+=c2->obj[i].x;
        sumy2+=c2->obj[i].y;
    }
    c22.x=sumx2=sumx2/c2->size;
    c22.y=sumy2=sumy2/c2->size;

    return obj_distance(&c11,&c22);
}

/*
 Funkce najde dva nejblizsi shluky. V poli shluku 'carr' o velikosti 'narr'
 hleda dva nejblizsi shluky. Nalezene shluky identifikuje jejich indexy v poli
 'carr'. Funkce nalezene shluky (indexy do pole 'carr') uklada do pameti na
 adresu 'c1' resp. 'c2'.
*/
void find_neighbours(struct cluster_t *carr, int narr, int *c1, int *c2)
{
    assert(narr > 0);

    struct dat {        //struktura ktora uklada 2 zhluky a ich vzdialenost
        float distance;
        int idx1;
        int idx2;
    }pomocna;

    if (narr>1){              //su tam aspon 2 objekty
        pomocna.idx1=0;
        pomocna.idx2=1;

        pomocna.distance=cluster_distance(&carr[pomocna.idx1],&carr[pomocna.idx2]);
    }

    for (int i = 0; i < narr-1 ; ++i) {
        for (int j = i+1; j < narr ; ++j) {
            if (pomocna.distance>(cluster_distance(&carr[i],&carr[j]))){
                pomocna.distance=cluster_distance(&carr[i],&carr[j]);
                pomocna.idx1=i;
                pomocna.idx2=j;
            }
        }
    }
    *c1=pomocna.idx1;
    *c2=pomocna.idx2;
}

// pomocna funkce pro razeni shluku
static int obj_sort_compar(const void *a, const void *b)
{
    // TUTO FUNKCI NEMENTE
    const struct obj_t *o1 = (const struct obj_t *)a;
    const struct obj_t *o2 = (const struct obj_t *)b;
    if (o1->id < o2->id) return -1;
    if (o1->id > o2->id) return 1;
    return 0;
}

/*
 Razeni objektu ve shluku vzestupne podle jejich identifikatoru.
*/
void sort_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    qsort(c->obj, c->size, sizeof(struct obj_t), &obj_sort_compar);
}

/*
 Tisk shluku 'c' na stdout.
*/
void print_cluster(struct cluster_t *c)
{
    // TUTO FUNKCI NEMENTE
    for (int i = 0; i < c->size; i++)
    {
        if (i) putchar(' ');
        printf("%d[%g,%g]", c->obj[i].id, c->obj[i].x, c->obj[i].y);
    }
    putchar('\n');
}


//moja f
//read Number_of_clusters

int count_clustters (char *filename){ //pouzivam len koli tomu aby naalokoval dostatok miesta

    int Number_of_clusters;
    FILE *file = fopen(filename, "r");

    if (file== NULL){
        fprintf(stderr,"chybz pri otvarani suboru");
        exit(EXIT_FAILURE);
    }
    fscanf(file, "count=%d", &Number_of_clusters);
    if (isdigit(Number_of_clusters)){
        fprintf(stderr,"neplatny format na prvom riadku v subore");
        exit(EXIT_FAILURE);
    }
    if(fclose(file)==EOF){
        fprintf(stderr,"chyba pri zatvarani suboru");
        exit(EXIT_FAILURE);
    }
    return (Number_of_clusters);
}

//moja funkcia
//funkcia naalokuje pole clusterov

void init_clusters (struct cluster_t **c, int Number_of_clusters){ //

    assert(Number_of_clusters>=0);

    *c=malloc(sizeof(struct cluster_t)*Number_of_clusters); //(struct cluster_t*)
    if (*c==NULL) {
        fprintf(stderr,"chyba pri alokacii zhluku");
        exit(EXIT_FAILURE);
    }

    for (int i=0;i<Number_of_clusters;i++){
        init_cluster(&(*c)[i], 1);
    }

}/*
float verify_float (char *argument) {
    char *ostatne = '\0';
    float vystup = strtof(argument, &ostatne);

    if (*ostatne != '\0') {
        fprintf(stderr, "neplatny vtup %s", argument);
        exit(EXIT_FAILURE);
    }
    if ((0 <= vystup && vystup <= 1000)) {
        fprintf(stderr, "argument mimo rozsah pozri --help %s", argument);
        exit(EXIT_FAILURE);
    }
    return (vystup);
}*/
int verify_int (char *argument) {
    char *ostatne = NULL;
    int vystup = (int) strtof(argument, &ostatne);

    if (ostatne == NULL) {
        fprintf(stderr, "neplatny vtup %s", argument);
        exit(EXIT_FAILURE);
    }

    return (vystup);
}
/*
 Ze souboru 'filename' nacte objekty. Pro kazdy objekt vytvori shluk a ulozi
 jej do pole shluku. Alokuje prostor pro pole vsech shluku a ukazatel na prvni
 polozku pole (ukalazatel na prvni shluk v alokovanem poli) ulozi do pameti,
 kam se odkazuje parametr 'arr'. Funkce vraci pocet nactenych objektu (shluku).
 V pripade nejake chyby uklada do pameti, kam se odkazuje 'arr', hodnotu NULL.
*/
int load_clusters(char *filename, struct cluster_t **arr)
{
    assert(arr != NULL);

    int Number_of_clusters;
    struct obj_t pomocna; //aby som mohol pouzit apend

    FILE *file = fopen(filename, "r");
    if (file== NULL){
        fprintf(stderr,"chybz pri otvarani suboru");
        exit(EXIT_FAILURE);
    }

    if (fscanf(file, "count=%d",&Number_of_clusters)!=1) {
        fprintf(stderr, "chyba formatu na prvom riadku suboru");
        exit(EXIT_FAILURE);
    }

    for (int i=0;i<Number_of_clusters;i++) {


        if(fscanf(file, "%d %f %f", &pomocna.id, &pomocna.x, &pomocna.y)!=3){
            fprintf(stderr,"chyba pri formate %d. objektu v poradi", i);
            exit(EXIT_FAILURE);
        }

        if ( (pomocna.x)<=0 || (pomocna.x)>=1000) { //kontrola ci je cislo + rozsah
            fprintf(stderr, "chyba pri formate %d. objektu v poradi", i);
            exit(EXIT_FAILURE);
        }
        if ( (pomocna.y)<=0 || (pomocna.y)>=1000) { //kontrola ci je cislo + rozsah
            fprintf(stderr, "chyba pri formate %d. objektu v poradi", i);
            exit(EXIT_FAILURE);
        }
        append_cluster(&(*arr)[i],pomocna);
    }
    if(fclose(file)==EOF){
        fprintf(stderr,"chyba pri zatvarani suboru");
        exit(EXIT_FAILURE);
    }
    return Number_of_clusters;
}

/*
 Tisk pole shluku. Parametr 'carr' je ukazatel na prvni polozku (shluk).
 Tiskne se prvnich 'narr' shluku.
*/
void print_clusters(struct cluster_t *carr, int narr)
{
    printf("Clusters:\n");
    for (int i = 0; i < narr; i++)
    {
        printf("cluster %d: ", i);
        print_cluster(&carr[i]); }
}


int main(int argc, char *argv[])
{
    struct cluster_t *clusters;
    int n=0;
    int clustters_count= count_clustters(argv[1]);
    int idx1,idx2;

    if(argc == 2){
        n = 1;

    }else if(argc == 3) {
        n = verify_int(argv[2]);
    } else{
        fprintf(stderr,"chyba pri zadavani argumentu");
    }
    if(n>clustters_count) {   //nacita zhluky zo suboru a skontroluje ci n nie je vacsie ako pocet clusterov
        fprintf(stderr,"neplatny vstup");
        exit(EXIT_FAILURE);
    }

    init_clusters(&clusters,clustters_count);      //inicializuje priesto na pole zhlukov a na objekty v zhlukoch
    load_clusters(argv[1],&clusters);

    while(clustters_count > n){
   //     print_clusters(clusters, clustters_count);

        find_neighbours(clusters, clustters_count, &idx1, &idx2);
     //   print_clusters(clusters, clustters_count);


        merge_clusters(&clusters[idx1], &clusters[idx2]);
       // print_clusters(clusters, clustters_count);

        clustters_count = remove_cluster(clusters, clustters_count, idx2);
        //print_clusters(clusters, clustters_count);


    }
   // sort_cluster( clusters);

    print_clusters(clusters, clustters_count);
    for(int i = 0; i < n; i++){
        clear_cluster(&clusters[i]);
    }
    free(clusters);

    return 0;
}