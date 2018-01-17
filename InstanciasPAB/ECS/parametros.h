/*
Nome:		parametros.c
Data:		15/02/2015
Vers�o:		2.0
Autor:		Victor Hugo Barros, Marcelo barnco do Nascimento
Descri��o:	Parametros utilizados no problema lidos a partir de arquivo de inst�ncia
*/

#ifndef PARAMETROS_H
#define PARAMETROS_H

#include <stdlib.h>
#include <stdio.h>
#include <math.h>


// Estrutura com Atributos
typedef struct _dados_ {
	int *navios;			// Navios utilizados no problema
	int *mare_chegada;		// Mar� de chegada de cada navio
	int *velocidade_berco;	// Velocidade (vaz�o) de cada ber�o
	int *demurrage;         // Demurrage de cada Navio
	double *estoque;		// N�veis de estoques iniciais
	double *consumo;		// Consumo de carga por mat�ria-prima em cada mar�
	double **carga;			// Carga transportada: navio x mat�ria-prima
	int **handling;			// Tempo de atendimento de cada navio em cada ber�o

	int numeroNavios;		// N�mero de navios
	int numeroMares;		// N�mero de mar�s
	int numeroBercos;		// N�mero de ber�os
	int numeroMaterias;		// N�mero de mat�rias
}Dados;

void calcularHandling(Dados *dados);
Dados *lerDados(const char *instancia);
void imprimirdados(Dados *dados);

#endif // PARAMETROS_H
