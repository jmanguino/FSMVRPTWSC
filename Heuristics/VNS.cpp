#include "Header.h"
#include "Functions.h"

double Shake(int(&SolAtual)[M][M],int iIntensidade, int iVizinhanca,int iMaxRelocate,int iMaxCross,int iMax2Opt) {
	switch (iVizinhanca) {
	case 3:// Relocate
		return Relocate(SolAtual, 1, 1, min(iIntensidade, N*iMaxRelocate/100), false); //Relocate vai remover e reinserir aleatoriamente
		break;
	case 2:// Cross
		return dCROSS(SolAtual, min(iIntensidade, N*iMaxCross/100), CrossMaxTamTrecho,false);
		break;
	case 1:// 2-Opt*
		return dDoisOptEstrela(SolAtual, min(iIntensidade, N*iMax2Opt/100),false);
		break;
	default:
		return CustoSolucao(SolAtual);
		break;
	}
	return CustoSolucao(SolAtual);
}


string sLocalSearchName(int iVizinhanca) {
	switch (iVizinhanca) {
	case 3:// Relocate
		return "Relocate";
	case 2:// Cross
		return "Cross";
	case 1:// 2-Opt*
		return "2opt";
	default:
		return "Unknown";
	}
}

double BestImprovement(int(&SolTemp)[M][M], int iVizinhanca) {
	switch (iVizinhanca) {
	case 3:// Relocate
		return Relocate(SolTemp, OperadorRemocao, OperadorInsercao, (int)round(N*.4), true);
		break;
	case 2:// Cross
		return dCROSS(SolTemp,1, CrossMaxTamTrecho,true);
		break;
	case 1:// 2-Opt*
		return dDoisOptEstrela(SolTemp,1,true);
		break;
	default:
		return CustoSolucao(SolTemp);
		break;
	}
}


double VNS(int(&SolInicial)[M][M], int iTotVizinhancas, double dMaxTempoExec, string sNomeProb,int &iMelhorias, int iExpShake, int iMaxRelocate, int iMaxCross, int iMax2Opt) {
	int SolAtual[M][M], SolTemp[M][M];
	double dRodada;
	double dTempoExec = 0;
	double dCostAtual, dCostAfterShake, dCostAfterLS;
	bool bShake;
	int iIntensidadeShake;

	iMelhorias = 0;
	ofstream Evolucao;
	if (PrintEvolution) {
		string sFileName = "EvolucaoVNS_" + sNomeProb + "s" + std::to_string(iExpShake) + "r" + std::to_string(iMaxRelocate) + "c"  +std::to_string(iMaxCross) + "o" + std::to_string(iMax2Opt) + ".txt";
		Evolucao.open(sFileName);
		Evolucao << "Evolucao das solucoes do VNS por etapa" << endl;
		Evolucao << "Tempo(s);" << "Rodada;" << "Etapa" << ";" << "FuncaoObjetivo" << endl;
		Evolucao << "0;0; SCIH" << Heuristica << ";" << CustoSolucao(SolInicial);
		//cout << endl << "Sol Heuristica: " << CustoSolucao(SolInicial);
	}

	CopiaSol(SolAtual, SolInicial);

	clock_t tStart = clock();
	double dMinutos = 0;
	//Primeiro faremos a busca local sem shake:
	bShake = false;
	dRodada = 0;
	int iRodadaSemMelhoria = 0;
	do{
		dRodada++;
		CopiaSol(SolTemp, SolAtual);
		dCostAtual = CustoSolucao(SolTemp);
		int iVizinhanca = 1;
		do{
			if (bShake) {
				iIntensidadeShake = (int)trunc(iRodadaSemMelhoria / iExpShake) + 1;
				dCostAfterShake = Shake(SolTemp, iIntensidadeShake, iVizinhanca,  iMaxRelocate,  iMaxCross,  iMax2Opt);
				if (!bValidaSol(SolTemp))
					int iErro = 1;
			}
			dCostAfterLS = BestImprovement(SolTemp, iVizinhanca);
			if (!bValidaSol(SolTemp))
				int iErro = 1;
			if (dCostAfterLS < dCostAtual) {
				//Se melhora, imediatamente salvo a sol melhor:
				CopiaSol(SolAtual, SolTemp);
				dCostAtual = dCostAfterLS;
				//Conto melhorias, zero rodadas sem melhoriae cancelo o proximo shake
				iMelhorias++;
				bShake = false;
				iRodadaSemMelhoria = 0;
				//E, se for o caso, registro o feito:
				if(PrintEvolution)
					Evolucao << endl <<  dElapsedTime(tStart,clock()) << ";" << dRodada <<";" << sLocalSearchName(iVizinhanca) << ";" << dCostAtual;
				//cout << endl << "Minuto " << dMinutos +1 << ", Rodada " << dRodada << ": Nova Melhoria! Custo: " << dCostAtual;
			} else {
				iVizinhanca++;
				bShake = true;
			}
		} while (iVizinhanca <= iTotVizinhancas);
		bShake = true;
		iRodadaSemMelhoria++;
		dTempoExec = dElapsedTime(tStart, clock());
		if (dTempoExec > (dMinutos + 1) * 60) {
			cout << endl << "Tempo: " << dTempoExec << "s; Rodadas: " << dRodada << " Melhorias: " << iMelhorias <<" Melhor Sol: " << dCostAtual;
			dMinutos++;
		}
	} while (dTempoExec <= dMaxTempoExec);
	cout << endl << "Final - Tempo: " << dTempoExec << "s; Rodadas: " << dRodada << " Melhorias: " << iMelhorias << " Melhor Sol: " << dCostAtual;
	Evolucao << endl << dTempoExec << ";" << dRodada << ";" << "Final" << ";" << dCostAtual;
	Evolucao.close();

	if (PrintRoutes)
		MostraRotas(sNomeProb + "_VNS_Shake-" + TesteBL + "s" + std::to_string(iExpShake) + "r" + std::to_string(iMaxRelocate) + "c" + std::to_string(iMaxCross) + "o" + std::to_string(iMax2Opt), SolAtual);

	return dCostAtual;
}
