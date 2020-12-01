#ifndef TRABALHO_HPP
#define TRABALHO_HPP

#include <string>

#define MAX_NAVIOS 100
#define MAX_BERCOS 20

// Estruturas de dados

typedef struct tSolucao
{
    int fo;
    int MAT[MAX_BERCOS][MAX_NAVIOS];
    int qtd_navio_no_berco[MAX_BERCOS];
} Solucao;

//Dados de entrada
static int NUMNAVIOS; //numero de navios
static int NUMBERCOS; //numero de bercos

static int MAT_ATENDIMENTO[MAX_BERCOS][MAX_NAVIOS]; //matriz de atedimento BercosXNavios

static int tempAbertura[MAX_BERCOS];
static int tempFechamento[MAX_BERCOS];

static int tempChegada[MAX_NAVIOS];
static int tempSaida[MAX_NAVIOS];

int matInicioAtendimento[MAX_BERCOS][MAX_NAVIOS];
int matTerminoAtendimento[MAX_BERCOS][MAX_NAVIOS];

int vetTerminoAtendimento[MAX_BERCOS]; //do ultimo navio

//functions
void lerDados(std::string arq);

void testarDados(char *arq);

void clonaSolucao(Solucao &s, Solucao &s_clone);

void calcFO(Solucao &s);

//heuristica
void heuConGul(Solucao &s);

void escreverSol(const Solucao &s, char *arq); //imprime na tela e no arquivo a solucao.

void ordenarPosicaoMenorTempoChegada(int vetTempChegadaOrd[MAX_NAVIOS], int qtd);

int totalViolacoesNavios();

int totalViolacoesBercos();

//meta-heuristica
void simulated_annealing(const double alfa, const int sa_max, const double temp_ini,
                         const double temp_con, const double tempo_max,
                         Solucao &s, double &tempo_melhor, double &tempo_total);

void gerar_vizinha_1(Solucao &s);

void gerar_vizinha_2(Solucao &s);

void gerar_vizinha_3(Solucao &s);

void shiftMenosUm(int berco[MAX_BERCOS], int inicio, int qtd);

bool estaNoBerco(int berco[MAX_NAVIOS], int navio, int qtd);

void ordemAtendimento(Solucao &s);

#endif /* TRABALHO_HPP */