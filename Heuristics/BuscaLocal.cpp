#include "Header.h"
#include "Functions.h"


//Fun??o sem utilidade, somente para verificar se c?pia de solu??es deu certo.
bool VerificaSolucoes(int Sol1[][M], int QtRotas1, int Sol2[][M], int QtRotas2) {
	if (QtRotas1 != QtRotas2)
		return false;
	for (int i = 1; i <= QtRotas1; i++) {
		int j = 1;
		while (Sol1[i][j] != 0) {
			if (Sol1[i][j] != Sol2[i][j])
				return false;
			j++;
		}
		if (Sol1[i][j] != Sol2[i][j])
			return false;
	}
	return true;
}

int RemoveClient(int(&Sol)[M][M], int iRoute, int iPos) {
	int iRouteCount = ContaRotas(Sol);
	int iClient = Sol[iRoute][iPos];
	//1- Esse e o unico cliente dessa rota e ela sera removida?
	if ((iPos == 1) && (Sol[iRoute][iPos + 1] == 0)) {
		while (iRoute < iRouteCount) {
			iRoute++;
			for (iPos = 1; Sol[iRoute][iPos] != 0; iPos++)
				Sol[iRoute - 1][iPos] = Sol[iRoute][iPos];

			Sol[iRoute - 1][iPos] = 0;
		}
		Sol[iRoute][0] = 0;
		Sol[iRoute][1] = 0;
		iRouteCount--;
	}
	else {
		//2- N?o ? o ?nico, basta "puxar" o resto da rota
		while (Sol[iRoute][iPos] != 0) {
			Sol[iRoute][iPos] = Sol[iRoute][iPos + 1];
			iPos++;
		}
		Sol[iRoute][iPos] = 0;
	}
	return iClient;
}

/*------RELOCATE------*/

double CustoRotaSimulada(int iRota[]) {
	double dDemanda = 0;
	double dDistancia = 0;

	//Premissa: Rota e valida!
	int iPos = 1;
	while (iRota[iPos] != 0) {
		dDemanda = dDemanda + Demanda[iRota[iPos]];
		dDistancia = dDistancia + Dist[iRota[iPos - 1]][iRota[iPos]];
		iPos++;
	}
	//Mais uma vez, para a volta ao CD:
	dDistancia = dDistancia + Dist[iRota[iPos - 1]][iRota[iPos]];

	return CustoRota(dDistancia, dDemanda);
}

string NomeOperadorRelocate(int iOp, string cTipo) {
	if (cTipo == "R") {
		switch (iOp) {
		case 1://RR - Random Removal
			return "RR";
			break;

		case 2: //WDR worst distance removal
			return "WDR";
			break;

		case 3: //GR greedy removal
			return "GR";
			break;

		case 4: //WTR worst time removal
			return "WTR";
			break;

		case 5: //C1R C1 removal
			return "C1R";
			break;

		default:
			break;
		}
	}
	if (cTipo == "I") {
		switch (iOp) {
		case 1://RI - Random Insertion
			return "RI";
			break;
		case 2://SDI - Smallest Distance Ins
			return "SDI";
			break;
		case 3://GI - Greedy insertion
			return "GI";
			break;
		case 4://C1I - C1 Insertion
			return "C1I";
			break;
		default:
			break;
		}
	}
	return "N/A";
}

/*Remocao*/
int WTR(int(&Sol)[M][M], bool bListaClientes[]) {
	int iCliente;
	int iRota, iPos;
	double dTempo, dEspera, dMaiorEspera = -1;

	int QtdRotas = ContaRotas(Sol);
	//Roda-se todos arcos das rotas ate descobrir a dupla que quando removido gera a maior reducaoo de distancia.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		iPos = 1;
		while (Sol[iRota][iPos] != 0) {
			if (!bListaClientes[Sol[iRota][iPos]]) {
				if (iPos == 1)
					dTempo = InicioJan[0];
				dTempo = Tempo[Sol[iRota][iPos - 1]][Sol[iRota][iPos]];
				dEspera = InicioJan[Sol[iRota][iPos]] - dTempo;
				if (dEspera < 0) dEspera = 0;

				if (dEspera > 0)
					dTempo = InicioJan[Sol[iRota][iPos]];

				if (dEspera > dMaiorEspera && !bListaClientes[Sol[iRota][iPos]]) {
					dMaiorEspera = dEspera;
					iCliente = Sol[iRota][iPos];
				}

				dTempo = dTempo + Serv[Sol[iRota][iPos]];
			}

			iPos++;
		}
	}
	return iCliente;
}

int WDR(int(&Sol)[M][M], bool bListaClientes[]) {
	int iCliente;
	int QtdRotas, iRota, iPos;
	double dDist, dMaiorDist = -1;

	QtdRotas = ContaRotas(Sol);
	//Roda-se todos arcos das rotas at? descobrir a dupla que quando removido gera a maior redu??o de dist?ncia.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		iPos = 1;
		while (Sol[iRota][iPos] != 0) {
			if (!bListaClientes[Sol[iRota][iPos]]) {
				dDist = Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos]] + Dist[Sol[iRota][iPos]][Sol[iRota][iPos + 1]] - Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos + 1]];
				if (dDist > dMaiorDist) {
					dMaiorDist = dDist;
					iCliente = Sol[iRota][iPos];
				}
			}
			iPos++;
		}
	}
	return iCliente;
}

int GR(int(&Sol)[M][M], bool bListaClientes[]) {
	int iCliente = 0;
	int iRota, iPos;
	double dDistAntes, dDemAntes, dDistDepois, dDemDepois;
	double dCustoAntes, dCustoDepois, dMaiorReducao = -1;
	double dReducaoDist = 0;

	int QtdRotas = ContaRotas(Sol);
	//Roda-se todos arcos das rotas at? descobrir a dupla que quando removido gera a maior redu??o de dist?ncia.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		//Coleta os dados da rota "as is"
		dDistAntes = DistanciaRota(Sol, iRota);
		dDemAntes = DemandaRota(Sol, iRota);
		dCustoAntes = CustoRota(dDistAntes, dDemAntes);
		//Agora exclui-se cliente a cliente da rota
		iPos = 1;
		while (Sol[iRota][iPos] != 0) {
			if (!bListaClientes[Sol[iRota][iPos]]) {
				if ((Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos]] + Dist[Sol[iRota][iPos]][Sol[iRota][iPos + 1]] - Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos + 1]]) < 0) {
					//Existem trocas em que os 3 clientes est?o em perfeita linha ent?o a dist?ncia n?o muda tirando cliente do meio
					// Por?m pode acontecer que na hora de calcular as dist?ncias entre os 3 clientes, arredondamentos fa?am parecer que a dist?ncia aumentou com a tirada do cliente. Esse if ? para evitar exatamente isso.
					dDistDepois = dDistAntes;
				}
				else {
					dDistDepois = dDistAntes - (Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos]] + Dist[Sol[iRota][iPos]][Sol[iRota][iPos + 1]] - Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos + 1]]);
				}
				dDemDepois = dDemAntes - Demanda[Sol[iRota][iPos]];
				dCustoDepois = CustoRota(dDistDepois, dDemDepois);
				if ((dCustoAntes - dCustoDepois > dMaiorReducao) || (dCustoAntes - dCustoDepois == dMaiorReducao && dDistAntes - dDistDepois > dReducaoDist)) {
					iCliente = Sol[iRota][iPos];
					dMaiorReducao = dCustoAntes - dCustoDepois;
					dReducaoDist = dDistAntes - dDistDepois;
				}
			}
			iPos++;
		}
	}
	return iCliente;
}

int C1R(int(&Sol)[M][M], bool bListaClientes[]) {
	int iCliente, iClienteTeste;
	int iSolTeste[2][M];
	int i, iRota, iPos, iPosTeste;
	double dC1;

	int QtdRotas = ContaRotas(Sol);
	double MC1 = CustoFaixa[abc][K][F - 1] + 1000 * CustoFaixa[abc][K][F];
	double dMaiorC1 = -1 * MC1;
	//dC1 = MC1;

	//Roda-se todos arcos das rotas at? descobrir a dupla que quando removido gera o maior C1.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		//Agora exclui-se cliente a cliente da rota
		iSolTeste[1][0] = 0;
		iPos = 1;
		while (Sol[iRota][iPos] != 0) {
			iClienteTeste = Sol[iRota][iPos];
			if (!bListaClientes[iClienteTeste]) {
				//Simula a rota sem o cliente em quest?o:
				iPosTeste = 1;
				i = 1;
				while (Sol[iRota][i] != 0) {
					if (i != iPos) {
						iSolTeste[1][iPosTeste] = Sol[iRota][i];
						iPosTeste++;
					}
					i++;
				}
				iSolTeste[1][iPosTeste] = 0;

				dC1 = CalculaC1_SCIHv2(iClienteTeste, iPos, MC1, iSolTeste, 1);
				if (dC1 > MC1)
					dC1 = MC1 + 1;
				if (dC1 < MC1 && dC1 > dMaiorC1) {
					iCliente = iClienteTeste;
					dMaiorC1 = dC1;
				}
			}
			iPos++;
		}
	}
	return iCliente;
}

int RemoveOperator(int iOperadorRemocao, int(&Sol)[M][M], bool bListaClientes[]) {
	int iClienteExcluido;
	int QtdRotas, iRota;
	int iPos=0;

	// Escolhe quem excluir de acordo com o operador:
	QtdRotas = ContaRotas(Sol);
	switch (iOperadorRemocao) {
	case 1://RR - Random Removal
		   //srand(time(NULL));
		do {
			iClienteExcluido = rand() % (N)+1;
		} while (bListaClientes[iClienteExcluido]);
		break;

	case 2: //WDR ? worst distance removal
		iClienteExcluido = WDR(Sol, bListaClientes);
		break;

	case 3: //GR ? greedy removal
		iClienteExcluido = GR(Sol, bListaClientes);
		break;

	case 4: //WTR ? worst time removal
		iClienteExcluido = WTR(Sol, bListaClientes);
		break;

	case 5: //C1R ? C1 removal
		iClienteExcluido = C1R(Sol, bListaClientes);
		break;

	default:
		break;
	}

	//Agora tira das rotas:
	//Pimeiro, acha o cliente
	bool bAchou = false;
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		iPos = 1;
		while (Sol[iRota][iPos] != 0) {
			if (Sol[iRota][iPos] == iClienteExcluido) {
				bAchou = true;
				break;
			}
			iPos++;
		}
		if (bAchou) break;
	}

	//Achou, agora tira e rearranja tudo:
	RemoveClient(Sol, iRota, iPos);

	return iClienteExcluido;
}

/*Insertion*/
void SimulateInsertionRoute(int iSolAtual[][M], int(&iRotaTeste)[M], int iCliente, int iRota, int iPos) {

	int i, iQtdRotas = ContaRotas(iSolAtual);
	//Simula-se a rota com o cliente candidato
	iRotaTeste[0] = 0;
	if (iRota > iQtdRotas) {
		iRotaTeste[1] = iCliente;
		iRotaTeste[2] = 0;
	}
	else {
		for (i = 1; iSolAtual[iRota][i] != 0; i++) {
			if (i < iPos)
				iRotaTeste[i] = iSolAtual[iRota][i];
			if (i == iPos)
				iRotaTeste[i] = iCliente;
			if (i > iPos)
				iRotaTeste[i] = iSolAtual[iRota][i - 1];
		}
		if (iSolAtual[iRota][iPos] == 0)
			iRotaTeste[i] = iCliente;
		else
			iRotaTeste[i] = iSolAtual[iRota][i - 1];
		iRotaTeste[i + 1] = 0;
	}
}

bool bValidateInsertion(int iSol[M][M], int iRota, int iPos, int iCliente, int(&iRotaTeste)[M]) {
	int i;
	SimulateInsertionRoute(iSol, iRotaTeste, iCliente, iRota, iPos);
	//Verifica factibilidade em capacidade e Janelas de tempo:
	double dDem = 0;
	double b = InicioJan[0];
	for (i = 1; iRotaTeste[i] != 0; i++) {
		dDem += Demanda[iRotaTeste[i]];
		b = CalcInicioAtend(iRotaTeste[i - 1], iRotaTeste[i], b);
		if (b > FimJan[iRotaTeste[i]] || dDem > Capac[K])
			return false;
	}
	b = CalcInicioAtend(iRotaTeste[i], 0, b);
	if (b > FimJan[0])
		return false;

	//Verifica se cabe no maior veiculo:
	if (dDem > Capac[K])
		return false;

	return true;
}

void InsereSolucao(int(&Sol)[M][M], int iRota, int iPos, int iCliente) {
	int temp;

	int QtdRotas = ContaRotas(Sol);
	if (iRota > QtdRotas) {
		QtdRotas++;
		Sol[iRota][0] = 0;
		Sol[iRota][1] = 0;
		//Para garantir que contagem identifica a nova ultima rota:
		Sol[iRota + 1][0] = 0;
		Sol[iRota + 1][1] = 0;
	}
	while (Sol[iRota][iPos] != 0) {
		temp = Sol[iRota][iPos];
		Sol[iRota][iPos] = iCliente;
		iCliente = temp;
		iPos++;
	}
	Sol[iRota][iPos] = iCliente;
	Sol[iRota][iPos + 1] = 0;
}

//GI busca a insercao que gera o menor aumento total de dist?ncia no problema:
double GI_GreedyInsertion(int iCliente, int(&Sol)[M][M], int &iMelhorRota, int &iMelhorPos) {
	int iRota, iPos, iRotaTeste[M];
	double dDist, dMenorDist = 0;

	int QtdRotas = ContaRotas(Sol);

	//A maior dist?ncia ser? inicializada com a dist?ncia do bate-volta que sempre ser? uma possibilidade
	dMenorDist = Dist[0][iCliente] + Dist[iCliente][0];
	iMelhorRota = QtdRotas + 1;
	iMelhorPos = 1;

	//Roda-se todos arcos das rotas at? descobrir em qual a inser??o do cliente gera o menor aumento de dist?ncia, iunclusive o bate-volta.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		iPos = 1;
		//Verifica se a adicao do cliente nessa rota estoura a demanda do maior veiculo:
		if (Demanda[iCliente] + DemandaRota(Sol, iRota) <= Capac[K]) {
			while (Sol[iRota][iPos] != 0) {
				dDist = Dist[Sol[iRota][iPos - 1]][iCliente] + Dist[iCliente][Sol[iRota][iPos]] - Dist[Sol[iRota][iPos - 1]][Sol[iRota][iPos]];
				if (dDist < dMenorDist) {
					//Agora verifica se ? fact?vel pelas janelas de tempo:
					if (bValidateInsertion(Sol, iRota, iPos, iCliente, iRotaTeste)) {
						dMenorDist = dDist;
						iMelhorRota = iRota;
						iMelhorPos = iPos;
					}
				}
				iPos++;
			}
		}
	}
	return dMenorDist;
}

double SCI_SmallestCostInsertion(int iCliente, int(&Sol)[M][M], int &iMelhorRota, int &iMelhorPos) {
	int iRota, iPos;
	int iRotaOriginal[M], iRotaTeste[M];
	double dCustoTeste, dMelhorCusto;

	int QtdRotas = ContaRotas(Sol);

	//Primeiro, avalia-se o custo de criar uma nova rota:
	iRotaTeste[0] = 0;
	iRotaTeste[1] = iCliente;
	iRotaTeste[2] = 0;
	dMelhorCusto = CustoRotaSimulada(iRotaTeste);
	iMelhorRota = QtdRotas + 1;
	iMelhorPos = 1;

	//A ideia e objetivamente testar em TODAS as posicoes e ver se o custo e valido.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		if (Demanda[iCliente] + DemandaRota(Sol, iRota) <= Capac[K]) {
			//Copio a rota original para teste:
			iRotaOriginal[0] = 0;
			iPos = 1;
			while (Sol[iRota][iPos] != 0) {
				iRotaOriginal[iPos] = Sol[iRota][iPos];
				iPos++;
			}
			iRotaOriginal[iPos] = 0;
			//Agora simulo as insercoes:
			for (iPos = 1; Sol[iRota][iPos] != 0; iPos++) {
				//Se e factivel:
				if (bValidateInsertion(Sol, iRota, iPos, iCliente, iRotaTeste)) {
					//Calcula distancia e demanda da rota:
					dCustoTeste = CustoRotaSimulada(iRotaTeste) - CustoRotaSimulada(iRotaOriginal);
					if (dCustoTeste < dMelhorCusto) {
						iMelhorPos = iPos;
						iMelhorRota = iRota;
						dMelhorCusto = dCustoTeste;
					}
				}
			}
		}
	}
	return dMelhorCusto;
}

double C1I(int iCliente, int(&Sol)[M][M], int &iMelhorRota, int &iMelhorPos) {
	int iRota, iPos;
	double dC1;

	double MC1 = CustoFaixa[abc][K][F - 1] + 1000 * CustoFaixa[abc][K][F];
	double dMenorC1 = MC1;
	int QtdRotas = ContaRotas(Sol);

	//Inicializo essas variaveis com uma nova rota para o caso de nenhum C1 viavel.
	iMelhorPos = 1;
	iMelhorRota = QtdRotas + 1;


	//dC1 = MC1;

	/*Primeiro, avalia-se o C1 numa nova rota:
	Sol[QtdRotas + 1][0] = 0;
	Sol[QtdRotas + 1][1] = 0;
	iPos = 1; iRota = QtdRotas + 1;
	dMenorC1 = CalculaC1_SCIHv2(iCliente, iPos, MC1, Sol, iRota);
	iMelhorRota = iRota;
	iMelhorPos = iPos;
	*/

	//A ideia ? objetivamente testar em TODAS as posi??es e ver se o custo ? v?lido.
	for (iRota = 1; iRota <= QtdRotas; iRota++) {
		if (Demanda[iCliente] + DemandaRota(Sol, iRota) <= Capac[K]) {
			for (iPos = 1; Sol[iRota][iPos] != 0; iPos++) {
				//Se ? fact?vel:
				dC1 = CalculaC1_SCIHv2(iCliente, iPos, MC1, Sol, iRota);
				if (dC1 < dMenorC1) {
					//Calcula distancia e demanda da rota:	
					dMenorC1 = dC1;
					iMelhorPos = iPos;
					iMelhorRota = iRota;
				}
			}
		}
	}

	return dMenorC1;
}

void RI_RandomInsertion(int iCliente, int(&iSol)[M][M], int &iRota, int &iPos) {
	//Iremos inserir aleatoriamente no primeiro local factível (que respeite capacidades e janelas de tempo)
	int iRotaTeste[M];
	int iTam;
	int QtdRotas = ContaRotas(iSol);
	do {
		iRota = 1 + rand() % (QtdRotas + 1); // we use QtdRotas +1 to allow for a new individual route
		iTam = iTamanhoRota(iSol, iRota);
		//	srand(time(NULL));
		iPos = 1 + rand() % (iTam + 1); // iTam + 1 means it can be in the last position
	} while (!bValidateInsertion(iSol, iRota, iPos, iCliente, iRotaTeste));
}

void InsertOperator(int iOperador, int iCliente, int(&Sol)[M][M]) {
	int iMelhorRota, iMelhorPos;
	// Escolhe quem excluir de acordo com o operador:
	switch (iOperador) {
	case 1://RI - Random Insertion
		RI_RandomInsertion(iCliente, Sol, iMelhorRota, iMelhorPos);
		break;
	case 2://SDI - Smallest Distance Insertion
		GI_GreedyInsertion(iCliente, Sol, iMelhorRota, iMelhorPos);
		break;
	case 3://GI - greedy insertion
		SCI_SmallestCostInsertion(iCliente, Sol, iMelhorRota, iMelhorPos);
		break;
	case 4://C1I ? C1 Insertion
		C1I(iCliente, Sol, iMelhorRota, iMelhorPos);
		break;
	default:
		break;
	}
	InsereSolucao(Sol, iMelhorRota, iMelhorPos, iCliente);
	if (!bValidaSol(Sol))
		int iErro = 1;
}

/*Codigo geral Relocate*/

double Relocate(int(&iSol)[M][M], int iOpRemocao, int iOpInsercao, int iIntensidade, bool bSomenteMelhora) {
	int SolRodada[M][M], MelhorSolRodada[M][M];
	int iCliente;
	bool bListaClientes[M]; //Lista para controlar se ciente j? foi ou n?o usado na rodada;
	bool bMelhorou;

	//int iMaxTrocas = min(N*MaxShakeRelocate / 100, iIntensidade);

	//Copia a solucao vigente:
	double dMelhorRodada = CustoSolucao(iSol);

	int iRodadas = 0;
	do {
		//At each run, we start from the original solution:
		iRodadas++;

		//Marca todos os clientes como nao testados:
		for (int i = 1; i < M; i++)
			bListaClientes[i] = false;

		//Executa a quantidade de rotas combinadas:
		bMelhorou = false;
		for (int iTroca = 1; iTroca <= iIntensidade; iTroca++) {

			//A troca sempre comeca da ultima rodada:
			CopiaSol(SolRodada, iSol);

			iCliente = RemoveOperator(iOpRemocao, SolRodada, bListaClientes);
			bListaClientes[iCliente] = true; //Atualiza lista de clientes testados

			InsertOperator(iOpInsercao, iCliente, SolRodada);

			//Avalia a reloca??o:
			if (CustoSolucao(SolRodada) < dMelhorRodada || !bSomenteMelhora) {
				CopiaSol(MelhorSolRodada, SolRodada);
				dMelhorRodada = CustoSolucao(MelhorSolRodada);
				bMelhorou = true;
				if (FirstAccept) break;
			}
		}
		if (bMelhorou || !bSomenteMelhora)
			CopiaSol(iSol, MelhorSolRodada);

	} while (bSomenteMelhora && bMelhorou);
	if (!bValidaSol(iSol))
		int iErro = 1;

	return dMelhorRodada;
}

/*Codigo CROSS*/
void GeraTrecho(int iSol[][M], int(&iTrecho)[M], int &iRota, int &iPos, int &iTam) {
	//Não tem mais seleção aleatória de coisa alguma, só gera o trecho e boa.
	/*Seleciona qualquer rota: (rand() gera um valor aleatório entra 0 e 1. Então multiplicar 
	iRota = rand() % ContaRotas(iSol) + 1;
	//Obtenho o tamanho da rota escolhida:
	iTam = iTamanhoRota(iSol, iRota);

	//Seleciono qualquer posição nessa rota:
	iPos = rand() % iTam + 1;
	//Qual o tamanho de rota que "sobrou" após essa posição:
	iTam = iTam - iPos + 1;

	//Agora, dentro desse espaço que sobrou, determino o tamanho do trecho:
	iTam = rand() % iTam + 1;*/

	//Extrai o Trecho:
	int iPosTrecho = 0;
	for (int i = iPos; i <= iPos + iTam - 1; i++) {
		iPosTrecho++;
		iTrecho[iPosTrecho] = iSol[iRota][i];
	}
}

void ColocaTrechoNaRota(int(&iSol)[M][M], int iRota, int iPosIni, int iPosDps, int iTrecho[], int iTam) {
	//Ja ajeito a rota:
	int iDif = (iPosIni + iTam) - iPosDps;
	int iFimRota = iTamanhoRota(iSol, iRota);
	if (iDif > 0) {
		//Se o trecho for maior do que o que vai ser extraido dessa rota, puxo tudo pra frente
		for (int i = iFimRota + iDif; i >= iPosDps; i--)
			iSol[iRota][i + iDif] = iSol[iRota][i];
		if (iSol[iRota][1] == 0)
			int Erro = 1;
	}
	else if (iDif < 0) {
		//Se o trecho for menor do que o que vai ser extra?do dessa rota, puxo tudo pra antes
		for (int i = iPosIni + iTam; i <= iFimRota; i++)
			iSol[iRota][i] = iSol[iRota][i - iDif];
		if (iSol[iRota][1] == 0)
			int Erro = 1;
	}
	iSol[iRota][iFimRota + iDif + 1] = 0;
	if (iSol[iRota][1] == 0)
		int Erro = 1;
	for (int i = 1; i <= iTam; i++)
		iSol[iRota][iPosIni + i - 1] = iTrecho[i];
	if (iSol[iRota][1] == 0)
		int Erro = 1;
}

void Cross(int(&iSol)[M][M],int iMaxTamTrecho,bool bBestSol) { //Se bBestSol true, roda todas as trocas até achar a melhor, se false, pega aleatorio
	int iTrecho1[M], iRota1, iTamRota1,iPosTrecho1, iTamTrecho1;
	int iTrecho2[M], iRota2, iTamRota2, iPosTrecho2, iTamTrecho2;
	int iQtdRotas = ContaRotas(iSol);
	int iSolTeste[M][M], iSolBest[M][M];

	double dBestCost = CustoSolucao(iSol);

	//Trecho 1:
	if (bBestSol) {
	//Rodamos todas as trocas possíveis com a limitação apenas do tamanho do trecho:
		for (iRota1 = 1; iRota1 < iQtdRotas; iRota1++) {
			iTamRota1 = iTamanhoRota(iSol, iRota1);
			for (iPosTrecho1 = 1; iPosTrecho1 <= iTamRota1; iPosTrecho1++)
				for (iTamTrecho1 = 1; iTamTrecho1 <= iMaxTamTrecho && iPosTrecho1 + iTamTrecho1 - 1 <= iTamRota1; iTamTrecho1++) {
					//Extrai e o trecho 1:
					GeraTrecho(iSol, iTrecho1, iRota1, iPosTrecho1, iTamTrecho1);
					//Agora o trecho 2:
					for (iRota2 = iRota1 + 1; iRota2 <= iQtdRotas; iRota2++) {
						iTamRota2 = iTamanhoRota(iSol, iRota2);
						for (iPosTrecho2 = 1; iPosTrecho2 <= iTamRota2; iPosTrecho2++)
							for (iTamTrecho2 = 1; iTamTrecho2 <= iMaxTamTrecho && iPosTrecho2 + iTamTrecho2 - 1 <= iTamRota2; iTamTrecho2++) {
								//Extrai e o trecho 2:
								GeraTrecho(iSol, iTrecho2, iRota2, iPosTrecho2, iTamTrecho2);
								//Executa a troca:
								CopiaSol(iSolTeste, iSol);
								ColocaTrechoNaRota(iSolTeste, iRota1, iPosTrecho1, iPosTrecho1 + iTamTrecho1, iTrecho2, iTamTrecho2);
								ColocaTrechoNaRota(iSolTeste, iRota2, iPosTrecho2, iPosTrecho2 + iTamTrecho2, iTrecho1, iTamTrecho1);
								//Avalia a Solução:
								if (bValidaSol(iSolTeste))
									if (CustoSolucao(iSolTeste) < dBestCost) {
										dBestCost = CustoSolucao(iSolTeste);
										CopiaSol(iSolBest, iSolTeste);
										if (FirstAccept)
											goto BL_Cross_AfterLoop;
									}
							}
					}
				}
		}
BL_Cross_AfterLoop:
		if (dBestCost < CustoSolucao(iSol))
			CopiaSol(iSol, iSolBest);
	}
	else {
	//Escolhemos uma trocas aleatória porém factível:
		int iContaTentativas = 0;
		do {
			iContaTentativas++;
			iRota1 = rand() % ContaRotas(iSol) + 1;
			iTamRota1 = iTamanhoRota(iSol, iRota1);
			iPosTrecho1 = rand() % iTamRota1 + 1;
			iTamTrecho1 = rand() % (iTamRota1 - iPosTrecho1+1) + 1;
			GeraTrecho(iSol, iTrecho1, iRota1, iPosTrecho1, iTamTrecho1);

			do iRota2 = rand() % ContaRotas(iSol) + 1; while (iRota1 == iRota2);
			iTamRota2 = iTamanhoRota(iSol, iRota2);
			iPosTrecho2 = rand() % iTamRota2 + 1;
			iTamTrecho2 = rand() % (iTamRota2 - iPosTrecho2+1) + 1;
			GeraTrecho(iSol, iTrecho2, iRota2, iPosTrecho2, iTamTrecho2);

			CopiaSol(iSolTeste, iSol);
			ColocaTrechoNaRota(iSolTeste, iRota1, iPosTrecho1, iPosTrecho1 + iTamTrecho1, iTrecho2, iTamTrecho2);
			ColocaTrechoNaRota(iSolTeste, iRota2, iPosTrecho2, iPosTrecho2 + iTamTrecho2, iTrecho1, iTamTrecho1);
		} while (!bValidaSol(iSolTeste) && iContaTentativas < N*N);
		if(bValidaSol(iSolTeste)) CopiaSol(iSol, iSolTeste);
	}

}

double dCROSS(int(&Sol)[M][M],int iIntensidade,int iMaxTamTrecho,bool bBestSol) { //Se bBestSol = true, vamos procurar a melhor troca possivel. Se falso, vai ser troca aleatoria
	int SolRodada[M][M];// , MelhorSolRodada[M][M];
	int iErro = 0;
	//Executa a quantidade de rotas combinadas:
	double dSolCost = CustoSolucao(Sol);

	if (ContaRotas(Sol) <= 1)
		return dSolCost;

	for (int iTroca = 1; iTroca <= iIntensidade; iTroca++) {
		//A troca sempre come?a da ultima rodada:
		CopiaSol(SolRodada, Sol);
		
		//Escolhe a melhor troca do Cross:
		Cross(SolRodada,iMaxTamTrecho,bBestSol);
		if (ContaRotas(SolRodada) == 0)
			iErro++;

		//Avalia a troca:
		if (bValidaSol(SolRodada)) {
			if ((CustoSolucao(SolRodada) < dSolCost) | (!bBestSol)) {
				CopiaSol(Sol, SolRodada);
				dSolCost = CustoSolucao(Sol);
				if (FirstAccept) break;
			}
		}
		//else
		//	iTroca--;
	}

	//CopiaSol(SolOriginal, MelhorSolRodada);

	return CustoSolucao(Sol);
}

/*------2-opt*-----------------------*/
int TrocaDoisOptEstrela(int(&iSol)[M][M], int iRotaA, int iPosA, int iRotaB, int iPosB) {
	int iTemp[M];

	//Busco o restante da rota para um vetor temporario:
	int i = 0;
	while (iSol[iRotaA][iPosA + i] != 0) {
		iTemp[i] = iSol[iRotaA][iPosA + i];
		i++;
	}
	iTemp[i] = 0;

	//Executa a troca:
	i = 0;
	while (iSol[iRotaB][iPosB + i] != 0) {
		iSol[iRotaA][iPosA + i] = iSol[iRotaB][iPosB + i];
		i++;
	}
	iSol[iRotaA][iPosA + i] = 0;

	i = 0;
	while (iTemp[i] != 0) {
		iSol[iRotaB][iPosB + i] = iTemp[i];
		i++;
	}
	iSol[iRotaB][iPosB + i] = 0;

	return 1;
}
/*
void DoisOptEstrela(int(&iSol)[M][M]) {
	int  iRota1, iPos1;
	int  iRota2, iPos2;
	int iTam;
	int iQtdRotas = ContaRotas(iSol);

	//Trecho 1:
	//Seleciona rota e posicapo da troca:
	do {
		iRota1 = 1 + rand() % (iQtdRotas - 1);
		iTam = iTamanhoRota(iSol, iRota1);
	} while (iTam < 2);
	//	srand(time(NULL));
	iPos1 = 2 + rand() % (iTam - 1);

	//Trecho 2:
	//Seleciona rota e posicapo da troca:
	do {
		iRota2 = 1 + rand() % (iQtdRotas - 1);
		iTam = iTamanhoRota(iSol, iRota2);
	} while (iTam < 2 || iRota1 == iRota2);
	iPos2 = 2 + rand() % (iTam - 1);

	TrocaDoisOptEstrela(iSol, iRota1, iPos1, iRota2, iPos2);
}
*/
double dDoisOptEstrela(int(&SolOriginal)[M][M], int iIntensidade, bool bBestSol) {
	int SolRodada[M][M], MelhorSolRodada[M][M];
	int iRotaTot;
	int iRotaA,iRotaB,iPosA, iPosB, iTamA, iTamB;
	int iErro = 0;
	
	int iTrocas = min(ContaRotas(SolOriginal)*MaxShake2Opt / 100, iIntensidade);

	//Copia a solucao vigente:
	CopiaSol(SolRodada, SolOriginal);
	CopiaSol(MelhorSolRodada, SolOriginal);
	if (ContaRotas(SolRodada) == 0)
		iErro++;

	//Determina quantas rodadas acontecerao:
	iRotaTot = ContaRotas(SolRodada);
	if (iRotaTot <= 1)
		return CustoSolucao(SolRodada);

	//Executa a quantidade de rotas combinadas:
	if (bBestSol) {
		double dCustoSolRodada = CustoSolucao(SolRodada);
		double dMelhorCusto = dCustoSolRodada;
		for (iRotaA = 1; iRotaA <= iRotaTot; iRotaA++) {
			iTamA = iTamanhoRota(SolRodada, iRotaA);
			for (iPosA = 1; iPosA <= iTamA; iPosA++) {
				for (iRotaB = iRotaA + 1; iRotaB <= iRotaTot; iRotaB++) {
					iTamB = iTamanhoRota(SolRodada, iRotaB);
					for (iPosB = 1; iPosB <= iTamB; iPosB++) {
						TrocaDoisOptEstrela(SolRodada, iRotaA, iPosA, iRotaB, iPosB);
						//Avalia a troca:
						if (bValidaSol(SolRodada)) {
							dCustoSolRodada = CustoSolucao(SolRodada);
							if (dCustoSolRodada < dMelhorCusto) {
								CopiaSol(MelhorSolRodada, SolRodada);
								dMelhorCusto = dCustoSolRodada;
								if (FirstAccept)
									goto BL_2opt_PosLoop;
							}
						}
						//A troca sempre comeca da original:
						CopiaSol(SolRodada, SolOriginal);
					}
				}
			}
		}
		BL_2opt_PosLoop:
		CopiaSol(SolOriginal, MelhorSolRodada);
	} else {
		for (int iTroca = 1; iTroca <= iTrocas; iTroca++) {
			int iContaTentativas = 0;
			do {
				iContaTentativas++;
				iRotaA = rand() % iRotaTot + 1;
				do iRotaB = rand() % iRotaTot + 1; while (iRotaA == iRotaB);
				iPosA = rand() % iTamanhoRota(SolOriginal, iRotaA) + 1;
				iPosB = rand() % iTamanhoRota(SolOriginal, iRotaB) + 1;
				CopiaSol(MelhorSolRodada, SolOriginal);
				TrocaDoisOptEstrela(MelhorSolRodada, iRotaA, iPosA, iRotaB, iPosB);
			} while (!bValidaSol(MelhorSolRodada)&& iContaTentativas < N*N);
			CopiaSol(SolOriginal, MelhorSolRodada);
		}
	}

	return CustoSolucao(SolOriginal);
}

double dRouteElimition(int(&SolOriginal)[M][M], int iTamMax) {
	int SolRodada[M][M], SolRodadaWIP[M][M], MelhorSolRodada[M][M];
	int iRotaDestino, iRotaExclusiva;

	CopiaSol(MelhorSolRodada, SolOriginal);
	//Rodamos todas as rotas que servem a um cliente exclusivamente e tentamos alocar em rotas com mais de 1 clientes
	for (iRotaExclusiva = 1; iRotaExclusiva <= ContaRotas(SolOriginal); iRotaExclusiva++) {
		CopiaSol(SolRodada, SolOriginal);
		if (1 == iTamanhoRota(SolRodada, iRotaExclusiva)) {
			//Remove essa rotinha exclusiva da solucao:
			int iClient = RemoveClient(SolRodada, iRotaExclusiva, 1);
			CopiaSol(SolRodadaWIP, SolRodada);
			for (iRotaDestino = 1; iRotaDestino <= ContaRotas(SolRodada); iRotaDestino++)
				if (Demanda[iClient] + DemandaRota(SolRodada, iRotaDestino)<Capac[K])
					for (int iPos = 1; iPos <= iTamanhoRota(SolRodada, iRotaDestino); iPos++) {
						InsereSolucao(SolRodada, iRotaDestino, iPos, iClient);
						if (bValidaSol(SolRodada))
							if (CustoSolucao(SolRodada) <= CustoSolucao(MelhorSolRodada))
								CopiaSol(MelhorSolRodada, SolRodada);
						CopiaSol(SolRodada, SolRodadaWIP);
					}
		}
	}

	if (CustoSolucao(MelhorSolRodada) <= CustoSolucao(SolOriginal))
		CopiaSol(SolOriginal, MelhorSolRodada);
	return  CustoSolucao(SolOriginal);
}

/*------Codigo Geral Busca Local-----*/

double BuscaLocal(int(&Sol)[M][M], string sVizinhanca, string sNomeInstancia, int iOpRemocao, int iOpInsercao, int iCliRelocate,int iMaxTrechoCross, bool bRouteElimination, int &iMelhorias) {
	int SolOriginal[M][M];
	double dCustoOriginal, dCustoFinal;

	CopiaSol(SolOriginal, Sol);

	int iIteracao = 0;
	dCustoFinal = CustoSolucao(SolOriginal);
	do {
		iIteracao++;
		dCustoOriginal = dCustoFinal;

		//Antes Route Elimination:
		if (bRouteElimination)
			while (dRouteElimition(SolOriginal, 1) < dCustoFinal)
				dCustoFinal = CustoSolucao(SolOriginal);

		if (sVizinhanca == "Relocate")
			dCustoFinal = Relocate(SolOriginal, iOpRemocao, iOpInsercao, iCliRelocate, true);

		if (sVizinhanca == "Cross")
			dCustoFinal = dCROSS(SolOriginal,1, iMaxTrechoCross,true);

		if (sVizinhanca == "2Opt*")
			dCustoFinal = dDoisOptEstrela(SolOriginal,true,1);

	} while (dCustoFinal < dCustoOriginal);

	//O numero de melhorias e o de iteracoes menos a rodada que nao deu em nada
	iMelhorias = iIteracao - 1;
	if (PrintRoutes)
		MostraRotas(sNomeInstancia + "_SCIH+" + sVizinhanca, SolOriginal);
	CopiaSol(Sol,SolOriginal);

	return dCustoFinal;
}