#include "Header.h"
#include "Functions.h"

double Hybrid(int(&iBestSol)[M][M], string sNomeInstancia, int& iMelhorias, double dShareAleat, int iTotVizinhancas, int iTamLRCd,int iTipoLRCC2,int iTamLRCC2, int iExpShake, int iMaxRelocate, int iMaxCross, int iMax2Opt) {
	int iSol[M][M];
	double dCurrSol;
	double dBestSol = CustoSolucao(iBestSol);
	double dTempoExec;
	int iRodada = 0;
	
	ofstream Evolucao;
	if (PrintEvolution) {
		string sFileName = "Evolucao_Hybrid_" + sNomeInstancia + "_ShareSCIHAleat" + std::to_string(dShareAleat*100) + ".txt";
		Evolucao.open(sFileName);
		Evolucao << "Evolucao das solucoes do VNS por etapa" << endl;
		Evolucao << "Tempo(s);" << "Rodada;" << "Etapa" << ";" << "FuncaoObjetivo" << endl;
		Evolucao << "0;0; SCIH" << Heuristica << ";" << CustoSolucao(iBestSol);
		//cout << endl << "Sol Heuristica: " << CustoSolucao(SolInicial);
	}

	clock_t tInicio = clock();
	iMelhorias = 0;


	//Precisa iniciar com a SCIH2:
	while (dElapsedTime(tInicio, clock()) < dMaxExec * dShareAleat){
		iRodada++;
		dCurrSol = SCIH_Aleat(iSol, iTamLRCd, iTipoLRCC2, iTamLRCC2);
		if (dCurrSol < dBestSol) {
			iMelhorias++;
			CopiaSol(iBestSol, iSol);
			dBestSol = dCurrSol;
			if (PrintEvolution)
				Evolucao << endl << dElapsedTime(tInicio,clock()) << ";" << iRodada << ";SCIH_Aleat;" << dBestSol;
			cout << endl << "Tempo: " << dElapsedTime(tInicio, clock()) << "s; Rodadas: " << iRodada << " Melhorias: " << iMelhorias << " Melhor Sol: " << dBestSol;
		}
	}
	//Evolucao << endl << dElapsedTime(tInicio, clock()) << ";" << iRodada << ";FIM_SCIH_Aleat;" << dBestSol;
	cout << endl << "Fim da SCIHAleat. Inicio do VNS";

	//Agora o VNS:
	double dCostAfterShake, dCostAfterLS;
	int iVizinhanca;
	bool bShake = false;
	int iIntensidadeShake = 0;
	int iRodadaSemMelhoria = 0;
	double dMinutos = 0;

	while (dElapsedTime(tInicio, clock()) < dMaxExec) {
		CopiaSol(iSol, iBestSol);
		dCurrSol = CustoSolucao(iSol);
		iVizinhanca = 1;
		iRodada++;
		do {
			if (bShake) {
				iIntensidadeShake = (int)trunc(iRodadaSemMelhoria / iExpShake) + 1;
				dCostAfterShake = Shake(iSol, iIntensidadeShake, iVizinhanca, iMaxRelocate, iMaxCross, iMax2Opt);
				if (!bValidaSol(iSol))
					int iErro = 1;
			}
			dCostAfterLS = BestImprovement(iSol, iVizinhanca);
			if (!bValidaSol(iSol))
				int iErro = 1;
			if (dCostAfterLS < dBestSol) {
				//Se melhora, imediatamente salvo a sol melhor:
				CopiaSol(iBestSol, iSol);
				dBestSol = dCostAfterLS;
				//Conto melhorias, zero rodadas sem melhoriae cancelo o proximo shake
				iMelhorias++;
				bShake = false;
				iRodadaSemMelhoria = 0;
				//E, se for o caso, registro o feito:
				if (PrintEvolution)
					Evolucao << endl << dElapsedTime(tInicio, clock()) << ";" << iRodada << ";" << sLocalSearchName(iVizinhanca) << ";" << dBestSol;
				//cout << endl << "Minuto " << dMinutos +1 << ", Rodada " << dRodada << ": Nova Melhoria! Custo: " << dCostAtual;
			}
			else {
				iVizinhanca++;
				bShake = true;
			}
		} while (iVizinhanca <= iTotVizinhancas);
		bShake = true;
		iRodadaSemMelhoria++;
		dTempoExec = dElapsedTime(tInicio, clock());
		if (dTempoExec > (dMinutos + 1) * 60) {
			cout << endl << "Tempo: " << dTempoExec << "s; Rodadas: " << iRodada << " Melhorias: " << iMelhorias << " Melhor Sol: " << dBestSol;
			dMinutos++;
		}
	}

	Evolucao.close();

	return dBestSol;
}
