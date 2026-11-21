#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAM 1000
#define BRANCO 'B'
#define MAX_TRANSICOES 30

typedef struct {
    char estado[10];
    char ler;
    char escrever;
    char movimento;
    char proximoEstado[10];
} Transicao;

typedef struct {
    char fita[TAM];
    int cabecote;
    int passos;
    int tamanhoEntrada;
    char estadoAtual[10];
} MaquinaTuring;

Transicao transicoes[MAX_TRANSICOES];
int quantidadeTransicoes = 0;

// Cadastra uma regra da maquina
void adicionar(char estado[], char ler,
               char escrever, char movimento,
               char proximo[]) {

    strcpy(transicoes[quantidadeTransicoes].estado, estado);
    transicoes[quantidadeTransicoes].ler = ler;
    transicoes[quantidadeTransicoes].escrever = escrever;
    transicoes[quantidadeTransicoes].movimento = movimento;
    strcpy(transicoes[quantidadeTransicoes].proximoEstado, proximo);

    quantidadeTransicoes++;
}

// Configura as regras da maquina
void configurarTransicoes() {

    // Estado q0: percorre o primeiro numero
    adicionar("q0", '1', '1', 'R', "q0");

    // Encontra o +, substitui por 1
    adicionar("q0", '+', '1', 'R', "q1");

    // Estado q1: percorre o segundo numero
    adicionar("q1", '1', '1', 'R', "q1");

    // Encontra o branco e volta uma posicao
    adicionar("q1", 'B', 'B', 'L', "q2");

    // Estado q2: apaga o ultimo 1 e finaliza
    adicionar("q2", '1', 'B', 'R', "qf");
}

// Procura uma regra compativel com o estado e simbolo
int buscarTransicao(char estado[], char simbolo,
                    Transicao *resultado) {

    for (int i = 0; i < quantidadeTransicoes; i++) {

        if (strcmp(transicoes[i].estado, estado) == 0 &&
            transicoes[i].ler == simbolo) {

            *resultado = transicoes[i];
            return 1;
        }
    }

    return 0;
}

// Valida entrada para duplicacao unaria: apenas '1' (nao vazio)
int validarEntradaDuplicacao(char entrada[]) {
    if (entrada[0] == '\0') {
        return 0;
    }

    for (int i = 0; entrada[i] != '\0'; i++) {
        if (entrada[i] != '1') {
            return 0;
        }
    }

    return 1;
}

void configurarDuplicacao() {
    quantidadeTransicoes = 0;

    // 1. Vai ate o fim da entrada original e coloca '#'
    adicionar("q0", '1', '1', 'R', "q0");
    adicionar("q0", 'B', '#', 'L', "q1");

    // 2. Volta ate o inicio da fita ('B' na posicao 0)
    adicionar("q1", '1', '1', 'L', "q1");
    adicionar("q1", 'B', 'B', 'R', "q2");

    // 3. Procura o proximo '1' antes do '#' para marcar com 'X'
    adicionar("q2", 'X', 'X', 'R', "q2");
    adicionar("q2", '1', 'X', 'R', "q3");
    adicionar("q2", '#', '#', 'L', "q5"); // acabou!

    // 4. Avanca ate o primeiro 'B' depois do '#' e escreve um '1'
    adicionar("q3", '1', '1', 'R', "q3");
    adicionar("q3", '#', '#', 'R', "q3");
    adicionar("q3", 'B', '1', 'L', "q4");

    // 5. Volta ate o 'B' inicial (posicao 0) para pegar o proximo '1'
    adicionar("q4", '1', '1', 'L', "q4");
    adicionar("q4", '#', '#', 'L', "q4");
    adicionar("q4", 'X', 'X', 'L', "q4");
    adicionar("q4", 'B', 'B', 'R', "q2");

    // 6. Volta ate o 'B' inicial para restaurar tudo
    adicionar("q5", 'X', 'X', 'L', "q5");
    adicionar("q5", 'B', 'B', 'R', "q6");

    // 7. Restaura 'X' -> '1', troca '#' -> '1' e vai apagar o ultimo '1'
    adicionar("q6", 'X', '1', 'R', "q6");
    adicionar("q6", '#', '1', 'R', "q6");
    adicionar("q6", '1', '1', 'R', "q6");
    adicionar("q6", 'B', 'B', 'L', "q7");

    // 8. Apaga o ultimo '1' (que compensa o '#' virando '1') e finaliza
    adicionar("q7", '1', 'B', 'R', "qf");
}

// Inicializa a fita e o cabecote
void inicializar(MaquinaTuring *m, char entrada[]) {

    for (int i = 0; i < TAM; i++) {
        m->fita[i] = BRANCO;
    }

    // Copia a entrada para a fita, sem substituir o branco por '\0'
    for (int i = 0; entrada[i] != '\0'; i++) {
        m->fita[i + 1] = entrada[i];
    }

    m->cabecote = 1;
    m->passos = 0;
    m->tamanhoEntrada = strlen(entrada);

    strcpy(m->estadoAtual, "q0");
}

// Exibe um trecho da fita suficiente para ver a duplicacao e o cabecote
void mostrarFita(MaquinaTuring *m, int limiteExibicao) {
    if (limiteExibicao >= TAM) {
        limiteExibicao = TAM - 1;
    }

    printf("Fita: ");
    for (int i = 0; i <= limiteExibicao; i++) {
        printf("%c ", m->fita[i]);
    }

    printf("\n      ");
    for (int i = 0; i < m->cabecote; i++) {
        printf("  ");
    }

    printf("^\n");
    printf("Estado atual: %s\n", m->estadoAtual);
    printf("Passos: %d\n\n", m->passos);
}

// Executa uma transicao
int executarPasso(MaquinaTuring *m) {

    char simbolo = m->fita[m->cabecote];
    Transicao t;

    if (!buscarTransicao(m->estadoAtual, simbolo, &t)) {
        return 0;
    }

    m->fita[m->cabecote] = t.escrever;

    strcpy(m->estadoAtual, t.proximoEstado);

    if (t.movimento == 'R') {
        m->cabecote++;
    } else if (t.movimento == 'L') {
        m->cabecote--;
    }

    m->passos++;

    return 1;
}

// Executa a maquina ate chegar ao estado final
// Executa a maquina ate chegar ao estado final
void executarMaquina(MaquinaTuring *m, int opcao) {
    int limite = (opcao == 2) ? (m->tamanhoEntrada * 2 + 2) : (m->tamanhoEntrada + 1);

    printf("\n--- EXECUCAO DA MAQUINA ---\n\n");

    mostrarFita(m, limite);

    while (strcmp(m->estadoAtual, "qf") != 0) {

        if (!executarPasso(m)) {
            printf("Erro: nenhuma transicao encontrada!\n");
            return;
        }

        mostrarFita(m, limite);
    }

    printf("Maquina finalizada!\n");
}

// Verifica se a entrada esta no formato correto
int validarEntrada(char entrada[]) {

    int quantidadeMais = 0;

    for (int i = 0; entrada[i] != '\0'; i++) {

        if (entrada[i] == '+') {
            quantidadeMais++;
        } else if (entrada[i] != '1') {
            return 0;
        }
    }

    return quantidadeMais == 1;
}

// Mostra o resultado da adicao
void mostrarResultado(MaquinaTuring *m) {

    printf("\nResultado: ");

    int encontrouUm = 0;

    for (int i = 0; i < TAM; i++) {

        if (m->fita[i] == '1') {
            printf("1");
            encontrouUm = 1;
        }
    }

    if (!encontrouUm) {
        printf("0 (fita vazia)");
    }

    printf("\n");
}

int main() {

MaquinaTuring maquina;
    char entrada[999];
    int opcao;

    printf("SIMULADOR DE MAQUINA DE TURING (UNARIO)\n");
    printf("1 - Adicao unaria (ex: 11+111)\n");
    printf("2 - Duplicacao unaria (ex: 111)\n");
    printf("Escolha uma opcao: ");

    if (scanf("%d", &opcao) != 1 || (opcao != 1 && opcao != 2)) {
        printf("Opcao invalida!\n");
        return 1;
    }

    if (opcao == 1) {
        printf("Digite os numeros separados por +: ");
        if (scanf("%998s", entrada) != 1 || !validarEntrada(entrada)) {
            printf("Entrada invalida! Use apenas 1 e um unico +.\n");
            return 1;
        }
        configurarTransicoes(); // sua funcao de adicao
    } else {
        printf("Digite o numero em unario (apenas 1): ");
        if (scanf("%998s", entrada) != 1 || !validarEntradaDuplicacao(entrada)) {
            printf("Entrada invalida! Use apenas 1.\n");
            return 1;
        }
        configurarDuplicacao();
    }

    inicializar(&maquina, entrada);
    executarMaquina(&maquina, opcao);

    if (strcmp(maquina.estadoAtual, "qf") == 0) {
        mostrarResultado(&maquina);
    }

    return 0;
}