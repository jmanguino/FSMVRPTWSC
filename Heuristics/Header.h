#include <stdio.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <cstdlib>
//#include <time.h>
#include <algorithm>

using namespace std;

#define M 140 //Tamanho de todas as arrays de clientes. 140 e o num. de clientes da instancia da Votorantim
#define Teste "Mini"
/* Determina o que será testado. Opcoes:
	"168" - Testa as instancias de Referencia
	"Mini" - Testa outras instancias
	"CalibracaoIndices" - Calibrates alphas & Bethas*/

#define TesteBL "Hybrid"
/* Determines what Search procedure will be conducted. Options:
	"None"		Only Heuristics
	"Relocate"	Only runs the Relocate LS
	"Cross"		Only runs the Cross LS
	"2Opt*"		Only runs the 2-Opt* LS
	"All"		Runs all the LS in the above sequence
	"VNS"		Runs the metha-Heuristic VNS Procedure
	"GRASP"		Runs only the GRASP Method
	"Hybrid"	Runs only the Hybrid Method */

#define RodadaVNS "Total"
/* Determines what Search procedure will be conducted. Options:
	"Calibra" - Only runs the 3 instances per group (R101a/b/c, R102a/b/c e R103a/b/c)
	"Compl" - Completes the missing instancer after Calibra
	"CalibraShake" - Runs only canditade combinations of shake parameters
	"Total" - Runs it all
*/

#define Heuristica 3 //Which of the SCIH will be conducted

#define PrintRoutes true
#define PrintEvolution true

//*****Parametros BL:
#define OperadorRemocao 3
/*0: Test all
  1: RR	 Random Removal
  2: WDR worst distance removal
  3: GR	 greatest cost (greedy) removal
  4: WTR worst time removal
  5: C1R C1 removal */
#define OperadorInsercao 3
/*0: Test all
  1: RI  Random Insertion 
  2: SDI smallest distance insertion
  3: GI	 smallest cost (greedy) insertion
  4: C1I C1 Insertion*/
#define CrossMaxTamTrecho 5
#define CientesRelocate 40
#define FirstAccept false	//True: First Accept; False: Best Accept

//Parâmetros VNS:
#define iQtdLS  3		// Quantas buscas locais temos
#define dMaxExec  720	// Tempo em segundos que deixaremos a metaheurística rodar

// Parâmetros Shake:
#define RodadasShake 10		// Numero de rodadas sem melhorias antes de expandir o shake
#define MaxShakeRelocate 40	// Numero de trocas maximas do shake em porcentagem	
#define MaxShake2Opt 50		// Numero de trocas maximas do shake em porcentagem	
#define MaxShakeCross 1	// Numero de trocas maximas do shake em porcentagem

//Parâmetros GRASP:
#define GRASP_TamLRCd 5 //Tamanho da LRC do cliente mais distante para criacao de nvoa rota
#define GRASP_LRCC2_Tipo 2
/* 1: Tamanho Fixo
   2: Proporcao do Maior pro Menor
   3: Tolerancia do Maior */ 
#define GRASP_LRCC2_Tam 10 //Tamanho da LRC do C2 - Se for fixo e esse o numero de clientes na rota, se nao esse e o theta ou lambda

   //Parâmetros Hybrid:
#define HYBRID_ShareAleat .05 //Parcela do tempo que o Hybrid vai ter para fazer o SCIH_Aleat
#define HYBRID_MinShareAleat .01 //Parcela do tempo que o Hybrid vai ter para fazer o SCIH_Aleat
#define HYBRID_MaxShareAleat .03 //Parcela do tempo que o Hybrid vai ter para fazer o SCIH_Aleat
#define HYBRID_PassoShareAleat .02 //Parcela do tempo que o Hybrid vai ter para fazer o SCIH_Aleat

//----- PARAMETROS GLOBAIS------
extern int		N, K, F, abc;	/*Par�metros: Numero de clientes, de tipos de veiculos e de faixas de distancia*/
extern double	alfa[5], beta[5],
				Dist[M][M], Tempo[M][M], Demanda[M], Serv[M], InicioJan[M], FimJan[M], /*Parametros dos clientes*/
				Capac[10], InicioFaixa[10], CustoFaixa[4][10][10]; /*Parametros dos veiculos*/
extern int	Rota[M][M];
extern string NomeProb;

extern int v;
extern double InicioAtend[M];
extern double Demanda_Rota[M],DistTotal[M],Custo[M];