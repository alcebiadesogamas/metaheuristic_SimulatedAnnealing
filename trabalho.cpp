//inclusões
#include <iostream>
#include <memory.h>
#include <time.h>
#include "trabalho.hpp"
#include <stdlib.h>
#include <math.h>

//definições
#define MAX(X, Y) ((X > Y) ? X : Y)
//#define DBG

#define PESO_TLNAVIO 10000 //PENALIZACAO POR ULTRAPASSAR TEMPO LIMITE DO NAVIO NO PORTO.
#define PESO_TFBERCO 10000 //PENALIZACAO POR ULTRAPASSAR TEMPO DE FECHAMENTO DO BERCO.

using namespace std;

int main(int argc, char const *argv[])
{
    //srand(time(NULL));
    lerDados("i01.txt");
    Solucao sol;
    heuConGul(sol);
    clock_t h;
    double tempo_limite, tempo_melhor, tempo_total;

    tempo_limite = 10;

    simulated_annealing(0.97500, (NUMBERCOS * (NUMNAVIOS) / 2), 3500, 0.01, tempo_limite, sol, tempo_melhor, tempo_total);
    printf("SA - FO: %d\tTempo melhor: %.5f\tTempo total: %.5f\n", sol.fo, tempo_melhor, tempo_total);

    ordemAtendimento(sol);
    escreverSol(sol, "Solucao.txt");

    /*
    Solucao s;
    clock_t h;
    double tempo;
    h = clock();
    heuConGul(s);
    calcFO(s);
    h = clock() - h;
    tempo = (double)h / CLOCKS_PER_SEC;
    cout << s.fo << "\n";
    printf("Construtiva Gulosa...: %.5f seg.\n", tempo);

    h = clock();
    for (int i = 0; i < 10000; i++)
    {
        heuConGul(s);
    }
    h = clock() - h;
    tempo = (double)h / CLOCKS_PER_SEC;
    printf("Tempo heuristica Construtiva Gulosa...: %.5f seg.\n", tempo);

    h = clock();
    for (int i = 0; i < 10000; i++)
    {
        calcFO(s);
    }
    h = clock() - h;
    tempo = (double)h / CLOCKS_PER_SEC;
    printf("Tempo calculo FO...: %.5f seg.\n", tempo);

    escreverSol(s,"Solucao.txt");
    */
    return 0;
}

void lerDados(string arq)
{
    memset(&MAT_ATENDIMENTO, 0, sizeof(MAT_ATENDIMENTO));
    FILE *f = fopen(arq.c_str(), "r");

    fscanf(f, "%d %d\n", &NUMNAVIOS, &NUMBERCOS);

    for (int i = 0; i < NUMBERCOS; i++)
    {
        for (int j = 0; j < NUMNAVIOS; j++)
        {
            fscanf(f, "%d", &MAT_ATENDIMENTO[i][j]);
        }
    }

    for (int i = 0; i < NUMBERCOS; i++)
    {
        fscanf(f, "%d", &tempAbertura[i]);
        fscanf(f, "%d", &tempFechamento[i]);
    }
    for (int i = 0; i < NUMNAVIOS; i++)
    {
        fscanf(f, "%d", &tempChegada[i]);
    }
    for (int i = 0; i < NUMNAVIOS; i++)
    {
        fscanf(f, "%d", &tempSaida[i]);
    }
}

void testarDados(char *arq)
{
    FILE *f;
    if (arq == "")
        f = stdout;
    else
        f = fopen(arq, "w");

    fprintf(f, "%d %d\n", NUMNAVIOS, NUMBERCOS);

    for (int i = 0; i < NUMBERCOS; i++)
    {
        for (int j = 0; j < NUMNAVIOS; j++)
        {
            fprintf(f, "%d ", MAT_ATENDIMENTO[i][j]);
        }
        fprintf(f, "\n");
    }

    for (int i = 0; i < NUMBERCOS; i++)
    {
        fprintf(f, "%d ", tempAbertura[i]);
        fprintf(f, "%d ", tempFechamento[i]);
        fprintf(f, "\n");
    }

    for (int i = 0; i < NUMNAVIOS; i++)
    {
        fprintf(f, "%d ", tempChegada[i]);
    }
    fprintf(f, "\n");

    for (int i = 0; i < NUMNAVIOS; i++)
    {
        fprintf(f, "%d ", tempSaida[i]);
    }
    fclose(f);
}

void clonaSolucao(Solucao &s, Solucao &s_clone)
{
    memcpy(&s_clone, &s, sizeof(s));
}

void heuConGul(Solucao &s)
{
    memset(&s.qtd_navio_no_berco, 0, sizeof(s.qtd_navio_no_berco));
    memset(&s.MAT, -1, sizeof(s.MAT));
    
    int j;

    for (int i = 0; i < NUMNAVIOS; i++)
    {        
        do
        {
            j = rand()%NUMBERCOS;
        }         
        while (MAT_ATENDIMENTO[j][i] == 0);
        
        s.MAT[j][s.qtd_navio_no_berco[j]] = i;
        s.qtd_navio_no_berco[j]++;
    }

    for (int i = 0; i < NUMBERCOS; i++)
    {
        ordenarPosicaoMenorTempoChegada(s.MAT[i], s.qtd_navio_no_berco[i]);
    }

    /*
    int j = 0;
    for (int i = 0; i < NUMNAVIOS; i++)
    {
        while (MAT_ATENDIMENTO[j][i] == 0)
        {
            j++;
            j = j % NUMBERCOS;
        }
        s.MAT[j][s.qtd_navio_no_berco[j]] = i;
        s.qtd_navio_no_berco[j]++;
        j++;
    }

    for (int i = 0; i < NUMBERCOS; i++)
    {
        ordenarPosicaoMenorTempoChegada(s.MAT[i], s.qtd_navio_no_berco[i]);
    }*/
}

//-------------------------------------------------
void simulated_annealing(const double alfa, const int sa_max, const double temp_ini,
                         const double temp_con, const double tempo_max,
                         Solucao &s, double &tempo_melhor, double &tempo_total)
{
    int op = 0;
    clock_t hI, hF;
    Solucao s_atual, s_vizinha;
    double temp, delta, x;
    printf("\n\n>>> EXECUTANDO O SA...\n\n");
    hI = clock();
    heuConGul(s);
    calcFO(s);
    hF = clock();
    tempo_melhor = ((double)(hF - hI)) / CLOCKS_PER_SEC;
#ifdef DBG
    printf("FO: %d\tTempo: %.2f\n", s.fo, tempo_melhor);
#endif
    tempo_total = tempo_melhor;
    clonaSolucao(s, s_atual);
    while (tempo_total < tempo_max)
    {
        temp = temp_ini;
        while (temp > temp_con)
        {
            for (int i = 0; i < sa_max; i++)
            {
                clonaSolucao(s_atual, s_vizinha);
                op = rand() % 3 + 1;
                //cout << op << endl;
                if (op == 1)
                    gerar_vizinha_1(s_vizinha);
                else if (op == 2)
                    gerar_vizinha_2(s_vizinha);
                else if (op == 3)
                    gerar_vizinha_3(s_vizinha);
                delta = s_vizinha.fo - s_atual.fo;
                if (delta < 0)
                {
                    clonaSolucao(s_vizinha, s_atual);
                    if (s_vizinha.fo < s.fo)
                    {
                        clonaSolucao(s_vizinha, s);
                        hF = clock();
                        tempo_melhor = ((double)(hF - hI)) / CLOCKS_PER_SEC;
#ifdef DBG
                        printf("FO: %d\tTempo: %.2f\n", s.fo, tempo_melhor);
#endif
                    }
                }
                else
                {
                    x = rand() % 1001;
                    x = x / 1000.0;
                    if (x < exp(-delta / temp))
                        clonaSolucao(s_vizinha, s_atual);
                }
            }
            temp = temp * alfa;
            hF = clock();
            tempo_total = ((double)(hF - hI)) / CLOCKS_PER_SEC;
            if (tempo_total >= tempo_max)
                break;
        }
    }
}

//-------------------------------------------------
void gerar_vizinha_1(Solucao &s) //troca o navio de berco
{
    int pnavio, bnavio, berconovo, navio;
    //escoloha do navio aleatório.

    do
        bnavio = rand() % NUMBERCOS; //seleciono uma linha/berco aleatório.
    while (s.qtd_navio_no_berco[bnavio] == 0);

    pnavio = rand() % s.qtd_navio_no_berco[bnavio];

    navio = s.MAT[bnavio][pnavio];

    do
        berconovo = rand() % NUMBERCOS; //seleciono novo berco aleatório.
    while (MAT_ATENDIMENTO[berconovo][navio] == 0 || bnavio == berconovo);

    //retirando o navio do berco antigo.
    for (int j = pnavio; j < s.qtd_navio_no_berco[bnavio]; j++)
    {
        s.MAT[bnavio][j] = s.MAT[bnavio][j + 1];
    }
    s.qtd_navio_no_berco[bnavio]--; //diminui a quantidade de navios no berco antigo.

    s.MAT[berconovo][s.qtd_navio_no_berco[berconovo]] = navio; //colocando navio no berco novo.
    s.qtd_navio_no_berco[berconovo]++;                         //incrementa a quantidade de navios no berco novo.

    ordenarPosicaoMenorTempoChegada(s.MAT[berconovo], s.qtd_navio_no_berco[berconovo]);

    calcFO(s);
}

void gerar_vizinha_2(Solucao &s) //troca dois navios de bercos, um pro berco do outro respectivamente desde que não esteja no mesmo berco.
{
    int pnavio1, pnavio2, berco1, berco2, navio1, navio2;
//seleciono dois navios aleatorios.
DENOVO:
    do
    {
        berco1 = rand() % NUMBERCOS;
        berco2 = rand() % NUMBERCOS;
    } while (berco1 == berco2 || s.qtd_navio_no_berco[berco1] == 0 || s.qtd_navio_no_berco[berco2] == 0);

    pnavio1 = rand() % s.qtd_navio_no_berco[berco1];
    pnavio2 = rand() % s.qtd_navio_no_berco[berco2];

    navio1 = s.MAT[berco1][pnavio1];
    navio2 = s.MAT[berco2][pnavio2];

    if (MAT_ATENDIMENTO[berco1][navio2] != 0 && MAT_ATENDIMENTO[berco2][navio1] != 0)
    {
        s.MAT[berco1][pnavio1] = navio2;
        s.MAT[berco2][pnavio2] = navio1;
        ordenarPosicaoMenorTempoChegada(s.MAT[berco1], s.qtd_navio_no_berco[berco1]);
        ordenarPosicaoMenorTempoChegada(s.MAT[berco2], s.qtd_navio_no_berco[berco2]);
    }
    else
    {
        goto DENOVO;
    }

    calcFO(s);
}

void gerar_vizinha_3(Solucao &s) //troco a ordem de atendimento, pode ser que quem chegou primeiro seja o ultimo a ser atendido
{
    int pnavio1, pnavio2, berco, aux;
    
    berco = rand() % NUMBERCOS; //seleciono o berco aleatorio
    
    if (s.qtd_navio_no_berco[berco] > 1)
    {
        pnavio1 = rand() % s.qtd_navio_no_berco[berco]; //seleciono uma posicao de navio aleatório.

        do
            pnavio2 = rand() % s.qtd_navio_no_berco[berco]; //seleciono outra posicao de navio aleatório.
        while (pnavio1 == pnavio2);

        aux = s.MAT[berco][pnavio1];
        s.MAT[berco][pnavio1] = s.MAT[berco][pnavio2];
        s.MAT[berco][pnavio2] = aux;

        calcFO(s);
    }
}

void ordenarPosicaoMenorTempoChegada(int vetTempChegadaOrd[MAX_NAVIOS], int qtd)
{

    //INSERTION SORT O(N^2)
    /*  int i, key, j, value;  
    for (i = 1; i < qtd; i++) 
    {  
      
        value = vetTempChegadaOrd[i];
        j = i - 1;  
  
        /* Move elements of arr[0..i-1], that are  
        greater than key, to one position ahead  
        of their current position 
        while (j >= 0 && tempChegada[vetTempChegadaOrd[j]] > tempChegada[vetTempChegadaOrd[i]]) 
        {  
            vetTempChegadaOrd[j + 1] = vetTempChegadaOrd[j];  
            j = j - 1;  
        }  
        vetTempChegadaOrd[j + 1] = value;  
    } */

    //BUBBLE SORT O(N^2)
    int flag, aux;
    flag = 1;

    while (flag)
    {
        flag = 0;
        for (int j = 0; j < qtd - 1; j++)
        {

            if (tempChegada[vetTempChegadaOrd[j]] > tempChegada[vetTempChegadaOrd[j + 1]])
            {
                flag = 1;
                aux = vetTempChegadaOrd[j];
                vetTempChegadaOrd[j] = vetTempChegadaOrd[j + 1];
                vetTempChegadaOrd[j + 1] = aux;
            }
        }
    }
}

void calcFO(Solucao &s)
{
    s.fo = 0; //inicia com valor 0.
    ordemAtendimento(s);
    //calcula valor da FO.
    for (int i = 0; i < NUMBERCOS; i++)
    {
        for (int j = 0; j < s.qtd_navio_no_berco[i]; j++)
        {

            s.fo += (matInicioAtendimento[i][j] - tempChegada[s.MAT[i][j]] + MAT_ATENDIMENTO[i][s.MAT[i][j]]) + (PESO_TFBERCO * MAX(0, vetTerminoAtendimento[i] - tempFechamento[i])) + (PESO_TLNAVIO * MAX(0, matTerminoAtendimento[i][j] - tempSaida[s.MAT[i][j]]));
            //penalizar se estourou o tempo berco, do navio.
        }
    }
}

void ordemAtendimento(Solucao &s)
{
    /* for (int i = 0; i < NUMBERCOS; i++)
    {
        vetTerminoAtendimento[i] = tempAbertura[i]; //começa com o tempo de abertura do berco.
    }*/

    memset(&matInicioAtendimento, 0, sizeof(matInicioAtendimento));
    memset(&matTerminoAtendimento, 0, sizeof(matTerminoAtendimento));
    for (int i = 0; i < NUMBERCOS; i++)
    {
        vetTerminoAtendimento[i] = tempAbertura[i];
        for (int j = 0; j < s.qtd_navio_no_berco[i]; j++)
        {
            if (vetTerminoAtendimento[i] > tempChegada[s.MAT[i][j]]) // se o navio chegou antes do termino de atendimento ou antes do berço abrir
            {
                matInicioAtendimento[i][j] = vetTerminoAtendimento[i];       //inicio do atendimento do navio é após o termino do anterior
                vetTerminoAtendimento[i] += MAT_ATENDIMENTO[i][s.MAT[i][j]]; //termino do atendimento será somado com abertura do berco
                matTerminoAtendimento[i][j] = vetTerminoAtendimento[i];
            }
            else //se o navio chegou com o berço disponivel.
            {
                matInicioAtendimento[i][j] = tempChegada[s.MAT[i][j]];                                 //inicio do atendimento do navio é após o termino do anterior
                vetTerminoAtendimento[i] = tempChegada[s.MAT[i][j]] + MAT_ATENDIMENTO[i][s.MAT[i][j]]; //termino de atendimento
                matTerminoAtendimento[i][j] = vetTerminoAtendimento[i];
            }
        }
    }
}

int totalViolacoesBercos()
{
    int total = 0;
    for (int i = 0; i < NUMBERCOS; i++)
    {
        total += MAX(0, vetTerminoAtendimento[i] - tempFechamento[i]);
    }
    return total;
}

int totalViolacoesNavios(Solucao &s)
{
    int total = 0;

    for (int i = 0; i < NUMBERCOS; i++)
    {
        for (int j = 0; j < NUMNAVIOS; j++)
        {
            total += MAX(0, matTerminoAtendimento[i][j] - tempSaida[s.MAT[i][j]]);
        }
    }
    return total;
}

void escreverSol(Solucao &s, char *arq)
{
    FILE *f;
    if (arq == "")
        f = stdout;
    else
        f = fopen(arq, "w");

    int countBercos = 0;
    int countNavios = 0;
    cout << "< ----------------------------- SOLUÇÃO ---------------------------- >";
    cout << "\n";

    for (int i = 0; i < NUMBERCOS; i++)
    {
        if (s.qtd_navio_no_berco[i] != 0)
            countBercos++;
    }

    cout << "Número de berços utilizados.........................: " << countBercos;
    cout << "\n";

    for (int i = 0; i < NUMBERCOS; i++)
    {
        if (s.qtd_navio_no_berco[i] != 0)
            countNavios += s.qtd_navio_no_berco[i];
    }

    cout << "Número de navios atendidos..........................: " << countNavios;
    cout << "\n";

    cout << "Tempo total de operação.............................: " << s.fo;
    cout << "\n";

    cout << "Total de viol. nas jan. de tempo dos bercos.........: " << totalViolacoesBercos();
    cout << "\n";

    cout << "Total de viol. nas jan. de tempo dos navios.........: " << totalViolacoesNavios(s);
    cout << "\n";

    cout << "FO da solução.......................................: " << s.fo;
    cout << "\n";

    //qtd navios no berço j.
    for (int i = 0; i < NUMBERCOS; i++)
    {
        cout << "Quantidade de navios atendidos no berço " << i + 1 << ": " << s.qtd_navio_no_berco[i] << "\n";
    }
    cout << "\n";

    //inicio de atendimento do navio j no berco i.
    for (int i = 0; i < NUMBERCOS; i++)
    {
        for (int j = 0; j < s.qtd_navio_no_berco[i]; j++)
        {
            if (matInicioAtendimento[i][j] != 0)
                cout << "Incio de atendimento do navio " << s.MAT[i][j] << " no berço " << i + 1 << ": " << matInicioAtendimento[i][j] << "\n";
        }
        cout << "\n";
    }
    cout << "\n";

    //############Arquivo SOLUCAO##########################

    fprintf(f, "%s", "< ----------------------------- SOLUÇÃO ---------------------------- >\n");

    fprintf(f, "%s %d", "Número de berços utilizados.........................: ", countBercos);
    fprintf(f, "%s", "\n");

    fprintf(f, "%s %d", "Número de navios atendidos..........................: ", countNavios);
    fprintf(f, "%s", "\n");

    fprintf(f, "%s %d", "Tempo total de operação.............................: ", s.fo);
    fprintf(f, "%s", "\n");

    fprintf(f, "%s %d", "Total de viol. nas jan. de tempo dos bercos.........: ", totalViolacoesBercos());
    fprintf(f, "%s", "\n");

    fprintf(f, "%s %d", "Total de viol. nas jan. de tempo dos navios.........: ", totalViolacoesNavios(s));
    fprintf(f, "%s", "\n");

    fprintf(f, "%s %d", "FO da solução.......................................: ", s.fo);
    fprintf(f, "%s", "\n\n");

    //qtd navios no berço j.
    for (int i = 0; i < NUMBERCOS; i++)
    {
        fprintf(f, "%s %d %s %d %s", "Quantidade de navios atendidos no berço ", i + 1, ": ", s.qtd_navio_no_berco[i], "\n");
    }
    fprintf(f, "%s", "\n");

    //inicio de atendimento do navio j no berco i.
    for (int i = 0; i < NUMBERCOS; i++)
    {
        for (int j = 0; j < NUMNAVIOS; j++)
        {
            if (matInicioAtendimento[i][j] != 0)
                fprintf(f, "%s %d %s %d %s %d %s", "Incio de atendimento do navio ", s.MAT[i][j], " no berço ", i + 1, ": ", matInicioAtendimento[i][j], "\n");
        }
        fprintf(f, "%s", "\n");
    }
    fprintf(f, "%s", "\n");
}