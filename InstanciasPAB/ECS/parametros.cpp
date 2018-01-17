/*
Nome:		parametros.c
Data:		15/02/2015
Versão:		2.0
Autor:		Victor Hugo Barros, Marcelo barnco do Nascimento
Descrição:	Parametros utilizados no problema lidos a partir de arquivo de instância
*/

#include "parametros.h"

//------------------------------------------------------------------------------
// Calcula tempo de serviço (handling time) de cada navio em cada berço
//------------------------------------------------------------------------------
void calcularHandling(Dados *dados) {
	dados->handling = (int**)calloc(dados->numeroNavios, sizeof(int));
	int i, j, k;
	for (i = 0; i < dados->numeroNavios; i++) {
		dados->handling[i] = (int*)calloc(dados->numeroBercos, sizeof(int));
		for (j = 0; j < dados->numeroBercos; j++) {
			double carga_total = 0;		// Carga total do navio, considerando todos os tipos de matéria-prima
			for (k = 0; k < dados->numeroMaterias; k++) {
				carga_total += dados->carga[i][k];
			}
			dados->handling[i][j] = ceil(carga_total / dados->velocidade_berco[j]);
		}
	}
}


//------------------------------------------------------------------------------
// Lê dados do arquivo e atribui os valores de parâmetros às variáveis de
// instância. By Priest.
//------------------------------------------------------------------------------
Dados *lerDados(const char *instancia) {

	FILE *fp = NULL;
	char ch = 'x';
	int i = 0, j = 0;

	Dados *dados = (Dados *)malloc(sizeof(Dados));

	dados->numeroNavios = 0;
	dados->numeroBercos = 0;
	dados->numeroMares = 0;
	dados->numeroMaterias = 0;


	fp = fopen(instancia, "r");
	if (fp == NULL) {
		printf("Erro ao abrir arquivo!\n");
		exit(1);
	}

	while (ch != '=')
		ch = getc(fp);


	//obtendo quantidade de navios
	while (ch != ';') {
		ch = getc(fp);
		if (ch == ' ')
			dados->numeroNavios++;
	}

	dados->navios = (int *)malloc(sizeof(int) * dados->numeroNavios);

	for (i = 0;i<dados->numeroNavios;i++)
		dados->navios[i] = i + 1;

	while (ch != '=')
		ch = getc(fp);

	//obtendo quantidade de marés
	while (ch != ';') {
		ch = getc(fp);
		if (ch == ' ')
			dados->numeroMares++;
	}

	while (ch != '=')
		ch = getc(fp);

	//obtendo quantidade de tipos de carga diferente 'nK'
	while (ch != ';') {
		ch = getc(fp);
		if (ch == ' ')
			dados->numeroMaterias++;
	}

	while (ch != '=')
		ch = getc(fp);

	//obtendo a quantidade de bercos
	while (ch != ';') {
		ch = getc(fp);
		if (ch == ' ')
			dados->numeroBercos++;
	}

	dados->velocidade_berco = (int*)malloc(sizeof(int)*dados->numeroBercos);

	while (ch != '=')
		ch = getc(fp);

	//ignorando esta parte do arquivo: "param Mares:=", ja foi lido no inicio
	while (ch != ';')
		ch = getc(fp);

	while (ch != '=')
		ch = getc(fp);

	//obtendo velocidade 'v' de cada berco
	for (i = 0;i<dados->numeroBercos;i++) {
		while (ch != ' ')
			ch = getc(fp);

		fscanf(fp, "%d", &dados->velocidade_berco[i]);
		ch = 'x';
	}



	// obtendo demurrage 'd' de cada navio
	dados->demurrage = (int*)malloc(sizeof(int)*dados->numeroNavios);

	while (ch != '=')
		ch = getc(fp);

    for (i = 0;i<dados->numeroNavios;i++) {
		while (ch != ' ')
			ch = getc(fp);

		fscanf(fp, "%d", &dados->demurrage[i]);
		ch = 'x';
	}

	dados->mare_chegada = (int*)malloc(sizeof(int)*dados->numeroNavios);

	while (ch != '=')
		ch = getc(fp);

	//obtendo tempo de chegada 'a' de cada navio
	for (i = 0;i<dados->numeroNavios;i++) {
		while (ch != ' ')
			ch = getc(fp);

		fscanf(fp, "%d", &dados->mare_chegada[i]);
		ch = 'x';
	}



	dados->estoque = (double *)malloc(sizeof(double) * (dados->numeroMaterias));

	while (ch != '=')
		ch = getc(fp);

	//obtendo estoque inicial 'e' de cada matéria-prima
	for (i = 0;i<dados->numeroMaterias;i++) {
		while (ch != ' ')
			ch = getc(fp);

		fscanf(fp, "%lf", &dados->estoque[i]);
		ch = 'x';
	}



	dados->consumo = (double *)malloc(sizeof(double) * (dados->numeroNavios));
    while (ch != '=')
		ch = getc(fp);
	//obtendo o consumo de cada matéria-prima por maré
	for (i = 0;i<dados->numeroMaterias;i++) {
		while (ch != ' ')
			ch = getc(fp);

		fscanf(fp, "%lf", &dados->consumo[i]);
		ch = 'x';
	}

	while (ch != '=')
		ch = getc(fp);

	dados->carga = (double **)malloc(sizeof(double) * dados->numeroNavios);

	for (i = 0;i<dados->numeroNavios;i++)
		dados->carga[i] = (double *)malloc(sizeof(double) * dados->numeroMaterias);

	//obtendo a qtd de carga que cada navio descarrega ou retira do estoque por tipo de carga
	for (i = 0;i<dados->numeroNavios;i++) {
		while (ch != '	')
			ch = getc(fp);

		for (j = 0; j<dados->numeroMaterias; j++) {
			fscanf(fp, "%lf", &dados->carga[i][j]);

			//printf("%lf ", carga[i][j]);
		}
		//printf(" \n");


		ch = 'x';
	}

	return dados;
}

//------------------------------------------------------------------------------
// Imprime os dados na tela a partir da instância lida
//------------------------------------------------------------------------------
void imprimirdados(Dados *dados) {

	printf("Numero de navios: %d\n", dados->numeroNavios);
	printf("Numero de bercos: %d\n", dados->numeroBercos);
	printf("Numero de mares: %d\n", dados->numeroMares);
	printf("Numero de materiais: %d\n", dados->numeroMaterias);
	printf("\n");

	int i, j;
	printf("NAVIOS\n");
	for (i = 0; i < dados->numeroNavios; i++) {
		printf("%d ", dados->navios[i]);
	}
	printf("\n\n");

	printf("MARE(S) DE CHEGADA\n");
	for (i = 0; i < dados->numeroNavios; i++) {
		printf("%d ", dados->mare_chegada[i]);
	}
	printf("\n\n");

	printf("VELOCIDADE DO(S) BERCO(S)\n");
	for (i = 0; i < dados->numeroBercos; i++) {
		printf("%d ", dados->velocidade_berco[i]);
	}
	printf("\n\n");

    printf("DEMURRAGE DO(S) NAVIO(S)\n");
	for (i = 0; i < dados->numeroNavios; i++) {
		printf("%d ", dados->demurrage[i]);
	}
	printf("\n\n");

	printf("ESTOQUES\n");
	for (i = 0; i < dados->numeroMaterias; i++) {
		printf("%.6lf ", dados->estoque[i]);
	}
	printf("\n\n");

	printf("CONSUMOS\n");
	for (i = 0; i < dados->numeroBercos; i++) {
		printf("%.6lf ", dados->consumo[i]);
	}
	printf("\n\n");

	printf("CARGAS\n");
	for (i = 0; i < dados->numeroNavios; i++) {
		for (j = 0; j < dados->numeroMaterias; j++) {
			printf("%.6lf\t", dados->carga[i][j]);
		}
		printf("\n");
	}
	printf("\n\n");

}
