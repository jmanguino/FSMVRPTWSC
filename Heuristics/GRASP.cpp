#include "Header.h"
#include "Functions.h"

//ofstream Passos;

void GeraLRC_Distantes(int (&iLista)[M], bool bAtendido[],int iTamLista) {
	double dDists[M], dCurDist;
	int iCurCli;
	for (int i = 1; i <= iTamLista; i++) {
		dDists[i] = 0;
		iLista[i] = 0;
	}
	for (int i = 1; i <= N; i++) {
		if (!bAtendido[i]) {
			dCurDist = Dist[0][i];
			iCurCli = i;
			for (int j = 1; j <= iTamLista; j++)
				if (dCurDist > dDists[j]) {
					double dTemp = dDists[j];
					int iTemp = iLista[j];
					dDists[j] = dCurDist;
					iLista[j] = iCurCli;
					dCurDist = dTemp;
					iCurCli = iTemp;
				}
		}
	}
}
//(iLRC_C2, dListaC2,iTipoListaC2, iLimListaC2, dC2Candidatos, iCandidatos)
void GeraLRC_C2(int(&iLista)[M], double(&dListaC2)[M], int iTipoLRCC2, int iTamLista, double dCandidatosC2[], int iCandidatos[]) {
	double dBestC2 = 0;
	int iBestCliC2 = 0;

	//Primeira rodada - Determina o Maior C2 e ja coloca na lista:
	for (int i = 1; iCandidatos[i] > 0; i++) {
		if (dCandidatosC2[i] > dBestC2) {
			dBestC2 = dCandidatosC2[i];
			iBestCliC2 = iCandidatos[i];
		}
	}
	int iPosLRC = 1;
	dListaC2[iPosLRC] = dBestC2;
	iLista[iPosLRC] = iBestCliC2;

	//Agora determinar o menor C2:
	//O tipo 1, o Minimo eh positivo:
	double dMinC2 = 0.001;
	//O tipo 2, devemos achar o menos C2 e dps calcular pela diferenca pro maior:
	if (iTipoLRCC2 == 2) {
		dMinC2 = dBestC2;
		for (int i = 1; iCandidatos[i] > 0; i++) {
			if (dCandidatosC2[i] > 0 && dCandidatosC2[i] < dMinC2)
				dMinC2 = dCandidatosC2[i];
		}
		//Agora sim, a proporcao:
		dMinC2 = dMinC2 + (dBestC2 - dMinC2) * (1 - (double)iTamLista / 100);
	}
	//O tioo 3, tanto porcento a menos que o maior:
	if(iTipoLRCC2 == 3)
		dMinC2 = dBestC2* (1 - (double)iTamLista / 100);

	//Agora roda todos os C2 que estejam maior que esse minimo:
	double dNextC2;
	do {
		dNextC2 = 0.01;
		for (int i = 1; iCandidatos[i] > 0; i++) {
			if (dCandidatosC2[i] > dNextC2 ) {
				bool bNew = true;
				for (int j = 1; j <= iPosLRC; j++)
					if (iLista[j] == iCandidatos[i])
						bNew = false;
				if (bNew) {
					dNextC2 = dCandidatosC2[i];
					iBestCliC2 = iCandidatos[i];
				}
			}
		}
		if (dNextC2 > dMinC2) {
			iPosLRC++;
			dListaC2[iPosLRC] = dNextC2;
			iLista[iPosLRC] = iBestCliC2;
			//Caso seja tamanho fixo, só sigo criando a LRC_C2 até chegar no tamanho desejado:
			if (iTipoLRCC2 == 1)
				if (iPosLRC >= iTamLista)
					break;
		}
	} while (dNextC2 > dMinC2);
}

int iEscolheLRC2_Proporcional(int iLRC_C2[], double dListaC2[]) {
	//A formula é P(i) = C2(i)/sum(C2)
	int iEscolha;
	double dLimite[M];
	double dSumC2 = 0;
	for(int i = 1; dListaC2[i] > 0; i++){
		dSumC2 += dListaC2[i];
	}

	double dShare = dSumC2;
	dLimite[0] = dSumC2;
	for (int i = 1; dListaC2[i] > 0; i++) {
		dShare = dShare - dListaC2[i];
		dLimite[i] = dShare;
		dLimite[i+1] = -1;
	}
	do {
		int iSorteio = rand() % (int)(dSumC2 * 10000);
		double dSorteio = (double)iSorteio / 10000;
		iEscolha = 0;
		while (dLimite[iEscolha] >= dSorteio) //Da ultima vez o erro foi bem aqui
			iEscolha++;
	} while (iLRC_C2[iEscolha] <= 0);
	return iLRC_C2[iEscolha];
}

double SCIH_Aleat(int(&SolAtual)[M][M], int iLimListaDist, int iTipoListaC2, int iLimListaC2) {
	int v;
	int iListaCandidatos[M];
	double dC2Candidatos[M];
	bool bAtendido[M];
	int iLRC[M], iLRC_C2[M];
	double dListaC2[M];
	bool HaViaveis;
	int iCliente;

	int NaoRoteados = N;
	double dMC1 = CustoFaixa[abc][K][F - 1] + 1000 * CustoFaixa[abc][K][F];

	//Zera variáveis para rodar, daqueadamente, as instâncias:
	for (int j = 1; j <= N; j++) bAtendido[j] = false;
	//Zera a rota para garantir que partimos de uma solução zerada.
	for (int i = 0; i < N; i++)
		for (int j = 0; j < N; j++)
			SolAtual[i][j] = 0;

	//Passos.open("PassosSCIHAleat" + NomeProb + ".txt");

	//Começa a resolver o problema
	v = 0;
	while (NaoRoteados>0) {
/***** ETAPA 1 - Cria nova rota com o cliente mais distante *****/
		//Nova Rota:
		GeraLRC_Distantes(iLRC, bAtendido, iLimListaDist);
		int iEscolhido = 0;
		do
			iEscolhido = iLRC[rand() % (iLimListaDist)+1];
		while (iEscolhido == 0);
		v++;
		SolAtual[v][0] = 0;
		SolAtual[v][1] = iEscolhido;
		SolAtual[v][2] = 0;
		NaoRoteados--;
		bAtendido[SolAtual[v][1]] = true;
		//Passos << endl << "Nova Rota: " << v << " = " << 0 << "-" << SolAtual[v][1] << "-" << SolAtual[v][2] << endl;
		do {
/***** ETAPA 2 - Lista candidatos para adicionar à rota *****/
			double CapDisp = Capac[K] - DemandaRota(SolAtual, v);
			int iCandidatos = 0;
			iListaCandidatos[1] = 0;
			int iBestPos[M];
			HaViaveis = false;
			for (iCliente = 1; iCliente <= N; iCliente++) {
				if (!bAtendido[iCliente] && Demanda[iCliente] <= CapDisp) {
					iCandidatos++;
					iListaCandidatos[iCandidatos] = iCliente;
					iListaCandidatos[iCandidatos + 1] = 0;
				}
			}
/***** ETAPA 3 - Escolhe melhor candidato e posição para adição à rota *****/
			for (int i = 0; i <= N; i++) {
				iLRC_C2[i] = 0;
				dListaC2[i] = 0;
			}
			//Passos << "Candidatos" << iCandidatos << ":";
			for(int i = 1;i<=iCandidatos;i++){
				iCliente = iListaCandidatos[i];
				double dBestC1 = dMC1 + 1;
				for (int iPos = 1; iPos <= iTamanhoRota(SolAtual, v) + 1; iPos++) {
					double dC1 = dCalculaC1(Heuristica, iCliente, SolAtual, v, iPos, dMC1,iListaCandidatos);
					if (dC1 < dBestC1) {
						dBestC1 = dC1;
						iBestPos[iCliente] = iPos;
					}
				}
				if (dBestC1 < dMC1) {
					double dC2 = dCalculaC2(Heuristica, iCliente, dBestC1, iListaCandidatos);
					if (dC2 > 0.1) {
						dC2Candidatos[i] = dC2;
						HaViaveis = true;
					}
					else
						dC2Candidatos[i] = -1;
				}
				else 
					dC2Candidatos[i] = -1;
				//	Passos << iCliente << " Inv,";
			}
			//Passos << endl;
			if (HaViaveis) {
				GeraLRC_C2(iLRC_C2, dListaC2,iTipoListaC2, iLimListaC2, dC2Candidatos, iListaCandidatos);
/***** ETAPA 4 - Adiciona o melhor candidato à rota *****/
				//int iEscolha = 0;
				//do
				//	iEscolha = rand() % (iLimListaC2)+1;
				//while (iLRC_C2[iEscolha] == 0);
				////Passos << "Cliente a adicionar: " << iLRC_C2[iEscolha] << " com C2 = " << dListaC2[iEscolha] << endl;
				//iEscolhido = iLRC_C2[iEscolha];
				iEscolhido = iEscolheLRC2_Proporcional(iLRC_C2, dListaC2);
				if (iEscolhido > 0) {
					bAtendido[iEscolhido] = true;
					NaoRoteados--;
					int iTemp = SolAtual[v][iBestPos[iEscolhido]];
					for (int iPos = iBestPos[iEscolhido]; iPos <= iTamanhoRota(SolAtual, v) + 1; iPos++) {
						SolAtual[v][iPos] = iEscolhido;
						iEscolhido = iTemp;
						iTemp = SolAtual[v][iPos + 1];
					}
				}
				//Passos << "Rota Atualizada: " << v << " = " << 0 << "-" << SolAtual[v][1];
				//int i = 1;
				//do {
				//	i++;
				//	Passos << "-" << SolAtual[v][i];
				//} while (SolAtual[v][i] != 0);
				//Passos << endl;
			}
			//else fprintf(Relatorio,"ETAPA2: Não existem candidatos viáveis para essa rota");
		} while (HaViaveis);
	}
	//Passos.close();
	return CustoSolucao(SolAtual);
}

double dGRASP(int(&iBestSol)[M][M], string sNomeInstancia, int iTamLRCd, int iTipoLRCC2,int iTamLRCC2,int &iMelhorias,int iMaxTrechoCross, int iClientesRelocate, int iRelocate_OpRemocao, int iRelocate_OpInsercao) {
	int iSol[M][M];
	//double  dBest = N*( CustoFaixa[abc][K][F - 1] + 1000 * CustoFaixa[abc][K][F]);
	double dBest = CustoSolucao(iBestSol);
	ofstream Evolucao;
	if (PrintEvolution) {
		string sFileName = "EvolucaoGRASP_" + sNomeInstancia + "_LRCD" + std::to_string(iTamLRCd) + "_LRCC2_Tipo" + std::to_string(iTipoLRCC2) + "Tam" + std::to_string(iTamLRCC2) +".txt";
		Evolucao.open(sFileName);
		Evolucao << "Evolucao das solucoes do GRASP por etapa" << endl;
		Evolucao << "Tempo(s);" << "Rodada;" << "Etapa" << ";" << "FuncaoObjetivo" << endl;
		Evolucao << "0;0; SCIH" << Heuristica << ";" << CustoSolucao(Rota);
	}
	int iRodada = 0;
	double dMinutos = 0;
	iMelhorias = 0;
	clock_t tStart = clock();
	do {
		iRodada++;
		double dCustoSol = SCIH_Aleat(iSol, iTamLRCd, iTipoLRCC2, iTamLRCC2);
		//cout << endl << "Resultado Heuristica Aleat: " << dCustoSol;
		//if (PrintEvolution)
		//	Evolucao << endl << (double)(clock() - tStart) / CLOCKS_PER_SEC << ";" << iRodada << ";Apos_SCIH_Aleatoria;" << CustoSolucao(iSol);
		if (bValidaSol(iSol)) {
			if (dCustoSol < dBest) {
				dBest = dCustoSol;
				CopiaSol(iBestSol, iSol);
				iMelhorias++;
				cout << endl << "New Best!!  " << dBest;
				if (PrintEvolution)
					Evolucao << endl << dElapsedTime(tStart,clock()) << ";" << iRodada << ";SCIH_Aleatoria;" << dBest;
			}
			int iMelhoriasBL = 0;
			//if (PrintEvolution)
			//	Evolucao << endl << (double)(clock() - tStart) / CLOCKS_PER_SEC << ";" << iRodada << ";Antes_Cross;" << CustoSolucao(iSol);
			dCustoSol = BuscaLocal(iSol, "Cross", sNomeInstancia, iRelocate_OpRemocao, iRelocate_OpInsercao, iClientesRelocate, iMaxTrechoCross, false, iMelhoriasBL);
			//if (PrintEvolution)
			//	Evolucao << endl << (double)(clock() - tStart) / CLOCKS_PER_SEC << ";" << iRodada << ";Depois_Cross;" << CustoSolucao(iSol);
			if (dCustoSol < dBest) {
				dBest = dCustoSol;
				CopiaSol(iBestSol, iSol);
				iMelhorias++;
				cout << endl << "New Best!!  " << dBest;
				if (PrintEvolution)
					Evolucao << endl << dElapsedTime(tStart, clock()) << ";" << iRodada << ";BuscaLocal_Cross;" << dBest;
			}
			/*dCustoSol = BuscaLocal(iSol, "Relocate", sNomeInstancia, iRelocate_OpRemocao, iRelocate_OpInsercao, iClientesRelocate, iMaxTrechoCross, true, iMelhoriasBL);
			if (dCustoSol < dBest) {
				dBest = dCustoSol;
				CopiaSol(iBestSol, iSol);
				iMelhorias++;
			}*/
			if (dElapsedTime(tStart, clock()) > (dMinutos + 1) * 60) {
				cout << endl << "Tempo: " << dElapsedTime(tStart, clock()) << "s; Rodadas: " << iRodada << " Melhorias: " << iMelhorias << " Melhor Sol: " << dBest;
				dMinutos++;
			}
		} else
			cout << endl << "A SCIH_Aleat gerou uma solucao inválida que foi ignorada!";
	} while (dElapsedTime(tStart, clock()) <= dMaxExec);

	return dBest;
}