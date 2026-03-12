// tarifas.c - Módulo de Gestão de Tarifas
// Autor: [Seu Nome]
// Data: [Data]
// Descrição: Contém funções para carregar, alterar e salvar as tarifas
//           do sistema conforme especificado nas funcionalidades 1-b
//           e requisito de alteração fácil de tarifas do enunciado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "tarifas.h"
#include "datastructs.h"
#include "utils.h"
#include "logs.h"

// -----------------------------------------------------------------------------
// VARIÁVEL GLOBAL DO SISTEMA
// -----------------------------------------------------------------------------

/**
 * Estrutura global que contém todas as tarifas configuradas no sistema.
 * Esta variável é acedida por múltiplos módulos, especialmente no
 * cálculo de valores de estacionamento.
 */
TarifasSistema sistema;

// -----------------------------------------------------------------------------
// DECLARAÇÕES EXTERNAS
// -----------------------------------------------------------------------------

/**
 * Estruturas globais do sistema declaradas externamente.
 * Usadas para referência em algumas operações de validação.
 */
extern ParqueDinamico parqueDinamico;
extern ListaEstacionamentos listaEstacionamentos;

// -----------------------------------------------------------------------------
// FUNÇÃO: carregarTarifas()
// -----------------------------------------------------------------------------

/**
 * Carrega as tarifas do ficheiro "tarifas.txt" para memória.
 * Esta função implementa parte da funcionalidade 1-b do enunciado.
 * 
 * Processamento:
 * 1. Tenta abrir o ficheiro "tarifas.txt"
 * 2. Se existe: lê e valida as 4 tarifas
 * 3. Se não existe: oferece opções ao utilizador
 *    a. Usar tarifas padrão
 *    b. Configurar manualmente
 * 4. Valida dados e regista erros em "erros.txt" (conforme alínea c)
 * 
 * @param sistema Ponteiro para a estrutura TarifasSistema a preencher
 * 
 * @note Formato esperado do ficheiro (Figura 1 do enunciado):
 *       <TpTarifa>\t<CodTarifa>\t<Horalnf>\t<HoraSup>\t<ValorHora>
 *       Exemplo: H\tCT1\t08:00\t22:00\t0.60€
 */
void carregarTarifas(TarifasSistema *sistema) {
    // Tentar abrir ficheiro de tarifas
    FILE *file = fopen("tarifas.txt", "r");
    
    if (file != NULL) {
        // Ficheiro existe: carregar tarifas
        adicionarLog(LOG_INFO, "Tarifas carregadas do ficheiro");
        
        // Nomes para mensagens de erro
        char *nomesTarifas[] = {"T1", "T2", "T3", "T4"};
        
        // Array de ponteiros para facilitar processamento
        Tarifa *tarifas[4] = { 
            &sistema->tarifaT1, 
            &sistema->tarifaT2, 
            &sistema->tarifaT3, 
            &sistema->tarifaT4 
        };
        
        // Ler as 4 linhas do ficheiro (uma para cada tarifa)
        for (int i = 0; i < 4; i++) {
            char linha[100];
            
            if (fgets(linha, sizeof(linha), file) != NULL) {
                // Remover newline do final da linha
                linha[strcspn(linha, "\n")] = 0;
                
                // Processar os campos da linha
                // Formato: Tipo\tCódigo\tHH:MM\tHH:MM\tValor
                char valorStr[20];
                int camposLidos = sscanf(linha, "%c\t%3s\t%d:%d\t%d:%d\t%19[^\n]",
                       &tarifas[i]->tipoTarifa,
                       tarifas[i]->codigo,
                       &tarifas[i]->inicio.hora,
                       &tarifas[i]->inicio.minuto,
                       &tarifas[i]->fim.hora,
                       &tarifas[i]->fim.minuto,
                       valorStr);
                
                if (camposLidos != 7) {
                    // Erro no formato da linha
                    char msgErro[200];
                    snprintf(msgErro, sizeof(msgErro), 
                             "Erro no formato da tarifa %s na linha %d: '%s'", 
                             nomesTarifas[i], i+1, linha);
                    
                    // Registrar erro no ficheiro "erros.txt" conforme requisito
                    registrarErro("Carregar Tarifas", 
                                  "Formato inválido", 
                                  i+1, 
                                  linha, 
                                  1);  // Registro descartado
                    
                    adicionarLog(LOG_ERROR, msgErro);
                    
                    // Usar valor padrão para esta tarifa específica
                    switch(i) {
                        case 0: tarifas[i]->valor = TARIFA_T1_PADRAO; break;
                        case 1: tarifas[i]->valor = TARIFA_T2_PADRAO; break;
                        case 2: tarifas[i]->valor = TARIFA_T3_PADRAO; break;
                        case 3: tarifas[i]->valor = TARIFA_T4_PADRAO; break;
                    }
                } else {
                    // Processamento bem sucedido, converter valor string para float
                    
                    // Limpar valor (remover símbolo € e substituir vírgulas por pontos)
                    char valorLimpo[20];
                    int j = 0;
                    for (int k = 0; valorStr[k] != '\0'; k++) {
                        if (isdigit(valorStr[k]) || valorStr[k] == '.' || valorStr[k] == ',') {
                            // Substituir vírgula por ponto para conversão
                            valorLimpo[j++] = (valorStr[k] == ',') ? '.' : valorStr[k];
                        }
                    }
                    valorLimpo[j] = '\0';
                    
                    // Converter para float
                    tarifas[i]->valor = atof(valorLimpo);
                    
                    // Verificar se o valor é válido (não negativo)
                    if (tarifas[i]->valor < 0) {
                        char msg[150];
                        snprintf(msg, sizeof(msg), 
                                 "Tarifa %s com valor negativo: %.2f", 
                                 tarifas[i]->codigo, tarifas[i]->valor);
                        
                        // Registrar como aviso (não descartar o registo)
                        registrarErro("Carregar Tarifas", 
                                      "Valor negativo", 
                                      i+1, 
                                      linha, 
                                      0);  // Não descartado, apenas aviso
                        
                        adicionarLog(LOG_ERROR, msg);
                    }
                }
            } else {
                // Erro ao ler linha do ficheiro
                char msgErro[150];
                snprintf(msgErro, sizeof(msgErro), 
                         "Falha ao ler linha %d do ficheiro de tarifas", i+1);
                
                registrarErro("Carregar Tarifas", 
                              "Falha na leitura", 
                              i+1, 
                              "", 
                              1);  // Descartado
                
                adicionarLog(LOG_ERROR, msgErro);
            }
        }
        
        fclose(file);
        printf("Tarifas carregadas do ficheiro tarifas.txt\n");
        
    } else {
        // Ficheiro não existe: criar configuração inicial
        adicionarLog(LOG_WARNING, "Ficheiro de tarifas não encontrado");
        
        printf("Ficheiro de tarifas não encontrado.\n");
        printf("Pressione 1 para usar tarifas padrão.\n");
        printf("Pressione 2 para configurar manualmente.\n");

        int escolha;
        if (scanf("%d", &escolha) != 1 || (escolha != 1 && escolha != 2)) {
            printf("Entrada inválida. Atribuindo tarifas padrão.\n");
            adicionarLog(LOG_WARNING, "Entrada inválida na escolha de tarifas, usando padrão");
            escolha = 1;
        }
        getchar(); // Limpar buffer do teclado

        // Array de ponteiros para facilitar configuração
        Tarifa *tarifas[4] = { 
            &sistema->tarifaT1, 
            &sistema->tarifaT2, 
            &sistema->tarifaT3, 
            &sistema->tarifaT4 
        };

        if (escolha == 1) {
            // Opção 1: Usar tarifas padrão definidas nas constantes
            adicionarLog(LOG_INFO, "Utilizando tarifas padrão");
            
            // Configurar tarifa T1 (horária diurna)
            tarifas[0]->tipoTarifa = 'H'; 
            strcpy(tarifas[0]->codigo, "CT1"); 
            tarifas[0]->inicio.hora = 8; 
            tarifas[0]->inicio.minuto = 0;
            tarifas[0]->fim.hora = 22; 
            tarifas[0]->fim.minuto = 0; 
            tarifas[0]->valor = TARIFA_T1_PADRAO;  // 0.60€

            // Configurar tarifa T2 (horária noturna)
            tarifas[1]->tipoTarifa = 'H'; 
            strcpy(tarifas[1]->codigo, "CT2"); 
            tarifas[1]->inicio.hora = 22; 
            tarifas[1]->inicio.minuto = 0;
            tarifas[1]->fim.hora = 8; 
            tarifas[1]->fim.minuto = 0; 
            tarifas[1]->valor = TARIFA_T2_PADRAO;  // 0.30€

            // Configurar tarifa T3 (dia completo)
            tarifas[2]->tipoTarifa = 'D'; 
            strcpy(tarifas[2]->codigo, "CT3"); 
            tarifas[2]->inicio.hora = 0; 
            tarifas[2]->inicio.minuto = 0;
            tarifas[2]->fim.hora = 0; 
            tarifas[2]->fim.minuto = 0; 
            tarifas[2]->valor = TARIFA_T3_PADRAO;  // 8.00€

            // Configurar tarifa T4 (vários dias)
            tarifas[3]->tipoTarifa = 'D'; 
            strcpy(tarifas[3]->codigo, "CT4"); 
            tarifas[3]->inicio.hora = 0; 
            tarifas[3]->inicio.minuto = 0;
            tarifas[3]->fim.hora = 0; 
            tarifas[3]->fim.minuto = 0; 
            tarifas[3]->valor = TARIFA_T4_PADRAO;  // 6.00€
            
        } else {
            // Opção 2: Configuração manual pelo utilizador
            adicionarLog(LOG_INFO, "Configuração manual de tarifas");
            
            for (int i = 0; i < 4; i++) {
                printf("Configuracao da Tarifa CT%d:\n", i+1);
                
                // Solicitar tipo de tarifa (H=Horária, D=Diária)
                do {
                    printf("Tipo (H=hora, D=dia): ");
                    scanf(" %c", &tarifas[i]->tipoTarifa);
                    tarifas[i]->tipoTarifa = toupper(tarifas[i]->tipoTarifa);
                } while (tarifas[i]->tipoTarifa != 'H' && tarifas[i]->tipoTarifa != 'D');

                // Atribuir código automático
                sprintf(tarifas[i]->codigo, "CT%d", i+1);

                // Solicitar horário de aplicação
                printf("Hora de inicio (HH MM): ");
                scanf("%d %d", &tarifas[i]->inicio.hora, &tarifas[i]->inicio.minuto);

                printf("Hora de fim (HH MM): ");
                scanf("%d %d", &tarifas[i]->fim.hora, &tarifas[i]->fim.minuto);

                // Solicitar valor
                printf("Valor (em euros, ex: 0.60): ");
                scanf("%f", &tarifas[i]->valor);
                
                // Validar valor durante configuração manual
                if (tarifas[i]->valor < 0) {
                    char mensagem[100];
                    sprintf(mensagem, "Tarifa %s configurada com valor negativo: %.2f", 
                            tarifas[i]->codigo, tarifas[i]->valor);
                    adicionarLog(LOG_ERROR, mensagem);
                }
            }
        }

        // Criar/sobrescrever ficheiro com as tarifas configuradas
        file = fopen("tarifas.txt", "w");
        if (file != NULL) {
            // Escrever cada tarifa no formato padrão
            for (int i = 0; i < 4; i++) {
                fprintf(file, "%c\t%s\t%02d:%02d\t%02d:%02d\t%.2f\n",
                        tarifas[i]->tipoTarifa,
                        tarifas[i]->codigo,
                        tarifas[i]->inicio.hora, tarifas[i]->inicio.minuto,
                        tarifas[i]->fim.hora, tarifas[i]->fim.minuto,
                        tarifas[i]->valor);
            }
            fclose(file);
            adicionarLog(LOG_INFO, "Tarifas gravadas em tarifas.txt");
            printf("Tarifas gravadas em tarifas.txt\n");
        } else {
            adicionarLog(LOG_ERROR, "Erro ao criar o ficheiro tarifas.txt");
            printf("Erro ao criar o ficheiro tarifas.txt\n");
        }
    }
}

// -----------------------------------------------------------------------------
// FUNÇÃO: alterarTarifas()
// -----------------------------------------------------------------------------

/**
 * Permite alterar os valores das tarifas do sistema.
 * Esta função implementa o requisito do enunciado que indica que
 * os valores das tarifas devem poder ser facilmente alterados no futuro.
 * 
 * Processamento:
 * 1. Mostra tarifas atuais
 * 2. Pergunta se deseja alterar
 * 3. Se sim, solicita novos valores para cada tarifa
 * 4. Valida entradas
 * 5. Confirma alterações
 * 6. Atualiza sistema global e ficheiro
 * 7. Regista alteração em log especializado
 * 
 * @note Preserva tipos e códigos das tarifas, altera apenas horários e valores.
 */
void alterarTarifas() {
    limparEcra();
    printf("=========================================\n");
    printf("           ALTERAR TARIFAS               \n");
    printf("=========================================\n");
    
    // Carregar tarifas atuais do ficheiro
    TarifasSistema presenteSistema;
    carregarTarifas(&presenteSistema);
    
    int opcao;
    
    do {
        // Mostrar tarifas atuais ao utilizador
        printf("\n--- TARIFAS ATUAIS ---\n");
        printf("1. Tarifa T1: Tipo: %c, Código: %s, Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
               presenteSistema.tarifaT1.tipoTarifa, presenteSistema.tarifaT1.codigo,
               presenteSistema.tarifaT1.inicio.hora, presenteSistema.tarifaT1.inicio.minuto,
               presenteSistema.tarifaT1.fim.hora, presenteSistema.tarifaT1.fim.minuto,
               presenteSistema.tarifaT1.valor);
        printf("2. Tarifa T2: Tipo: %c, Código: %s, Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
               presenteSistema.tarifaT2.tipoTarifa, presenteSistema.tarifaT2.codigo,
               presenteSistema.tarifaT2.inicio.hora, presenteSistema.tarifaT2.inicio.minuto,
               presenteSistema.tarifaT2.fim.hora, presenteSistema.tarifaT2.fim.minuto,
               presenteSistema.tarifaT2.valor);
        printf("3. Tarifa T3: Tipo: %c, Código: %s, Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
               presenteSistema.tarifaT3.tipoTarifa, presenteSistema.tarifaT3.codigo,
               presenteSistema.tarifaT3.inicio.hora, presenteSistema.tarifaT3.inicio.minuto,
               presenteSistema.tarifaT3.fim.hora, presenteSistema.tarifaT3.fim.minuto,
               presenteSistema.tarifaT3.valor);
        printf("4. Tarifa T4: Tipo: %c, Código: %s, Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
               presenteSistema.tarifaT4.tipoTarifa, presenteSistema.tarifaT4.codigo,
               presenteSistema.tarifaT4.inicio.hora, presenteSistema.tarifaT4.inicio.minuto,
               presenteSistema.tarifaT4.fim.hora, presenteSistema.tarifaT4.fim.minuto,
               presenteSistema.tarifaT4.valor);
        
        // Menu de escolha
        printf("\n--- ESCOLHER TARIFA A ALTERAR ---\n");
        printf("1 - Alterar Tarifa T1\n");
        printf("2 - Alterar Tarifa T2\n");
        printf("3 - Alterar Tarifa T3\n");
        printf("4 - Alterar Tarifa T4\n");
        printf("5 - Alterar TODAS as tarifas\n");
        printf("0 - Cancelar e voltar\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            
            // Limpar buffer de entrada
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar();
            return;
        }
        
        getchar(); // Limpar buffer
        
        if (opcao == 0) {
            printf("Operação cancelada.\n");
            printf("Pressione Enter para continuar...");
            getchar();
            return;
        }
        
        if (opcao < 1 || opcao > 5) {
            printf("Opção inválida!\n");
            printf("Pressione Enter para continuar...");
            getchar();
            continue;
        }
        
        // Coletar novas configurações - iniciamos com as atuais
        TarifasSistema novoSistema = presenteSistema;
        
        if (opcao == 1 || opcao == 5) {
            printf("\n--- CONFIGURAR TARIFA T1 ---\n");
            printf("Tipo atual: %c, Código: %s\n", 
                   presenteSistema.tarifaT1.tipoTarifa, presenteSistema.tarifaT1.codigo);
            
            // Solicitar hora de início com validação
            do {
                printf("Hora de início (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT1.inicio.hora, presenteSistema.tarifaT1.inicio.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT1.inicio.hora, &novoSistema.tarifaT1.inicio.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT1.inicio = presenteSistema.tarifaT1.inicio;
                    while (getchar() != '\n'); // Limpar buffer
                }
            } while (novoSistema.tarifaT1.inicio.hora < 0 || novoSistema.tarifaT1.inicio.hora > 23 || 
                     novoSistema.tarifaT1.inicio.minuto < 0 || novoSistema.tarifaT1.inicio.minuto > 59);
            
            // Solicitar hora de fim com validação
            do {
                printf("Hora de fim (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT1.fim.hora, presenteSistema.tarifaT1.fim.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT1.fim.hora, &novoSistema.tarifaT1.fim.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT1.fim = presenteSistema.tarifaT1.fim;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT1.fim.hora < 0 || novoSistema.tarifaT1.fim.hora > 23 || 
                     novoSistema.tarifaT1.fim.minuto < 0 || novoSistema.tarifaT1.fim.minuto > 59);
            
            // Solicitar valor
            printf("Valor (em euros) [atual: %.2f]: ", presenteSistema.tarifaT1.valor);
            if (scanf("%f", &novoSistema.tarifaT1.valor) != 1) {
                printf("Entrada inválida. Usando valor atual.\n");
                novoSistema.tarifaT1.valor = presenteSistema.tarifaT1.valor;
                while (getchar() != '\n');
            }
            getchar(); // Limpar buffer
        }
        
        if (opcao == 2 || opcao == 5) {
            printf("\n--- CONFIGURAR TARIFA T2 ---\n");
            printf("Tipo atual: %c, Código: %s\n", 
                   presenteSistema.tarifaT2.tipoTarifa, presenteSistema.tarifaT2.codigo);
            
            // Solicitar hora de início com validação
            do {
                printf("Hora de início (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT2.inicio.hora, presenteSistema.tarifaT2.inicio.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT2.inicio.hora, &novoSistema.tarifaT2.inicio.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT2.inicio = presenteSistema.tarifaT2.inicio;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT2.inicio.hora < 0 || novoSistema.tarifaT2.inicio.hora > 23 || 
                     novoSistema.tarifaT2.inicio.minuto < 0 || novoSistema.tarifaT2.inicio.minuto > 59);
            
            // Solicitar hora de fim com validação
            do {
                printf("Hora de fim (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT2.fim.hora, presenteSistema.tarifaT2.fim.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT2.fim.hora, &novoSistema.tarifaT2.fim.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT2.fim = presenteSistema.tarifaT2.fim;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT2.fim.hora < 0 || novoSistema.tarifaT2.fim.hora > 23 || 
                     novoSistema.tarifaT2.fim.minuto < 0 || novoSistema.tarifaT2.fim.minuto > 59);
            
            // Solicitar valor
            printf("Valor (em euros) [atual: %.2f]: ", presenteSistema.tarifaT2.valor);
            if (scanf("%f", &novoSistema.tarifaT2.valor) != 1) {
                printf("Entrada inválida. Usando valor atual.\n");
                novoSistema.tarifaT2.valor = presenteSistema.tarifaT2.valor;
                while (getchar() != '\n');
            }
            getchar(); // Limpar buffer
        }
        
        if (opcao == 3 || opcao == 5) {
            printf("\n--- CONFIGURAR TARIFA T3 ---\n");
            printf("Tipo atual: %c, Código: %s\n", 
                   presenteSistema.tarifaT3.tipoTarifa, presenteSistema.tarifaT3.codigo);
            
            // Solicitar hora de início com validação
            do {
                printf("Hora de início (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT3.inicio.hora, presenteSistema.tarifaT3.inicio.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT3.inicio.hora, &novoSistema.tarifaT3.inicio.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT3.inicio = presenteSistema.tarifaT3.inicio;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT3.inicio.hora < 0 || novoSistema.tarifaT3.inicio.hora > 23 || 
                     novoSistema.tarifaT3.inicio.minuto < 0 || novoSistema.tarifaT3.inicio.minuto > 59);
            
            // Solicitar hora de fim com validação
            do {
                printf("Hora de fim (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT3.fim.hora, presenteSistema.tarifaT3.fim.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT3.fim.hora, &novoSistema.tarifaT3.fim.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT3.fim = presenteSistema.tarifaT3.fim;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT3.fim.hora < 0 || novoSistema.tarifaT3.fim.hora > 23 || 
                     novoSistema.tarifaT3.fim.minuto < 0 || novoSistema.tarifaT3.fim.minuto > 59);
            
            // Solicitar valor
            printf("Valor (em euros) [atual: %.2f]: ", presenteSistema.tarifaT3.valor);
            if (scanf("%f", &novoSistema.tarifaT3.valor) != 1) {
                printf("Entrada inválida. Usando valor atual.\n");
                novoSistema.tarifaT3.valor = presenteSistema.tarifaT3.valor;
                while (getchar() != '\n');
            }
            getchar(); // Limpar buffer
        }
        
        if (opcao == 4 || opcao == 5) {
            printf("\n--- CONFIGURAR TARIFA T4 ---\n");
            printf("Tipo atual: %c, Código: %s\n", 
                   presenteSistema.tarifaT4.tipoTarifa, presenteSistema.tarifaT4.codigo);
            
            // Solicitar hora de início com validação
            do {
                printf("Hora de início (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT4.inicio.hora, presenteSistema.tarifaT4.inicio.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT4.inicio.hora, &novoSistema.tarifaT4.inicio.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT4.inicio = presenteSistema.tarifaT4.inicio;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT4.inicio.hora < 0 || novoSistema.tarifaT4.inicio.hora > 23 || 
                     novoSistema.tarifaT4.inicio.minuto < 0 || novoSistema.tarifaT4.inicio.minuto > 59);
            
            // Solicitar hora de fim com validação
            do {
                printf("Hora de fim (HH MM) [atual: %02d:%02d]: ", 
                       presenteSistema.tarifaT4.fim.hora, presenteSistema.tarifaT4.fim.minuto);
                if (scanf("%d %d", &novoSistema.tarifaT4.fim.hora, &novoSistema.tarifaT4.fim.minuto) != 2) {
                    printf("Entrada inválida. Usando valor atual.\n");
                    novoSistema.tarifaT4.fim = presenteSistema.tarifaT4.fim;
                    while (getchar() != '\n');
                }
            } while (novoSistema.tarifaT4.fim.hora < 0 || novoSistema.tarifaT4.fim.hora > 23 || 
                     novoSistema.tarifaT4.fim.minuto < 0 || novoSistema.tarifaT4.fim.minuto > 59);
            
            // Solicitar valor
            printf("Valor (em euros) [atual: %.2f]: ", presenteSistema.tarifaT4.valor);
            if (scanf("%f", &novoSistema.tarifaT4.valor) != 1) {
                printf("Entrada inválida. Usando valor atual.\n");
                novoSistema.tarifaT4.valor = presenteSistema.tarifaT4.valor;
                while (getchar() != '\n');
            }
            getchar(); // Limpar buffer
        }
        
        // Confirmar alterações
        printf("\n=========================================\n");
        printf("           CONFIRMAR ALTERAÇÕES          \n");
        printf("=========================================\n");
        printf("Nova configuração de tarifas:\n");
        
        if (opcao == 1 || opcao == 5) {
            printf("1. Tarifa T1: Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
                   novoSistema.tarifaT1.inicio.hora, novoSistema.tarifaT1.inicio.minuto,
                   novoSistema.tarifaT1.fim.hora, novoSistema.tarifaT1.fim.minuto,
                   novoSistema.tarifaT1.valor);
        }
        if (opcao == 2 || opcao == 5) {
            printf("2. Tarifa T2: Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
                   novoSistema.tarifaT2.inicio.hora, novoSistema.tarifaT2.inicio.minuto,
                   novoSistema.tarifaT2.fim.hora, novoSistema.tarifaT2.fim.minuto,
                   novoSistema.tarifaT2.valor);
        }
        if (opcao == 3 || opcao == 5) {
            printf("3. Tarifa T3: Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
                   novoSistema.tarifaT3.inicio.hora, novoSistema.tarifaT3.inicio.minuto,
                   novoSistema.tarifaT3.fim.hora, novoSistema.tarifaT3.fim.minuto,
                   novoSistema.tarifaT3.valor);
        }
        if (opcao == 4 || opcao == 5) {
            printf("4. Tarifa T4: Início: %02d:%02d, Fim: %02d:%02d, Valor: %.2f€\n",
                   novoSistema.tarifaT4.inicio.hora, novoSistema.tarifaT4.inicio.minuto,
                   novoSistema.tarifaT4.fim.hora, novoSistema.tarifaT4.fim.minuto,
                   novoSistema.tarifaT4.valor);
        }
        
        // Solicitar confirmação final
        printf("\nConfirmar alterações? (S/N): ");
        char confirmar;
        scanf(" %c", &confirmar);
        getchar();  // Limpar newline
        
        if (toupper(confirmar) != 'S') {
            printf("Alterações canceladas.\n");
            printf("Pressione Enter para continuar...");
            getchar();
            return;
        }
        
        // Atualizar sistema global apenas com as alterações feitas
        if (opcao == 1 || opcao == 5) {
            sistema.tarifaT1 = novoSistema.tarifaT1;
        }
        if (opcao == 2 || opcao == 5) {
            sistema.tarifaT2 = novoSistema.tarifaT2;
        }
        if (opcao == 3 || opcao == 5) {
            sistema.tarifaT3 = novoSistema.tarifaT3;
        }
        if (opcao == 4 || opcao == 5) {
            sistema.tarifaT4 = novoSistema.tarifaT4;
        }
        
        // Salvar no ficheiro de texto
        FILE *fT = fopen("tarifas.txt", "w");
        if (fT != NULL) {
            fprintf(fT, "%c\t%s\t%02d:%02d\t%02d:%02d\t%.2f\n",
                    sistema.tarifaT1.tipoTarifa, sistema.tarifaT1.codigo,
                    sistema.tarifaT1.inicio.hora, sistema.tarifaT1.inicio.minuto,
                    sistema.tarifaT1.fim.hora, sistema.tarifaT1.fim.minuto,
                    sistema.tarifaT1.valor);
            fprintf(fT, "%c\t%s\t%02d:%02d\t%02d:%02d\t%.2f\n",
                    sistema.tarifaT2.tipoTarifa, sistema.tarifaT2.codigo,
                    sistema.tarifaT2.inicio.hora, sistema.tarifaT2.inicio.minuto,
                    sistema.tarifaT2.fim.hora, sistema.tarifaT2.fim.minuto,
                    sistema.tarifaT2.valor);
            fprintf(fT, "%c\t%s\t%02d:%02d\t%02d:%02d\t%.2f\n",
                    sistema.tarifaT3.tipoTarifa, sistema.tarifaT3.codigo,
                    sistema.tarifaT3.inicio.hora, sistema.tarifaT3.inicio.minuto,
                    sistema.tarifaT3.fim.hora, sistema.tarifaT3.fim.minuto,
                    sistema.tarifaT3.valor);
            fprintf(fT, "%c\t%s\t%02d:%02d\t%02d:%02d\t%.2f\n",
                    sistema.tarifaT4.tipoTarifa, sistema.tarifaT4.codigo,
                    sistema.tarifaT4.inicio.hora, sistema.tarifaT4.inicio.minuto,
                    sistema.tarifaT4.fim.hora, sistema.tarifaT4.fim.minuto,
                    sistema.tarifaT4.valor);
            fclose(fT);
            
            // Registrar a alteração em log específico
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            
            FILE *logFile = fopen("alteracoes_tarifas.log", "a");
            if (logFile != NULL) {
                fprintf(logFile, "%04d-%02d-%02d %02d:%02d - Tarifas alteradas (opção %d)\n",
                        tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                        tm_info->tm_hour, tm_info->tm_min, opcao);
                fclose(logFile);
            }
            
            printf("\nTarifas atualizadas com sucesso!\n");
            printf("Alteração registrada em 'alteracoes_tarifas.log'\n");
            
        } else {
            printf("Erro ao criar o ficheiro tarifas.txt\n");
            adicionarLog(LOG_ERROR, "Falha ao salvar tarifas alteradas");
        }
        
        printf("\nPressione Enter para continuar...");
        getchar();
        return;
        
    } while (opcao != 0);
}

// -----------------------------------------------------------------------------
// FUNÇÃO: salvarTarifasBinario()
// -----------------------------------------------------------------------------

/**
 * Salva as tarifas atuais em formato binário para persistência.
 * Esta função implementa parte da funcionalidade 9 do enunciado,
 * permitindo que as tarifas configuradas sejam mantidas entre
 * execuções da aplicação de forma eficiente.
 * 
 * @param sistema Ponteiro para a estrutura TarifasSistema a ser salva
 * @return 1 se a operação foi bem sucedida, 0 em caso de erro
 * 
 * @note O ficheiro binário permite recuperação rápida dos dados.
 * @note Complementa o ficheiro texto que é mais fácil de editar manualmente.
 */
int salvarTarifasBinario(TarifasSistema *sistema) {
    // Abrir ficheiro binário para escrita
    FILE *file = fopen("tarifas.dat", "wb");
    if (file == NULL) {
        adicionarLog(LOG_ERROR, "Erro ao criar ficheiro binário de tarifas");
        return 0;  // Falha
    }
    
    // Escrever estrutura completa de uma vez
    size_t escritos = fwrite(sistema, sizeof(TarifasSistema), 1, file);
    fclose(file);
    
    if (escritos == 1) {
        adicionarLog(LOG_INFO, "Tarifas salvas em formato binário");
        return 1;  // Sucesso
    } else {
        adicionarLog(LOG_ERROR, "Erro ao escrever tarifas em binário");
        return 0;  // Falha
    }
}