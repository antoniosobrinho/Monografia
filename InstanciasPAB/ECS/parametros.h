/*
Nome:		parametros.c
Data:		15/02/2015
Versão:		2.0
Autor:		Victor Hugo Barros, Marcelo barnco do Nascimento
Descrição:	Parametros utilizados no problema lidos a partir de arquivo de instância
*/

#ifndef PARAMETROS_H
#define PARAMETROS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


// Estrutura com Atributos
typedef struct _dados_ {
	int *navios;			// Navios utilizados no problema
	int *mare_chegada;		// Maré de chegada de cada navio
	int *velocidade_berco;	// Velocidade (vazão) de cada berço
	int *demurrage;         // Demurrage de cada Navio
	double *estoque;		// Níveis de estoques iniciais
	double *consumo;		// Consumo de carga por matéria-prima em cada maré
	double **carga;			// Carga transportada: navio x matéria-prima
	int **handling;			// Tempo de atendimento de cada navio em cada berço

	int numeroNavios;		// Número de navios
	int numeroMares;		// Número de marés
	int numeroBercos;		// Número de berços
	int numeroMaterias;		// Número de matérias
}Dados;

void calcularHandling(Dados *dados);
Dados *lerDados(const char *instancia);
void imprimirdados(Dados *dados);

#endif // PARAMETROS_H
