#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//Definições de tamanho da fita, simbolo branco e quantidade maxima de transicoes
#define TAM 500
#define BRANCO 'B'
#define MAX_TRANSICOES 30

//Estrutura para representar uma transicao da maquina
typedef struct {
    char estado[10]; //Nome do estado atual
    char ler; //Simbolo a ser lido na fita
    char escrever; //Simbolo a ser escrito na fita
    char movimento; //Movimento do cabecote ('L' para esquerda, 'R' para direita)
    char proximoEstado[10]; //Nome do proximo estado
} Transicao;

//Estrutura para representar a maquina de Turing
typedef struct {
    char fita[TAM];//Fita da maquina
    int cabecote;//Posicao atual do cabecote na fita
    int passos;//Contador de passos executados
    int tamanhoEntrada;//Tamanho da entrada fornecida pelo usuario
    char estadoAtual[10];//Nome do estado atual da maquina
} MaquinaTuring;

//Array para armazenar as transicoes da maquina e contador de quantidade de transicoes
Transicao transicoes[MAX_TRANSICOES];
int quantidadeTransicoes = 0;

// Cadastra uma regra da maquina
void adicionar(char estado[], char ler, char escrever, char movimento, char proximo[]) {

    strcpy(transicoes[quantidadeTransicoes].estado, estado); //Copia o nome do estado atual
    transicoes[quantidadeTransicoes].ler = ler; //Define o simbolo a ser lido na fita
    transicoes[quantidadeTransicoes].escrever = escrever; //Define o simbolo a ser escrito na fita
    transicoes[quantidadeTransicoes].movimento = movimento; //Define o movimento do cabecote
    strcpy(transicoes[quantidadeTransicoes].proximoEstado, proximo); //Copia o nome do proximo estado
    
    //Incrementa a quantidade de transicoes cadastradas
    quantidadeTransicoes++;
}

//Configura as regras de adição da maquina
void configurarAdicao() {

    //Estado q0: percorre o primeiro numero
    adicionar("q0", '1', '1', 'R', "q0");

    //Encontra o +, substitui por 1
    adicionar("q0", '+', '1', 'R', "q1");

    //Estado q1: percorre o segundo numero
    adicionar("q1", '1', '1', 'R', "q1");

    //Encontra o branco e volta uma posicao
    adicionar("q1", 'B', 'B', 'L', "q2");

    //Estado q2: apaga o ultimo 1 e finaliza
    adicionar("q2", '1', 'B', 'R', "qf");
}

//Procura uma regra compativel com o estado e simbolo
int buscarTransicao(char estado[], char simbolo, Transicao *resultado) {

    //Percorre todas as transicoes cadastradas
    for (int i = 0; i < quantidadeTransicoes; i++) {

        //Verifica se o estado e simbolo correspondem a uma transicao
        if (strcmp(transicoes[i].estado, estado) == 0 && transicoes[i].ler == simbolo) {

            //Se encontrou, copia a transicao para o resultado e retorna sucesso
            *resultado = transicoes[i];
            return 1;
        }
    }

    //Se nao encontrou nenhuma transicao compativel, retorna falha
    return 0;
}

// Valida entrada para duplicacao unaria: apenas '1' (nao vazio)
int validarEntradaDuplicacao(char entrada[]) {
    
    //Verifica se a entrada está vazia
    if (entrada[0] == '\0') {
        return 0;
    }
    
    //Verifica se a entrada é muito grande para duplicação
    if (strlen(entrada) > (TAM - 2) / 3){ 
        return 0;   //166 com TAM 500
    }

    //Verifica se todos os caracteres da entrada são '1'
    for (int i = 0; entrada[i] != '\0'; i++) {
        //Se encontrar algum caractere diferente de '1', retorna inválido
        if (entrada[i] != '1') {
            return 0;
        }
    }
    //Se passou por todas as verificações, a entrada é válida
    return 1;
}

//Configura as regras de duplicacao da maquina
void configurarDuplicacao() {
    //Reinicia a quantidade de transicoes para evitar duplicatas
    quantidadeTransicoes = 0;

    //1. Vai ate o fim da entrada original e coloca '#'
    adicionar("q0", '1', '1', 'R', "q0");
    adicionar("q0", 'B', '#', 'L', "q1");

    //2. Volta ate o inicio da fita ('B' na posicao 0)
    adicionar("q1", '1', '1', 'L', "q1");
    adicionar("q1", 'B', 'B', 'R', "q2");

    //3. Procura o proximo '1' antes do '#' para marcar com 'X'
    adicionar("q2", 'X', 'X', 'R', "q2");
    adicionar("q2", '1', 'X', 'R', "q3");
    adicionar("q2", '#', '#', 'L', "q5"); // acabou!

    //4. Avanca ate o primeiro 'B' depois do '#' e escreve um '1'
    adicionar("q3", '1', '1', 'R', "q3");
    adicionar("q3", '#', '#', 'R', "q3");
    adicionar("q3", 'B', '1', 'L', "q4");

    //5. Volta ate o 'B' inicial (posicao 0) para pegar o proximo '1'
    adicionar("q4", '1', '1', 'L', "q4");
    adicionar("q4", '#', '#', 'L', "q4");
    adicionar("q4", 'X', 'X', 'L', "q4");
    adicionar("q4", 'B', 'B', 'R', "q2");

    //6. Volta ate o 'B' inicial para restaurar tudo
    adicionar("q5", 'X', 'X', 'L', "q5");
    adicionar("q5", 'B', 'B', 'R', "q6");

    //7. Restaura 'X' -> '1', troca '#' -> '1' e vai apagar o ultimo '1'
    adicionar("q6", 'X', '1', 'R', "q6");
    adicionar("q6", '#', '1', 'R', "q6");
    adicionar("q6", '1', '1', 'R', "q6");
    adicionar("q6", 'B', 'B', 'L', "q7");

    //8. Apaga o ultimo '1' (que compensa o '#' virando '1') e finaliza
    adicionar("q7", '1', 'B', 'R', "qf");
}

// Inicializa a fita e o cabecote
void inicializar(MaquinaTuring *m, char entrada[]) {

    //Preenche a fita com o simbolo branco
    for (int i = 0; i < TAM; i++) {
        m->fita[i] = BRANCO;
    }

    //Copia a entrada para a fita, sem substituir o branco por '\0'
    for (int i = 0; entrada[i] != '\0'; i++) {
        m->fita[i + 1] = entrada[i];
    }

    //Define a posicao inicial do cabecote e o estado inicial da maquina
    m->cabecote = 1;
    m->passos = 0;//Calcula o tamanho da entrada para uso posterior
    m->tamanhoEntrada = strlen(entrada);//Define o estado inicial da maquina

    //Define o estado inicial da maquina
    strcpy(m->estadoAtual, "q0");
}

// Exibe um trecho da fita suficiente para ver a duplicacao e o cabecote
void mostrarFita(MaquinaTuring *m, int limiteExibicao) {
    //Limita a exibicao da fita para evitar overflow
    if (limiteExibicao >= TAM) {
        limiteExibicao = TAM - 1;
    }

    //Exibe a fita da maquina, e mostra apenas os simbolos relevantes
    printf("Fita: ");
    for (int i = 0; i <= limiteExibicao; i++) {
        printf("%c ", m->fita[i]);
    }

    //Exibe a posicao do cabecote na fita
    printf("\n      ");
    for (int i = 0; i < m->cabecote; i++) {
        printf("  ");
    }

    //Exibe o indicador do cabecote
    printf("^\n");
    printf("Estado atual: %s\n", m->estadoAtual);
    printf("Passos: %d\n\n", m->passos);
}

// Executa uma transicao
int executarPasso(MaquinaTuring *m) {

    //Obtém o simbolo atual na fita sob o cabecote
    char simbolo = m->fita[m->cabecote];
    Transicao t;

    //Procura uma transicao compativel com o estado atual e simbolo lido
    if (!buscarTransicao(m->estadoAtual, simbolo, &t)) {
        return 0;
    }

    //Escreve o simbolo, atualiza o estado e move o cabecote
    m->fita[m->cabecote] = t.escrever;

    //Atualiza o estado atual da maquina para o proximo estado definido na transicao
    strcpy(m->estadoAtual, t.proximoEstado);

    //Move o cabecote para a direita ou esquerda conforme definido na transicao
    if (t.movimento == 'R') {
        m->cabecote++;
    } else if (t.movimento == 'L') {
        m->cabecote--;
    }

    //Incrementa o contador de passos executados
    m->passos++;

    //Verifica se o cabecote saiu dos limites da fita e ajusta para o simbolo branco
    return 1;
}

// Executa a maquina ate chegar ao estado final
void executarMaquina(MaquinaTuring *m, int opcao) {
    
    //Define o limite de exibicao da fita com base na opcao escolhida, adicao ou duplicacao
    int limite = (opcao == 2) ? (m->tamanhoEntrada * 2 + 2) : (m->tamanhoEntrada + 1);

    printf("\n--- EXECUCAO DA MAQUINA ---\n\n");

    //Exibe o estado inicial da fita antes de iniciar a execucao
    mostrarFita(m, limite);

    //Executa a maquina enquanto nao atingir o estado final "qf"
    while (strcmp(m->estadoAtual, "qf") != 0) {

        //Tenta executar um passo da maquina; se nao houver transicao valida, exibe erro e encerra
        if (!executarPasso(m)) {
            printf("Erro: nenhuma transicao encontrada!\n");
            return;
        }

        //Exibe o estado atual da fita apos cada passo da execucao
        mostrarFita(m, limite);
    }

    printf("Maquina finalizada!\n");
}

// Verifica se a entrada esta no formato correto
int validarEntrada(char entrada[]) {

    //Verifica se a entrada está vazia
    if (strlen(entrada) > TAM - 2){ 
        return 0;
    }

    //Conta a quantidade de + na entrada e verifica se todos os outros caracteres são 1
    int quantidadeMais = 0;

    //Percorre cada caractere da entrada
    for (int i = 0; entrada[i] != '\0'; i++) {

        //Se encontrar um +, incrementa o contador de +
        if (entrada[i] == '+') {    
            quantidadeMais++;
        } else if (entrada[i] != '1') { //Se encontrar um caractere diferente de 1 e +, retorna inválido
            return 0;
        }
    }

    //Retorna verdadeiro se houver exatamente um + na entrada, caso contrário, retorna falso
    return quantidadeMais == 1;
}

//Mostra o resultado da adicao
void mostrarResultado(MaquinaTuring *m) {
    
    printf("\nResultado: ");

    //Verifica se encontrou pelo menos um 1 na fita
    int encontrouUm = 0;

    //Percorre a fita e exibe os simbolos 1 encontrados
    for (int i = 0; i < TAM; i++) {
        if (m->fita[i] == '1') {
            printf("1");
            encontrouUm = 1;
        }
    }

    //Se não encontrou nenhum 1, exibe mensagem indicando que a fita está vazia
    if (!encontrouUm) {
        printf("0 (fita vazia)");
    }

    printf("\n");
}

int main() {

    //Declara a maquina de Turing, a entrada do usuario e a opcao escolhida
    MaquinaTuring maquina;
    char entrada[999];
    int opcao;

    //Exibe o menu de opcoes para o usuario
    printf("SIMULADOR DE MAQUINA DE TURING (UNARIO)\n");
    printf("1 - Adicao unaria (ex: 11+111)\n");
    printf("2 - Duplicacao unaria (ex: 111)\n");
    printf("Escolha uma opcao: ");

    if (scanf("%d", &opcao) != 1 || (opcao != 1 && opcao != 2)) {
        printf("Opcao invalida!\n");
        return 1;
    }

    if (opcao == 1) {
        //Solicita ao usuario que digite os numeros separados por + e valida a entrada
        printf("Digite os numeros separados por +: ");
        if (scanf("%998s", entrada) != 1 || !validarEntrada(entrada)) {
            printf("Entrada invalida! Use apenas 1 e um unico +.\n");
            return 1;
        }
        configurarAdicao(); //Carrega as regras da adicao

    } else {
        //Solicita ao usuario que digite o numero em unario e valida a entrada
        printf("Digite o numero em unario (apenas 1): ");
        if (scanf("%998s", entrada) != 1 || !validarEntradaDuplicacao(entrada)) {
            printf("Entrada invalida! Use apenas 1.\n");
            return 1;
        }
        configurarDuplicacao(); // carrega as regras da duplicacao
    }

    //Inicializa a maquina com a entrada fornecida pelo usuario e a executa
    inicializar(&maquina, entrada);
    executarMaquina(&maquina, opcao);

    //Se a maquina terminou no estado final, exibe o resultado da operacao
    if (strcmp(maquina.estadoAtual, "qf") == 0) {
        mostrarResultado(&maquina);
    }

    return 0;
}