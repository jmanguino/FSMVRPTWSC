#pragma once
//Funções gerais:
double CalcInicioAtend(int ant, int cliente, double AtendAnt);
int ContaRotas(int Sol[][M]);
double CustoRota(double dist, double dem);
double CustoSolucao(int Sol[][M]);
void DadosRota(int rota);
double DemandaRota(int iSol[][M], int iRota);
double DistanciaRota(int iSol[][M], int iRota);
int FaixaDistancia(double d);
void MostraRotas(string sNomeInstancia, int iSol[][M]);
int iTamanhoRota(int iSol[][M], int iRota);
int TipodeVeiculo(double d);
bool bValidaSol(int iSol[][M]);
double dElapsedTime(clock_t Start, clock_t End);

//Funções da Heuristica
double SCIH();
double dCalculaC1(int iHeuristica, int iCliente, int(&SolAtual)[M][M], int iRota, int iPos, double dBigM,int iLista[]);
double dCalculaC2(int iHeuristica, int iCliente, double dC1, int iLista[]);
double CalculaC1_SCIHv2(int cliente, int pos, double MC1, int iSol[M][M], int iRota);
double CalculaC2_SCIHv2(int c, double C1);

//Funções da Busca Local
double BuscaLocal(int(&Sol)[M][M], string sVizinhanca, string sNomeInst, int iOpRemocao, int iOpInsercao, int iCliRelocate, int iMaxTrechoCross, bool bRouteElimination, int &iMelhorias);
void CopiaSol(int(&SolDestino)[M][M], int SolOrigem[][M]);
double dCROSS(int(&Sol)[M][M], int iIntensidade, int iMaxTamTrecho, bool bBestSol); //Se bBestSol = true, vamos procurar a melhor troca possivel. Se falso, vai ser troca aleatoria
double dDoisOptEstrela(int(&SolOriginal)[M][M], int iIntensidade, bool bBestSol);
string NomeOperadorRelocate(int iOp, string cTipo);
double Relocate(int(&iSol)[M][M], int iOpRemocao, int iOpInsercao, int iIntensidade, bool bSomenteMelhora);

//Meta-Heurtistica:
double VNS(int(&SolInicial)[M][M], int iTotVizinhancas, double dMaxTempoExec, string sNomeProb, int &iMelhorias, int iExpShake, int iMaxRelocate, int iMaxCross, int iMax2Opt);
double SCIH_Aleat(int(&SolAtual)[M][M], int iLimListaDist, int iTipoListaC2, int iLimListaC2);
double dGRASP(int(&iBestSol)[M][M], string sNomeInstancia, int iTamLRCd, int iTipoLRCC2, int iTamLRCC2, int &iMelhorias, int iMaxTrechoCross, int iClientesRelocate, int iRelocate_OpRemocao, int iRelocate_OpInsercao);
double Hybrid(int(&iBestSol)[M][M], string sNomeInstancia, int& iMelhorias, double dShareAleat, int iTotVizinhancas, int iTamLRCd, int iTipoLRCC2, int iTamLRCC2, int iExpShake, int iMaxRelocate, int iMaxCross, int iMax2Opt);

//VNS:
double BestImprovement(int(&SolTemp)[M][M], int iVizinhanca);
string sLocalSearchName(int iVizinhanca);
double Shake(int(&SolAtual)[M][M], int iIntensidade, int iVizinhanca, int iMaxRelocate, int iMaxCross, int iMax2Opt);