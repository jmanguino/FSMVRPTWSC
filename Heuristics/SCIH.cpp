#pragma once
#include "Header.h"
#include "Functions.h"

//Vars Globais registradas aqui:
int v;
double InicioAtend[M];
double Demanda_Rota[M], DistTotal[M], Custo[M];

//Variáveis só desse modulo:
int Atendido[M];
int TamanhoRota;
int Candidato[M], viaveis;
int iClienteEscolhido, iPosicaoClienteEscolhido;
//ofstream PassosSCIH;

void NovaRota(int cliente) {
	/*Cria rota nova, com apenas um cliente atendido:*/
	v++;
	Rota[v][0] = 0;
	Rota[v][1] = cliente;
	Rota[v][2] = 0;
	Atendido[cliente] = 1;										/*Marca esse cliente como ja roteado*/
	TamanhoRota = 2;
}

void AdicionaRota(int c, int p) {

	Atendido[c] = 1; //Marca Cliente como Roteado
	TamanhoRota++;
	/*Log de atividades Etapa 4:
	fprintf(Relatorio,"\nETAPA4: Cliente %d com maior C2 adicionado à rota na posicao %d",c,p);*/
	//Adiciona o cliente
	while (p < TamanhoRota) {
		int temp = Rota[v][p];
		Rota[v][p] = c;
		c = temp;
		p++;
	}
	Rota[v][p] = 0;
	//Atualiza dados da rota:
	DadosRota(v);

}

double CalculaC1_SCIHv1(int cliente, int pos, double MC1) {
	double C11, C12, C13, bnew;

	int antes = Rota[v][pos - 1];
	int depois = Rota[v][pos];

	double NovaDist = DistTotal[v] + Dist[antes][cliente] + Dist[cliente][depois] - Dist[antes][depois];
	double  NovaDem = Demanda_Rota[v] + Demanda[cliente];

	double  AtendCliente = CalcInicioAtend(antes, cliente, InicioAtend[antes]);
	if (AtendCliente > FimJan[cliente])
		return MC1 + 1;

	//Verifica Factibilidade da inserção e já obtém bnew
	double  b = CalcInicioAtend(cliente, Rota[v][pos], AtendCliente);

	if (b > FimJan[Rota[v][pos]])
		return MC1 + 1;
	if (pos == TamanhoRota)
		bnew = b + Serv[Rota[v][pos]];

	for (int i = pos + 1; i <= TamanhoRota; i++) {
		b = CalcInicioAtend(Rota[v][i - 1], Rota[v][i], b);
		if (b > FimJan[Rota[v][i]])
			return MC1 + 1;
		if (i == TamanhoRota)
			bnew = b + Serv[Rota[v][i]];
	}

	/*Calcula b - antes de adição de u*/
	for (int i = 1; i <= TamanhoRota - 1; i++)
		b = CalcInicioAtend(Rota[v][i - 1], Rota[v][i], b);
	b = b + Serv[Rota[v][TamanhoRota - 1]];


	/*Calculo do C11 em funcao da variação na distância*/
	C11 = NovaDist - DistTotal[v];

	/*Calculo do C12 eh em funcao da alteracao do incio do atendimento no cliente que estava na posicao a ser avaliada*/
	if (depois == 0)
		C12 = CalcInicioAtend(cliente, 0, AtendCliente) - CalcInicioAtend(antes, 0, InicioAtend[antes]);
	else
		C12 = CalcInicioAtend(cliente, depois, AtendCliente) - InicioAtend[depois];
	//	printf("Inicio do atendimento no cliente %d atrasa %.1f ",depois,C12);
	/*Calculo de C13, se refere à variação do custo fixo*/

	C13 = CustoFaixa[abc][TipodeVeiculo(NovaDem)][FaixaDistancia(NovaDist)] - CustoFaixa[abc][TipodeVeiculo(Demanda_Rota[v])][FaixaDistancia(DistTotal[v])];

	double  C1 = alfa[1] * C11 + alfa[2] * C12 + alfa[3] * C13;
	return C1;
}

double CalculaC2_SCIHv1(int c, double  C1) {

	double  C2 = beta[1] * Dist[0][c] + beta[2] * (Tempo[0][c] + Serv[c]) + beta[3] * CustoFaixa[abc][TipodeVeiculo(Demanda[c])][FaixaDistancia(2 * Dist[0][c])] - C1;

	return  C2;
}

double CalculaC1_SCIHv3(int cliente, int pos, double MC1,int iSolAtual[M][M], int iRota,int iLista[]) {
	double C11, C12, C13, C14, C1;
	double dIniAtend_Antes, dIniAtend_Dps;
	int iTamanho = iTamanhoRota(iSolAtual, iRota);
	//Simula-se a rota com o cliente candidato - 30/12/2016
	int iRotaTeste[M];
	for (int i = 0; i <= iTamanho + 1; i++) {
		if (i < pos)  iRotaTeste[i] = iSolAtual[iRota][i];
		if (i == pos) iRotaTeste[i] = cliente;
		if (i > pos)  iRotaTeste[i] = iSolAtual[iRota][i - 1];
	}
	int antes = iSolAtual[iRota][pos - 1];
	int depois = iSolAtual[iRota][pos];

	//Verifica Factibilidade da insercao:
	double b = 0;
	for (int i = 0; i < iTamanho + 1; i++) {
		b = CalcInicioAtend(iRotaTeste[i], iRotaTeste[i + 1], b);
		if (b > FimJan[iRotaTeste[i + 1]])
			return MC1 + 1;
		if (i == pos - 1) {
			dIniAtend_Antes = b;
			dIniAtend_Dps = CalcInicioAtend(iSolAtual[iRota][i], iSolAtual[iRota][i + 1], b);
		}
	}

	double AtendCliente = CalcInicioAtend(antes, cliente, dIniAtend_Antes);

	//Dado que é factível, calcula como fica distância e Demanda após essa inserção:
	double dNovaDem = DemandaRota(iSolAtual,iRota) + Demanda[cliente];
	double dNovaDist = DistanciaRota(iSolAtual, iRota) - Dist[antes][depois] + Dist[antes][cliente] + Dist[cliente][depois];

	/*Calculo do C12 eh em funcao da alteracao do incio do atendimento no cliente que estava na posicao a ser avaliada*/
	if (depois == 0)
		C12 = CalcInicioAtend(cliente, 0, AtendCliente) - CalcInicioAtend(antes, 0, dIniAtend_Antes);
	else
		C12 = CalcInicioAtend(cliente, depois, AtendCliente) - InicioAtend[depois];

	/*Calculo do C13 em funcao da variação no custo*/
	C13 = CustoRota(dNovaDist, dNovaDem) - CustoRota(DistanciaRota(iSolAtual, iRota), DemandaRota(iSolAtual, iRota));

	// C14 avalia a oportunidade em capacidade do novo veículo, avaliando qual o maior cliente capaz de usufruir disso:
	double dCapDisp = Capac[TipodeVeiculo(dNovaDem)] - dNovaDem;
	C14 = 0;
	for (int j = 1; iLista[j] != 0; j++) {
		int i = iLista[j];
		if (i != cliente && Demanda[i] <= dCapDisp)
			if (Demanda[i] > C14)
				C14 = Demanda[i];
		if (C14 == dCapDisp)
			break;
	}

	/*C11 é o quanto de oportunidade ainda existe nessa adicao. Seja no quesito faixa de distancia, seja no capac veiculo*/
	int fnovo = FaixaDistancia(dNovaDist);
	double dDistDisp = 0;
	if (fnovo < F)
		dDistDisp = InicioFaixa[fnovo + 1] - dNovaDist;
	C11 = 0;

	for (int j = 1; iLista[j] != 0; j++) {
		int iTeste = iLista[j];
		if (iTeste != cliente)
			for (int k = 0; k < iTamanho + 1; k++) {
				//Avaliamos a iserção em cada arco: - 30/12/2016
				int iTesteAntes = iRotaTeste[k];
				int iTesteDepois = iRotaTeste[k + 1];
				double dTesteInsercao = Dist[iTeste][iTesteDepois] + Dist[iTesteAntes][iTeste] - Dist[iTesteAntes][iTesteDepois];
				if (dTesteInsercao <= dDistDisp && dTesteInsercao > C11)
					C11 = dTesteInsercao;
			}
		if (C11 == dDistDisp)
			break;
	}

	//Provisoriamente, para testes, estamos usando o mesmo alfa que C11.
	C1 = (alfa[3] * C13 + alfa[2] * C12 - alfa[1] * C11 - alfa[4] * C14) / 10;

	return C1;
}

double CalculaC2_SCIHv3(int c, double C1, int iLista[]) {
	double dCapacDisp, dDistDisp;

	int fbatevolta = FaixaDistancia(Dist[0][c] + Dist[c][0]);
	if (fbatevolta < F)
		dDistDisp = InicioFaixa[fbatevolta + 1] - Dist[0][c] - Dist[c][0];
	else
		dDistDisp = 0;

	int VeiculoBateVolta = TipodeVeiculo(Demanda[c]);

	double C22 = Tempo[0][c] + Serv[c];
	double C23 = CustoRota(Dist[0][c] + Dist[c][0], Demanda[c]);

	//Para avaliar as oportunidades, devemos verificar todos os clientes não-roteados que poderiam usufruir de fato dessas oportunidades:
	//C21 é a opotunidade na distânca que sobra na faixa do bate-volta
	double C21 = 0;
	for (int j = 1; iLista[j]!=0; j++) {
		int i = iLista[j];
		if (i != c) {
			if (Dist[i][0] * 2 <= dDistDisp && 2 * Dist[i][0] > C21)
				C21 = 2 * Dist[i][0];
			if (Dist[i][c] * 2 <= dDistDisp && 2 * Dist[i][c] > C21)
				C21 = 2 * Dist[i][c];
			if (C21 == dDistDisp)
				break;

		}
	}

	//C24 é a opotunidade em capacidade
	dCapacDisp = Capac[VeiculoBateVolta] - Demanda[c];
	double C24 = 0;
	for (int j = 1; iLista[j] != 0; j++) {
		int i = iLista[j];
		if (i != c && Demanda[i] <= dCapacDisp && Demanda[i] > C24)
			C24 = Demanda[i];
		if (C24 == dCapacDisp)
			break;
	}

	double C2 = (beta[3] * C23 + beta[2] * C22 - beta[1] * C21 - beta[4] * C24) / 10 - C1;

	return C2;
}

double CalculaC1_SCIHv2(int cliente, int pos, double MC1, int iSol[][M], int iRota) {
	double C11, C12, C13, C14, C1;
	int antes, depois;
	double NovaDem, NovaDist, AtendCliente;

	depois = iSol[iRota][pos];

	AtendCliente = InicioJan[0];
	for(antes = 1;antes<pos;antes++){
		AtendCliente = CalcInicioAtend(iSol[iRota][antes - 1], iSol[iRota][antes], AtendCliente);
	}
	antes = iSol[iRota][pos - 1];
	AtendCliente = CalcInicioAtend(antes, cliente, AtendCliente);

	//Verifica Factibilidade da inserção
	if (AtendCliente > FimJan[cliente])
		return MC1 + 1;
	double b = CalcInicioAtend(cliente, iSol[iRota][pos], AtendCliente);
	if (b > FimJan[iSol[iRota][pos]])
		return MC1 + 1;
	
	while (iSol[iRota][pos] != 0) {
		pos++;
		b = CalcInicioAtend(iSol[iRota][pos - 1], iSol[iRota][pos], b);
		if (b > FimJan[iSol[iRota][pos]])
			return MC1 + 1;
	}

	NovaDem = DemandaRota(iSol,iRota) + Demanda[cliente];
	if (NovaDem > Capac[K])
		return MC1;

	NovaDist = DistTotal[v] - Dist[antes][depois] + Dist[antes][cliente] + Dist[cliente][depois];

	// C14 é o novo termo que avalia a oportunidade em capacidade do novo veículo
	C14 = Capac[TipodeVeiculo(NovaDem)] - NovaDem;

	/*Calculo do C13 em funcao da variação no custo*/
	C13 = CustoRota(NovaDist, NovaDem) - Custo[v];

	/*Calculo do C12 eh em funcao da alteracao do incio do atendimento no cliente que estava na posicao a ser avaliada*/
	if (depois == 0)
		C12 = CalcInicioAtend(cliente, 0, AtendCliente) - CalcInicioAtend(antes, 0, InicioAtend[antes]);
	else
		C12 = CalcInicioAtend(cliente, depois, AtendCliente) - InicioAtend[depois];

	/*C11 é o quanto de oportunidade ainda existe nessa adicao. Seja no quesito faixa de distancia, seja no capac veiculo*/
	int fnovo = FaixaDistancia(NovaDist);
	if (fnovo < F)
		C11 = InicioFaixa[fnovo + 1] - NovaDist;
	else
		C11 = 0;

	//Provisoriamente, para testes, estamos usando o mesmo alfa que C11.
	C1 = (alfa[3] * C13 + alfa[2] * C12 - alfa[1] * C11 - alfa[4] * C14) / 10;

	return C1;
}

double CalculaC2_SCIHv2(int c, double C1) {
	double C23, C22, C21, C24, C2;
	int VeiculoBateVolta;

	VeiculoBateVolta = TipodeVeiculo(Demanda[c]);
	C24 = Capac[VeiculoBateVolta] - Demanda[c];
	C23 = CustoRota(Dist[0][c] + Dist[c][0], Demanda[c]);
	C22 = Tempo[0][c] + Serv[c];

	int fbatevolta = FaixaDistancia(Dist[0][c] + Dist[c][0]);

	if (fbatevolta < F)
		C21 = InicioFaixa[fbatevolta + 1] - Dist[0][c] - Dist[c][0];
	else
		C21 = 0;

	C2 = (beta[3] * C23 + beta[2] * C22 - beta[1] * C21 - beta[4] * C24) / 10 - C1;

	return C2;
}

double CalculaC1_Kritikos(int cliente, int pos, double MC1, int iSol[][M], int iRota) {
	double C11, C12, C13, C14, C15, bnew;

	int antes = iSol[iRota][pos - 1];
	int depois = iSol[iRota][pos];
	
	double dDemandaAntes = DemandaRota(iSol, iRota);
	double dDemandaDps = dDemandaAntes + Demanda[cliente];

	if (dDemandaDps > Capac[K])
		return MC1 + 1;

	double dDistAntes = DistanciaRota(iSol, iRota);
	double dDistDps = dDistAntes+ +Dist[antes][cliente] + Dist[cliente][depois] - Dist[antes][depois];

	double  AtendCliente = CalcInicioAtend(antes, cliente, InicioAtend[antes]);
	if (AtendCliente > FimJan[cliente])
		return MC1 + 1;

	//Verifica Factibilidade da inserção e já obtém bnew
	double  b = CalcInicioAtend(cliente, iSol[iRota][pos], AtendCliente);

	if (b > FimJan[iSol[v][pos]])
		return MC1 + 1;
	if (pos == TamanhoRota)
		bnew = b + Serv[iSol[v][pos]];

	for (int i = pos + 1; i <= TamanhoRota; i++) {
		b = CalcInicioAtend(iSol[iRota][i - 1], iSol[iRota][i], b);
		if (b > FimJan[iSol[iRota][i]])
			return MC1 + 1;
		if (i == TamanhoRota)
			bnew = b + Serv[iSol[iRota][i]];
	}

	/*Calcula b - antes de adição de u*/
	for (int i = 1; i <= TamanhoRota - 1; i++)
		b = CalcInicioAtend(iSol[iRota][i - 1], iSol[iRota][i], b);
	b = b + Serv[iSol[iRota][TamanhoRota - 1]];
	
	/*C0 e C1 são juntos:*/
	C11 = bnew - InicioJan[cliente];
	C11 = C11 + FimJan[cliente] - (bnew + Serv[cliente] + Tempo[antes][cliente]);

	/*Calculo do C12 em funcao da variação na distância*/
	C12 = Dist[antes][cliente] + Dist[cliente][depois] - Dist[antes][depois];

	/*Calculo do C13 em funcao da capacidade que sobra em módulo*/
	C13 = abs(Capac[TipodeVeiculo(dDemandaAntes)]- dDemandaAntes - Demanda[cliente]);
	
	/*Calculo de C14, se refere à diminuição do custo fixo comparado com demanda*/
	C14 = abs((CustoFaixa[abc][TipodeVeiculo(dDemandaAntes)][1]/ dDemandaAntes) - (CustoFaixa[abc][TipodeVeiculo(dDemandaAntes)][1] / dDemandaDps));

	/*Ai vem o Critério Dullaert adaptado*/
	C15 = CustoFaixa[abc][TipodeVeiculo(dDemandaDps)][FaixaDistancia(dDistDps)] - CustoFaixa[abc][TipodeVeiculo(Demanda_Rota[v])][FaixaDistancia(DistTotal[v])];


	double  C1 = alfa[1] * C11 + alfa[2] * C12 + alfa[3] * C13;
	return C1;
}

double CalculaC2_Kritikos(int c, double  C1) {

	double  C2 = beta[1] * Dist[0][c] + beta[2] * (Tempo[0][c] + Serv[c]) + beta[3] * CustoFaixa[abc][TipodeVeiculo(Demanda[c])][FaixaDistancia(2 * Dist[0][c])] - C1;

	return  C2;
}

double dCalculaC2(int iHeuristica, int iCliente,double dC1,int iListaCandidatos[]) {
	switch (iHeuristica) {
	case 1:
		return CalculaC2_SCIHv1(iCliente, dC1);
	case 2:
		return CalculaC2_SCIHv2(iCliente,dC1);
	case 3:
		return CalculaC2_SCIHv3(iCliente,dC1, iListaCandidatos);
	case 4:
		return CalculaC2_Kritikos(iCliente,dC1);
	}
	return -1; //Significa que deu tudo errado
}
//
//double CalculaC1_SCIHv2(int cliente, int pos, double MC1, int iSol[M][M], int iRota)
//{
//	return 0.0;
//}

double dCalculaC1(int iHeuristica, int iCliente, int(&SolAtual)[M][M], int iRota, int iPos, double dBigM, int iLista[]) {
	switch (iHeuristica) {
	case 1:
		return CalculaC1_SCIHv1(iCliente, iPos, dBigM);
	case 2:
		return CalculaC1_SCIHv2(iCliente, iPos, dBigM, SolAtual, iRota);
	case 3:
		return CalculaC1_SCIHv3(iCliente, iPos, dBigM, SolAtual, iRota,iLista);
	case 4:
		return CalculaC1_Kritikos(iCliente, iPos, dBigM, SolAtual, iRota);
	}
	return dBigM; //Sifnigica que algo deu errado
}

void Etapa1_NovaRota(string sEscolha) {
	/***** ETAPA 1 - Cria nova rota *****/
	if (sEscolha == "Mais distante") {
		double MaiorDist = 0;
		int MaisDistante = 0;
		for (int j = 1; j <= N; j++) {
			if (Atendido[j] == 0)
				if (Dist[0][j] > MaiorDist) {
					MaiorDist = Dist[0][j];
					MaisDistante = j;
				}
		}
		if (MaisDistante > 0) {
			NovaRota(MaisDistante);
			DadosRota(v);
		}
	}
	if (sEscolha == "Fim Janela") {
		double dMaisCedo = FimJan[0];
		int iClienteMaisCedo = 0;
		for (int j = 1; j <= N; j++) {
			if (Atendido[j] == 0)
				if (FimJan[j] < dMaisCedo) {
					dMaisCedo = FimJan[j];
					iClienteMaisCedo = j;
				}
		}
		if (iClienteMaisCedo > 0) {
			NovaRota(iClienteMaisCedo);
			DadosRota(v);
		}
	} 
	if (sEscolha == "Janela mais apertada") {
		double dMaisApertada = FimJan[0] - InicioJan[0];
		int iClienteMaisApertado = 0;
		for (int j = 1; j <= N; j++) {
			if (Atendido[j] == 0)
				if (FimJan[j] - InicioJan[j] < dMaisApertada) {
					dMaisApertada = FimJan[j] - InicioJan[j];
					iClienteMaisApertado = j;
				}
		}
		if (iClienteMaisApertado > 0) {
			NovaRota(iClienteMaisApertado);
			DadosRota(v);
		}
	}
	//Log de acoes da ETAPA 1:
	//fprintf(Relatorio,"\nETAPA 1:\nCliente mais distante: %d. \nRota %d iniciada com:\nDistancia %.0f, demanda %.1f, B[%d] = %.1f\nVeiculo %d e faixa de custo %d. Custo total: %.1f\n",Rota[v][1],v, DistTotal[v],Demanda_Rota[v],Rota[v][1],InicioAtend[Rota[v][1]], TipodeVeiculo(Demanda_Rota[v]),FaixaDistancia(DistTotal[v]),Custo[v]);
}

bool Etapa2_ListaCandidatos() {
	double CapacDisp = Capac[K] - Demanda_Rota[v]; /*Verifica diferença da capacidade do maior tipo de veiculo e a demanda ja alocada na rota v*/
	viaveis = 0;
	for (int j = 1; j <= N; j++)
		if (Atendido[j] == 0 && Demanda[j] <= CapacDisp) {
			viaveis++;
			Candidato[viaveis] = j;
			Candidato[viaveis+1] = 0;
		}
	if (viaveis > 0)
		return true;
	else
		return false;
}

bool Etapa3_MelhorCandidato() {
	double C1, C2;
	double MaiorC2 = 0;
	double MC1 = CustoFaixa[abc][K][F - 1] + 1000 * CustoFaixa[abc][K][F];
	double MenorC1[M];
	int MelhorPosicao[M];

	//PassosSCIH << "Candidatos:";

	for (int j = 1; j <= viaveis; j++) {
		MenorC1[j] = MC1;
		int pos = 1;
		while (pos <= TamanhoRota) {
			if (Heuristica == 1)
				C1 = CalculaC1_SCIHv1(Candidato[j], pos, MC1);
			if (Heuristica == 2)
				C1 = CalculaC1_SCIHv2(Candidato[j], pos, MC1,Rota,v);
			if (Heuristica == 3)
				C1 = CalculaC1_SCIHv3(Candidato[j], pos, MC1,Rota,v,Candidato);

			if (C1 < MenorC1[j]) {
				MenorC1[j] = C1;
				MelhorPosicao[j] = pos;
			}
			pos++;
		}
		if (MenorC1[j] < MC1) {
			if (Heuristica == 1)
				C2 = CalculaC2_SCIHv1(Candidato[j], MenorC1[j]);
			if (Heuristica == 2)
				C2 = CalculaC2_SCIHv2(Candidato[j], MenorC1[j]);
			if (Heuristica == 3) {
				//if (Candidato[j] == 51 || Candidato[j] == 35)
				//	break;
				C2 = CalculaC2_SCIHv3(Candidato[j], MenorC1[j], Candidato);
			}
			if (C2 > MaiorC2) {
				MaiorC2 = C2;
				iClienteEscolhido = Candidato[j];
				iPosicaoClienteEscolhido = MelhorPosicao[j];
			}
			//PassosSCIH << Candidato[j] << " C2=" << C2 << ", ";
		}
		//else
		//	PassosSCIH << Candidato[j] << " Inv,";
	}
	//PassosSCIH << endl;
	if (MaiorC2 > 0) {
		//PassosSCIH << "Cliente a adicionar: " << iClienteEscolhido << " com C2 = " << MaiorC2 << endl;
		return true;
	}
	else
		return false;
}

double SCIH() {
	bool HaViaveis;
	int NaoRoteados = N;
	double custo = 0;

	//Zera variáveis para rodar, daqueadamente, as instâncias:
	for (int j = 1; j <= N; j++) Atendido[j] = 0;

	for (int i = 0; i < N; i++) {
		Custo[i] = 0;
		DistTotal[i] = 0;
		Demanda_Rota[i] = 0;
		for (int j = 0; j < N; j++) {
			Rota[i][j] = 0;
		}
	}

	//Começa a resolver o problema
	//PassosSCIH.open("PassosSCIH3" + NomeProb + ".txt");
	v = 0;
	while (NaoRoteados>0) {
		Etapa1_NovaRota("Mais distante");
		//PassosSCIH << endl << "Nova Rota: " << v << " = " << 0 << "-" << Rota[v][1] << "-" << Rota[v][2] << endl;
		NaoRoteados--;
		/***** ETAPA 2 - Lista candidatos para adicionar à rota *****/
		do{
			HaViaveis = Etapa2_ListaCandidatos();
			if (HaViaveis) {
				/***** ETAPA 3 - Escolhe melhor candidato e posição para adição à rota *****/
				HaViaveis = Etapa3_MelhorCandidato();
				if (HaViaveis) {
					/***** ETAPA 4 - Adiciona o melhor candidato à rota *****/
					AdicionaRota(iClienteEscolhido, iPosicaoClienteEscolhido);
					//PassosSCIH << "Rota Atualizada: " << v << " = " << 0 << "-" << Rota[v][1];
					//int i = 1;
					//do {
					//	i++;
					//	PassosSCIH << "-" << Rota[v][i];
					//} while (Rota[v][i] != 0);
					//PassosSCIH << endl;
					NaoRoteados--;
				}
			}
			//else fprintf(Relatorio,"ETAPA2:\nNão existem candidatos viáveis para essa rota");
		} while (HaViaveis);
	}
	//PassosSCIH.close();
	for (int j = 1; j <= v; j++) custo += Custo[j];
	return custo;
}