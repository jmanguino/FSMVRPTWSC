#pragma once
#include "Header.h"
#include "Functions.h"


 int		N, K, F, abc;	/*Par�metros: Numero de clientes, de tipos de veiculos e de faixas de distancia*/
 double	alfa[5], beta[5],
		Dist[M][M], Tempo[M][M], Demanda[M], Serv[M], InicioJan[M], FimJan[M], /*Parametros dos clientes*/
		Capac[10], InicioFaixa[10], CustoFaixa[4][10][10]; /*Parametros dos ve�culos*/
 int	Rota[M][M];
string NomeProb;

int contRodadas;

ifstream ArqInstancia;

//Calcula o tempo em segundos
double dElapsedTime(clock_t Start, clock_t End) {
	return ((double)End - (double)Start) / CLOCKS_PER_SEC;
}

void CopiaSol(int(&SolDestino)[M][M], int SolOrigem[][M]) {
	int iQtRotas;
	iQtRotas = ContaRotas(SolOrigem);
	for (int i = 1; i <= iQtRotas; i++) {
		SolDestino[i][0] = 0;
		int j = 1;
		while (SolOrigem[i][j] != 0) {
			SolDestino[i][j] = SolOrigem[i][j];
			j++;
		}
		SolDestino[i][j] = 0;
	}
	SolDestino[iQtRotas + 1][0] = 0;
	SolDestino[iQtRotas + 1][1] = 0;
	SolDestino[iQtRotas + 1][2] = 0;
}

void ColetaDadosGrupos(string sNomeArquivo) { //Coleta dados que s�o comuns a todas as inst�ncias de cada grupo (R1/R2/C1/...)
	ifstream Problema;
	Problema.open("../"+sNomeArquivo);

/*    if (!file.is_open(Problema))
    {
        throw std::runtime_error("Erro ao abrir arquivo");
    }
*/
	Problema >> N >> K >> F;

	for (int i = 0; i <= N; i++)
		for (int j = 0; j <= N; j++) {
			Problema >> Dist[i][j];		//Dist�ncia entre clientes
			Tempo[i][j] = Dist[i][j];				//Tempo entre clientes
		}

	for (int i = 1; i <= K; i++)
		Problema >> Capac[i];		//Capacidade dos tipos de ve�culo
	for (int i = 1; i <= F; i++)
		Problema >> InicioFaixa[i];	//In�cio das faixas de dist�ncia
	for (int h = 1; h <= 3;h++)
		for (int i = 1; i <= K; i++)
			for (int j = 1; j <= F; j++)
				Problema >> CustoFaixa[h][i][j];// Os custos a, b e c de ve�culos
	Problema.close();
}

void ColetaDadosInstancias() {							//Coleta dados que s�o comuns a todas as inst�ncias de cada grupo (R1/R2/C1/...)

	ArqInstancia >> NomeProb;
	for (int j = 0; j <= N; j++)
		ArqInstancia >> Demanda[j] >> InicioJan[j] >> FimJan[j] >> Serv[j]; //Demanda, janelas de tempo e tempo de servi�o em cada cliente
}

double CalcInicioAtend(int ant, int cliente, double AtendAnt) {
	if (AtendAnt + Serv[ant] + Tempo[ant][cliente] > InicioJan[cliente])
		return AtendAnt + Serv[ant] + Tempo[ant][cliente];
	return InicioJan[cliente];
}

double DistanciaRota(int iSol[][M], int iRota) {
	double dD=0;

	int iPos = 1;
	while (iSol[iRota][iPos]!=0){
		dD += Dist[iSol[iRota][iPos - 1]][iSol[iRota][iPos]];
		iPos++;
	}
	dD += Dist[iSol[iRota][iPos - 1]][iSol[iRota][iPos]];
	return dD;
}

double DemandaRota(int iSol[][M], int iRota) {
	double dD = 0;

	int iPos = 1;
	while (iSol[iRota][iPos] != 0) {
		dD += Demanda[iSol[iRota][iPos]];
		iPos++;
	}
	return dD;
}

int iTamanhoRota(int iSol[][M], int iRota) {
	int iPos = 1;
	while (iSol[iRota][iPos] != 0) {
		iPos++;
	}
	return iPos-1;
}

int TipodeVeiculo(double d) { //Determina o tipo de veiculo
	int k = 1;
	while (Capac[k] < d)
		k++;
	return k;
}

int FaixaDistancia(double d) { // Determina a faixa de distancia
	int f = F;
	if (d == 0)
		return 0;
	while (InicioFaixa[f] > d)
		f--;
	return f;
}

int ContaRotas(int Sol[M][M]) {
	int iClienteRoteado[M];
	int iContaRota=0;

	for (int i = 1; i < M; i++)
		iClienteRoteado[i] = 0;

	for (iContaRota = 1; iContaRota <= M; iContaRota++){
		int iPos = 1;
		if (Sol[iContaRota][iPos] < 1)
			return iContaRota - 1;
		while (Sol[iContaRota][iPos] != 0) {
			iClienteRoteado[Sol[iContaRota][iPos]]++;
			if (iClienteRoteado[Sol[iContaRota][iPos]] > 1)
				return iContaRota - 1;
			iPos++;
		}

	}
	return iContaRota;
}

double CustoRota(double dist, double dem) {
	int k, f;
	double custo;

	k = TipodeVeiculo(dem);
	f = FaixaDistancia(dist);

	//Calculo do custo total da rota:
	if (f == 0)
		return 0;

	if (f<F)
		custo = CustoFaixa[abc][k][f];
	else
		custo = CustoFaixa[abc][k][F - 1] + (dist - InicioFaixa[F])*CustoFaixa[abc][k][F];
	return custo;
}

void DadosRota(int rota) { //Determina a distancia total e o inicio de atendimento em cada cliente
	double fDistancia = 0;
	double fDemanda = 0;
	double fCusto;

	InicioAtend[0] = 0;
	int i = 1;
	do {
		fDistancia = fDistancia + Dist[Rota[rota][i - 1]][Rota[rota][i]];
		fDemanda = fDemanda + Demanda[Rota[rota][i]];
		InicioAtend[Rota[rota][i]] = CalcInicioAtend(Rota[rota][i - 1], Rota[rota][i], InicioAtend[Rota[rota][i - 1]]);
		i++;
	} while (Rota[rota][i] > 0);
	fDistancia = fDistancia + Dist[Rota[rota][i - 1]][Rota[rota][0]]; //Necess�rio somar a �ltima pernada da viagem, de volta ao armaz�m.

	fCusto = CustoRota(fDistancia, fDemanda);

	Demanda_Rota[rota] = fDemanda;
	DistTotal[rota] = fDistancia;
	Custo[rota] = fCusto;
}

bool bValidaRota(int iSol[][M], int iRota,bool(&bAtendido)[M]) {
	double dDemanda = 0;
	double dInicioAtend = 0;
	for (int iPos = 1; iSol[iRota][iPos] != 0; iPos++) {
		dDemanda += Demanda[iSol[iRota][iPos]];
		//Se a demanda for superior a capacidade do maior veiculo:
		if (dDemanda > Capac[K])
			return false;
		//Se as janelas de tempo nao forem atendidas:
		dInicioAtend = CalcInicioAtend(iSol[iRota][iPos - 1], iSol[iRota][iPos], dInicioAtend);
		if (dInicioAtend > FimJan[iSol[iRota][iPos]])
			return false;
		// se um cliente ja foi atendido significa que ele foi roteado 2 vezes:
		if (bAtendido[iSol[iRota][iPos]])
			return false;
		else
			bAtendido[iSol[iRota][iPos]] = true;
	}
	return true;
}

bool bValidaSol(int iSol[][M]) {
	bool bAtendido[M];
	bool bValida = true;

	for (int i = 1; i <= N; i++) bAtendido[i] = false;

	int iTotRotas = ContaRotas(iSol);
	//cada rota deve ser avaliada individualmente
	for (int iRota = 1; iRota <= iTotRotas; iRota++) {
		bValida = bValida && bValidaRota(iSol, iRota,bAtendido);
		if (!bValida)
			return false;
	}
	//Apos rodar todas as rotas, verifico se faltou alguem:
	for (int i = 1; i <= N; i++)
		if(!bAtendido[i])
			return false;
	return true;
}

double CustoSolucao(int Sol[][M]) {
	double dCusto = 0;
	int QtdRotas = ContaRotas(Sol);

	for (int i = 1; i <= QtdRotas; i++) {
		double dDist = 0;
		double dDem = 0;
		int p = 1;
		while (Sol[i][p] != 0) {
			dDist = dDist + Dist[Sol[i][p - 1]][Sol[i][p]];
			dDem = dDem + Demanda[Sol[i][p]];
			p++;
		}
		dDist = dDist + Dist[Sol[i][p - 1]][Sol[i][p]];

		dCusto = dCusto + CustoRota(dDist, dDem);
	}
	//assert(dCusto >=0);
	return dCusto;
}

void MostraRotas(string sNomeInstancia, int iSol[][M]) {
	ofstream SolDetalhada, SolResumida;
	SolDetalhada.open("SolucaoDetalhada" + sNomeInstancia + ".txt");

	//Dados totais do problema:
	double dDistRota[M], dDemRota[M], dCustoRota[M];
	double CustoTotal = 0;
	double DistanciaTotal = 0;
	int QtdRotas = ContaRotas(iSol);

	for (int i = 1; i <= QtdRotas; i++) {
		dDistRota[i] = 0;
		dDemRota[i] = 0;
		int j = 1;
		while (iSol[i][j] != 0){
			dDistRota[i] += Dist[iSol[i][j-1]][iSol[i][j]];
			dDemRota[i] += Demanda[iSol[i][j]];
			j++;
		}
		dDistRota[i] += Dist[iSol[i][j - 1]][iSol[i][j]];

		dCustoRota[i] = CustoRota(dDistRota[i], dDemRota[i]);
		CustoTotal += dCustoRota[i];
		DistanciaTotal += dDistRota[i];
	}

	//Preenche Solu��o Detalhada:
	SolDetalhada << "---------------------------------------" << endl << "Nome: " << sNomeInstancia << endl;
	SolDetalhada << "Indices:" << endl << "Alfa1: " << alfa[1] / 10 << " Alfa2: " << alfa[2] / 10 << " Alfa3: " << alfa[3] / 10 << " Alfa4: " << alfa[4] / 10 << endl << "Beta1: " << beta[1] / 10 << " Beta2: " << beta[2] / 10 << " Beta3: " << beta[3] / 10 << " Beta4: " << beta[4] / 10 << endl;
	SolDetalhada << "Qtd_Rotas: " << QtdRotas << " Dist_Total: " << DistanciaTotal << " Custo_Total: " << CustoTotal << endl;
	SolDetalhada << "-------------ROTAS---------------------" << endl << "#  Custo Demanda Distancia Clientes" << endl;
	for (int i = 1;i <= QtdRotas;i++) {
		SolDetalhada << i << " " << dCustoRota[i] << " " << dDemRota[i] << " " << dDistRota[i];
		int c = 1;
		SolDetalhada << " " << 0;
		while (iSol[i][c]>0) {
			SolDetalhada << " " << iSol[i][c];
			c++;
		}
		SolDetalhada << " " << iSol[i][c] << endl;
	}
	SolDetalhada << endl;
	SolDetalhada.close();
}

bool Indices(int inst) {
	//	bool ok = false;
	if (Teste == "168") {
		if (contRodadas == 0) {
			if (Heuristica == 1) {
				if (inst <= 2) {			//R1 e R2
					alfa[1] = (double)10;
					alfa[2] = (double)0;
					alfa[3] = (double)2;
					alfa[4] = (double)0;
					beta[1] = (double)1;
					beta[2] = (double)1;
					beta[3] = (double)1;
					beta[4] = (double)0;
				}
				else if (inst <= 4) {		//C1 e C2
					alfa[1] = (double)8;
					alfa[2] = (double)0;
					alfa[3] = (double)3;
					alfa[4] = (double)0;
					beta[1] = (double)2;
					beta[2] = (double)0;
					beta[3] = (double)1;
					beta[4] = (double)0;
				}
				else {						//RC1 e RC2
					alfa[1] = (double)10;
					alfa[2] = (double)0;
					alfa[3] = (double)2;
					alfa[4] = (double)0;
					beta[1] = (double)1;
					beta[2] = (double)1;
					beta[3] = (double)1;
					beta[4] = (double)0;
				}
			}
			if (Heuristica == 2 || Heuristica == 23) {
				if (inst <= 2) {			//R1 e R2
					alfa[1] = (double)1;
					alfa[2] = (double)0;
					alfa[3] = (double)9;
					alfa[4] = (double)2;
					beta[1] = (double)1;
					beta[2] = (double)0;
					beta[3] = (double)5;
					beta[4] = (double)1;
				}
				else if (inst <= 4) {			//C1 e C2
					alfa[1] = (double)4;
					alfa[2] = (double)0;
					alfa[3] = (double)9;
					alfa[4] = (double)1;
					beta[1] = (double)0;
					beta[2] = (double)1;
					beta[3] = (double)3;
					beta[4] = (double)1;
				}
				else {						//RC1 e RC2
					alfa[1] = (double)10;
					alfa[2] = (double)0;
					alfa[3] = (double)2;
					alfa[4] = (double)0;
					beta[1] = (double)1;
					beta[2] = (double)1;
					beta[3] = (double)1;
					beta[4] = (double)0;
				}
			}
			if (Heuristica == 3 || Heuristica == 32) {
				/* Indices iguais para todas as inst�ncias:
				alfa[1] = (double)1;
				alfa[2] = (double)0;
				alfa[3] = (double)10;
				alfa[4] = (double)2;
				beta[1] = (double)0;
				beta[2] = (double)2;
				beta[3] = (double)2;
				beta[4] = (double)3;
				*/
				if (inst <= 2) {			//R1 e R2
					alfa[1] = (double)1;
					alfa[2] = (double)0;
					alfa[3] = (double)9;
					alfa[4] = (double)1;
					beta[1] = (double)0;
					beta[2] = (double)2;
					beta[3] = (double)5;
					beta[4] = (double)4;
				}
				else if (inst <= 4) {		//C1 e C2
					alfa[1] = (double)4;
					alfa[2] = (double)0;
					alfa[3] = (double)10;
					alfa[4] = (double)1;
					beta[1] = (double)0;
					beta[2] = (double)1;
					beta[3] = (double)2;
					beta[4] = (double)3;
				}
				else {						//RC1 e RC2
					alfa[1] = (double)2;
					alfa[2] = (double)0;
					alfa[3] = (double)10;
					alfa[4] = (double)1; //Teste
					beta[1] = (double)1;
					beta[2] = (double)3;
					beta[3] = (double)3;
					beta[4] = (double)2;
				} 
			}
		}
		else {
			return false;
		}
	}
	if (Teste == "Mini") {
		if (contRodadas == 0) {
			if (Heuristica == 1) {
				alfa[1] = (double)10;
				alfa[2] = (double)0;
				alfa[3] = (double)2;
				alfa[4] = (double)0;
				beta[1] = (double)1;
				beta[2] = (double)1;
				beta[3] = (double)1;
				beta[4] = (double)0;
			}
			if (Heuristica == 2 || Heuristica == 23) {
				alfa[1] = (double)2;
				alfa[2] = (double)0;
				alfa[3] = (double)10;
				alfa[4] = (double)1;
				beta[1] = (double)0;
				beta[2] = (double)3;
				beta[3] = (double)1;
				beta[4] = (double)1;
			}
			if (Heuristica == 3 || Heuristica == 32) {
				alfa[1] = (double)2;
				alfa[2] = (double)0;
				alfa[3] = (double)10;
				alfa[4] = (double)1; //Teste
				beta[1] = (double)1;
				beta[2] = (double)3;
				beta[3] = (double)3;
				beta[4] = (double)2;
			}
		}
		else {
			return false;
		}
	}
	if (Teste == "CalibracaoIndices") {
		int maxAlfa1, maxAlfa2, maxAlfa3, maxAlfa4, maxBeta1, maxBeta2, maxBeta3, maxBeta4;
		int minAlfa1, minAlfa2, minAlfa3, minAlfa4, minBeta1, minBeta2, minBeta3, minBeta4;
		if (Heuristica == 1) {
			maxAlfa1 = 10; maxAlfa2 = 3; maxAlfa3 = 10; maxAlfa4 = 0; maxBeta1 = 5; maxBeta2 = 3; maxBeta3 = 4; maxBeta4 = 0;
			minAlfa1 = 7; minAlfa2 = 0; minAlfa3 = 0; minAlfa4 = 0; minBeta1 = 0; minBeta2 = 0; minBeta3 = 0; minBeta4 = 0;
		}
		if (Heuristica == 2 || Heuristica == 23) {
			maxAlfa1 = 3; maxAlfa2 = 1; maxAlfa3 = 10; maxAlfa4 = 2; maxBeta1 = 2; maxBeta2 = 4; maxBeta3 = 5; maxBeta4 = 2;
			minAlfa1 = 1; minAlfa2 = 0; minAlfa3 = 6; minAlfa4 = 0; minBeta1 = 0; minBeta2 = 1; minBeta3 = 0; minBeta4 = 0;
		}
		if (Heuristica == 3 || Heuristica == 32) {
			maxAlfa1 = 5; maxAlfa2 = 2; maxAlfa3 = 10; maxAlfa4 = 3; maxBeta1 =5; maxBeta2 = 3; maxBeta3 = 8; maxBeta4 = 4;
			minAlfa1 = 0; minAlfa2 = 0; minAlfa3 = 5; minAlfa4 = 0; minBeta1 = 0; minBeta2 = 0; minBeta3 = 2; minBeta4 = 1;
		}
		if (contRodadas == 0) {
			alfa[1] = (double)minAlfa1;
			alfa[2] = (double)minAlfa2;
			alfa[3] = (double)minAlfa3;
			alfa[4] = (double)minAlfa4;
			beta[1] = (double)minBeta1;
			beta[2] = (double)minBeta2;
			beta[3] = (double)minBeta3;
			beta[4] = (double)minBeta4;
		}
		else {
			if (beta[4] < maxBeta4)
				beta[4] ++;
			else {
				beta[4] = (double)minBeta4;
				if (beta[3] <  maxBeta3)
					beta[3]++;
				else {
					beta[3] = (double)minBeta3;
					if (beta[2] < maxBeta2)
						beta[2]++;
					else {
						beta[2] = (double)minBeta2;
						if (beta[1] <  maxBeta1)
							beta[1]++;
						else {
							beta[1] = (double)minBeta1;
							if (alfa[4] <  maxAlfa4)
								alfa[4]++;
							else {
								alfa[4] = (double)minAlfa4;
								if (alfa[3] < maxAlfa3)
									alfa[3]++;
								else {
									alfa[3] = (double)minAlfa3;
									if (alfa[2] < maxAlfa2)
										alfa[2]++;
									else {
										alfa[2] = (double)minAlfa2;
										if (alfa[1] < maxAlfa1)
											alfa[1]++;
										else {
											return false;
										}
									}
								}
							}
						}
					}
				}
			}
		}
	}
	return true;
}

//Prompt para arquivos especificos
string sGetSourcefile() {
	ifstream infile;

	cout << "Please enter the input file name> " << flush;
	while (true)
	{
		string infilename;
		getline(cin, infilename);
		infile.open(infilename.c_str());
		if (infile) {
			return infilename;
		}
		cout << "Invalid file. Please enter a valid input file name> " << flush;
	}
}

void SetCalibHeader(std::ostream& Output){
	for (int iExpShake = 5; iExpShake <= 10; iExpShake += 5)
		for (int iLimRelocate = 20; iLimRelocate <= 40; iLimRelocate += 10)
			for (int iLimCross = 1; iLimCross <= 10; iLimCross += 5)
				for (int iLim2Opt = 25; iLim2Opt <= 75; iLim2Opt += 25) {
					//Ajuste para ficar 1-5-10:
					if (iExpShake == 6)iExpShake = 5;
					Output << " s" << iExpShake << "r" << iLimRelocate;
					if (iLimCross == 6) iLimCross = 5;
					Output << "c" << iLimCross << "o" << iLim2Opt << " QtdMelhorias";
				}
}

//Funcoes para calcular as mini-instancias:
void RodaMiniInstancias(string sNomeHeuristica) {
	clock_t tInicio; //Para marcar o tempo
	double dTempo;
	ifstream Dados;
	ofstream MiniResultados;
	int iQtdInst;
	string sNomeInstancia;
	double dResultado, dResultadoBL;

	//Abre o arquivo para salvar os resultados das miniinstancias:
	MiniResultados.open("ResumoMini_" + sNomeHeuristica + ".txt");

	//Preparamos o Cabecalho busca local:
	MiniResultados << "Instancia Tempo_Heuristica Resultado_Heuristica ";
	if (TesteBL == "VNS") {
		MiniResultados << " Tempo_VNS VNS QtdMelhorias";
	}
	else if (TesteBL == "GRASP")
		MiniResultados << " Tempo_BL Custo_BL QtdMelhorias_BL Tempo_GRASP SolGRASP Rotas QtdMelhorias";
	else {
		int iRemDe, iRemPara, iInsDe, iInsPara;
		if (OperadorRemocao == 0) { iRemDe = 1; iRemPara = 5; }
		else { iRemDe = OperadorRemocao; iRemPara = OperadorRemocao; }
		if (OperadorInsercao == 0) { iInsDe = 1; iInsPara = 4; }
		else { iInsDe = OperadorInsercao; iInsPara = OperadorInsercao; }
		for (int iRem = iRemDe; iRem <= iRemPara; iRem++)
			for (int iIns = iInsDe; iIns <= iInsPara; iIns++)
				MiniResultados << " Tempo_" << NomeOperadorRelocate(iRem, "R") << "+" << NomeOperadorRelocate(iIns, "I") << " " << NomeOperadorRelocate(iRem, "R") << "+" << NomeOperadorRelocate(iIns, "I");
	}
	//Abre o arquivo das mini-inst�ncias:
	//Dados.open("MiniInstancias.txt");
	Dados.open(sGetSourcefile());
	//Descobre a quantidade de inst�ncias que est�o l�:
	Dados >> iQtdInst;

	//Roda uma por uma:
	for (int iInstancia = 1;iInstancia <= iQtdInst;iInstancia++) {

			//Coleta os dados dessa inst�ncia:
			Dados >> sNomeInstancia;
			cout << endl << "Agora avaliando a instancia " << sNomeInstancia;
			Dados >> N >> K >> F;
			for (int i = 0; i <= N; i++)
				for (int j = 0; j <= N; j++)
					Dados >> Dist[i][j];
			for (int i = 0; i <= N; i++)
				for (int j = 0; j <= N; j++)
					Dados >> Tempo[i][j];
			for (int i = 0; i <= N; i++)
				Dados >> InicioJan[i] >> FimJan[i] >> Serv[i] >> Demanda[i];

			for (int i = 1; i <= K; i++)
				Dados >> Capac[i];		//Capacidade dos tipos de veiculo
			for (int i = 1; i <= F; i++)
				Dados >> InicioFaixa[i];	//Inicio das faixas de distancia
			for (int i = 1; i <= K; i++)
				for (int j = 1; j <= F; j++)
					Dados >> CustoFaixa[1][i][j];// Os custos a, b e c de veiculos

			//Parametro de outros testes que temos que usar:
			abc = 1;
			contRodadas = 0;
			//		double inst = 6 * ((double)iInstancia / (double)iQtdInst);
			Indices((int)ceil(6 * ((double)iInstancia / (double)iQtdInst)));

			//if (iInstancia > 1) { //Para pular as primeiras


			//Passo a resolver a heur�stica:
			tInicio = clock();
			dResultado = SCIH();
			//tFim = clock();
			//Armazeno os resultados:
			dTempo = dElapsedTime(tInicio, clock());
			if (PrintRoutes)
				MostraRotas(sNomeInstancia +"_" + sNomeHeuristica, Rota);
			MiniResultados << endl << sNomeInstancia << " " << dTempo << " " << dResultado;

			int iMelhorias = 0;
			//Iniciamos a busca local ou Meta Heuristica:
			srand(42);
			if (TesteBL == "VNS") {
				tInicio = clock();
				dResultadoBL = VNS(Rota, iQtdLS, dMaxExec, sNomeInstancia + "_" + sNomeHeuristica, iMelhorias, RodadasShake, MaxShakeRelocate, MaxShakeCross, MaxShake2Opt);
				//tFim = clock();
				dTempo = dElapsedTime(tInicio, clock());
				MiniResultados << " " << dTempo << " " << dResultadoBL << " " << iMelhorias;
			}
			else if (TesteBL == "GRASP") {
				tInicio = clock();
				dResultadoBL = BuscaLocal(Rota, "Cross", sNomeInstancia + "_" + sNomeHeuristica, OperadorRemocao, OperadorInsercao, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
				//tFim = clock();
				dTempo = dElapsedTime(tInicio, clock());
				MiniResultados << " " << dTempo << " " << dResultadoBL << " " << iMelhorias;
				cout << endl << "Resultado Busca Local: " << dResultadoBL;
				tInicio = clock();
				dResultadoBL = dGRASP(Rota,sNomeInstancia + "_" + sNomeHeuristica, GRASP_TamLRCd, GRASP_LRCC2_Tipo, GRASP_LRCC2_Tam, iMelhorias, CrossMaxTamTrecho, CientesRelocate, OperadorRemocao,OperadorInsercao);
				//tFim = clock();
				dTempo = dElapsedTime(tInicio, clock());
				//dTempo = ((double)tFim - (double)tInicio) / CLOCKS_PER_SEC;
				MiniResultados << " " << dTempo << " " << dResultadoBL << " " << ContaRotas(Rota) << " " << iMelhorias;
				cout << endl << "Resultado GRASP: " << dResultadoBL;
				if (PrintRoutes)
					MostraRotas(sNomeInstancia + "_" + sNomeHeuristica + "_GRASP", Rota);
			}
			else if (TesteBL == "Hybrid") {
				tInicio = clock();
				dResultadoBL = Hybrid(Rota, sNomeInstancia + "_" + sNomeHeuristica, iMelhorias, HYBRID_ShareAleat, iQtdLS, GRASP_TamLRCd, GRASP_LRCC2_Tipo, GRASP_LRCC2_Tam, RodadasShake, MaxShakeRelocate, MaxShakeCross, MaxShake2Opt);
				MiniResultados << " " << dElapsedTime(tInicio, clock()) << " " << dResultadoBL << " " << ContaRotas(Rota) << " " << iMelhorias;
				if (PrintRoutes)
					MostraRotas(sNomeInstancia + "_" + sNomeHeuristica + "_" + TesteBL, Rota);
			}
			else {
				if (TesteBL != "None") {
					if (TesteBL == "Relocate" || TesteBL == "All") {
						int iRemDe, iRemPara, iInsDe, iInsPara;
						if (OperadorRemocao == 0) { iRemDe = 1; iRemPara = 5; }
						else { iRemDe = OperadorRemocao; iRemPara = OperadorRemocao; }
						if (OperadorInsercao == 0) { iInsDe = 1; iInsPara = 4; }
						else { iInsDe = OperadorInsercao; iInsPara = OperadorInsercao; }
						for (int iRem = iRemDe; iRem <= iRemPara; iRem++)
							for (int iIns = iInsDe; iIns <= iInsPara; iIns++) {
								tInicio = clock();
								dResultadoBL = BuscaLocal(Rota, "Relocate", sNomeInstancia, iRem, iIns, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
								//tFim = clock();
								//dTempo = ((double)tFim - (double)tInicio) / CLOCKS_PER_SEC;
								MiniResultados << " " << dElapsedTime(tInicio, clock()) << " " << dResultadoBL << " " << iMelhorias;
							}
					}
					if (TesteBL == "Cross" || TesteBL == "All") {
						tInicio = clock();
						dResultadoBL = BuscaLocal(Rota, "Cross", sNomeInstancia, 0, 0, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
						//tFim = clock();
						//dTempo = ((double)tFim - (double)tInicio) / CLOCKS_PER_SEC;
						MiniResultados << " " << dElapsedTime(tInicio, clock()) << " " << dResultadoBL << " " << iMelhorias;
					}
					if (TesteBL == "2Opt*" || TesteBL == "All") {
						tInicio = clock();
						dResultadoBL = BuscaLocal(Rota, "2Opt*", sNomeInstancia, 0, 0, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
						//tFim = clock();
						//dTempo = ((double)tFim - (double)tInicio) / CLOCKS_PER_SEC;
						MiniResultados << " " << dElapsedTime(tInicio, clock()) << " " << dResultadoBL << " " << iMelhorias;
					}
				}
			//} //Somente para pular as primeiras
			}
	}
	MiniResultados.close();
}

int main() {
	ofstream Results;
	string Instancia[8];
	int qtd;
	double CustoInstancia = 0;
	double CustoInstanciaBL = 0;
	string veic, sNomeHeuristica;
	int iMelhorias;
	int iSolTemp[M][M];

	clock_t Inicio, InicioBL; //Para marcar o tempo
	double TempoExec;

	switch (Heuristica) {
	case 1:
		sNomeHeuristica = "SCIH1";
		break;
	case 2: sNomeHeuristica = "SCIH2";
		break;
	case 3: sNomeHeuristica = "SCIH3";
		break;
	case 23: sNomeHeuristica = "SCIH2+3";
		break;
	case 32: sNomeHeuristica = "SCIH3+2";
		break;
	default:
		break;
	}
	//De cara, vamos confirmar o que vai ser rodado na tela:
	cout << "Sera rodado a heuristica construtiva " << sNomeHeuristica << " e";
	if (TesteBL == "None")
		cout << " nada mais.";
	else {
		if (TesteBL == "VNS")
			cout << " VNS ";
		else if (TesteBL == "GRASP")
			cout << " GRASP ";
		else if (TesteBL == "Hybrid")
			cout << " Metaheuristica Hibrida ";
		else
			cout << " Busca Local";
		cout << "por " << dMaxExec << " segundos.";
	}
	cout << endl;

	cout << "Inicio dos testes" << endl;
	if (Teste != "Mini") {
	
		if (Teste == "168"){
			Results.open("ResumoSols.txt");
			Results << "Inicio Teste: " << clock();
			if (TesteBL == "VNS" && RodadaVNS != "CalibraShake")
				Results << " RodadasSemMelhoraShale:" << RodadasShake << " MaxTrocasRelocate:" << MaxShakeRelocate << " MaxTrocasCross:" << MaxShakeCross << " MaxTrocas2opt:" << MaxShake2Opt;
			Results << endl << "Problema" << " " << "Tempo_SCIH" << Heuristica << " " << "Result_SCIH" << Heuristica;
			if (TesteBL == "All" || TesteBL == "Relocate") {
				int iRemDe, iRemPara, iInsDe, iInsPara;
				if (OperadorRemocao == 0) { iRemDe = 1; iRemPara = 5; }
				else { iRemDe = OperadorRemocao; iRemPara = OperadorRemocao; }
				if (OperadorInsercao == 0) { iInsDe = 1; iInsPara = 4; }
				else { iInsDe = OperadorInsercao; iInsPara = OperadorInsercao; }
				for (int iRem = iRemDe; iRem <= iRemPara; iRem++)
					for (int iIns = iInsDe; iIns <= iInsPara; iIns++)
						Results << " Tempo_Relocate_" << NomeOperadorRelocate(iRem, "R") << NomeOperadorRelocate(iIns, "I") << " Relocate_" << NomeOperadorRelocate(iRem, "R") << NomeOperadorRelocate(iIns, "I") <<" QtdMelhorias";
			}
			if (TesteBL == "All" || TesteBL == "Cross")
				Results << " Tempo_Cross Cross QtdMelhorias";
			if (TesteBL == "All" || TesteBL == "2Opt*")
				Results << " Tempo_2Opt* 2Opt* QtdMelhorias";
			if (TesteBL == "VNS") {
				//22/08/2019 - Vou tentar fazer tudo no mesmo txt:
				if (RodadaVNS == "CalibraShake")
					SetCalibHeader(Results);
				else Results << " Tempo_VNS VNS QtdMelhorias";
			}
			if (TesteBL == "GRASP")
				if (RodadaVNS == "CalibraShake") {
					for (int iTamLista_Dist = 2; iTamLista_Dist <= 5; iTamLista_Dist++) {
						for (int iTamLista_C2 = 2; iTamLista_C2 <= 5; iTamLista_C2++) {
							Results << " Tempo_LRCd" << iTamLista_Dist << "_LRCC2f" << iTamLista_C2;
							Results << " Custo_LRCd" << iTamLista_Dist << "_LRCC2f" << iTamLista_C2;
							Results << " QtdMelhoria_LRCd" << iTamLista_Dist << "_LRCC2f" << iTamLista_C2;
						}
						for (int iTamLista_C2 = 10; iTamLista_C2 <= 50; iTamLista_C2 += 20) {
							Results << " Tempo_LRCd" << iTamLista_Dist << "_LRCC2d" << iTamLista_C2;
							Results << " Custo_LRCd" << iTamLista_Dist << "_LRCC2d" << iTamLista_C2;
							Results << " QtdMelhoria_LRCd" << iTamLista_Dist << "_LRCC2d" << iTamLista_C2;
						}
						for (int iTamLista_C2 = 10; iTamLista_C2 <= 50; iTamLista_C2 += 20) {
							Results << " Tempo_LRCd" << iTamLista_Dist << "_LRCC2t" << iTamLista_C2;
							Results << " Custo_LRCd" << iTamLista_Dist << "_LRCC2t" << iTamLista_C2;
							Results << " QtdMelhoria_LRCd" << iTamLista_Dist << "_LRCC2t" << iTamLista_C2;
						}
					}
				}
				else
					Results << " Tempo_BL Custo_BL QtdMelhorias_BL Tempo_GRASP SolGRASP QtdMelhorias";
			if (TesteBL == "Hybrid")
				if (RodadaVNS == "Calibra" || RodadaVNS == "CalibraShake")
					for (double dShare = HYBRID_MinShareAleat; dShare <= HYBRID_MaxShareAleat; dShare += HYBRID_PassoShareAleat)
						Results << " SolShare"<< dShare*100 << "% QtdMelhorias" << dShare * 100 << "%";
				else
					Results << " Tempo_Hybrid Custo_Hybrid QtdMelhorias_Hybrid";

		}
		//Results.close();
		//Prepara arquivos das instancias)*/
		Instancia[1] = "R1";
		Instancia[2] = "R2";
		Instancia[3] = "C1";
		Instancia[4] = "C2";
		Instancia[5] = "RC1";
		Instancia[6] = "RC2";

		for (int grupoinst = 1; grupoinst <= 6; grupoinst++) {  //Gambiarra pra rodar s� a inst�ncia que quero
			ColetaDadosGrupos("Geral" + Instancia[grupoinst] + ".txt");

			ArqInstancia.open("../Inst" + Instancia[grupoinst] + ".txt");

			ArqInstancia >> qtd;
			if (Teste == "CalibracaoIndices")	qtd = 3;
			if (RodadaVNS == "CalibraShake")	qtd = 2; //Alteração conforme discutido em 22/08/2019
			if (RodadaVNS == "Calibra")			qtd = 3;

			for (int i = 1; i <= qtd; i++) {
				ColetaDadosInstancias();
				//if (RodadaVNS == "Compl" && i > 3) {
					for (abc = 1; abc <= 3; abc++) {
						contRodadas = 0;
						if (abc == 1) veic = "a";
						if (abc == 2) veic = "b";
						if (abc == 3) veic = "c";

						cout << endl << "--------------------" << endl << "Agora avaliando a instancia " << NomeProb << veic;

						//Prepara cabecalho do arquivo de output:
						if (Teste == "CalibracaoIndices") {
							Results.open("ResumoInst" + NomeProb + veic + ".txt");
							//Cabecalho dos resultados:
							Results << "Alfa1;Alfa2;Alfa3;Alfa4;Beta1;Beta2;Beta3;Beta4;Resultado" << endl;
							cout << "Agora testando indices no problema " << NomeProb << veic << endl;
						}
						while (Indices(grupoinst)) {
							contRodadas++;
							if (Teste == "CalibracaoIndices")
								Results << alfa[1] << ";" << alfa[2] << ";" << alfa[3] << ";" << alfa[4] << ";" << beta[1] << ";" << beta[2] << ";" << beta[3] << ";" << beta[4] << ";";

							//Roda Construtiva:
							Inicio = clock();
							CustoInstancia = SCIH();
							//FimSCIH = clock();

							TempoExec = dElapsedTime(Inicio, clock());
							if (Teste == "168") {
								if (PrintRoutes)
									MostraRotas(NomeProb + veic + "_" + sNomeHeuristica, Rota);
								Results << endl << NomeProb << veic << " " << TempoExec << " ";
							}
							Results << CustoInstancia;
							cout << endl << "Resultado Heuristica: " << CustoInstancia;

							//Iniciamos a busca local:
							srand(42);
							InicioBL = clock();
							if (TesteBL == "VNS") {
								if (RodadaVNS == "CalibraShake")
									for (int iExpShake = 5; iExpShake <= 10; iExpShake += 5)
										for (int iLimRelocate = 20; iLimRelocate <= 40; iLimRelocate += 10)
											for (int iLimCross = 1; iLimCross <= 10; iLimCross += 5)
												for (int iLim2Opt = 25; iLim2Opt <= 75; iLim2Opt += 25) {
													//Ajuste para ficar 1-5-10:
													if (iExpShake == 6) iExpShake = 5;
													if (iLimCross == 6) iLimCross = 5;
													//Manda ver!
													iMelhorias = 0;
													cout << endl << endl << "Instancia: " << NomeProb << veic << " - Shake: s" << iExpShake << "r" << iLimRelocate << "c" << iLimCross << "o" << iLim2Opt;
													CustoInstanciaBL = VNS(Rota, iQtdLS, dMaxExec, NomeProb + veic, iMelhorias, iExpShake, iLimRelocate, iLimCross, iLim2Opt);
													Results << " " << CustoInstanciaBL << " " << iMelhorias;
												}
								else {
									CustoInstanciaBL = VNS(Rota, iQtdLS, dMaxExec, NomeProb + veic, iMelhorias, RodadasShake, MaxShakeRelocate, MaxShakeCross, MaxShake2Opt);
									Results << " " << CustoInstanciaBL << " " << iMelhorias;
									//FimBL = clock();
									//TempoExec = ((double)FimBL - (double)InicioBL) / CLOCKS_PER_SEC;
									Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
								}
							}
							else if (TesteBL == "GRASP") {
								if (RodadaVNS == "CalibraShake") {
									int iSolGRASP[M][M];
									for (int iTamLista_Dist = 2; iTamLista_Dist <= 5; iTamLista_Dist++) {
										int iTipoLRCC2 = 1;
										for (int iTamLista_C2 = 2; iTamLista_C2 <= 5; iTamLista_C2++) {
											cout << endl << "Testando Parametros: LRC_Dist Lim: " << iTamLista_Dist << ", LRC_C2f Lim: " << iTamLista_C2;
											CopiaSol(iSolGRASP, Rota);
											InicioBL = clock();
											CustoInstanciaBL = dGRASP(iSolGRASP, NomeProb + veic, iTamLista_Dist, iTipoLRCC2, iTamLista_C2, iMelhorias, CrossMaxTamTrecho, (int)round(N*CientesRelocate / 100), OperadorRemocao, OperadorInsercao);
											//FimBL = clock();
											//TempoExec = (double)(FimBL - InicioBL) / CLOCKS_PER_SEC;
											Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
											cout << endl << "Resultado GRASP: " << CustoInstanciaBL;
										}
										iTipoLRCC2++;
										for (int iTamLista_C2 = 10; iTamLista_C2 <= 50; iTamLista_C2 += 20) {
											cout << endl << "Testando Parametros: LRC_Dist Lim: " << iTamLista_Dist << ", LRC_C2d Lim: " << iTamLista_C2 << "%";
											CopiaSol(iSolGRASP, Rota);
											InicioBL = clock();
											CustoInstanciaBL = dGRASP(iSolGRASP, NomeProb + veic, iTamLista_Dist, iTipoLRCC2, iTamLista_C2, iMelhorias, CrossMaxTamTrecho, (int)round(N*CientesRelocate / 100), OperadorRemocao, OperadorInsercao);
											//FimBL = clock();
											//TempoExec = (double)(FimBL - InicioBL) / CLOCKS_PER_SEC;
											Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
											cout << endl << "Resultado GRASP: " << CustoInstanciaBL;
										}
										iTipoLRCC2++;
										for (int iTamLista_C2 = 10; iTamLista_C2 <= 50; iTamLista_C2 += 20) {
											cout << endl << "Testando Parametros: LRC_Dist Lim: " << iTamLista_Dist << ", LRC_C2t Lim: " << iTamLista_C2 << "%";
											CopiaSol(iSolGRASP, Rota);
											InicioBL = clock();
											CustoInstanciaBL = dGRASP(iSolGRASP, NomeProb + veic, iTamLista_Dist, iTipoLRCC2, iTamLista_C2, iMelhorias, CrossMaxTamTrecho, (int)round(N*CientesRelocate / 100), OperadorRemocao, OperadorInsercao);
											//FimBL = clock();
											//TempoExec = (double)(FimBL - InicioBL) / CLOCKS_PER_SEC;
											Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
											cout << endl << "Resultado GRASP: " << CustoInstanciaBL;
										}
									}
								} else {
									//Vamos primeiro coletar o valor com a BL:
									InicioBL = clock();
									CustoInstanciaBL = BuscaLocal(Rota, "Cross", NomeProb + veic, 0, 0, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
									//FimBL = clock();
									//TempoExec = ((double)FimBL - (double)InicioBL) / CLOCKS_PER_SEC;
									Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
									cout << endl << "Resultado Busca Local: " << CustoInstanciaBL;
									//Agora sim, vamos ao GRASP:
									InicioBL = clock();
									CustoInstanciaBL = dGRASP(Rota, NomeProb + veic, GRASP_TamLRCd, GRASP_LRCC2_Tipo, GRASP_LRCC2_Tam, iMelhorias, CrossMaxTamTrecho, (int)round(N*CientesRelocate / 100), OperadorRemocao, OperadorInsercao);
									//FimBL = clock();
									//TempoExec = ((double)FimBL - (double)InicioBL) / CLOCKS_PER_SEC;
									Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
									if (PrintRoutes)
										MostraRotas(NomeProb + veic + "_GRASP", Rota);
									cout << endl << "Resultado GRASP: " << CustoInstanciaBL;
								}
							}
							else if (TesteBL == "Hybrid") {
								if (RodadaVNS == "Calibra")
									for (double dShare = HYBRID_MinShareAleat; dShare <= HYBRID_MaxShareAleat; dShare += HYBRID_PassoShareAleat) {
										cout << endl << endl << "Avaliando Share de " << dShare * 100 << "% do tempo para o SCIHAleat.";
										CopiaSol(iSolTemp, Rota);
										CustoInstanciaBL = Hybrid(iSolTemp, NomeProb + veic, iMelhorias, dShare,iQtdLS, GRASP_TamLRCd, GRASP_LRCC2_Tipo, GRASP_LRCC2_Tam, RodadasShake, MaxShakeRelocate, MaxShakeCross, MaxShake2Opt);
										Results << " " << CustoInstanciaBL << " " << iMelhorias;
									}
								else {
									CopiaSol(iSolTemp, Rota);
									InicioBL = clock();
									CustoInstanciaBL = Hybrid(iSolTemp, NomeProb + veic, iMelhorias, HYBRID_ShareAleat, iQtdLS, GRASP_TamLRCd, GRASP_LRCC2_Tipo, GRASP_LRCC2_Tam, RodadasShake, MaxShakeRelocate, MaxShakeCross, MaxShake2Opt);
									Results << " " << CustoInstanciaBL << " " << iMelhorias;
									Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
									if (PrintRoutes)
										MostraRotas(NomeProb + veic,iSolTemp);
								}
							} else {
								if (TesteBL != "None") {
									if (TesteBL == "Relocate" || TesteBL == "All") {
										int iRemDe, iRemPara, iInsDe, iInsPara;
										if (OperadorRemocao == 0) { iRemDe = 1; iRemPara = 5; }
										else { iRemDe = OperadorRemocao; iRemPara = OperadorRemocao; }
										if (OperadorInsercao == 0) { iInsDe = 1; iInsPara = 4; }
										else { iInsDe = OperadorInsercao; iInsPara = OperadorInsercao; }
										for (int iRem = iRemDe; iRem <= iRemPara; iRem++)
											for (int iIns = iInsDe; iIns <= iInsPara; iIns++) {
												InicioBL = clock();
												CustoInstanciaBL = BuscaLocal(Rota, "Relocate", NomeProb + veic, iRem, iIns, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
												//FimBL = clock();
												//TempoExec = ((double)FimBL - (double)InicioBL) / CLOCKS_PER_SEC;
												Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
											}
									}
									if (TesteBL == "Cross" || TesteBL == "All") {
										InicioBL = clock();
										CustoInstanciaBL = BuscaLocal(Rota, "Cross", NomeProb + veic, 0, 0, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
										//FimBL = clock();
										//TempoExec = ((double)FimBL - (double)InicioBL) / CLOCKS_PER_SEC;
										Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
									}
									if (TesteBL == "2Opt*" || TesteBL == "All") {
										InicioBL = clock();
										CustoInstanciaBL = BuscaLocal(Rota, "2Opt*", NomeProb + veic, 0, 0, (int)round(N*CientesRelocate / 100), CrossMaxTamTrecho, false, iMelhorias);
										//FimBL = clock();
										//TempoExec = (double)(FimBL - InicioBL) / CLOCKS_PER_SEC;
										Results << " " << dElapsedTime(InicioBL, clock()) << " " << CustoInstanciaBL << " " << iMelhorias;
									}
								}
							}
						}
						if (Teste == "CalibracaoIndices")
							Results.close();
					}
				//}
			}
			ArqInstancia.close();
		}
		if (Teste == "168")
			Results.close();
		//system("PAUSE");
	} else{
		RodaMiniInstancias(sNomeHeuristica);
	}
	return 0;
}