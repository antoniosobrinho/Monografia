/***
ASSIMILACAO POR CAMINHO COM TESTE PRA GUARDAR
A MELHOR SOLUCAO DA TRAJET�RIA SE ELA FOR
MELHOR QUE A P.MELHOR

>>>TEM QUE TRANSFERIR PARA ECS_NUMERICO...

V4470 47 37 9
X0 48 40 11
V4090 x y 10

V4470 32509 176631 451377
X0 18136 43033 117384
W3 79089 203892 495306

***/


//w1.txt 1 .txt 5000000 720 4 30 40 40 10 0.5 1234 1
//w2.txt 1 .txt 5000000 720 14 30 40 40 10 0.5 1234 1
//w3.txt 1 .txt 5000000 720 18 30 40 40 10 0.5 1234 1
//w4.txt 1 .txt 5000000 720 27 30 40 40 10 0.5 1234 1

//instancia.10.15.3A.dat 1 .txt 5000000 720 31 30 40 40 10 0.5 1234 1
//instancia.10.15.4A.dat 1 .txt 5000000 720 29 30 40 40 10 0.5 1234 1
//instancia.10.15.4B.dat 1 .txt 5000000 720 21 30 40 40 10 0.5 1234 1
//instancia.10.20.3A.dat 1 .txt 5000000 720 44 30 40 40 10 0.5 1234 1
//instancia.10.20.3B.dat 1 .txt 5000000 720 27 30 40 40 10 0.5 1234 1
//instancia.10.25.2A.dat 1 .txt 5000000 720 43 30 40 40 10 0.5 1234 1
//instancia.10.25.2B.dat 1 .txt 5000000 720 33 30 40 40 10 0.5 1234 1

//instancia.15.30.4A.dat 1 .txt 5000000 720 77 30 40 40 10 0.5 1234 1
//instancia.15.35.3A.dat 1 .txt 5000000 720 86 30 40 40 10 0.5 1234 1
//instancia.15.40.2A.dat 1 .txt 5000000 720 121 30 40 40 10 0.5 1234 1

//instancia.20.40.3A.dat 1 .txt 5000000 720 107 30 40 40 10 0.5 1234 1

//#define EDCS
#define PAB

#define NUMTIMES 1

//#define FULLINFO

#define GARE "ECSS3."
#define NOXLS
#define NODUMP
#define CONSO
#define NOCONVER
#define VLSI 1
#define MOSP 2
#define OPT


// Aqui troca a função objetivo

//#define DMUR
//#define MSPA
#define TBI

/* PLOTS E CONVERGENCIA */
#define CONVAVAL 10

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cmath>
#include <time.h>
#include <vector>
#include "parametros.h"

/* Problema */
#define MAXGER 1800	//N�mero m�ximo de gera��es
#define NUMCRU 200	//N�mero m�ximo de cruzamentos
#define TAXERR 0.001	//Taxa de Erro

#define MORTUS 200
#define GELADO 1
#define QUENTE 2

#ifdef EDCS
#define ACTIVE	 1
#define INACTIVE 0
#endif

#define NASRECOMBI 1
#define ASCAMINHO 2
#define AAPALFA 0.25

/* Populacao*/
#define PPIOR 0.1

/* Operadores Evolutivos */
#define NUMSELS  2 //N�mero de Indiv�duos selecionados em cada roleta
#define ROLGIRO  6 //N�mero de deslocamentos para a direita da roleta
#define BLXALFA  0.35
#define NI        2
#define MUTNUNI   1
#define PREBATI (rand()%101/100.F)

/* Parametros comuns Busca Local */
#define PLIMI   0.5F

/* Constantes */
#define TRUE 1
#define FALSE 0
#define INFINITO 9999999
#define NDEF 0

#define max(x,y) (x>y?x:y)
#define randi(x,y) (x+((rand()%1001/1000.)*(y-x)))


/*---------------------------------------------------------------------------#
Author : MRS

T�tulo : EVOLUTIONARY CLUSTERING SEARCH - BERTH ALOCATION PROBLEM (BAP)

clock_t clock(void);

*/

/*************** G L O B A I S *******************/

Dados *param;
//float tempo_parada;
FILE *entrada = NULL, *saida = NULL;
int funcao;
int SOLUCAO; //Valor da solu��o
int TIPO;			//tipo do problema (VLSI=1/MOSP=2)
int ROLETA = 1;
int MAXAVA;			//n�mero m�ximo de avalia��es
int TEMPMAX;
int PASSOS = 10;
int LARGO = 10;
int MAXPOP = 10;	//n�mero m�ximo de indiv�duos da popula��o
int NUMCLUS = 20;   //n�mero m�ximo de clusters
int NUMGATES;		//n�mero de elementos da string
int NUMNETS;		//n�mero de pilhas
double MUTPROB = 1; //probabilidade da muta��o
double PROMI = 2.0F; //Fator de controle


typedef unsigned char Byte;


struct MATRIZ
{
    Byte    **string; //Cojuntos de bits (0 ou 1)
    int     numgate; //valor m�ximo que cada elemento da string podem ter
    int		numnet; //n�mero de pilhas
    int     numAval; //Valor do fitness
} Matriz; //Estrutura da matriz do problema


typedef struct
{
    Byte           *var; //string de valores
    int             fit; //fitness da fun��o objetivo
    int             mos; //overlap(trilhas ou m�ximo de pilhas abertas)
    int             tos; //quantidade de n�meros 1's da matriz (incluindo os mo)
    int             sel; //quantidade de vezes que foi selecionado
    int             mut;
    int            *matriz;
} Cromossomo; //Estrutura do Indiv�duo

typedef struct
{
    Cromossomo      *indiv; //indiv�duos da popula��o
    Cromossomo      centr;    // desnecess�rio
    double          sumFit; //soma de todos os fitness da popula��o
    double          media; //m�dia dos fitness da popula��o
    double          dvpad; // desvio padr�o da popula��o
    int             tamPop; //tamanho da popula��o
    int             tamInd; //n�mero de elementos da string do Indiv�duo
    int             melhor; //�ndice da melhor solu��o da popula��o
    int             pior; //pior solu��o dapopula��o
    int             numMuta; //n�mero de muta��es da popula��o
    int             iguais; //quantidade de sele�oes do mesmo indiv�duo
    int             gerMelhor; //gera��o em que foi encontrado o melhor
    int             pai[NUMSELS];       // deixa variavel
#ifdef EDCS
    double			melhorFitness;
    int				*clusterIndexes;
#endif
} Populacao; //Estrutura da popula��o

//Estrutura do cluster
typedef struct
{
    Cromossomo      ponto; //centro do cluster
    int             conta; // Contador de indiv�duos do clusters
    int             stats; //status do cluster (MORTUS, GELADO, QUENTE)

#ifdef EDCS
    int				size;
    int				sizeGlobal;
    int				sizeLocal;
    double			**graph; // grafo que representa centro do cluster
    double			**graphGlobal; // grafo que representa centro do cluster
    double			**graphLocal; // grafo que representa centro do cluster
    double			**graphseq; // grafo que representa sequencias do cluster
    double			***graphseqpos; // grafo que representa sequencias do cluster
    int				indexCenter; //Armazena o �ndice do centro em rela��o � popula��o
    int				numBuscas; //N�mero de vezes que foi realizado a busca local em umdetermindao centro
    int				statsBusca;
#endif
} Centro;

//Estrutura do espa�o de busca (estrutura que cont�m os clusters)
typedef struct
{
    Centro          *grupos; //conjunto de clusters
    int             posGrp; //�ndice da posi��o do �ltimo cluster
    int             numGrp; //quantidade atual de clusters (pos - MORTUS)
    int             maxGrp; //n�mero m�ximo de clusters
#ifdef EDCS
    double          limiar; // valor do threshold (limiar) global
    int				*melhorIndexes;
#else
    int             limiar; // valor do threshold (limiar) global
#endif
    int             densid; // densdidade para medir se o cluster se tornou promissor
} Prototipos;


/* Parametros do EDCS */

int global = 0;

FILE *resultado;
FILE *debug;

/* ************************  GENETICS **************************** */

void print_populacao(Populacao *p)
{

    int i, j;

    fprintf(debug, "\n###################################### POPULACAO ####################################\n");
    for (i = 0; i < p->tamPop; i++)
    {
        fprintf(debug, "Ind %d: (%d) (%d) [", i + 1, p->indiv[i].mos, p->indiv[i].fit);
        for (j = 0; j < p->tamInd - 1; j++)
        {
            fprintf(debug, "%d, ", p->indiv[i].var[j]);
        }
        fprintf(debug, "%d]\n", p->indiv[i].var[p->tamInd - 1]);
    }

    fprintf(debug, "\n\n");
}

float randgen(float fLlim, float fUlim)
{
    float fRandomVal;

    fRandomVal = rand() % 101 / 100.;        // rand entre 0 e 1

    return(fLlim + (float)(fRandomVal * (fUlim - fLlim)));

}

/*Calcula Fitness do Indiv�duo
ALTERA ATRIBUTOS DE: I
*/
#ifdef PAB

int *mares_acima;

void initMareAcima()
{
    mares_acima = new int[param->numeroNavios];
    for (int i = 0; i < param->numeroNavios; i++)
    {
        mares_acima[i] = 0;
    }
}

void deleteMatrinb(int **matrixnb)
{
    for (int i = 0; i < param->numeroNavios; ++i)
        delete[] matrixnb[i];
    delete[] matrixnb;
}

int tempo(int i, int l)
{
    double sum = 0.0f;
    //double aux = 0.0f;

    for (int j = 0; j < param->numeroMaterias; j++)
    {

        sum = sum + param->carga[i][j];
    }

    float val = (double)(sum / param->velocidade_berco[l]);
    return (int)(ceil(val));
}

int **Matrixnb(int *vector)
{
    int** mnb;
    mnb = new int*[param->numeroNavios];
    for (int i = 0; i < param->numeroNavios; i++)
    {
        mnb[i] = new int[param->numeroMares];
    }

    int ini;
    bool flag;

    for (int j = 1; j <= param->numeroBercos; j++)
    {
        ini = 0;
        flag = false;

        for (int k = 0; k < param->numeroNavios; k++)
        {
            if (vector[k] == j)
            {
                if (flag == false)
                {
                    ini = param->mare_chegada[k] - 1;
                    flag = true;
                }
                if (ini <= (param->mare_chegada[k] - 1))
                {
                    ini = param->mare_chegada[k] - 1;
                }

                for (int n = 0; n < param->numeroMares; n++)
                {
                    if (n >= ini && n <= ini + tempo(k, vector[k] - 1) - 1)
                    {
                        mnb[k][n] = 1;
                    }
                    else
                    {
                        mnb[k][n] = 0;
                    }
                }
                ini = ini + tempo(k, vector[k] - 1);
                if (ini > param->numeroMares)
                {
                    mares_acima[k] = ini - param->numeroMares - 1;
                }
                else
                {
                    mares_acima[k] = 0;
                }
            }
        }
    }
    return mnb;
}

void CalculaGPAB(Cromossomo *I)
{
    double sum = 0.0f;

    int valorNavio = 1;

#ifdef MSPA
    int mareIni, mareFim;
#endif // MSPA



    int *vector = (int *)calloc(param->numeroNavios, sizeof(int));

    initMareAcima();

    for (int v = 0; v < NUMGATES; v++)
    {
        if (I->var[v] > param->numeroNavios)
        {
            valorNavio++;
        }
        else
        {
            vector[I->var[v] -1] = valorNavio;
        }
    }

//    for (int v = 0; v < param->numeroNavios; v++) {
//    	printf("%d ", vector[v]);
//    }
//
//    printf("\n");

    int **aux = Matrixnb(vector);
    I->matriz = vector;


    double t = 0.0f;

    int cont1 = 0, cont2 = 0, cont3 = 0;

#ifdef DMUR

    for (int i = 0; i < param->numeroNavios; i++)
    {
        for (int j = 0; j <  param->numeroMares; j++)
        {
            t = (double)tempo(i, vector[i]-1);
            sum = sum + ceil((j)*(param->demurrage[i]/(t)))*aux[i][j]; // o t daqui � o hil da disserta��o do VH

        }
    }

#endif // DMUR
#ifdef TBI
	for (int i = 0; i < param->numeroNavios;i++) {
		for (int j = param->mare_chegada[i]; j <= param->numeroMares; j++) {
			t = (double)tempo(i, vector[i] - 1);
			sum = sum + ceil((j - param->mare_chegada[i] + 1) / (t))*aux[i][j - 1];
		}
	}

#endif

#ifdef MSPA

    mareIni=-1;
    mareFim= -1;


    for (int j = 0; j <param->numeroMares; j++)

    {
        for (int i = 0; i < param->numeroNavios; i++)

        {
            if ((aux[i][j])&&(mareIni == -1))
            {
                mareIni = j;
            }
            else if(aux[i][j])
            {
                mareFim = j;
            }


        }
    }

    sum = mareFim - mareIni +1;

#endif // MSPA


    for (int i = 0; i < param->numeroNavios; i++)
    {
        if (mares_acima[i] > 0)
        {
            sum = sum + param->numeroMares*10;   // ACMO
            cont1++;
        }
    }

    for (int i = 0; i < param->numeroNavios; i++)
    {
        int val = 0;
        int auxsum = 0;
        for (int j = 0; j < param->numeroMares; j++)
        {
            auxsum = auxsum + aux[i][j];
            if (auxsum > 0)
            {
                val = j + 1;
                break;
            }
        }
        if (val < param->mare_chegada[i] && val > 0)
        {
            sum = sum + param->numeroMares*100;   // ACMO
            cont2++;
        }
    }

    int temp = 0;

    double *mat = new double[param->numeroMaterias];
    for (int t = 0; t < param->numeroMaterias; t++)
    {
        mat[t] = 0.0f;
    }

    double sum2 = 0.0f;
    for (int m = 0; m < param->numeroMares; m++)
    {
        for (int c = 0; c < param->numeroMaterias; c++)
        {
            for (int n = 0; n < param->numeroNavios; n++)
            {
                temp = tempo(n, vector[n] - 1);
                for (int j = 0; j <= m; j++)
                {
                    sum2 = sum2 + ((param->carga[n][c]) / ((double)(temp)))*aux[n][j];
                }
            }
            if (sum2 < ((m + 1)*param->consumo[c] - param->estoque[c]))
            {
                sum = sum + param->numeroMares*10;   // ACMO;
                cont3++;
            }
            sum2 = 0.0f;
        }
    }


    free(vector);
    free(mat);

    for (int i = 0; i < param->numeroNavios; i++)
    {
        free(aux[i]);
    }

    free(aux);
    free(mares_acima);

    I->fit = (int)(ceil(sum));
    I->mos = (int)(ceil(sum));
    I->sel = 0;
    I->mut = 0;
    Matriz.numAval++;

}
#else
void CalculaG(Cromossomo *I)
{
    Byte i, j;
    int max, custo, acum;
    Byte *esq, *dir;

    esq = (Byte *)malloc(sizeof(Byte) * Matriz.numnet);
    dir = (Byte *)malloc(sizeof(Byte) * Matriz.numnet);
    if (!esq || !dir)
    {
        puts("CALCULAG_MSG_MEMOY_FAULT");
        exit(12);
    }
    for (j = 0; j < Matriz.numnet; j++)
    {
        esq[j] = dir[j] = 0;
        for (i = 0; i < Matriz.numgate; i++)
        {
            if (Matriz.string[j][I->var[i] - 1] == '1')
            {
                if (!esq[j])    esq[j] = i + 1;
                else            dir[j] = i + 1;
            }
        }
        if (!dir[j]) dir[j] = esq[j];
    }
    /* calculo das trilhas, custo e do numero de #�s */
    for (i = 0, max = 0, custo = 0; i< Matriz.numgate; i++)
    {
        for (j = 0, acum = 0; j< Matriz.numnet; j++)
            if (i >= esq[j] - 1 && i <= dir[j] - 1) acum += 1;
        max = (acum > max ? acum : max);
        custo += acum;
    }
    I->fit = Matriz.numgate * Matriz.numnet * max + custo;
    I->mos = max;
    I->tos = custo;
    I->sel = 0;
    I->mut = 0;
    free(esq);
    free(dir);
    Matriz.numAval++;
}
#endif

/*Inicia a popula��o
ALTERA ATRIBUTOS DE: c
*/
void IniciaPop(Populacao *p, int mp, int mv)
{
    int i;
    Byte j;
    int melhor, pior, pos;
    double soma;

    //#Aloca os indiv�duos na Popula��o
    p->indiv = (Cromossomo *)malloc(sizeof(Cromossomo)*mp);
    if (p->indiv == NULL)
    {
        fprintf(saida, "ERRO(2a): Problemas de memoria!!!");
        exit(-1);
    }
    // Aloca, Gera e avalia individuos
    for (i = 0, soma = 0, pior = 0, melhor = 0; i < mp; i++)
    {
        p->indiv[i].var = (Byte *)malloc(sizeof(Byte)*mv);
        if (p->indiv[i].var == NULL)
        {
            fprintf(saida, "ERRO(2b): Problemas de memoria!!!");
            exit(-1);
        }
        // inicia todo o individuo com -1 (CORRIGIDO: com 0)
        for (j = 0; j < Matriz.numgate; j++)
            p->indiv[i].var[j] = NDEF;
        // gera todos os valores da permuta sequencialmente
        for (j = 1; j <= Matriz.numgate; j++)
        {
            // sorteia a posicao aonde cada valor vai ficar
            // se j� definido algo l�, procura outra posicao para valor
            pos = rand() % Matriz.numgate;
            while (p->indiv[i].var[pos] != NDEF)
            {
                pos = (++pos) % Matriz.numgate;
            }
            p->indiv[i].var[pos] = j;
        }

        //Calcula Fitness
#ifdef PAB
        CalculaGPAB(&(p->indiv[i]));
#else
        CalculaG(&(p->indiv[i]));
#endif

        if (p->indiv[i].fit > p->indiv[pior].fit) /*regras de minimizacao*/
            pior = i;
        if (p->indiv[i].fit < p->indiv[melhor].fit)
            melhor = i;
        soma += p->indiv[i].fit;
    }

    p->tamPop = mp; //Tamnanho da popula��o
    p->tamInd = mv; //n�mero de elementos da string
    p->numMuta = 0; //quantidade de muta��es
    p->iguais = 0;
    p->sumFit = soma; //Somat�ria dos fitness da popula��o
    p->melhor = melhor; //�ndice do melhor individuo
    p->pior = pior; //�ndice do pior individuo
    p->media = p->sumFit / p->tamPop; //m�dia dos fitness da popula��o

#ifdef EDCS
    p->clusterIndexes = (int *)calloc(mp, sizeof(int));
    for (int m = 0; m < mp; m++)
    {
        p->clusterIndexes[m] = -1;
    }
#endif
}

/*Cria um conjunto de clusters
ALTERA ATRIBUTOS DE: c
*/
void IniciaCls(Prototipos * c, int mc, int mv)
{
    int i;
    c->grupos = (Centro *)malloc(sizeof(Centro)*NUMCLUS);

    for (i = 0; i < mc; i++)
    {
        c->grupos[i].conta = 0;
        c->grupos[i].stats = MORTUS;
        c->grupos[i].ponto.var = (Byte *)malloc(sizeof(double)*mv);
        if (c->grupos[i].ponto.var == NULL)
        {
            fprintf(saida, "ERRO/IniciaCls(#1): Memoria!!!");
            exit(-1);
        }
    }
    c->maxGrp = mc;
    c->posGrp = 0;
    c->numGrp = 0;
    c->limiar = 0;
}

/*Seleciona dois indiv�duos utilizando press�o auto-adaptativa
ALTERA ATRIBUTOS DE: p
*/
void RoletaPressaoSeletiva(Populacao *p, int melhorfit)
{
    int i, pos;
    int fator; //N�mero de Vezes que o indiv�duo foi selecionado
    int sel; //N�mero m�ximo de indiv�duos selecionados
    double z; //Avali��o Seletiva
    double gatilho, acum;

    sel = 0;
    while (sel < NUMSELS)
    {
        pos = rand() % p->tamPop; //pega uma posi��o aleat�ria da popula��o
        fator = (p->indiv[pos].sel>3 ? 3 : p->indiv[pos].sel); //Verifica se o indiv�duo foi selecionado mais de 3 vezes
        z = (1.0F / pow((p->indiv[pos].fit - melhorfit + 1), fator));
        gatilho = z * (ROLGIRO - (ROLGIRO - 1)*z);
        acum = 0;
        i = 0;
        while (acum < gatilho && i <= ROLGIRO)
        {
            pos = (pos < p->tamPop - 1 ? pos + 1 : 0); //desloca "pos" uma posi��o para a direita
            fator = (p->indiv[pos].sel>3 ? 3 : p->indiv[pos].sel);
            z = 1.0F / pow((p->indiv[pos].fit - melhorfit + 1), fator);
            acum += z;
            i++;
        }
        p->indiv[pos].sel++;
        p->pai[sel] = pos;
        sel++;
    }
}

void AssimilaBOX(Cromossomo *c, Cromossomo *p)
{
    int i, j, k, cont;

    Byte *q, *r;

    q = (Byte*)malloc(sizeof(Byte) * Matriz.numgate);
    r = (Byte*)malloc(sizeof(Byte) * Matriz.numgate);

    if (!q || !r)
    {
        puts("ASSBOX_MSG_MEMOY_FAULT");
        exit(-2);
    }

    cont = Matriz.numgate;
    k = Matriz.numgate / 2;


    for (i = 0; i< Matriz.numgate; i++)
        r[i] = q[i] = NDEF;

    for (i = 0; i <= k; i++)
    {
        if ((j = rand() % Matriz.numgate))
        {
            r[j] = c->var[j];
            cont -= !q[r[j] - 1];
            q[r[j] - 1] = 1;
        }
    }
    for (i = 0, j = 0; i < Matriz.numgate && cont; i++)
    {
        if (!r[i])
        {
            while (q[p->var[j++] - 1]);
            r[i] = p->var[j - 1];
            cont--;
        }
    }
    // centro assume o r calculado
    memcpy(c->var, r, Matriz.numgate);
    free(q);
    free(r);
}

/*N�O USA*/
void AssimilaPATHII(Cromossomo *c, Cromossomo *p)
{
    int i, j, k, spin, cont, incr;

    Byte *defs;
    Cromossomo temp;

    //aloca memoria
    temp.var = (Byte*)malloc(sizeof(Byte)* Matriz.numgate);

    if (!temp.var)
    {
        puts("ASSBOX_MSG_MEMOY_FAULT");
        exit(-2);
    }

    // copia c em temp
    memcpy(temp.var, c->var, Matriz.numgate);

    spin = rand() % 2;
    incr = (double)AAPALFA*Matriz.numgate;

    // sentido do giro (dir-esq)
    if (spin)
    {
        for (i = Matriz.numgate - 1; i >= 0; i -= incr)
        {
            if (temp.var[i] != p->var[i])
            {
                // procura no individuo pelo elemento para troca
                for (j = 0; j<i && temp.var[j] != p->var[i]; j++);
                k = temp.var[i];
                temp.var[i] = temp.var[j];
                temp.var[j] = k;
#ifdef PAB
                CalculaGPAB(&temp);
#else
                CalculaG(&temp);
#endif
                // se � melhor que o centro anterior
                if (temp.fit < c->fit)
                {
                    // atualiza com a nova melhor posicao
                    memcpy(c->var, temp.var, Matriz.numgate);
                    c->fit = temp.fit;
                    c->mos = temp.mos;
                    c->tos = temp.tos;
                }
            }
        }
    }
    else
    {
        for (i = 0; i<Matriz.numgate; i += incr)
        {
            if (temp.var[i] != p->var[i])
            {
                // procura no individuo pelo elemento para troca
                for (j = Matriz.numgate - 1; j>i && temp.var[j] != p->var[i]; j--);
                k = temp.var[i];
                temp.var[i] = temp.var[j];
                temp.var[j] = k;
#ifdef PAB
                CalculaGPAB(&temp);
#else
                CalculaG(&temp);
#endif
                // se � melhor que o centro anterior
                if (temp.fit < c->fit)
                {
                    // atualiza com a nova melhor posicao
                    memcpy(c->var, temp.var, Matriz.numgate);
                    c->fit = temp.fit;
                    c->mos = temp.mos;
                    c->tos = temp.tos;
                }
            }
        }
    }

    // por ultimo, testa o ponto assimilado
    if (p->fit < c->fit)
    {
        memcpy(c->var, temp.var, Matriz.numgate);
        c->fit = p->fit;
        c->mos = p->mos;
        c->tos = p->tos;
    }

}

/*Faz o processo de assimila��o utilizando 2-Troca
ALTERA ATRIBUTOS DE: c
*/
void AssimilaPATH(Cromossomo *c, Cromossomo *p)
{
    int i, j, k, cont;
    int auxi; //vari�vel para auxilar na troca
    int maxt; //n�mero m�ximo de trocas

    Cromossomo temp;
    Byte *defs;

    //aloca memoria
    temp.var = (Byte*)malloc(sizeof(Byte)* Matriz.numgate);
    defs = (Byte*)malloc(sizeof(Byte)* Matriz.numgate);

    if (!defs || !temp.var)
    {
        puts("ASSBOX_MSG_MEMOY_FAULT");
        exit(-2);
    }

    // copia c em temp
    memcpy(temp.var, c->var, Matriz.numgate);
    for (j = 0; j < Matriz.numgate; j++)
    {
        defs[j] = NDEF;  //NDEF = 0
    }
    maxt = 1.0F / AAPALFA;

    for (k = 0; k<maxt; k++)
    {
        i = rand() % Matriz.numgate;
        while (defs[i] != NDEF)
        {
            i = (++i) % Matriz.numgate; //Desloca contador para direita at� encontrar um valor diferente de NDEF
        }
        if (temp.var[i] != p->var[i])
        {
            // procura no individuo pelo elemento para troca
            for (j = 0; j < Matriz.numgate && temp.var[j] != p->var[i]; j++);

            //realiza a troca de valores
            auxi = temp.var[i];
            temp.var[i] = temp.var[j];
            temp.var[j] = auxi;

            //Calculao fitness do novo indiv�duo gerado
#ifdef PAB
            CalculaGPAB(&temp);
#else
            CalculaG(&temp);
#endif
            // se � melhor que o centro anterior
            if (temp.fit < c->fit)
            {
                // atualiza com a nova melhor posicao
                memcpy(c->var, temp.var, Matriz.numgate);
                c->fit = temp.fit;
                c->mos = temp.mos;
                c->tos = temp.tos;
            }
        }
        defs[i] = TRUE;
    }

    // por ultimo, testa o ponto assimilado
    if (p->fit < c->fit)
    {
        //memcpy(c->var, temp.var, Matriz.numgate); //??(temp??)
        memcpy(c->var, p->var, Matriz.numgate); //CORRIGIDO
        c->fit = p->fit;
        c->mos = p->mos;
        c->tos = p->tos;
    }

    free(temp.var);
    free(defs);

}

void RecombinaBOX(Populacao *p, int base, int guia, int filho)
{
    int i, j, k, cont;

    Byte *q;

    q = (Byte*)malloc(sizeof(Byte) * Matriz.numgate);
    if (!q)
    {
        puts("BOX_MSG_MEMOY_FAULT");
        exit(-2);
    }

    cont = Matriz.numgate;
    k = Matriz.numgate / 2;


    for (i = 0; i< Matriz.numgate; i++)
        p->indiv[filho].var[i] = q[i] = NDEF;

    for (i = 0; i <= k; i++)
    {
        if ((j = rand() % Matriz.numgate))
        {
            p->indiv[filho].var[j] = p->indiv[base].var[j];
            cont -= !q[p->indiv[filho].var[j] - 1];
            q[p->indiv[filho].var[j] - 1] = 1;
        }
    }
    for (i = 0, j = 0; i < Matriz.numgate && cont; i++)
    {
        if (!p->indiv[filho].var[i])
        {
            while (q[p->indiv[guia].var[j++] - 1]);
            p->indiv[filho].var[i] = p->indiv[guia].var[j - 1];
            cont--;
        }
    }
    free(q);
}

void Opt(Cromossomo * I, int p, int q)
{
    int  i, j;
    Cromossomo aux;
    aux.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    memcpy(aux.var, I->var, Matriz.numgate);
    if (p + 1 == q)
    {
        j = 0;
        for (i = q; i < Matriz.numgate; i++, j++)
            I->var[j] = aux.var[i];
        for (i = 0; i <= p; i++, j++)
            I->var[j] = aux.var[i];
    }
    else
    {
        j = p + 1;
        for (i = q; i >= p + 1; i--, j++)
            I->var[j] = aux.var[i];
    }
    free(aux.var);
}

Byte MutacaoSwap(Cromossomo * I, float pmut)
{
    Byte     mutou = FALSE, x, y, temp;

    for (x = 0; x < Matriz.numgate; x++)
    {
        if (rand() % 100 < pmut)
        {
            mutou = TRUE;
            y = rand() % Matriz.numgate;
            temp = I->var[x];
            I->var[x] = I->var[y];
            I->var[y] = temp;
        }// if prob
    } // for
    return mutou;
}

int Mutacao2Opt(Cromossomo * I, int LARGUR)
{
    Cromossomo J, K;
    int i, j, sigmax = 0;
    int y, z;
    J.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    K.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    if (J.var == NULL || K.var == NULL)
    {
        puts("M2OPT_MSG_MEMORY_FAULT");
        exit(335);
    }
    y = (Matriz.numgate <= LARGUR ? 0 : rand() % (Matriz.numgate - LARGUR));
    z = y + (Matriz.numgate <= LARGUR ? Matriz.numgate : LARGUR); //o n�mero de posi��es trocadas � entre NUMGATES e 40 ou entre 40 e NUMGATES
    for (i = y, sigmax = 0; i<z; i++)
    {
        for (j = i + 1; j<z; j++)
        {
            memcpy(J.var, I->var, Matriz.numgate);
            Opt(&J, i, j);
#ifdef PAB
            CalculaGPAB(&J);
#else
            CalculaG(&J);
#endif
            if (I->fit - J.fit > sigmax)
            {
                memcpy(K.var, J.var, Matriz.numgate);
                K.fit = J.fit;
                K.mos = J.mos;
                K.tos = J.tos;
                sigmax = I->fit - J.fit;
            }
        }
    }
    if (sigmax > 0)
    {
        memcpy(I->var, K.var, Matriz.numgate);
        I->fit = K.fit;
        I->mos = K.mos;
        I->tos = K.tos;
        I->mut++;
        I->sel = 0;
        free(J.var);
        free(K.var);
        return TRUE;
    }
    free(J.var);
    free(K.var);
    return FALSE;
}

int Mutacao2Chg(Cromossomo * I, int LARGUR)
{
    Cromossomo J, K;
    int i, j, sigmax = 0;
    int y, z;
    Byte temp;

    J.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    K.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    if (J.var == NULL || K.var == NULL)
    {
        puts("M2OPT_MSG_MEMORY_FAULT");
        exit(335);
    }
    y = (Matriz.numgate <= LARGUR ? 0 : rand() % (Matriz.numgate - LARGUR));
    z = y + (Matriz.numgate <= LARGUR ? Matriz.numgate : LARGUR);
    for (i = y, sigmax = 0; i<z; i++)
    {
        for (j = i + 1; j<z; j++)
        {
            memcpy(J.var, I->var, Matriz.numgate);
            // MOVIMENTO 2-Troca
            temp = J.var[i];
            J.var[i] = J.var[j];
            J.var[j] = temp;
            //-------------------
#ifdef PAB
            CalculaGPAB(&J);
#else
            CalculaG(&J);
#endif
            if (I->fit - J.fit > sigmax)
            {
                memcpy(K.var, J.var, Matriz.numgate);
                K.fit = J.fit;
                K.mos = J.mos;
                K.tos = J.tos;
                sigmax = I->fit - J.fit;
            }
        }
    }
    if (sigmax > 0)
    {
        memcpy(I->var, K.var, Matriz.numgate);
        I->fit = K.fit;
        I->mos = K.mos;
        I->tos = K.tos;
        I->mut++;
        I->sel = 0;
        free(J.var);
        free(K.var);
        return TRUE;
    }
    free(J.var);
    free(K.var);
    return FALSE;
}

/*Calcula dist�ncia entre indiv�duos (SIMILARIDADE)*/
int DistSeq(Byte *x, Byte *y, int n)
{
    int i = n - 1, cont = 0;

    //A dist�ncia � baseada no n�mero de elementos diferentes entre indiv�duos
    do
    {
        cont += (x[i] != y[i]);
    }
    while (i--);

    return cont;
}

/*Faz o processo de similaridade e chama a fun��o de assimila��o
ALTERA ATRIBUTOS DE : c, p
*/
int AtualizaGrp(Prototipos *c, Populacao *p)
{
    int i, j, k;
    int indice, pertice;
    int dispice; //�ndice da �ltima posi��o do grupo de clusters
    int dist, menorDist;
    int maiorCont = 0; //Armazena a maior densidade entre os clusters
    int numsels = NUMSELS;
    char pertence; //Flag para verificar se o indiv�duo pertence a algum cluster


    for (i = 0; i<numsels; i++)
    {
        pertence = FALSE;
        menorDist = INFINITO;
        //insercao a priori na ultima posicao
        dispice = c->posGrp;
        for (j = 0; j < c->posGrp; j++)
        {
            if (c->grupos[j].stats != MORTUS)
            {
                dist = DistSeq(p->indiv[p->pai[i]].var, c->grupos[j].ponto.var, p->tamInd);
                if (dist < c->limiar && !pertence)
                {
                    pertence = TRUE;
                    pertice = j;            // relativo a pertincia

                }
                if (dist < menorDist)
                {
                    menorDist = dist;
                    indice = j;             // relativo a assimila��o
                }
            }
            else
                dispice = j;
        }
        // se n�o pertencer a ninguem o ultimo mortus sera usado
        // se n�o entrar nenhuma vez no if, vale a inicializacao (ultimo centro)
        if (!pertence &&  !(dispice >= c->maxGrp))
        {
            // salva o primeiro ponto e seu fitness
            memcpy(c->grupos[dispice].ponto.var, p->indiv[p->pai[i]].var, Matriz.numgate);
            c->grupos[dispice].ponto.fit = p->indiv[p->pai[i]].fit;
            c->grupos[dispice].ponto.mos = p->indiv[p->pai[i]].mos;
            c->grupos[dispice].ponto.tos = p->indiv[p->pai[i]].tos;
            c->grupos[dispice].ponto.sel = p->indiv[p->pai[i]].sel;
            c->grupos[dispice].ponto.mut = p->indiv[p->pai[i]].mut;
            // Cluster come�a gelado
            c->grupos[dispice].conta = 1;
            c->grupos[dispice].stats = GELADO;
            // aumenta posGrp se inseriu na ultima posicao
            c->posGrp += (dispice == c->posGrp);
            // independente disso, aumenta o num
            c->numGrp++;
        }
        else   // ou pertence ou nao cabe mais clusters, entao assimila o cluster mais proximo
        {
            if (menorDist >= 4)
            {
                //--------------------------------------------------
                // protege de assimila��o pontos muito pr�ximos ao centro
                // DOIS TIPOS DE ASSIMILA��O
                //--------------------------------------------------
#ifdef ASRECOMBI
                // N�o h� avaliacao do novo centro, at� antes da BLOCAL
                AssimilaBOX(&(c->grupos[indice].ponto), &(p->indiv[p->pai[i]]));
#endif

#ifdef ASCAMINHO
                AssimilaPATH(&(c->grupos[indice].ponto), &(p->indiv[p->pai[i]]));
                if (c->grupos[indice].ponto.fit < p->indiv[p->melhor].fit)   //Atualiza as informa��es do melhor indiv�duo
                {
                    memcpy(p->indiv[p->melhor].var, c->grupos[indice].ponto.var, Matriz.numgate);
                    p->indiv[p->melhor].fit = c->grupos[indice].ponto.fit;
                    p->indiv[p->melhor].mos = c->grupos[indice].ponto.mos;
                    p->indiv[p->melhor].tos = c->grupos[indice].ponto.tos;
                    p->indiv[p->melhor].sel = c->grupos[indice].ponto.sel;
                    p->indiv[p->melhor].mut = c->grupos[indice].ponto.mut;
                }
#endif

            } //if
            if (pertence)   // pertinente � o cluster proximo (limiar) mais antigo (primeiro a ser encontrado)
            {
                c->grupos[pertice].conta++;
                // guarda a maior contagem
                if (c->grupos[pertice].conta > maiorCont)
                    maiorCont = c->grupos[pertice].conta;

                // independente de ser o mesmo ou n�o, o cluster � esquentado
                c->grupos[pertice].stats = QUENTE;

                // se houver dispice do lado esquerdo de pos, pos � decrementado
                c->posGrp -= (c->posGrp == dispice + 1);
            }//if
        } // else (pertence)

    }
    return(maiorCont);
}

void AtualizaPop(Populacao *p, int pos, int ger)
{
    int i, j, salto, maxIt;

    if (p->indiv[pos].fit < p->indiv[p->melhor].fit)
    {
        p->melhor = pos;
        p->gerMelhor = ger;
    }

    /* ***** procura um outro pior ******** */
    maxIt = (int)ceil(PPIOR*p->tamPop);
    salto = (int)ceil(maxIt / 3.0F);
    for (i = 0; i < maxIt; i++)
    {
        j = rand() % p->tamPop;
        if (p->indiv[j].fit > p->indiv[p->pior].fit)
        {
            p->pior = j;
            i += salto;
        }
    }
}

void EstratIIIntenso(Prototipos& C, Populacao& P, int& bLocOk, int& bLocTot, int PASSOS, int LARGO)
{
    int index;
    int  fitant;
    int k;



#ifdef EDCS
    Centro C1;
    for (int i = 0; i < P.tamPop; i++)
    {
        if (P.indiv[i].mos <= (P.indiv[P.melhor].mos))
        {
            P.melhorFitness = P.indiv[0].fit;
            C1.ponto = P.indiv[i];
#ifdef PAB
            CalculaGPAB(&(C1.ponto));
#else
            CalculaG(&(C1.ponto));
#endif


#ifdef ASCAMINHO
            // N�o precisa avaliar o centro
            fitant = C1.ponto.fit;
#else
            // precisa avaliar o centro, pois em ASSIMILABOX isso nao foi feito
            CalculaG(&(C.grupos[index].ponto));
            fitant = C.grupos[index].ponto.fit;
#endif
            k = PASSOS + 2 * (bLocTot - bLocOk);

#ifdef CHG
            while (k-- && Mutacao2Chg(&(C.grupos[index].ponto), LARGO));
#endif

#ifdef OPT
            while (k-- && Mutacao2Opt(&(C1.ponto), LARGO));
#endif
            if (C1.ponto.fit < P.indiv[P.melhor].fit)
            {
                memcpy(P.indiv[P.melhor].var, C1.ponto.var, Matriz.numgate);
                P.indiv[P.melhor].fit = C1.ponto.fit;
                P.indiv[P.melhor].mos = C1.ponto.mos;
                P.indiv[P.melhor].tos = C1.ponto.tos;
                P.indiv[P.melhor].sel = C1.ponto.sel;
                P.indiv[P.melhor].mut = C1.ponto.mut;
                bLocOk++;
            }
            bLocTot++;
        }
    }
#else
    for (index = 0; index < C.posGrp; index++)   //para cada clusters
    {
        if (C.grupos[index].conta >= C.densid)
        {
#ifdef ASCAMINHO
            // N�o precisa avaliar o centro
            fitant = C.grupos[index].ponto.fit;
#else
            // precisa avaliar o centro, pois em ASSIMILABOX isso nao foi feito
            CalculaG(&(C.grupos[index].ponto));
            fitant = C.grupos[index].ponto.fit;
#endif
            k = PASSOS + 2 * (bLocTot - bLocOk);
#ifdef CHG
            while (k-- && Mutacao2Chg(&(C.grupos[index].ponto), LARGO));
#endif

#ifdef OPT
            while (k-- && Mutacao2Opt(&(C.grupos[index].ponto), LARGO));
#endif
            if (C.grupos[index].ponto.fit < P.indiv[P.melhor].fit)
            {
                memcpy(P.indiv[P.melhor].var, C.grupos[index].ponto.var, Matriz.numgate);
                P.indiv[P.melhor].fit = C.grupos[index].ponto.fit;
                P.indiv[P.melhor].mos = C.grupos[index].ponto.mos;
                P.indiv[P.melhor].tos = C.grupos[index].ponto.tos;
                P.indiv[P.melhor].sel = C.grupos[index].ponto.sel;
                P.indiv[P.melhor].mut = C.grupos[index].ponto.mut;
                bLocOk++;
            }
            bLocTot++;
            //                        if ( C.grupos[index].ponto.fit < fitant)
            //                        	bLocOk++;
            //reinicia so se foi verificado
            C.grupos[index].conta = 1;
        }
    }
#endif
    return;
}

/*Esfria Clusters rebaixando-os em uma posi��o (QUENTE vira GELADO ;GELADO vira MORTUS)*/
void EsfriaGrupos(Prototipos& C)
{
    int index;

    for (index = 0; index < C.posGrp && C.numGrp > 1; index++)
    {
        if (C.grupos[index].stats > MORTUS)
        {
            C.grupos[index].stats--;
            C.grupos[index].conta = 1;
            C.numGrp -= (C.grupos[index].stats == MORTUS);
        }
    }
    return;
}

#ifdef EDCS

void counting_sort(Populacao *p, int max)
{

    Cromossomo *id = p->indiv;

    Cromossomo *val = (Cromossomo *)calloc(NUMGATES, sizeof(Cromossomo));

    int *vetorAux = (int *)calloc(max + 1, sizeof(int));

    for (int i = 0; i <= max; i++)
    {
        vetorAux[i] = 0;
    }

    for (int i = 0; i < p->tamPop; i++)
    {
        vetorAux[p->indiv[i].fit]++;
    }

    int sum = 0;

    int dum;

    for (int i = 0; i <= max; i++)
    {
        dum = vetorAux[i];
        vetorAux[i] = sum;
        sum += dum;
    }
    Cromossomo *vetorOrdenado = (Cromossomo *)calloc(p->tamPop, sizeof(Cromossomo));

    for (int i = 0; i < p->tamPop; i++)
    {
        vetorOrdenado[vetorAux[p->indiv[i].fit]] = id[i];
        vetorAux[p->indiv[i].fit]++;
    }

    for (int i = 0; i < p->tamPop; i++)
    {
        id[i] = vetorOrdenado[i];
    }

    free(val);
    free(vetorAux);
    free(vetorOrdenado);
}

void swap(Populacao *p, int x, int y)
{
    Cromossomo c = p->indiv[x];
    Cromossomo aux;

    aux = c;
    c = p->indiv[y];
    p->indiv[y] = aux;

}

void print_cluster(FILE *file, Centro *c, int index)
{
    int i, j;


    fprintf(file, "\nCLUSTER %d (SIZE %d) ##########################################################################\n", index, c->conta);
    //printf("    ");
#ifdef FULLINFO
    fprintf(file, "    ");
    for (i = 0; i < NUMGATES; i++)
    {
        //printf("%.05d  ", i);
        fprintf(file, "%.05d  ", i + 1);
    }
    //printf("SIZES");
    fprintf(file, "SIZES");

    //printf("\n");
    fprintf(file, "\n");
    //printf("    ");
    fprintf(file, "    ");
    for (i = 0; i < NUMGATES; i++)
    {
        //printf("--", i);
        fprintf(file, "--", i);
    }
    //printf("\n");
    fprintf(file, "\n");
    for (i = 0; i < NUMGATES; i++)
    {
        //printf("%d:  ", i);
        fprintf(file, "%d:  ", i);
        for (j = 0; j < NUMGATES; j++)
        {
            //printf("%.3f  ", clusters->graph[i][j]);
            fprintf(file, "%.3f  ", c->graph[i][j]);
        }
        //printf("%.05d\n", clusters->sizes[i]);
        fprintf(file, "%.05d\n", c->conta);
    }
    //printf("\n");
    fprintf(file, "\n");

    if (c->conta > 0)
    {
        for (i = 0; i < NUMGATES; i++)
        {
            //printf("%d:  ", i);
            fprintf(file, "%d:  ", i);
            for (j = 0; j < NUMGATES - 1; j++)
            {
                //printf("%.3f  ", clusters->graph[i][j] / (float)clusters->sizes[i]);
                fprintf(file, "%.3f  ", c->graph[i][j] / (float)c->conta);
            }
            //printf("%.3f\n", clusters->graph[i][j] / (float)clusters->sizes[i]);
            fprintf(file, "%.3f\n", c->graph[i][j] / (float)c->conta);
        }
    }
#endif
}

void IniciaClsEDCS(Prototipos * c, int mc, int mv)
{


    int i, j, k;
    //Inicializa as Redes Bayesianas
    c->grupos = (Centro *)malloc(sizeof(Centro)*mc);
    for (i = 0; i < NUMCLUS; i++)
    {
        c->grupos[i].graphLocal = (double **)calloc(mv, sizeof(double*));
        for (j = 0; j < mv; j++)
        {
            c->grupos[i].graphLocal[j] = (double *)calloc(mv, sizeof(double));
        }

        for (j = 0; j < mv; j++)
        {
            for (k = 0; k < mv; k++)
            {
                c->grupos[i].graphLocal[j][k] = 1.0f;
            }
        }

        for (k = 0; k < mv; k++)
        {
            c->grupos[i].graphLocal[k][k] = 0.0f;
        }
    }

    for (i = 0; i < NUMCLUS; i++)
    {
        c->grupos[i].graphGlobal = (double **)calloc(mv, sizeof(double*));
        for (j = 0; j < mv; j++)
        {
            c->grupos[i].graphGlobal[j] = (double *)calloc(mv, sizeof(double));
        }

        for (j = 0; j < mv; j++)
        {
            for (k = 0; k < mv; k++)
            {
                c->grupos[i].graphGlobal[j][k] = 1.0f;
            }
        }
    }


    c->melhorIndexes = (int *)calloc(mc, sizeof(int));
    for (k = 0; k < mc; k++)
    {
        c->melhorIndexes[k] = 0;
    }

    for (i = 0; i < mc; i++)
    {
        c->grupos[i].conta = 0;
        c->grupos[i].stats = ACTIVE;
        c->grupos[i].sizeGlobal = mv;
        c->grupos[i].sizeLocal = mv - 1;
        c->grupos[i].indexCenter = -1;
        c->grupos[i].numBuscas = 0;
        c->grupos[i].ponto.var = (Byte *)malloc(sizeof(double)*mv);
        if (c->grupos[i].ponto.var == NULL)
        {
            fprintf(saida, "ERRO/IniciaCls(#1): Memoria!!!");
            exit(-1);
        }
    }
    c->maxGrp = mc;
    c->posGrp = 0;
    c->numGrp = 0;
    c->limiar = 0;

}

int exist_value(Byte *vector, int lenght, int val)
{
    int i = 0;

    for (int i = 0; i < lenght; i++)
    {
        if (vector[i] == val)
        {
            return 1;
        }
    }
    return 0;
}

int exist_valueInt(int *vector, int lenght, int val)
{
    int i = 0;

    for (int i = 0; i < lenght; i++)
    {
        if (vector[i] == val)
        {
            return 1;
        }
    }
    return 0;
}

void AssimilaProbabilistic(Cromossomo *c, Centro *g)
{
    int n, x, y;

    for (n = 0; n < NUMGATES; n++)
    {
        g->graphGlobal[n][c->var[n] - 1] += 1;
    }

    for (n = 0; n < NUMGATES - 1; n++)
    {
        x = c->var[n] - 1;
        y = c->var[n + 1] - 1;
        g->graphLocal[x][y] += 1;
    }
    g->graphLocal[c->var[NUMGATES - 1] - 1][c->var[0] - 1] += 1;

    g->sizeGlobal++;
    g->sizeLocal++;
    g->conta++;
}

double SimilarityEDCS(Centro &c, Cromossomo *ind)
{

    double rank = 1.0f;

    int i;
    double val;
    for (i = 0; i < NUMGATES; i++)
    {
        val = c.graphGlobal[i][ind->var[i] - 1] / (double)c.sizeGlobal;
        rank = rank + log(val);
    }
    return rank;
}

int AtualizaGrpEDCS(Prototipos *c, Populacao *p)
{
    int i, j, k, n;
    int maiorCont = 0; //Armazena a maior densidade entre os clusters
    int melhor = INFINITO;

    double rankGlobal = 0.0f;
    double rank = 0;
    int clusterIdxGlobal = -1;
    int cl;

    for (k = 0; k < p->tamPop; k++)
    {
        rankGlobal = -INFINITO;
        clusterIdxGlobal = -1;
        for (cl = 0; cl < c->maxGrp; cl++)
        {
            rank = SimilarityEDCS(c->grupos[cl], &p->indiv[k]);
            if (rank > rankGlobal)
            {
                rankGlobal = rank;
                clusterIdxGlobal = cl;
            }
        }

        AssimilaProbabilistic(&(p->indiv[k]), &(c->grupos[clusterIdxGlobal]));
        if (c->grupos[clusterIdxGlobal].conta == 1)
        {
            //AssimilaPATH(&(c->grupos[clusterIdxGlobal].ponto), &(p->indiv[k]));
            memcpy(c->grupos[clusterIdxGlobal].ponto.var, p->indiv[k].var, Matriz.numgate);
            c->grupos[clusterIdxGlobal].ponto.fit = p->indiv[k].fit;
            c->grupos[clusterIdxGlobal].ponto.mos = p->indiv[k].mos;
            c->grupos[clusterIdxGlobal].indexCenter = k;
        }
        else
        {
            if (p->indiv[k].fit < c->grupos[clusterIdxGlobal].ponto.fit)
            {
                memcpy(c->grupos[clusterIdxGlobal].ponto.var, p->indiv[k].var, Matriz.numgate);
                c->grupos[clusterIdxGlobal].ponto.fit = p->indiv[k].fit;
                c->grupos[clusterIdxGlobal].ponto.mos = p->indiv[k].mos;
                c->grupos[clusterIdxGlobal].indexCenter = k;
                c->grupos[clusterIdxGlobal].numBuscas = 0;
                c->grupos[clusterIdxGlobal].statsBusca = ACTIVE;
            }


        }
        p->clusterIndexes[k] = clusterIdxGlobal;
    }



    for (j = 0; j < c->maxGrp; j++)
    {
        if (c->grupos[j].ponto.mos < melhor &&
                c->grupos[j].conta > 0 &&
                c->grupos[j].statsBusca == ACTIVE)
        {
            melhor = c->grupos[j].ponto.mos;
        }
    }

    for (j = 0; j < c->maxGrp; j++)
    {
        if (c->grupos[j].ponto.mos == melhor)
        {
            c->melhorIndexes[j] = 1;
        }
        else
        {
            c->melhorIndexes[j] = 0;
        }
    }

    for (j = 0; j < c->maxGrp; j++)
    {
        if (c->grupos[j].conta > maiorCont)
        {
            maiorCont = c->grupos[j].conta;
        }
    }

    c->numGrp = 0;
    for (j = 0; j < c->maxGrp; j++)
    {
        if (c->grupos[j].conta > 0)
        {
            c->numGrp++;
        }
    }

    return maiorCont;
}

void OptEDCS(Centro * c, Cromossomo * I, int p, int q)
{
    int  i, j;
    Cromossomo aux;
    aux.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    memcpy(aux.var, I->var, Matriz.numgate);
    if ((c->graphGlobal[I->var[p] - 1][p] <= ceil((double)(c->sizeGlobal / 5.0f))) &&
            (c->graphGlobal[I->var[q] - 1][q] <= ceil((double)(c->sizeGlobal / 5.0f))))
    {
        if (p + 1 == q)
        {
            j = 0;
            for (i = q; i < Matriz.numgate; i++, j++)
                I->var[j] = aux.var[i];
            for (i = 0; i <= p; i++, j++)
                I->var[j] = aux.var[i];
        }
        else
        {
            j = p + 1;
            for (i = q; i >= p + 1; i--, j++)
                I->var[j] = aux.var[i];
        }
    }
    free(aux.var);
}

int Mutacao2OptGuided(Centro *c, Cromossomo * I, int LARGUR)
{
    Cromossomo J, K;
    int i, j, sigmax = 0;
    int y, z;
    J.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    K.var = (Byte *)malloc(sizeof(Byte) * Matriz.numgate);
    if (J.var == NULL || K.var == NULL)
    {
        puts("M2OPT_MSG_MEMORY_FAULT");
        exit(335);
    }
    y = (Matriz.numgate <= LARGUR ? 0 : rand() % (Matriz.numgate - LARGUR));
    z = y + (Matriz.numgate <= LARGUR ? Matriz.numgate : LARGUR); //o n�mero de posi��es trocadas � entre NUMGATES e 40 ou entre 40 e NUMGATES
    for (i = y, sigmax = 0; i < z; i++)
    {
        for (j = i + 1; j < z; j++)
        {
            memcpy(J.var, I->var, Matriz.numgate);
            OptEDCS(c, &J, i, j);
#ifdef PAB
            CalculaGPAB(&J);
#else
            CalculaG(&J);
#endif
            if (I->fit - J.fit > sigmax)
            {
                memcpy(K.var, J.var, Matriz.numgate);
                K.fit = J.fit;
                K.mos = J.mos;
                K.tos = J.tos;
                sigmax = I->fit - J.fit;
            }
        }
    }
    if (sigmax > 0)
    {
        memcpy(I->var, K.var, Matriz.numgate);
        I->fit = K.fit;
        I->mos = K.mos;
        I->tos = K.tos;
        I->mut++;
        I->sel = 0;
        free(J.var);
        free(K.var);
        return TRUE;
    }
    free(J.var);
    free(K.var);
    return FALSE;
}

void GuidedMutation(Cromossomo *M, Cromossomo *I, Centro *C, int lenght, double alpha)
{

    for (int i = 0; i < NUMGATES; i++)
    {
        M->var[i] = -1;
    }

    int *memory = (int *)calloc(lenght, sizeof(int));
    for (int i = 0; i < lenght; i++)
    {
        memory[i] = 0;
    }

    int *memorypos = (int *)calloc(lenght, sizeof(int));
    for (int i = 0; i < lenght; i++)
    {
        memorypos[i] = 0;
    }


    int sum = 0.0f;
    double probability = 0.0f;
    int acumulador = 0;
    int index;

    double limite = ((double)(4.0f / 5.0f))*lenght;
    if (lenght - limite > 10)
    {
        limite = 10;
    }

    for (int pos = 0; pos < ceil(((double)(4.0f / 5.0f))*lenght); pos++)
    {
        index = rand() % lenght;
        while (M->var[index] != 255)
        {
            index++;
            if (index >= lenght)
            {
                index = 0;
            }
        }
        if (M->var[index] != -1)
        {
            M->var[index] = I->var[index];
            memory[M->var[index] - 1] = 1;
            memorypos[index] = 1;
        }

    }

    //for (int u = 0; u < NUMGATES; u++) {
    //	printf("%d ", M->var[u]);
    //}
    //printf("\n");

    for (int val = 0; val < lenght; val++)
    {
        if (memory[val] == 0)
        {
            for (int pos = 0; pos < lenght; pos++)
            {
                if (memorypos[pos] == 0)
                {
                    sum += (int)C->graphGlobal[pos][val];
                }
            }

            if (sum == 0)
            {
                probability = 0;
            }
            else
            {
                probability = rand() % sum;
            }

            for (int pos = 0; pos < lenght; pos++)
            {
                if (memorypos[pos] == 0)
                {
                    acumulador += (int)C->graphGlobal[pos][val];
                }

                if (acumulador >= probability && memorypos[pos] == 0)
                {
                    M->var[pos] = val + 1;
                    memorypos[pos] = 1;
                    memory[val] = 1;
                    break;
                }
            }

        }
        acumulador = 0;
        sum = 0.0f;

        //for (int u = 0; u < NUMGATES; u++) {
        //	printf("%d ", M->var[u]);
        //}
        //printf("\n");
    }

#ifdef PAB
    CalculaGPAB(M);
#else
    CalculaG(M);
#endif


    //printf("%d\n", I->fit);
    //printf("%d\n", M->fit);


    free(memory);
    free(memorypos);

    return;
}

void LocalEDCS(Prototipos& C, Populacao& P, int& bLocOk, int& bLocTot, int PASSOS, int LARGO)
{
    int index;
    int fitant;
    int k;
    Cromossomo *M;
    M = (Cromossomo *)calloc(1, sizeof(Cromossomo));
    M->var = (Byte *)calloc(NUMGATES, sizeof(Byte));
    for (int i = 0; i < NUMGATES; i++)
    {
        M->var[i] = -1;
    }
    M->fit = INFINITO;
    M->mos = INFINITO;

    for (index = 0; index < C.maxGrp; index++)   //para cada clusters
    {
        if (C.melhorIndexes[index] == 1 && C.grupos[index].conta >= C.densid && C.grupos[index].statsBusca == ACTIVE)
        {
            k = PASSOS + 2 * (bLocTot - bLocOk);

            memcpy(M->var, C.grupos[index].ponto.var, Matriz.numgate);
            M->fit = C.grupos[index].ponto.fit;
            M->mos = C.grupos[index].ponto.mos;

            while (k-- && Mutacao2Opt(&(C.grupos[index].ponto), LARGO));

            if (C.grupos[index].ponto.fit < P.indiv[P.melhor].fit)
            {
                memcpy(P.indiv[P.melhor].var, C.grupos[index].ponto.var, Matriz.numgate);
                P.indiv[P.melhor].fit = C.grupos[index].ponto.fit;
                P.indiv[P.melhor].mos = C.grupos[index].ponto.mos;
                P.indiv[P.melhor].tos = C.grupos[index].ponto.tos;
                P.indiv[P.melhor].sel = C.grupos[index].ponto.sel;
                P.indiv[P.melhor].mut = C.grupos[index].ponto.mut;
                bLocOk++;

                if (C.grupos[index].ponto.fit < M->fit)
                {
                    C.grupos[index].numBuscas = 0;
                    //memcpy(M->var, C.grupos[index].ponto.var, Matriz.numgate);
                    //M->fit = C.grupos[index].ponto.fit;
                    //M->mos = C.grupos[index].ponto.mos;
                }
            }
            bLocTot++;



            //reinicia so se foi verificado
            C.grupos[index].conta = 1;
            //if (C.melhorIndexes[index] >= 10) {
            //	C.melhorIndexes[index] = 0;
            //	for (int j = 0; j < P.tamInd; j++) {
            //		for (int k = 0; k < P.tamInd; k++) {
            //			C.grupos[index].graphGlobal[j][k] = 1.0f;
            //		}
            //		C.grupos[index].sizeGlobal = P.tamInd;
            //	}
            //	C.grupos[index].ponto.fit = INFINITO;
            //}
            C.grupos[index].numBuscas++;
            if (C.grupos[index].numBuscas >= 10)
            {
                C.grupos[index].statsBusca = INACTIVE;
                C.melhorIndexes[index] = 0;
            }
        }
        else
        {
            if (C.grupos[index].indexCenter > -1)
            {
                GuidedMutation(M, &P.indiv[C.grupos[index].indexCenter], &C.grupos[index], NUMGATES, 0.95f);

                if (M->fit < P.indiv[C.grupos[index].indexCenter].fit)
                {
                    memcpy(P.indiv[C.grupos[index].indexCenter].var, M->var, NUMGATES);
                    P.indiv[C.grupos[index].indexCenter].fit = M->fit;
                    P.indiv[C.grupos[index].indexCenter].mos = M->mos;
                }
            }
        }
    }

    free(M->var);
    free(M);

}

int distHamming(Byte *v1, Byte *v2, int lenght)
{
    int dist = 0;
    for (int i = 0; i < lenght; i++)
    {
        if (v1[i] == v2[i])
        {
            dist++;
        }
    }
    return dist;
}

int *getGoodIndexes(Populacao *pop, int numClusters, int numTrials)
{
    int *indexes = (int *)calloc(numClusters, sizeof(int));
    int *indexesTemp = (int *)calloc(numClusters, sizeof(int));

    int rank = 0;
    int rankGlobal = INFINITO;

    int index = 0;

    int i, j, k, n;

    for (j = 0; j < numClusters; j++)
    {
        indexesTemp[j] = -1;
    }

    for (i = 0; i < numTrials; i++)
    {
        for (j = 0; j < numClusters; j++)
        {
            indexesTemp[j] = (int)(rand() % (pop->tamInd));
        }

        rank = 0;
        for (j = 0; j < numClusters; j++)
        {
            for (k = j + 1; k < numClusters; k++)
            {
                rank += distHamming(pop->indiv[indexesTemp[j]].var, pop->indiv[indexesTemp[k]].var, NUMGATES);
            }
        }
        if (rank < rankGlobal)
        {
            rankGlobal = rank;
            for (n = 0; n < numClusters; n++)
            {
                indexes[n] = indexesTemp[n];
            }
        }
        for (n = 0; n < numClusters; n++)
        {
            indexesTemp[n] = -1;
        }
    }
    return indexes;
}


void MutacaoGuided(Cromossomo *I, Prototipos *C, double alpha)
{

    Cromossomo *mut = (Cromossomo *)calloc(1, sizeof(Cromossomo));

    int index = -1;

    for (int i = 0; i < NUMGATES; i++)
    {
        mut->var[i] = -1;
    }
    mut->fit = INFINITO;
    mut->mos = INFINITO;
    mut->sel = 0;

    int val = (int)ceil(NUMGATES*alpha);
    if (val >= NUMGATES)
    {
        val = NUMGATES - 1;
    }
    for (int i = 0; i < (int)ceil(NUMGATES*alpha); i++)
    {
        index = rand() % NUMGATES;
        while (mut->var[index] != -1)
        {
            if (index >= NUMGATES - 1)
            {
                index = 0;
            }
        }
        mut->var[index] = I->var[index];
    }

    double probability;
    double accumulator;

    int val2 = NUMGATES - val;
    for (int i = 0; i < val2; i++)
    {
        probability = rand() % C->grupos[0].size;
        if (probability == 0)
        {
            probability = 1;
        }
        accumulator = 0.0f;

    }

    return;
}

#endif

int  main(int argc, char *argv[])
{

    Prototipos C;
    Populacao P;

    Byte ch;


    clock_t start, end; //Controle do tempo de execu��o
    char nomresultado[100];

    int numGeracoes; //Contador para o n�mero de Gera��es
    int numCruza;	//N�mero m�ximo de cruzamentos
    int bLocTot;
    int bLocOk;
    int nGrupos = 0; //Quantidade atual de clusters
    int resp; //Armazena a maior densidade entre os clusters

    double erro; //Taxa de erro para a solu��o

    int i, j, t;
    int **m;

    int semente;
    char achou; //Flag para verificar se achou a solu��o
    int mutou;

    if (argc < 12)
    {
        printf("\n ERRO(3): Falta argumentos! \nTente %s <resultado_in> <V=1,M=2> < .ext > <#semente>", argv[0]);
        puts("\nOnde ...");
        puts("#prob = {dj1=0,  dj3, dj4, dj5, uno, bmp=5,  gol(2), fea(2), sph, ros}");
        puts("#prob = {sch=10, ras, gri, mic(5/10), lan(5/10), she=15, ack, rot}");
        exit(0);
    }

    /****************************
    abre resultadouivo de matriz 01 e de saida
    ****************************/

    resultado = fopen("resultado.txt", "a+");
    debug = fopen("debug.txt", "w+");

    for (int lp = 0; lp < NUMTIMES; lp++)
    {
        printf("\nExecucao numero: %d\n", lp+1);
#ifdef PAB



        strcpy(nomresultado, argv[1]);
        strcpy(&nomresultado[strlen(nomresultado) - 4], GARE);
        strcpy(&nomresultado[strlen(nomresultado) - 3], argv[3]);

        if (!(saida = fopen(nomresultado, "w")))
        {
            perror("");
            exit(-1);
        }


        param = lerDados(argv[1]);


        MAXAVA = atoi(argv[4]);
        //TEMPMAX = atoi(argv[4]);
        MAXPOP = atoi(argv[5]);
        SOLUCAO = atoi(argv[6]);
        NUMCLUS = atoi(argv[7]);
        PASSOS = atoi(argv[8]);
        LARGO = atoi(argv[9]);
        MUTPROB = atof(argv[10]);
        PROMI = atof(argv[11]);
        semente = atoi(argv[12]);




   //      imprimirdados(param);

//        fprintf(stdout, "\nSETTINGS  **************");
//        fprintf(stdout, "\n    MAXAVA=%d,MAXPOP=%d,MUTPROB=%.2f, \n NUMCLUS=%d, PASSOS=%d, PROMI=%.2f ,  %d , %s, AA=%.2f",
//                MAXAVA, MAXPOP, MUTPROB, NUMCLUS, PASSOS, PROMI, ROLETA, (ROLETA ? "ROLETA" : "ALEATA"), AAPALFA);
//        fprintf(stdout, "\n************** SETTINGS  ");
//
//        fprintf(resultado, "\nSETTINGS  **************");
//        fprintf(resultado, "\n    MAXAVA=%d,MAXPOP=%d,MUTPROB=%.2f, \n NUMCLUS=%d, PASSOS=%d, PROMI=%.2f ,  %d , %s, AA=%.2f",
//                MAXAVA, MAXPOP, MUTPROB, NUMCLUS, PASSOS, PROMI, ROLETA, (ROLETA ? "ROLETA" : "ALEATA"), AAPALFA);
//        fprintf(resultado, "\n************** SETTINGS  ");


        /* randomico ou n�o */
        srand((unsigned)time(0) + semente);

        /*  LENDO resultadoUIVO DE ENTRADA */
#ifdef CONSO
        puts("Alocando mem�ria inicial ...");
#endif

        NUMGATES = param->numeroNavios + param->numeroBercos - 1;

        NUMNETS = param->numeroMares;
        Matriz.numnet = NUMNETS;
        Matriz.numgate = NUMGATES;

#else
        if (!(entrada = fopen(argv[1], "r")))
        {
            perror("");
            exit(-1);
        }




        strcpy(nomresultado, argv[1]);
        strcpy(&nomresultado[strlen(nomresultado) - 4], GARE);
        strcpy(&nomresultado[strlen(nomresultado) - 3], argv[3]);

        if (!(saida = fopen(nomresultado, "w")))
        {
            perror("");
            exit(-1);
        }
        TIPO = atoi(argv[2]);

        if (TIPO != 1 && TIPO != 2)
        {
            puts("ERRO(4): Problemas com o tipo de problema (VLSI=1/MOSP=2)!!");
            exit(-1);
        }

        MAXAVA = atoi(argv[4]);
        MAXPOP = atoi(argv[5]);
        SOLUCAO = atoi(argv[6]);
        NUMCLUS = atoi(argv[7]);
        PASSOS = atoi(argv[8]);
        LARGO = atoi(argv[9]);
        MUTPROB = atof(argv[10]);
        PROMI = atof(argv[11]);
        semente = atoi(argv[12]);


        fprintf(stdout, "\nSETTINGS  **************");
        fprintf(stdout, "\n    MAXAVA=%d,MAXPOP=%d,MUTPROB=%.2f, \n NUMCLUS=%d, PASSOS=%d, PROMI=%.2f ,  %d , %s, AA=%.2f",
                MAXAVA, MAXPOP, MUTPROB, NUMCLUS, PASSOS, PROMI, ROLETA, (ROLETA ? "ROLETA" : "ALEATA"), AAPALFA);
        fprintf(stdout, "\n************** SETTINGS  ");

        fprintf(resultado, "\nSETTINGS  **************");
        fprintf(resultado, "\n    MAXAVA=%d,MAXPOP=%d,MUTPROB=%.2f, \n NUMCLUS=%d, PASSOS=%d, PROMI=%.2f ,  %d , %s, AA=%.2f",
                MAXAVA, MAXPOP, MUTPROB, NUMCLUS, PASSOS, PROMI, ROLETA, (ROLETA ? "ROLETA" : "ALEATA"), AAPALFA);
        fprintf(resultado, "\n************** SETTINGS  ");

        /* randomico ou n�o */
        srand((unsigned)time(0) + semente);

        /*  LENDO resultadoUIVO DE ENTRADA */

#ifdef CONSO
        puts("Alocando mem�ria inicial ...");
#endif

        fscanf(entrada, "%d %d", &NUMGATES, &NUMNETS);


        if (TIPO == MOSP)
            fscanf(entrada, "%d\n%d\n", &NUMGATES, &NUMNETS);


        if (TIPO == MOSP)
        {
            Matriz.string = (Byte **)malloc(NUMNETS*sizeof(Byte *));
            if (!Matriz.string)
            {
                puts("MAIN_MSG_MEMORY_FAULT01");
                exit(4);
            }
            for (i = 0; i< NUMNETS; i++)
            {
                Matriz.string[i] = (Byte *)malloc(NUMGATES*sizeof(Byte));
                if (!Matriz.string[i])
                {
                    puts("MAIN_MSG_MEMORY_FAULT02");
                    exit(5);
                }
            }

            for (i = 0; i<NUMGATES && !feof(entrada); i++)
            {
                for (j = 0; j<NUMNETS && !feof(entrada);)
                {
                    fscanf(entrada, "%c\n", &ch);
                    if (ch == '0')
                    {
                        Matriz.string[j][i] = '0';
                        j++;
                    }
                    else
                    {
                        Matriz.string[j][i] = '1';
                        j++;
                    }
                }
            }
            fclose(entrada);
            if (j != NUMNETS || i != NUMGATES)
            {
                puts("MAIN_MSG_FILE_FAULT01");
                exit(6);
            }
        }

        if (TIPO == VLSI)
        {
            Matriz.string = (Byte **)malloc(NUMNETS*sizeof(Byte *));
            if (!Matriz.string)
            {
                puts("MAIN_MSG_MEMORY_FAULT11");
                exit(4);
            }
            for (i = 0; i< NUMNETS; i++)
            {
                Matriz.string[i] = (Byte *)malloc(NUMGATES*sizeof(Byte));
                if (!Matriz.string[i])
                {
                    puts("MAIN_MSG_MEMORY_FAULT12");
                    exit(5);
                }
            }

            for (i = 0; i<NUMNETS && !feof(entrada); i++)
            {
                for (j = 0; j<NUMGATES && !feof(entrada);)
                {
                    fread(&ch, sizeof(Byte), 1, entrada);
                    if (ch == '1' || ch == '0')
                    {
                        Matriz.string[i][j] = ch;
                        j++;
                    }
                }
            }
            fclose(entrada);
            if (i != NUMNETS || j != NUMGATES)
            {
                puts("MAIN_MSG_FILE_FAULT01");
                exit(6);
            }
        }

        /* Pode haver truncamento de n�meros inteiros*/
        Matriz.numnet = NUMNETS;
        Matriz.numgate = NUMGATES;

#endif


        IniciaPop(&P, MAXPOP, NUMGATES); //Inicia a Popula��o
        //print_populacao(&P);			 //print_populacao(&P);
#ifdef EDCS
        IniciaClsEDCS(&C, NUMCLUS, NUMGATES); //Inicia os clusters
#else
        IniciaCls(&C, NUMCLUS, NUMGATES); //Inicia os clusters
#endif


        printf("TAMANHO DA POPULACAO: %d\n", P.tamPop);
        printf("NUMERO DE ELEMENTOS: %d\n", P.tamInd);
        printf("NUMERO DE CLUSTERS: %d\n\n", NUMCLUS);

//        fprintf(resultado, "TAMANHO DA POPULACAO: %d\n", P.tamPop);
//        fprintf(resultado, "NUMERO DE ELEMENTOS: %d\n", P.tamInd);
//        fprintf(resultado, "NUMERO DE CLUSTERS: %d\n\n", NUMCLUS);

#ifdef EDCS
        C.posGrp = NUMCLUS;
        C.densid = (int)ceil(PROMI /*0.10*/ * NUMSELS * NUMCRU / NUMCLUS); //Densidade para saber se o cluster est� QUENTE; //Densidade para saber se o cluster est� QUENTE
#else
        C.limiar = (int)ceil((Matriz.numgate/*x_sup*/ - 2/*x_inf*/) / (2.0F*pow(NUMCLUS, (1.0F / P.tamInd)))); //Raio para cada cluster
        C.densid = (int)ceil(PROMI /*0.10*/ * NUMSELS * NUMCRU / NUMCLUS); //Densidade para saber se o cluster est� QUENTE
#endif



#ifdef CONSO
        printf("\n***   (%s) por MRS/ECP/UFMA    ***", GARE);
        printf("\n      Evoluindo %s com %d x %d \n\n", argv[1], NUMGATES, NUMNETS);
//        printf("\n      >>> MaxAvals/Pop/Cls=%d/%d/%d, Sol=%d, Pass/Larg=%d/%d", MAXAVA, MAXPOP, NUMCLUS, SOLUCAO, PASSOS, LARGO);
//        fprintf(resultado, "\n***   (%s) por MRS/BICT/UFMA    ***", GARE);
//        fprintf(resultado, "\n      Evoluindo %s com %d x %d \n\n", argv[1], NUMGATES, NUMNETS);
//        fprintf(resultado, "\n      >>> MaxAvals/Pop/Cls=%d/%d/%d, Sol=%d, Pass/Larg=%d/%d", MAXAVA, MAXPOP, NUMCLUS, SOLUCAO, PASSOS, LARGO);
#ifdef EDCS
        printf("\n      >>> MutP/Promi=%.2f/%.2f, Limiar=%.2f, Densidade=%d", MUTPROB, PROMI, C.limiar, C.densid);
        fprintf(resultado, "\n      >>> MutP/Promi=%.2f/%.2f, Limiar=%.2f, Densidade=%d", MUTPROB, PROMI, C.limiar, C.densid);
#else
        printf("\n      >>> MutP/Promi=%.2f/%.2f, Limiar=%d, Densidade=%d", MUTPROB, PROMI, C.limiar, C.densid);
        fprintf(resultado, "\n      >>> MutP/Promi=%.2f/%.2f, Limiar=%d, Densidade=%d", MUTPROB, PROMI, C.limiar, C.densid);
#endif
        printf("\n          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
        fprintf(resultado, "\n          ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n\n");
#endif

        /*Verifica se a solu��o � maior ou igual � solu��o �tima*/
        erro = P.indiv[P.melhor].mos - SOLUCAO;
        achou = (erro == 0 ? TRUE : FALSE);

        bLocTot = bLocOk = 0;
        numGeracoes = 1;

        start = clock();

        /*CONDIC��ES DE PARADA
        -> N�o ultrapassar o n�mero m�ximo para avalia��o
        -> Verificar se achou o �timo (erro == 0)
        -> N�o ultrapassar o n�mero m�ximo de gera��es
        */
        int pior = 0;

        int index = 0;
        Cromossomo *M;
        M = (Cromossomo *)calloc(1, sizeof(Cromossomo));
        M->var = (Byte *)calloc(NUMGATES, sizeof(Byte));
        for (int i = 0; i < NUMGATES; i++)
        {
            M->var[i] = -1;
        }
        M->fit = INFINITO;
        M->mos = INFINITO;
        while (Matriz.numAval < MAXAVA  && !achou && numGeracoes < MAXGER)
        //printf("\n %f \n", time_break());
       //while ( < 30  && !achou && numGeracoes < MAXGER)
        {

#ifdef EDCS

            resp = AtualizaGrpEDCS(&C, &P);

            if (resp >= C.densid)
            {

                LocalEDCS(C, P, bLocOk, bLocTot, PASSOS, LARGO);
            }

            numCruza = NUMCRU;

            while (numCruza--)
            {

                index = rand() % P.tamPop;

                GuidedMutation(M, &P.indiv[index], &C.grupos[P.clusterIndexes[index]], NUMGATES, 0.95f);

                if (M->fit < P.indiv[index].fit)
                {
                    memcpy(P.indiv[index].var, M->var, NUMGATES);
                    P.indiv[index].fit = M->fit;
                    P.indiv[index].mos = M->mos;
                }

                if (M->fit < P.indiv[P.melhor].fit)
                {
                    memcpy(P.indiv[P.melhor].var, M->var, NUMGATES);
                    P.indiv[P.melhor].fit = M->fit;
                    P.indiv[P.melhor].mos = M->mos;
                }

            }

#else
            numCruza = NUMCRU;

            while ((!achou) && (numCruza--) &&  Matriz.numAval < MAXAVA)
            {
                //Seleciona 2 indiv�duos
                if (ROLETA)
                {
                    RoletaPressaoSeletiva(&P, P.indiv[P.melhor].fit);
                }
                else
                {
                    P.pai[0] = rand() % P.tamPop;
                    P.pai[1] = (rand() % (P.pai[0] + 1)) + rand() % (P.tamPop - P.pai[0]);
                }

                resp = AtualizaGrp(&C, &P);
                if (resp >= C.densid)
                {
                    EstratIIIntenso(C, P, bLocOk, bLocTot, PASSOS, LARGO);
                }

#ifdef PAB
                CalculaGPAB(&(P.indiv[P.pior]));
#else
                CalculaG(&(P.indiv[P.pior]));
#endif

                if (P.pai[0] != P.pai[1] && P.pai[0] != P.pior && P.pai[1] != P.pior)
                    RecombinaBOX(&P, P.pai[0], P.pai[1], P.pior);
                else
                {
                    P.iguais++;
                    continue;
                }
                if ((rand() % 100 < MUTPROB) && (P.indiv[P.pior].fit >= P.indiv[P.melhor].fit))
                {
                    mutou = MutacaoSwap(&(P.indiv[P.pior]), MUTPROB);
                    if (mutou)
                    {
                        P.numMuta++;
#ifdef PAB
                        CalculaGPAB(&(P.indiv[P.pior]));
#else
                        CalculaG(&(P.indiv[P.pior]));
#endif
                    }
                }

                AtualizaPop(&P, P.pior, numGeracoes);

            } // while NUM CRUZA

#endif

            erro = P.indiv[P.melhor].mos - SOLUCAO;
            achou = (erro == 0 ? TRUE : FALSE);

#ifdef EDCS
            C.densid = (int)ceil(PROMI /*0.10*/ * NUMSELS * NUMCRU / NUMCLUS);
            nGrupos = C.numGrp;
#else
            if (!achou)
            {
                EsfriaGrupos(C);
                C.densid = (int)ceil(PROMI * NUMSELS*NUMCRU / C.numGrp);
                nGrupos = C.numGrp;
            }
#endif

#ifdef CONSO
            if ((numGeracoes - 1) % 5)
            {
                printf("\n %d)%d(%d)+%d=%d ;#Aval=%d; #BL=%d/%d; #GMP=%d/%d/%d, L/D/R=%d,%d,%d  ",
                       numGeracoes, P.indiv[P.melhor].mos, SOLUCAO, P.indiv[P.melhor].tos, P.indiv[P.melhor].fit, Matriz.numAval, bLocOk, bLocTot, C.numGrp, P.numMuta, PASSOS + 2 * (bLocTot - bLocOk), C.limiar, C.densid, resp);
              // printf("\n %d)%d(%d)+%d=%d ; #BL=%d/%d; #GMP=%d/%d/%d, L/D/R=%d,%d,%d  ",
              //        numGeracoes, P.indiv[P.melhor].mos, SOLUCAO, P.indiv[P.melhor].tos, P.indiv[P.melhor].fit, bLocOk, bLocTot, C.numGrp, P.numMuta, PASSOS + 2 * (bLocTot - bLocOk), C.limiar, C.densid, resp);
//                fprintf(resultado, "\n %d)%d(%d)+%d=%d ;#Aval=%d; #BL=%d/%d; #GMP=%d/%d/%d, L/D/R=%d,%d,%d  ",
//                        numGeracoes, P.indiv[P.melhor].mos, SOLUCAO, P.indiv[P.melhor].tos, P.indiv[P.melhor].fit, Matriz.numAval, bLocOk, bLocTot, C.numGrp, P.numMuta, PASSOS + 2 * (bLocTot - bLocOk), C.limiar, C.densid, resp);
            }
#endif
            numGeracoes++;
        } // while Geracoes

       // tempo_parada=(float)(end - start) / CLOCKS_PER_SEC;
        end = clock();

        /* *******************RESULTADOS ******************** */

        P.media = P.sumFit / P.tamPop;
        P.dvpad = 0;
        for (i = 0; i<P.tamPop; i++)
            P.dvpad += pow(P.indiv[i].fit - P.media, 2);
        P.dvpad = sqrt(P.dvpad / (P.tamPop - 1));

#ifdef XLS

        /*fprintf(saida,"%s; %d;  %d;   %d;   %d;  %.3f; %d; %d; %d; %d, %.3f,  %s\n",
        argv[1], Matriz.numgate, SOLUCAO, P.indiv[P.melhor].fit, Matriz.numAval,
        (float) (end - start) / CLOCKS_PER_SEC, numGeracoes, MAXPOP,  NUMCLUS,    PASSOS,    PROMI, (ROLETA?"ROLETA":"ALEATA") );*/

        // fprintf(saida,"  prob;  n; m; esp; stats;  mos; tos; fit; aval; (s); BLOK; TBLO; CLS;");
        fprintf(resultado, "\n %s;    %d; %d;  %d; %s;  %d; %d; %d;   %d; %.2f; %d; %d; %d; ",
                argv[1], Matriz.numgate, Matriz.numnet, SOLUCAO, P.indiv[P.melhor].mos <= SOLUCAO ? "JOIA" : "MAUS", P.indiv[P.melhor].mos, P.indiv[P.melhor].tos, P.indiv[P.melhor].fit, Matriz.numAval, (float)(end - start) / CLOCKS_PER_SEC, bLocOk, bLocTot, nGrupos);
        printf("\n %s;    %d; %d;  %d; %s;  %d; %d; %d;   %d; %.2f; %d; %d; %d; ",
                argv[1], Matriz.numgate, Matriz.numnet, SOLUCAO, P.indiv[P.melhor].mos <= SOLUCAO ? "JOIA" : "MAUS", P.indiv[P.melhor].mos, P.indiv[P.melhor].tos, P.indiv[P.melhor].fit, Matriz.numAval, (float)(end - start) / CLOCKS_PER_SEC, bLocOk, bLocTot, nGrupos);
#endif

#ifdef CONSO
        //printf("\n Variaveis ...");
        //for (i=0;i<P.tamInd;i++) {
        //	printf("\n%.d",P.indiv[P.melhor].var[i]);
        //}
        // se nao houve Blocal, n�o d� erro
//        bLocTot += !bLocTot;
//        printf("\n");
//        fprintf(resultado,"\n");
//        for (int v = 0; v < NUMGATES; v++)
//        {
//            printf("%d, ", P.indiv[P.melhor].var[v]);
//            fprintf(resultado, "%d, ", P.indiv[P.melhor].var[v]);
//        }
//
//        printf("\n");

//        printf("\n");
//
//
//        m =  Matrixnb(P.indiv[P.melhor].matriz);
//        fprintf(resultado,"\n");
//        fprintf(resultado,"\n");
//
//
//        for(int i=0; i< param->numeroNavios; i++)
//        {
//            for(int j=0; j< param->numeroMares; j++)
//            {
//                printf(" %i ", m[i][j]);
//                fprintf(resultado, " %i ", m[i][j]);
//            }
//            fprintf(resultado,"\n");
//            printf("\n");
//        }
//        fprintf(resultado,"\n");
//        printf("\n");
//


        printf("\n Em %.2f (s), min = %d(%d)+%d=%d %s !!; \n\t#Ger = %d; \n\t#Mut?= %d; \n\t#BLT = %d;\n\t#BLO = %d;\n\t#GRP = %d; ",
               (float)(end - start) / CLOCKS_PER_SEC, P.indiv[P.melhor].mos, SOLUCAO, P.indiv[P.melhor].tos, P.indiv[P.melhor].fit, P.indiv[P.melhor].mos <= SOLUCAO ? "SUCESSO" : "FRACASSO", numGeracoes, P.numMuta, bLocTot, bLocOk, nGrupos);
        fprintf(resultado, "\n Em %.2f (s), min = %d",
                (float)(end - start) / CLOCKS_PER_SEC, P.indiv[P.melhor].mos);
        //getchar();
#endif

#ifdef DUMP

        fprintf(saida, "\nD U M P **************");
        fprintf(saida, "\nPopulacao{Tpop=%d, Tind=%d, Som=%.4f, X*=%d, X =%d, #m=%d, #i=%d, (%.4f, %.4f)",
                P.tamPop, P.tamInd, P.sumFit, P.melhor, P.pior, P.numMuta, P.iguais, P.media, P.dvpad);

        for (i = 0; i<P.tamPop; i++)
        {
            fprintf(saida, "\nIndividuo%s (%d) = %d (%d+%d) - Sel/Mut%d/%d", (i == P.melhor ? "***" : "   "), i, P.indiv[i].fit, P.indiv[i].mos, P.indiv[i].tos, P.indiv[i].sel, P.indiv[i].mut);
            for (j = 0; j<P.tamInd; j++)
                fprintf(saida, "\n%d", P.indiv[i].var[j]);
        }

        for (i = 0; i<C.numGrp; i++)
        {
            fprintf(saida, "\nCentro (%d) CS=%d/%d; fit=%d(%d+%d) Sel/Mut%d/%d", i, C.grupos[i].conta, C.grupos[i].stats, C.grupos[i].ponto.fit, C.grupos[i].ponto.mos, C.grupos[i].ponto.tos, C.grupos[i].ponto.sel, C.grupos[i].ponto.mut);
            for (j = 0; j<P.tamInd; j++)
                fprintf(saida, "\n%d", C.grupos[i].ponto.var[j]);
        }


#endif

        fclose(saida);
        strcpy(&nomresultado[strlen(nomresultado) - 3], "doc");
        if (!(saida = fopen(nomresultado, "w")))
        {
            perror("");
            exit(-1);
        }

        fprintf(saida, "\nAJUSTES   **************");
#ifdef EDCS
        fprintf(saida, "\n    MAXAVA=%d,MAXPOP=%d,MUTPROB=%.2f, \n NUMCLUS=%d, PASSOS=%d, PROMI=%.2f , LIM= %.2f , DEN=%d ",
                MAXAVA, MAXPOP, MUTPROB, NUMCLUS, PASSOS, PROMI, C.limiar, C.densid);
#else
        fprintf(saida, "\n    TEMPMAX=%d,MAXPOP=%d,MUTPROB=%.2f, \n NUMCLUS=%d, PASSOS=%d, PROMI=%.2f , LIM= %d , DEN=%d ",
                TEMPMAX, MAXPOP, MUTPROB, NUMCLUS, PASSOS, PROMI, C.limiar, C.densid);
#endif

        fprintf(saida, "\n************** SETTINGS  ");
        for (int i = 0; i < NUMGATES; i++)
        {
            fprintf(debug, "%d ", P.indiv[P.melhor].var[i]);
        }


        free(M->var);
        free(M);
        free(&P.indiv->var);
      //  tempo_parada = 0;
    }

    fclose(resultado);
    fclose(debug);

}

