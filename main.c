// main.c - Programa Principal do Sistema de Gestão de Estacionamento
// Autor: Rodrigo Couto
// Descrição: Programa principal que coordena todas as funcionalidades
//           do sistema de gestão de estacionamento conforme especificado
//           no enunciado do projeto.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

// Inclusão de todos os headers do sistema
#include "datastructs.h"      // Estruturas de dados
#include "utils.h"           // Funções utilitárias
#include "parque.h"          // Gestão do parque
#include "tarifas.h"         // Gestão de tarifas
#include "estacionamentos.h" // Gestão de estacionamentos
#include "ficheiros.h"       // Exportação de ficheiros
#include "estatisticas.h"    // Estatísticas e relatórios
#include "logs.h"           // Sistema de logs

// -----------------------------------------------------------------------------
// DECLARAÇÃO DE VARIÁVEIS GLOBAIS DO SISTEMA
// -----------------------------------------------------------------------------

/**
 * Estrutura dinâmica que representa o parque de estacionamento.
 * Contém a configuração atual do parque (pisos, filas, lugares)
 * e o estado de ocupação de cada lugar.
 * @note Inicializada com valores nulos/nulos.
 */
ParqueDinamico parqueDinamico = {NULL, 0, 0, 0};

/**
 * Lista dinâmica que contém todos os registos de estacionamento.
 * Armazena tanto veículos atuais como históricos de estacionamento.
 * @note Inicializada com valores nulos/nulos.
 */
ListaEstacionamentos listaEstacionamentos = {NULL, 0, 0};

// -----------------------------------------------------------------------------
// FUNÇÃO PRINCIPAL: main()
// -----------------------------------------------------------------------------

/**
 * Função principal do programa.
 * Esta função coordena todo o sistema de gestão de estacionamento:
 * 1. Inicializa estruturas de dados e sistema de logs
 * 2. Apresenta menu principal com todas as funcionalidades
 * 3. Processa escolhas do utilizador
 * 4. Garante salvamento de dados e libertação de memória na saída
 * 
 * @return 0 se o programa terminou com sucesso, 1 em caso de erro crítico
 */
int main() {
    // 1. INICIALIZAÇÃO DO SISTEMA
    // ============================
    
    // Registar início da execução no log do sistema
    adicionarLog(LOG_INFO, "=================== SISTEMA INICIADO ===================");
    
    // Inicializar lista dinâmica de estacionamentos
    // Alocar memória inicial para 100 estacionamentos (pode ser redimensionada)
    listaEstacionamentos.capacidade = 100;
    listaEstacionamentos.estacionamentos = malloc(listaEstacionamentos.capacidade * sizeof(Estacionamento));
    
    // Verificar se a alocação de memória foi bem sucedida
    if (!listaEstacionamentos.estacionamentos) {
        printf("Erro crítico: Não foi possível alocar memória para os estacionamentos!\n");
        adicionarLog(LOG_ERROR, "Falha na alocação de memória para estacionamentos");
        return 1;  // Terminar com código de erro
    }
    
    // Inicializar contadores da lista
    listaEstacionamentos.total = 0;
    
    // Inicializar outras estruturas do sistema
    inicializarParque();  // Configuração inicial do parque
    inicializarDados();   // Carrega dados de ficheiros e configurações
    
    // 2. LOOP PRINCIPAL DO MENU
    // ==========================
    
    int opcao;  // Variável para armazenar escolha do utilizador
    
    do {
        // Limpar ecrã para melhor apresentação
        limparEcra();
        
        // Mostrar cabeçalho do sistema
        printf("===================================================\n");
        printf("               GESTOR DE ESTACIONAMENTO             \n");
        printf("===================================================\n");

        // Mostrar informação sobre lugares disponíveis (sempre visível)
        // Implementa funcionalidade 3 do enunciado
        exibirLugaresDisponiveis();

        printf(" \n");
        printf(" __________________MENU PRINCIPAL:________________\n");
        printf("|                                                 |\n");
        printf("| 1 - Registar Entrada de veículo                |\n");
        printf("| 2 - Registar Saída e calcular tarifa           |\n");
        printf("| 3 - Consultar/Remover/Alterar um ticket        |\n");
        printf("| 4 - Listar todos os tickets                    |\n");
        printf("| 5 - Gerir lugares indisponíveis                |\n");
        printf("| 6 - Listar lugares indisponíveis               |\n");
        printf("| 7 - Mostrar mapa do estacionamento por piso    |\n");
        printf("| 8 - Criar ficheiro de texto (TXT)              |\n");
        printf("| 9 - Estatísticas e relatórios                  |\n");
        printf("|10 - Exportar para ficheiro CSV                  |\n");
        printf("|11 - Ver/Alterar tarifas do sistema             |\n");
        printf("|12 - Alterar configuração do parque             |\n");
        printf("|13 - Verificar consistência de dados            |\n");
        printf("|14 - Listar logs do sistema                     |\n");
        printf("|15 - Limpar logs do sistema                     |\n");
        printf("|99 - Salvar dados manualmente                   |\n");
        printf("| 0 - Sair do programa                           |\n");
        printf("|_________________________________________________|\n");
        printf("Escolha uma opcao: ");
        
        // Ler opção do utilizador com validação
        if (scanf("%d", &opcao) != 1) {
            printf("Erro: Entrada inválida. Por favor, insira um número.\n");
            adicionarLog(LOG_WARNING, "Entrada inválida no menu principal");
            
            // Limpar buffer do teclado para evitar loops infinitos
            while (getchar() != '\n');
            continue;
        }
        
        // 3. PROCESSAR ESCOLHA DO UTILIZADOR
        // ===================================
        
        switch (opcao) {
            case 1:
                // Funcionalidade 4-a: Registar entrada de veículo
                registarEntrada();
                break;
                
            case 2:
                // Funcionalidade 6: Registar saída e calcular valor
                registarSaida();
                break;
                
            case 3:
                // Funcionalidade 4-b: Gestão de tickets
                menuTicket();
                break;
                
            case 4:
                // Funcionalidade 10: Listar todos os tickets
                listarTodosTickets();
                break;
                
            case 5:
                // Funcionalidade 7: Gerir lugares indisponíveis
                menuIndisponiveis();
                break;
                
            case 6:
                // Listar lugares indisponíveis
                listarIndisponiveis();
                break;
                
            case 7:
                // Funcionalidade 8: Mostrar mapa de ocupação
                imprimirMapaPiso();
                break;
                
            case 8:
                // Funcionalidade 10: Gerar ficheiro TXT
                gerarTXT();
                break;
                
            case 9:
                // Funcionalidades de estatísticas (11 e extras)
                estatisticas();
                break;
                
            case 10:
                // Extra E3: Exportar para CSV
                guardarCSV();
                break;
                
            case 11:
                // Gestão de tarifas (alterável conforme enunciado)
                alterarTarifas();
                break;
                
            case 12:
                // Alterar configuração do parque
                alterarParque();
                break;
                
            case 13:
                // Verificação de consistência de dados
                verificarConsistenciaDados();
                break;
                
            case 14:
                // Listar logs do sistema
                listarLogs();
                break;
                
            case 15:
                // Limpar logs do sistema
                limparLogs();
                break;
                
            case 99:
                // Funcionalidade adicional: Salvamento manual
                salvarDadosManual();
                break;
                
            case 0:
                // 4. SAÍDA DO PROGRAMA
                // =====================
                printf("Saindo do programa. Obrigado por usar o Gestor de Estacionamento!\n");
                
                // Salvar dados antes de sair (funcionalidade 9)
                salvarParque();
                salvarEstacionamentos();
                
                // Atualizar logs com informação da saída
                time_t t = time(NULL);
                struct tm *tm_info = localtime(&t);
                char msgSaida[150];
                snprintf(msgSaida, sizeof(msgSaida), 
                        "Sistema encerrado em %04d-%02d-%02d %02d:%02d com salvamento automático", 
                        tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                        tm_info->tm_hour, tm_info->tm_min);
                adicionarLog(LOG_INFO, msgSaida);
                break;

            default:
                printf("Opção inválida. Por favor, tente novamente.\n");
        }
        
    } while (opcao != 0);  // Continuar até o utilizador escolher sair (opção 0)
    
    // 5. LIMPEZA E LIBERTAÇÃO DE RECURSOS
    // ====================================
    adicionarLog(LOG_INFO, "=================== LIBERTANDO MEMÓRIA ===================");
    
    // Libertar memória da lista de estacionamentos
    if (listaEstacionamentos.estacionamentos) {
        free(listaEstacionamentos.estacionamentos);
        listaEstacionamentos.estacionamentos = NULL;
        listaEstacionamentos.total = 0;
        listaEstacionamentos.capacidade = 0;
        adicionarLog(LOG_INFO, "Memória dos estacionamentos libertada");
    }
    
    // Libertar memória da matriz 3D do parque
    if (parqueDinamico.parque) {
        // Percorrer todos os níveis da matriz para libertar memória
        for (int p = 0; p < parqueDinamico.numPisos; p++) {
            for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
                free(parqueDinamico.parque[p][f]);  // Libertar linha de lugares
            }
            free(parqueDinamico.parque[p]);  // Libertar array de filas
        }
        free(parqueDinamico.parque);  // Libertar array de pisos
        parqueDinamico.parque = NULL;
        adicionarLog(LOG_INFO, "Memória do parque libertada");
    }
    
    // Registar término normal do sistema
    adicionarLog(LOG_INFO, "=================== SISTEMA TERMINADO ===================");
    
    return 0;  // Programa terminado com sucesso
}