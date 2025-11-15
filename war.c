// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//
// OBJETIVOS:
// - Modularizar completamente o código em funções especializadas.
// - Implementar um sistema de missões para um jogador.
// - Criar uma função para verificar se a missão foi cumprida.
// - Utilizar passagem por referência (ponteiros) para modificar dados e
//   passagem por valor/referência constante (const) para apenas ler.
// - Foco em: Design de software, modularização, const correctness, lógica de jogo.
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// --- Constantes Globais -----------------------------------------------------

#define TOTAL_TERRITORIOS 5
#define TAM_STR 50
#define TOTAL_MISSOES 3

// --- Estrutura de Dados -----------------------------------------------------
// Define a estrutura para um território, contendo seu nome,
// a cor do exército que o domina e o número de tropas.

typedef struct {
    char nome[TAM_STR];
    char corExercito[TAM_STR];
    int tropas;
} Territorio;

typedef struct {
    int id;
    char descricao[120];
} Missao;

// --- Protótipos das Funções -------------------------------------------------

// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa(int quantidade);
void inicializarTerritorios(Territorio *mapa, int quantidade);
void liberarMemoria(Territorio *mapa);

// Funções de interface com o usuário:
void exibirMenuPrincipal(void);
void exibirMapa(const Territorio *mapa, int quantidade);
void exibirMissao(const Missao *missao);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio *mapa, int quantidade, const char *corJogador);
void simularAtaque(Territorio *origem, Territorio *destino);
int sortearMissao(void);
int verificarVitoria(const Territorio *mapa, int quantidade,
                     const Missao *missao, const char *corJogador);

// Função utilitária:
void limparBufferEntrada(void);

// --- Função Principal (main) -----------------------------------------------
// Função principal que orquestra o fluxo do jogo, chamando as outras funções em ordem.

int main(void) {
    setlocale(LC_ALL, "");       // 1. Configuração de locale
    srand((unsigned) time(NULL)); // 1. Semente de números aleatórios

    // 1. Configuração Inicial (Setup)
    Territorio *mapa = alocarMapa(TOTAL_TERRITORIOS);
    if (mapa == NULL) {
        fprintf(stderr, "Erro ao alocar memoria para o mapa.\n");
        return 1;
    }

    printf("=== PROJETO WAR ESTRUTURADO (VERSAO BASICA) ===\n\n");

    // Definir cor do jogador
    char corJogador[TAM_STR];
    printf("Digite a cor do seu exercito (ex: Azul, Vermelho): ");
    fgets(corJogador, TAM_STR, stdin);
    corJogador[strcspn(corJogador, "\n")] = '\0';

    // Inicializar territórios (entrada pelo usuário, como no exercício anterior)
    inicializarTerritorios(mapa, TOTAL_TERRITORIOS);

    // Sorteio de missão
    Missao missaoAtual;
    missaoAtual.id = sortearMissao();
    switch (missaoAtual.id) {
        case 1:
            strcpy(missaoAtual.descricao,
                   "Conquistar pelo menos 3 territorios com sua cor.");
            break;
        case 2:
            strcpy(missaoAtual.descricao,
                   "Ter pelo menos 15 tropas somadas em todos os seus territorios.");
            break;
        case 3:
        default:
            strcpy(missaoAtual.descricao,
                   "Dominar todos os territorios do mapa.");
            break;
    }

    int opcao;
    int venceu = 0;

    // 2. Laço Principal do Jogo (Game Loop)
    do {
        printf("\n=============================================\n");
        exibirMapa(mapa, TOTAL_TERRITORIOS);
        exibirMissao(&missaoAtual);
        exibirMenuPrincipal();

        printf("Escolha uma opcao: ");
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada invalida.\n");
            limparBufferEntrada();
            continue;
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, TOTAL_TERRITORIOS, corJogador);
                break;
            case 2:
                if (verificarVitoria(mapa, TOTAL_TERRITORIOS, &missaoAtual, corJogador)) {
                    printf("\n*** PARABENS! Voce cumpriu a missao! ***\n");
                    venceu = 1;
                } else {
                    printf("\nAinda nao cumpriu a missao. Continue jogando!\n");
                }
                break;
            case 0:
                printf("\nEncerrando o jogo...\n");
                break;
            default:
                printf("Opcao invalida.\n");
        }

        if (!venceu && opcao != 0) {
            printf("\nPressione ENTER para continuar...");
            getchar();
        }

    } while (opcao != 0 && !venceu);

    // 3. Limpeza
    liberarMemoria(mapa);

    printf("\nObrigado por jogar!\n");
    return 0;
}

// --- Implementação das Funções ---------------------------------------------

// alocarMapa():
// Aloca dinamicamente a memória para o vetor de territórios usando calloc.
// Retorna um ponteiro para a memória alocada ou NULL em caso de falha.
Territorio* alocarMapa(int quantidade) {
    Territorio *mapa = (Territorio *) calloc(quantidade, sizeof(Territorio));
    return mapa;
}

// inicializarTerritorios():
// Preenche os dados iniciais de cada território no mapa (nome, cor do exército, número de tropas).
// Esta função modifica o mapa passado por referência (ponteiro).
void inicializarTerritorios(Territorio *mapa, int quantidade) {
    printf("\n=== Cadastro de Territorios ===\n\n");
    for (int i = 0; i < quantidade; i++) {
        printf("Territorio %d:\n", i + 1);

        printf("Nome: ");
        fgets(mapa[i].nome, TAM_STR, stdin);
        mapa[i].nome[strcspn(mapa[i].nome, "\n")] = '\0';

        printf("Cor do exercito dominante: ");
        fgets(mapa[i].corExercito, TAM_STR, stdin);
        mapa[i].corExercito[strcspn(mapa[i].corExercito, "\n")] = '\0';

        printf("Numero de tropas: ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 0) {
            printf("Valor invalido. Digite um inteiro nao negativo: ");
            limparBufferEntrada();
        }
        limparBufferEntrada();
        printf("\n");
    }
}

// liberarMemoria():
// Libera a memória previamente alocada para o mapa usando free.
void liberarMemoria(Territorio *mapa) {
    free(mapa);
}

// exibirMenuPrincipal():
// Imprime na tela o menu de ações disponíveis para o jogador.
void exibirMenuPrincipal(void) {
    printf("\n=== MENU PRINCIPAL ===\n");
    printf("1 - Fase de Ataque\n");
    printf("2 - Verificar se cumpriu a missao\n");
    printf("0 - Sair\n");
}

// exibirMapa():
// Mostra o estado atual de todos os territórios no mapa, formatado como uma tabela.
// Usa 'const' para garantir que a função apenas leia os dados do mapa, sem modificá-los.
void exibirMapa(const Territorio *mapa, int quantidade) {
    printf("\n=== Mapa Atual ===\n");
    printf("%-3s | %-20s | %-15s | %-6s\n", "#", "Territorio", "Cor", "Tropas");
    printf("-------------------------------------------------------------\n");
    for (int i = 0; i < quantidade; i++) {
        printf("%-3d | %-20s | %-15s | %-6d\n",
               i, mapa[i].nome, mapa[i].corExercito, mapa[i].tropas);
    }
}

// exibirMissao():
// Exibe a descrição da missão atual do jogador com base no ID da missão sorteada.
void exibirMissao(const Missao *missao) {
    printf("\n=== Sua Missao ===\n");
    printf("ID: %d\n", missao->id);
    printf("Descricao: %s\n", missao->descricao);
}

// faseDeAtaque():
// Gerencia a interface para a ação de ataque, solicitando ao jogador os territórios de origem e destino.
// Chama a função simularAtaque() para executar a lógica da batalha.
void faseDeAtaque(Territorio *mapa, int quantidade, const char *corJogador) {
    int origem, destino;

    printf("\n=== Fase de Ataque ===\n");
    printf("Informe o indice do territorio de ORIGEM do ataque: ");
    if (scanf("%d", &origem) != 1) {
        printf("Entrada invalida.\n");
        limparBufferEntrada();
        return;
    }
    printf("Informe o indice do territorio de DESTINO do ataque: ");
    if (scanf("%d", &destino) != 1) {
        printf("Entrada invalida.\n");
        limparBufferEntrada();
        return;
    }
    limparBufferEntrada();

    if (origem < 0 || origem >= quantidade ||
        destino < 0 || destino >= quantidade) {
        printf("Indice de territorio invalido.\n");
        return;
    }

    if (strcmp(mapa[origem].corExercito, corJogador) != 0) {
        printf("Voce so pode atacar a partir de territorios que pertencem ao seu exercito (%s).\n",
               corJogador);
        return;
    }

    if (strcmp(mapa[destino].corExercito, corJogador) == 0) {
        printf("Voce nao pode atacar um territorio que ja eh seu.\n");
        return;
    }

    if (mapa[origem].tropas <= 1) {
        printf("Voce precisa de mais de 1 tropa para atacar.\n");
        return;
    }

    simularAtaque(&mapa[origem], &mapa[destino]);
}

// simularAtaque():
// Executa a lógica de uma batalha entre dois territórios.
// Aqui usamos uma lógica bem simples: sorteio de vencedor.
void simularAtaque(Territorio *origem, Territorio *destino) {
    printf("\nAtacando de '%s' para '%s'...\n",
           origem->nome, destino->nome);

    int resultado = rand() % 2; // 0 = defesa vence, 1 = ataque vence

    if (resultado == 1) {
        printf("O ataque foi bem-sucedido! Voce conquistou o territorio '%s'.\n",
               destino->nome);
        // Move 1 tropa para o território conquistado
        origem->tropas -= 1;
        destino->tropas = 1;
        strcpy(destino->corExercito, origem->corExercito);
    } else {
        printf("O ataque falhou! As tropas defensoras resistiram.\n");
        // Perde 1 tropa na origem
        origem->tropas -= 1;
        if (origem->tropas < 1) origem->tropas = 1; // garante pelo menos 1
    }
}

// sortearMissao():
// Sorteia e retorna um ID de missão aleatório para o jogador.
int sortearMissao(void) {
    // IDs 1, 2 ou 3
    return (rand() % TOTAL_MISSOES) + 1;
}

// verificarVitoria():
// Verifica se o jogador cumpriu os requisitos de sua missão atual.
// Retorna 1 (verdadeiro) se a missão foi cumprida, e 0 (falso) caso contrário.
int verificarVitoria(const Territorio *mapa, int quantidade,
                     const Missao *missao, const char *corJogador) {
    int i;
    switch (missao->id) {
        case 1: { // Conquistar pelo menos 3 territórios com sua cor
            int count = 0;
            for (i = 0; i < quantidade; i++) {
                if (strcmp(mapa[i].corExercito, corJogador) == 0) {
                    count++;
                }
            }
            return count >= 3;
        }
        case 2: { // Ter pelo menos 15 tropas somadas nos seus territórios
            int totalTropas = 0;
            for (i = 0; i < quantidade; i++) {
                if (strcmp(mapa[i].corExercito, corJogador) == 0) {
                    totalTropas += mapa[i].tropas;
                }
            }
            return totalTropas >= 15;
        }
        case 3: // Dominar todos os territórios
        default: {
            for (i = 0; i < quantidade; i++) {
                if (strcmp(mapa[i].corExercito, corJogador) != 0) {
                    return 0;
                }
            }
            return 1;
        }
    }
}

// limparBufferEntrada():
// Função utilitária para limpar o buffer de entrada do teclado (stdin).
void limparBufferEntrada(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // descarta caracteres
    }
}
