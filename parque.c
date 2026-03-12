// parque.c - Módulo de Gestão do Parque de Estacionamento
// Autor: [Seu Nome]
// Data: [Data]
// Descrição: Contém funções para configuração, inicialização e gestão
//           do parque de estacionamento utilizando memória dinâmica.
//           Implementa funcionalidades 1-a, 3, 7, 8 do enunciado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "parque.h"
#include "datastructs.h"
#include "utils.h"
#include "estacionamentos.h"
#include "logs.h"

// -----------------------------------------------------------------------------
// DECLARAÇÕES EXTERNAS DE VARIÁVEIS GLOBAIS
// -----------------------------------------------------------------------------

/**
 * Estrutura dinâmica do parque de estacionamento.
 * Declarada externamente para ser partilhada entre módulos.
 */
extern ParqueDinamico parqueDinamico; 

/**
 * Lista dinâmica de estacionamentos.
 * Usada para verificar ocupação do parque.
 */
extern ListaEstacionamentos listaEstacionamentos;

// -----------------------------------------------------------------------------
// FUNÇÃO: alocarParque()
// -----------------------------------------------------------------------------

/**
 * Aloca memória para a estrutura dinâmica do parque (matriz 3D).
 * Esta função cria uma matriz tridimensional para representar:
 * - Pisos (primeira dimensão)
 * - Filas por piso (segunda dimensão)
 * - Lugares por fila (terceira dimensão)
 * 
 * @param pisos Número de pisos a alocar (deve ser > 0)
 * @param filas Número de filas por piso a alocar (deve ser > 0)
 * @param lugares Número de lugares por fila a alocar (deve ser > 0)
 * 
 * @note Liberta memória anterior se existir (chama libertarParque()).
 * @note Valida dimensões antes de alocar.
 * @note Regista operação no sistema de logs.
 */
void alocarParque(int pisos, int filas, int lugares) {
    // 1. Libertar memória anterior (se existir)
    libertarParque();
    
    // 2. Validar dimensões
    if (pisos <= 0 || filas <= 0 || lugares <= 0) {
        printf("Dimensões inválidas para alocação do parque!\n");
        adicionarLog(LOG_ERROR, "Dimensões inválidas para alocação do parque");
        return;
    }
    
    // 3. Alocar memória para a matriz 3D (pisos)
    parqueDinamico.parque = (Lugar ***)malloc(pisos * sizeof(Lugar **));
    if (!parqueDinamico.parque) {
        printf("Erro ao alocar memória para pisos!\n");
        adicionarLog(LOG_ERROR, "Erro de alocação de memória para pisos");
        return;
    }
    
    // 4. Alocar memória para cada piso (filas)
    for (int p = 0; p < pisos; p++) {
        parqueDinamico.parque[p] = (Lugar **)malloc(filas * sizeof(Lugar *));
        if (!parqueDinamico.parque[p]) {
            printf("Erro ao alocar memória para filas no piso %d!\n", p+1);
            adicionarLog(LOG_ERROR, "Erro de alocação de memória para filas");
            
            // Libertar memória já alocada em caso de erro (cleanup)
            for (int i = 0; i < p; i++) {
                if (parqueDinamico.parque[i]) {
                    free(parqueDinamico.parque[i]);
                }
            }
            free(parqueDinamico.parque);
            parqueDinamico.parque = NULL;
            return;
        }
        
        // Inicializar ponteiros como NULL (para detetar erros mais facilmente)
        for (int f = 0; f < filas; f++) {
            parqueDinamico.parque[p][f] = NULL;
        }
    }
    
    // 5. Alocar memória para cada fila (lugares)
    for (int p = 0; p < pisos; p++) {
        for (int f = 0; f < filas; f++) {
            parqueDinamico.parque[p][f] = (Lugar *)malloc(lugares * sizeof(Lugar));
            if (!parqueDinamico.parque[p][f]) {
                printf("Erro ao alocar memória para lugares no piso %d, fila %d!\n", p+1, f+1);
                adicionarLog(LOG_ERROR, "Erro de alocação de memória para lugares");
                
                // Libertar memória já alocada em caso de erro (cleanup completo)
                for (int i = 0; i <= p; i++) {
                    if (parqueDinamico.parque[i]) {
                        for (int j = 0; j < (i == p ? f : filas); j++) {
                            if (parqueDinamico.parque[i][j]) {
                                free(parqueDinamico.parque[i][j]);
                            }
                        }
                        free(parqueDinamico.parque[i]);
                    }
                }
                free(parqueDinamico.parque);
                parqueDinamico.parque = NULL;
                return;
            }
            // 5. Alocar memória para cada lugare E INICIALIZAR
            for (int l = 0; l < lugares; l++) {
                // Estado inicial: livre
                parqueDinamico.parque[p][f][l].estado = 'L';
                
                // Gerar código do lugar: Piso + LetraFila + NúmeroLugar
                sprintf(parqueDinamico.parque[p][f][l].codigo, "%d%c%02d", 
                        p+1, 'A' + f, l+1);
                
                // Inicializar motivo (espaço = sem motivo)
                parqueDinamico.parque[p][f][l].motivo = ' ';
            }
        }
    }
        
    
    
    // 6. Atualizar dimensões na estrutura global
    parqueDinamico.numPisos = pisos;
    parqueDinamico.numFilasPorPiso = filas;
    parqueDinamico.numLugaresPorFila = lugares;
    
    // 7. Registar operação no log
    char msg[100];
    snprintf(msg, sizeof(msg), "Parque alocado: %d pisos, %d filas, %d lugares", 
             pisos, filas, lugares);
    adicionarLog(LOG_INFO, msg);
}

// -----------------------------------------------------------------------------
// FUNÇÃO: libertarParque()
// -----------------------------------------------------------------------------

/**
 * Liberta toda a memória alocada para a estrutura do parque.
 * Esta função liberta recursivamente todos os níveis da matriz 3D:
 * 1. Lugares (terceira dimensão)
 * 2. Filas (segunda dimensão)
 * 3. Pisos (primeira dimensão)
 * 
 * @note Previne memory leaks ao libertar todos os recursos alocados.
 * @note Define ponteiros como NULL após libertação.
 * @note Regista operação no sistema de logs.
 */
void libertarParque() {
    if (parqueDinamico.parque) {
        // Libertar memória de forma hierárquica (de dentro para fora)
        
        // 1. Libertar lugares (nível mais interno)
        for (int p = 0; p < parqueDinamico.numPisos; p++) {
            if (parqueDinamico.parque[p]) {
                for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
                    if (parqueDinamico.parque[p][f]) {
                        free(parqueDinamico.parque[p][f]);
                        parqueDinamico.parque[p][f] = NULL;
                    }
                }
                
                // 2. Libertar filas
                free(parqueDinamico.parque[p]);
                parqueDinamico.parque[p] = NULL;
            }
        }
        
        // 3. Libertar pisos
        free(parqueDinamico.parque);
        parqueDinamico.parque = NULL;
        
        // 4. Resetar dimensões
        parqueDinamico.numPisos = 0;
        parqueDinamico.numFilasPorPiso = 0;
        parqueDinamico.numLugaresPorFila = 0;
        
        // 5. Registar no log
        adicionarLog(LOG_INFO, "Memória do parque libertada");
    }
}

// -----------------------------------------------------------------------------
// FUNÇÃO: redimensionarParque()
// -----------------------------------------------------------------------------

/**
 * Redimensiona a estrutura do parque mantendo dados existentes quando possível.
 * Esta função permite alterar as dimensões do parque após a criação inicial.
 * 
 * @param novosPisos Novo número de pisos
 * @param novasFilas Novo número de filas por piso
 * @param novosLugares Novo número de lugares por fila
 * 
 * @note Se as novas dimensões são maiores, inicializa os novos lugares como livres.
 * @note Se as novas dimensões são menores, perde os dados dos lugares removidos.
 * @note Preserva dados existentes na interseção das dimensões antigas e novas.
 * @note Valida as novas dimensões antes de proceder.
 */
void redimensionarParque(int novosPisos, int novasFilas, int novosLugares) {
    // 1. Validar novas dimensões
    if (novosPisos <= 0 || novasFilas <= 0 || novosLugares <= 0) {
        printf("Dimensões inválidas para redimensionamento!\n");
        return;
    }
    
    // 2. Criar nova matriz 3D com as dimensões solicitadas
    Lugar ***novoParque = (Lugar ***)malloc(novosPisos * sizeof(Lugar **));
    if (!novoParque) {
        printf("Erro ao alocar memória para novos pisos!\n");
        return;
    }
    
    // 3. Inicializar nova estrutura
    for (int p = 0; p < novosPisos; p++) {
        novoParque[p] = (Lugar **)malloc(novasFilas * sizeof(Lugar *));
        if (!novoParque[p]) {
            printf("Erro ao alocar memória para novas filas!\n");
            
            // Cleanup: libertar o que já foi alocado
            for (int i = 0; i < p; i++) {
                free(novoParque[i]);
            }
            free(novoParque);
            return;
        }
        
        // 4. Alocar e inicializar lugares
        for (int f = 0; f < novasFilas; f++) {
            novoParque[p][f] = (Lugar *)malloc(novosLugares * sizeof(Lugar));
            if (!novoParque[p][f]) {
                printf("Erro ao alocar memória para novos lugares!\n");
                
                // Cleanup completo
                for (int i = 0; i <= p; i++) {
                    for (int j = 0; j < (i == p ? f : novasFilas); j++) {
                        free(novoParque[i][j]);
                    }
                    free(novoParque[i]);
                }
                free(novoParque);
                return;
            }
            
            // 5. Inicializar ou copiar dados de lugares
            for (int l = 0; l < novosLugares; l++) {
                if (p < parqueDinamico.numPisos && 
                    f < parqueDinamico.numFilasPorPiso && 
                    l < parqueDinamico.numLugaresPorFila) {
                    // Caso A: Lugar existe nas dimensões antigas - copiar dados
                    novoParque[p][f][l] = parqueDinamico.parque[p][f][l];
                } else {
                    // Caso B: Novo lugar - inicializar como livre
                    novoParque[p][f][l].estado = 'L'; // Livre
                    
                    // Gerar código do lugar: Piso + LetraFila + NúmeroLugar
                    sprintf(novoParque[p][f][l].codigo, "%d%c%02d", 
                            p+1, 'A' + f, l+1);
                    novoParque[p][f][l].motivo = ' '; // Sem motivo (não indisponível)
                }
            }
        }
    }
    
    // 6. Libertar matriz antiga
    libertarParque();
    
    // 7. Atualizar estrutura global com nova matriz
    parqueDinamico.parque = novoParque;
    parqueDinamico.numPisos = novosPisos;
    parqueDinamico.numFilasPorPiso = novasFilas;
    parqueDinamico.numLugaresPorFila = novosLugares;
    
    // 8. Registar operação no log
    char msg[150];
    snprintf(msg, sizeof(msg), "Parque redimensionado para: %d pisos, %d filas, %d lugares", 
             novosPisos, novasFilas, novosLugares);
    adicionarLog(LOG_INFO, msg);
}

// -----------------------------------------------------------------------------
// FUNÇÃO: configurarParque()
// -----------------------------------------------------------------------------

/**
 * Configura os parâmetros do parque de estacionamento.
 * Esta função é chamada durante a "fase de instalação" da aplicação
 * (funcionalidade 1-a do enunciado):
 * - Permite especificar o número de pisos (com limites)
 * - Permite especificar o número de filas por piso (com limites)
 * - Permite especificar o número de lugares por fila (com limites)
 * 
 * @note Valida todas as entradas do utilizador.
 * @note Inicializa todos os lugares como livres com códigos apropriados.
 * @note Salva a configuração em ficheiro binário.
 * @note Regista a operação no sistema de logs.
 */
void configurarParque() {
    limparEcra();
    printf("=========================================\n");
    printf("      CONFIGURAÇÃO DO PARQUE             \n");
    printf("=========================================\n");
    
    // Registar início da configuração
    adicionarLog(LOG_INFO, "Iniciando configuração do parque");
    
    // Variáveis para armazenar configuração
    int pisos, filas, lugares;
    
    // 1. Solicitar e validar número de pisos
    do {
        printf("Número de pisos (1-5): ");
        if (scanf("%d", &pisos) != 1) {
            printf("Entrada inválida!\n");
            
            // Limpar buffer de entrada
            while (getchar() != '\n');
            continue;
        }
        getchar(); // Limpar newline
    } while (pisos < 1 || pisos > 5);

    // 2. Solicitar e validar número de filas por piso
    do {
        printf("Quantas filas por piso (1-26)? ");
        if (scanf("%d", &filas) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();
    } while (filas < 1 || filas > 26);

    // 3. Solicitar e validar número de lugares por fila
    do {
        printf("Quantos lugares por fila (1-50)? ");
        if (scanf("%d", &lugares) != 1) {
            printf("Entrada inválida!\n");
            while (getchar() != '\n');
            continue;
        }
        getchar();
    } while (lugares < 1 || lugares > 50);

    // 4. Alocar memória dinamicamente para o parque
    alocarParque(pisos, filas, lugares);
    
    // 5. Verificar se a alocação foi bem sucedida
    if (!parqueDinamico.parque) {
        printf("Erro ao configurar o parque!\n");
        return;
    }

    // 6. Inicializar todos os lugares como livres
    for (int p = 0; p < pisos; p++) {
        for (int f = 0; f < filas; f++) {
            for (int l = 0; l < lugares; l++) {
                // Estado inicial: livre
                parqueDinamico.parque[p][f][l].estado = 'L';
                
                // Gerar código único do lugar: Piso + LetraFila + NúmeroLugar
                // Exemplo: Piso 1, Fila A (0), Lugar 1 → "1A01"
                sprintf(parqueDinamico.parque[p][f][l].codigo, "%d%c%02d", 
                        p+1, 'A' + f, l+1);
                
                // Inicializar motivo (espaço = sem motivo)
                parqueDinamico.parque[p][f][l].motivo = ' ';
            }
        }
    }
    
    // 7. Guardar configuração no ficheiro binário
    salvarParque();
    
    // 8. Registar configuração no log
    char msg[150];
    snprintf(msg, sizeof(msg), "Parque configurado: %d pisos, %d filas/piso, %d lugares/fila", 
            pisos, filas, lugares);
    adicionarLog(LOG_INFO, msg);
    
    // 9. Mensagem de confirmação ao utilizador
    printf("\nParque configurado e guardado com sucesso!\n");
    printf("Pressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: inicializarParque()
// -----------------------------------------------------------------------------

/**
 * Inicializa a estrutura do parque a partir do ficheiro binário ou cria novo.
 * Esta função é chamada no arranque da aplicação:
 * 1. Tenta carregar configuração do ficheiro "parque.dat"
 * 2. Se o ficheiro existe e é válido, carrega dados
 * 3. Se o ficheiro não existe ou é inválido, chama configurarParque()
 * 
 * @note Implementa parte da funcionalidade 1-b do enunciado (carregar dados).
 * @note Valida integridade dos dados lidos do ficheiro.
 * @note Regista todas as operações no sistema de logs.
 */
void inicializarParque() {
    // 1. Tentar abrir ficheiro binário existente
    FILE *file = fopen("parque.dat", "rb");
    
    if (file != NULL) {
        // Ficheiro existe - tentar carregar dados
        
        // 2. Ler dimensões do parque do ficheiro
        int pisos, filas, lugares;
        
        if (fread(&pisos, sizeof(int), 1, file) != 1 ||
            fread(&filas, sizeof(int), 1, file) != 1 ||
            fread(&lugares, sizeof(int), 1, file) != 1) {
            
            // Erro ao ler dimensões - ficheiro corrompido
            fclose(file);
            
            adicionarLog(LOG_WARNING, "Ficheiro de parque inválido. Configurando novo parque...");
            printf("Ficheiro de parque inválido. Configurando novo parque...\n");
            
            // Configurar novo parque
            configurarParque();
            
        } else {
            // Dimensões lidas com sucesso - alocar memória
            alocarParque(pisos, filas, lugares);
            
            // Verificar alocação bem sucedida
            if (!parqueDinamico.parque) {
                fclose(file);
                adicionarLog(LOG_ERROR, "Erro ao alocar memória para o parque");
                return;
            }
            
            // 3. Ler dados do parque (lugares individuais)
            size_t elementosLidos = 0;
            
            for (int p = 0; p < pisos; p++) {
                for (int f = 0; f < filas; f++) {
                    // Ler todos os lugares desta fila de uma vez
                    size_t lidos = fread(parqueDinamico.parque[p][f], 
                                        sizeof(Lugar), lugares, file);
                    elementosLidos += lidos;
                    
                    // Verificar se leu o número correto de lugares
                    if (lidos != (size_t)lugares) {
                        printf("Erro ao ler dados do piso %d, fila %d\n", p+1, f+1);
                    }
                }
            }
            
            fclose(file);
            
            // 4. Verificar integridade dos dados lidos
            if (elementosLidos == (size_t)(pisos * filas * lugares)) {
                // Dados carregados com sucesso
                char msg[150];
                snprintf(msg, sizeof(msg), "Parque carregado: %d pisos, %d filas/piso, %d lugares/fila", 
                        pisos, filas, lugares);
                adicionarLog(LOG_INFO, msg);
                
                printf("Parque carregado com sucesso:\n");
                printf("  Pisos: %d\n", pisos);
                printf("  Filas por piso: %d\n", filas);
                printf("  Lugares por fila: %d\n", lugares);
                printf("  Total de lugares: %d\n\n", pisos * filas * lugares);
                
            } else {
                // Erro na leitura dos dados
                adicionarLog(LOG_ERROR, "Erro ao ler dados do parque. Configurando novo parque...");
                printf("Erro ao ler dados do parque. Configurando novo parque...\n");
                
                // Configurar novo parque
                configurarParque();
            }
        }
        
    } else {
        // Ficheiro não existe - configuração inicial necessária
        adicionarLog(LOG_INFO, "Ficheiro de parque não encontrado. Configurando novo parque...");
        printf("Ficheiro de parque não encontrado. Configurando novo parque...\n");
        
        // Configurar novo parque
        configurarParque();
    }
}

// Continuação do ficheiro parque.c

// -----------------------------------------------------------------------------
// FUNÇÃO: salvarParque()
// -----------------------------------------------------------------------------

/**
 * Salva a configuração atual do parque para ficheiro binário.
 * Esta função implementa parte da funcionalidade 9 do enunciado:
 * - Persiste os parâmetros do parque (pisos, filas, lugares)
 * - Salva o estado atual de cada lugar (ocupado, livre, indisponível)
 * - Para que possam ser recuperados em futuras execuções da aplicação
 * 
 * @note Os dados são guardados em formato binário para eficiência.
 * @note Estrutura do ficheiro:
 *       1. Três inteiros: numPisos, numFilasPorPiso, numLugaresPorFila
 *       2. Matriz de estruturas Lugar (pisos × filas × lugares)
 * @note Regista operação no sistema de logs.
 */
void salvarParque() {
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        adicionarLog(LOG_WARNING, "Tentativa de salvar parque não configurado");
        return;
    }
    
    // 2. Abrir ficheiro binário para escrita
    FILE *file = fopen("parque.dat", "wb");
    if (file != NULL) {
        // 3. Salvar dimensões do parque (cabeçalho)
        fwrite(&parqueDinamico.numPisos, sizeof(int), 1, file);
        fwrite(&parqueDinamico.numFilasPorPiso, sizeof(int), 1, file);
        fwrite(&parqueDinamico.numLugaresPorFila, sizeof(int), 1, file);
        
        // 4. Salvar estado de cada lugar (dados)
        for (int p = 0; p < parqueDinamico.numPisos; p++) {
            for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
                // Escrever todos os lugares desta fila de uma vez
                fwrite(parqueDinamico.parque[p][f], sizeof(Lugar), 
                       parqueDinamico.numLugaresPorFila, file);
            }
        }
        
        fclose(file);
        adicionarLog(LOG_INFO, "Parque salvo com sucesso");
    } else {
        adicionarLog(LOG_ERROR, "Erro ao salvar o parque");
        printf("Erro ao salvar o parque.\n");
    }
}

// -----------------------------------------------------------------------------
// FUNÇÃO: alterarParque()
// -----------------------------------------------------------------------------

/**
 * Altera a configuração do parque após a instalação inicial.
 * Permite ajustar os parâmetros do parque sem perder dados existentes
 * (quando possível).
 * 
 * @note Requer validação cuidadosa para não afetar lugares ocupados.
 * @note Pode requerer redistribuição de veículos estacionados.
 * @note Oferece menu completo com múltiplas opções de alteração.
 * @note Inclui pré-visualização e confirmação de alterações.
 * @note Regista todas as operações em logs específicos.
 */
void alterarParque() {
    limparEcra();
    printf("=========================================\n");
    printf("      ALTERAR CONFIGURAÇÃO DO PARQUE     \n");
    printf("=========================================\n");
    
    // Registar início da operação
    adicionarLog(LOG_INFO, "Iniciando alteração da configuração do parque");
    
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado. Use a opção de configuração primeiro.\n");
        adicionarLog(LOG_WARNING, "Tentativa de alterar parque não configurado");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 2. Mostrar configuração atual
    printf("CONFIGURAÇÃO ATUAL DO PARQUE:\n");
    printf("==============================\n");
    printf("Número de pisos: %d\n", parqueDinamico.numPisos);
    printf("Filas por piso: %d\n", parqueDinamico.numFilasPorPiso);
    printf("Lugares por fila: %d\n", parqueDinamico.numLugaresPorFila);
    printf("Capacidade total: %d lugares\n\n", 
           parqueDinamico.numPisos * parqueDinamico.numFilasPorPiso * parqueDinamico.numLugaresPorFila);
    
    // 3. Mostrar estado atual de ocupação
    int lugaresLivres = 0, lugaresOcupados = 0, lugaresIndisponiveis = 0;
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                switch (parqueDinamico.parque[p][f][l].estado) {
                    case 'L': // Livre
                        lugaresLivres++;
                        break;
                    case 'O': // Ocupado
                        lugaresOcupados++;
                        break;
                    case 'I': // Indisponível
                    default:  // Outros estados
                        lugaresIndisponiveis++;
                        break;
                }
            }
        }
    }
    
    printf("ESTADO ATUAL DO PARQUE:\n");
    printf("========================\n");
    printf("Lugares livres: %d\n", lugaresLivres);
    printf("Lugares ocupados: %d\n", lugaresOcupados);
    printf("Lugares indisponíveis: %d\n\n", lugaresIndisponiveis);
    
    // 4. Verificar se há veículos ativos (ocupando lugares)
    int veiculosAtivos = 0;
    if (listaEstacionamentos.estacionamentos) {
        for (int i = 0; i < listaEstacionamentos.total; i++) {
            if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
                veiculosAtivos++;
            }
        }
    }
    
    // 5. Avisar sobre veículos ativos (se existirem)
    if (veiculosAtivos > 0) {
        printf("ATENÇÃO: Existem %d veículo(s) ativo(s) no parque!\n", veiculosAtivos);
        printf("Alterar o parque pode causar perda de dados.\n\n");
        
        char msg[100];
        snprintf(msg, sizeof(msg), "Alteração do parque com %d veículos ativos", veiculosAtivos);
        adicionarLog(LOG_WARNING, msg);
    }
    
    // 6. Perguntar se deseja prosseguir com a alteração
    printf("Deseja alterar as dimensões do parque? (S/N): ");
    char resposta;
    if (scanf(" %c", &resposta) != 1) {
        printf("Entrada inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    if (toupper(resposta) != 'S') {
        printf("Operação cancelada.\n");
        adicionarLog(LOG_INFO, "Alteração do parque cancelada pelo utilizador");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 7. Menu de opções de alteração
    int opcao;
    do {
        limparEcra();
        printf("=========================================\n");
        printf("      MENU DE ALTERAÇÃO DO PARQUE        \n");
        printf("=========================================\n");
        printf("1 - Alterar número de pisos\n");
        printf("2 - Alterar número de filas por piso\n");
        printf("3 - Alterar número de lugares por fila\n");
        printf("4 - Alterar todas as dimensões\n");
        printf("5 - Ver pré-visualização das alterações\n");
        printf("0 - Voltar (cancelar)\n");
        printf("=========================================\n");
        printf("Escolha: ");
        
        if (scanf("%d", &opcao) != 1) {
            printf("Opção inválida!\n");
            adicionarLog(LOG_WARNING, "Opção inválida no menu de alteração do parque");
            
            // Limpar buffer de entrada
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar();
            continue;
        }
        getchar();
        
        // Valores propostos para as novas dimensões
        int novosPisos = parqueDinamico.numPisos;
        int novasFilas = parqueDinamico.numFilasPorPiso;
        int novosLugares = parqueDinamico.numLugaresPorFila;
        
        // 8. Processar opção escolhida
        switch (opcao) {
            case 1:
                // Alterar apenas número de pisos
                printf("\nALTERAR NÚMERO DE PISOS\n");
                printf("Atual: %d pisos\n", parqueDinamico.numPisos);
                
                do {
                    printf("Novo número de pisos (1-5): ");
                    if (scanf("%d", &novosPisos) != 1) {
                        printf("Entrada inválida!\n");
                        while (getchar() != '\n'); // Limpar buffer
                        novosPisos = parqueDinamico.numPisos; // Manter valor atual
                    }
                    getchar();
                } while (novosPisos < 1 || novosPisos > 5); // Validar limites
                break;
                
            case 2:
                // Alterar apenas número de filas por piso
                printf("\nALTERAR NÚMERO DE FILAS POR PISO\n");
                printf("Atual: %d filas por piso\n", parqueDinamico.numFilasPorPiso);
                
                do {
                    printf("Novo número de filas (1-26): ");
                    if (scanf("%d", &novasFilas) != 1) {
                        printf("Entrada inválida!\n");
                        while (getchar() != '\n');
                        novasFilas = parqueDinamico.numFilasPorPiso;
                    }
                    getchar();
                } while (novasFilas < 1 || novasFilas > 26); // Máximo 26 filas (A-Z)
                break;
                
            case 3:
                // Alterar apenas número de lugares por fila
                printf("\nALTERAR NÚMERO DE LUGARES POR FILA\n");
                printf("Atual: %d lugares por fila\n", parqueDinamico.numLugaresPorFila);
                
                do {
                    printf("Novo número de lugares (1-50): ");
                    if (scanf("%d", &novosLugares) != 1) {
                        printf("Entrada inválida!\n");
                        while (getchar() != '\n');
                        novosLugares = parqueDinamico.numLugaresPorFila;
                    }
                    getchar();
                } while (novosLugares < 1 || novosLugares > 50); // Máximo 50 lugares
                break;
                
            case 4:
                // Alterar todas as dimensões
                printf("\nALTERAR TODAS AS DIMENSÕES\n");
                
                // Solicitar novo número de pisos
                do {
                    printf("Novo número de pisos (1-5): ");
                    if (scanf("%d", &novosPisos) != 1) {
                        printf("Entrada inválida!\n");
                        while (getchar() != '\n');
                        novosPisos = parqueDinamico.numPisos;
                    }
                    getchar();
                } while (novosPisos < 1 || novosPisos > 5);
                
                // Solicitar novo número de filas
                do {
                    printf("Novo número de filas por piso (1-26): ");
                    if (scanf("%d", &novasFilas) != 1) {
                        printf("Entrada inválida!\n");
                        while (getchar() != '\n');
                        novasFilas = parqueDinamico.numFilasPorPiso;
                    }
                    getchar();
                } while (novasFilas < 1 || novasFilas > 26);
                
                // Solicitar novo número de lugares
                do {
                    printf("Novo número de lugares por fila (1-50): ");
                    if (scanf("%d", &novosLugares) != 1) {
                        printf("Entrada inválida!\n");
                        while (getchar() != '\n');
                        novosLugares = parqueDinamico.numLugaresPorFila;
                    }
                    getchar();
                } while (novosLugares < 1 || novosLugares > 50);
                break;
                
            case 5:
                // Pré-visualização das alterações (sem aplicar)
                printf("\nPRÉ-VISUALIZAÇÃO DAS ALTERAÇÕES\n");
                printf("================================\n");
                printf("Configuração atual:\n");
                printf("  Pisos: %d\n", parqueDinamico.numPisos);
                printf("  Filas por piso: %d\n", parqueDinamico.numFilasPorPiso);
                printf("  Lugares por fila: %d\n", parqueDinamico.numLugaresPorFila);
                printf("  Capacidade: %d lugares\n\n", 
                       parqueDinamico.numPisos * parqueDinamico.numFilasPorPiso * parqueDinamico.numLugaresPorFila);
                
                printf("Nova configuração proposta:\n");
                printf("  Pisos: %d\n", novosPisos);
                printf("  Filas por piso: %d\n", novasFilas);
                printf("  Lugares por fila: %d\n", novosLugares);
                printf("  Nova capacidade: %d lugares\n\n", 
                       novosPisos * novasFilas * novosLugares);
                
                // Calcular e mostrar diferença
                int diferencaLugares = (novosPisos * novasFilas * novosLugares) - 
                                      (parqueDinamico.numPisos * parqueDinamico.numFilasPorPiso * parqueDinamico.numLugaresPorFila);
                printf("Diferença: %+d lugares\n", diferencaLugares);
                
                printf("\nPressione Enter para continuar...");
                getchar();
                continue; // Voltar ao menu sem sair do loop
                
            case 0:
                // Cancelar operação
                printf("Operação cancelada.\n");
                adicionarLog(LOG_INFO, "Alteração do parque cancelada no menu");
                printf("Pressione Enter para continuar...");
                getchar();
                return;
                
            default:
                printf("Opção inválida!\n");
                printf("Pressione Enter para continuar...");
                getchar();
                continue; // Voltar ao menu
        }
        
        // 9. Confirmar alterações propostas
        printf("\nCONFIRMAÇÃO DAS ALTERAÇÕES\n");
        printf("===========================\n");
        printf("Configuração atual: %d pisos, %d filas/piso, %d lugares/fila\n",
               parqueDinamico.numPisos, parqueDinamico.numFilasPorPiso, parqueDinamico.numLugaresPorFila);
        printf("Nova configuração: %d pisos, %d filas/piso, %d lugares/fila\n",
               novosPisos, novasFilas, novosLugares);
        
        // Avisos adicionais se houver veículos ativos
        if (veiculosAtivos > 0) {
            printf("\nATENÇÃO CRÍTICA: Existem %d veículo(s) ativo(s)!\n", veiculosAtivos);
            printf("Alterar as dimensões pode:\n");
            printf("1. Perder a localização dos veículos ativos\n");
            printf("2. Criar conflitos de lugares\n");
            printf("3. Corromper os dados existentes\n");
        }
        
        printf("\nTem certeza que deseja aplicar estas alterações? (S/N): ");
        if (scanf(" %c", &resposta) != 1) {
            printf("Entrada inválida!\n");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar();
            continue;
        }
        getchar();
        
        if (toupper(resposta) == 'S') {
            // 10. Aplicar alterações (redimensionar parque)
            redimensionarParque(novosPisos, novasFilas, novosLugares);
            
            // Verificar se o redimensionamento foi bem sucedido
            if (!parqueDinamico.parque) {
                printf("Erro ao redimensionar o parque!\n");
                adicionarLog(LOG_ERROR, "Erro ao redimensionar o parque");
                printf("Pressione Enter para continuar...");
                getchar();
                return;
            }
            
            // 11. Salvar alterações no ficheiro
            salvarParque();
            
            // 12. Registrar alteração em log específico
            time_t t = time(NULL);
            struct tm *tm_info = localtime(&t);
            
            FILE *logFile = fopen("alteracoes_parque.log", "a");
            if (logFile != NULL) {
                fprintf(logFile, "%04d-%02d-%02d %02d:%02d - Parque alterado: %dx%dx%d -> %dx%dx%d\n",
                        tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                        tm_info->tm_hour, tm_info->tm_min,
                        parqueDinamico.numPisos, parqueDinamico.numFilasPorPiso, parqueDinamico.numLugaresPorFila,
                        novosPisos, novasFilas, novosLugares);
                fclose(logFile);
            }
            
            // 13. Registrar no log principal do sistema
            char msg[200];
            snprintf(msg, sizeof(msg), "Parque alterado: %d pisos, %d filas/piso, %d lugares/fila -> %d pisos, %d filas/piso, %d lugares/fila",
                    parqueDinamico.numPisos, parqueDinamico.numFilasPorPiso, parqueDinamico.numLugaresPorFila,
                    novosPisos, novasFilas, novosLugares);
            adicionarLog(LOG_INFO, msg);
            
            // 14. Mostrar confirmação ao utilizador
            printf("\nParque alterado com sucesso!\n");
            printf("Nova configuração:\n");
            printf("  Pisos: %d\n", novosPisos);
            printf("  Filas por piso: %d\n", novasFilas);
            printf("  Lugares por fila: %d\n", novosLugares);
            printf("  Capacidade total: %d lugares\n\n", 
                   novosPisos * novasFilas * novosLugares);
            
            printf("Alteração registada em 'alteracoes_parque.log'\n");
            break; // Sair do loop após alteração bem sucedida
            
        } else {
            // Alterações não confirmadas
            printf("Alterações não aplicadas.\n");
            adicionarLog(LOG_INFO, "Alterações do parque não confirmadas pelo utilizador");
            printf("Pressione Enter para continuar...");
            getchar();
        }
        
    } while (opcao != 0); // Continuar no menu até o utilizador sair
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// Continuação do ficheiro parque.c

// -----------------------------------------------------------------------------
// FUNÇÃO: imprimirMapaPiso() - FUNCIONALIDADE 8
// -----------------------------------------------------------------------------

/**
 * Imprime o mapa de ocupação de um piso específico do parque.
 * Esta função implementa a funcionalidade 8 do enunciado:
 * - Mostra a ocupação do piso de forma explícita
 * - Usa símbolos: 'L' para livre, 'O' para ocupado, letra para indisponível
 * - Formata a visualização para fácil leitura
 * - Inclui legenda explicativa dos símbolos
 * - Mostra estatísticas de ocupação do piso
 * 
 * @note O utilizador pode selecionar qual piso visualizar.
 * @note Formato de saída: matriz de lugares com legenda explicativa.
 */
void imprimirMapaPiso() {
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    int pisoSelecionado;
    
    // 2. Solicitar piso a visualizar
    printf("Digite o piso a imprimir (1 a %d): ", parqueDinamico.numPisos);
    if (scanf("%d", &pisoSelecionado) != 1 || pisoSelecionado < 1 || pisoSelecionado > parqueDinamico.numPisos) {
        printf("Piso inválido.\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();
    
    // 3. Contadores para estatísticas
    int lugaresLivres = 0;
    int lugaresOcupados = 0;
    int lugaresIndisponiveis = 0;
    
    // 4. Mostrar legenda dos símbolos
    printf("\n=== SIGNIFICADO DO ESTADO DOS LUGARES ===\n");
    printf("  L : Livre (disponível para estacionamento)\n");
    printf("  O : Ocupado (veículo estacionado)\n");
    printf("  i : Indisponível - condições inadequadas\n");
    printf("  o : Indisponível - objeto de obras\n");
    printf("  r : Indisponível - reservado\n");
    printf("  m : Indisponível - outros motivos\n");
    
    // 5. Mostrar mapa do piso selecionado
    printf("\n=== MAPA DO PISO %d ===\n", pisoSelecionado);
    
    int pisoIndex = pisoSelecionado - 1;  // Converter para índice 0-based
    
    // 6. Percorrer todas as filas do piso
    for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
        // Mostrar identificador da fila (A, B, C, ...)
        printf("Fila %c: ", 'A' + f);
        
        // 7. Percorrer todos os lugares desta fila
        for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
            char estado = parqueDinamico.parque[pisoIndex][f][l].estado;
            
            // Mostrar símbolo do estado
            printf("%c ", estado);
            
            // Contabilizar por tipo de estado
            switch (estado) {
                case 'L': // Livre
                    lugaresLivres++;
                    break;
                case 'O': // Ocupado
                    lugaresOcupados++;
                    break;
                case 'i': // Inadequado
                case 'o': // Obras
                case 'r': // Reservado
                case 'm': // Outros motivos
                    lugaresIndisponiveis++;
                    break;
            }
        }
        printf("\n"); // Nova linha para próxima fila
    }
    
    // 8. Calcular e mostrar estatísticas do piso
    int totalLugares = lugaresLivres + lugaresOcupados + lugaresIndisponiveis;
    
    printf("\n=== ESTATÍSTICAS DO PISO %d ===\n", pisoSelecionado);
    printf("  Lugares livres: %d (%.1f%%)\n", 
           lugaresLivres,
           totalLugares > 0 ? (float)lugaresLivres / totalLugares * 100 : 0);
    printf("  Lugares ocupados: %d (%.1f%%)\n", 
           lugaresOcupados,
           totalLugares > 0 ? (float)lugaresOcupados / totalLugares * 100 : 0);
    printf("  Lugares indisponíveis: %d (%.1f%%)\n", 
           lugaresIndisponiveis,
           totalLugares > 0 ? (float)lugaresIndisponiveis / totalLugares * 100 : 0);
    printf("  Total de lugares: %d\n", totalLugares);
    
    // 9. Informação adicional sobre lugares indisponíveis
    if (lugaresIndisponiveis > 0) {
        printf("\n=== LUGARES INDISPONÍVEIS NO PISO %d ===\n", pisoSelecionado);
        
        int contagemMotivos[4] = {0}; // [0]=i, [1]=o, [2]=r, [3]=m
        char *descricoesMotivos[] = {"Inadequado", "Obras", "Reservado", "Outros"};
        
        // Contar por motivo
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                char motivo = parqueDinamico.parque[pisoIndex][f][l].motivo;
                
                switch (motivo) {
                    case 'i': contagemMotivos[0]++; break;
                    case 'o': contagemMotivos[1]++; break;
                    case 'r': contagemMotivos[2]++; break;
                    case 'm': contagemMotivos[3]++; break;
                }
            }
        }
        
        // Mostrar contagem por motivo
        for (int i = 0; i < 4; i++) {
            if (contagemMotivos[i] > 0) {
                printf("  %s: %d lugar(es)\n", descricoesMotivos[i], contagemMotivos[i]);
            }
        }
    }
    
    printf("\nPressione Enter para continuar...");
    
    // Limpar buffer de entrada
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// -----------------------------------------------------------------------------
// FUNÇÃO: marcarIndisponivel() - FUNCIONALIDADE 7-a
// -----------------------------------------------------------------------------

/**
 * Marca um lugar como indisponível e regista o motivo.
 * Esta função implementa a funcionalidade 7-a do enunciado:
 * - Permite especificar lugares não disponíveis
 * - Regista o motivo: 'i' (condições inadequadas), 'o' (obras),
 *   'r' (reservado), 'm' (outros motivos)
 * - Atualiza o estado do lugar na estrutura do parque
 * - Valida se o lugar pode ser marcado como indisponível
 * - Regista a operação no sistema de logs
 * 
 * @note O lugar marcado como indisponível não pode ser atribuído a veículos.
 * @note Verifica se o lugar já está ocupado antes de marcar como indisponível.
 */
void marcarIndisponivel() {
    limparEcra();
    printf("=========================================\n");
    printf("  MARCAR LUGAR COMO INDISPONÍVEL         \n");
    printf("=========================================\n");
    
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado. Configure o parque primeiro.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    char codigoLugar[5];
    
    // 2. Solicitar código do lugar
    printf("Digite o código do lugar a marcar como indisponível (ex: 1A01): ");
    if (scanf("%4s", codigoLugar) != 1) {
        printf("Entrada inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // 3. Procurar o lugar pelo código na estrutura do parque
    int lugarEncontrado = 0;
    int pisoIdx = -1, filaIdx = -1, lugarIdx = -1;
    
    for (int p = 0; p < parqueDinamico.numPisos && !lugarEncontrado; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso && !lugarEncontrado; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila && !lugarEncontrado; l++) {
                if (strcmp(parqueDinamico.parque[p][f][l].codigo, codigoLugar) == 0) {
                    lugarEncontrado = 1;
                    pisoIdx = p;
                    filaIdx = f;
                    lugarIdx = l;
                }
            }
        }
    }
    
    // 4. Verificar se o lugar foi encontrado
    if (!lugarEncontrado) {
        printf("Lugar '%s' não encontrado!\n", codigoLugar);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 5. Verificar estado atual do lugar
    char estadoAtual = parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].estado;
    
    if (estadoAtual == 'O') { // Ocupado
        printf("Lugar '%s' está ocupado por um veículo!\n", codigoLugar);
        printf("Não é possível marcar como indisponível enquanto ocupado.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    if (estadoAtual == 'i' || estadoAtual == 'o' || estadoAtual == 'r' || estadoAtual == 'm') {
        printf("Lugar '%s' já está marcado como indisponível!\n", codigoLugar);
        printf("Motivo atual: ");
        
        // Mostrar descrição do motivo atual
        switch (estadoAtual) {
            case 'i': printf("Inadequado (dimensões, acesso, etc.)\n"); break;
            case 'o': printf("Obras/manutenção\n"); break;
            case 'r': printf("Reservado (para funcionários, clientes especiais)\n"); break;
            case 'm': printf("Outros motivos\n"); break;
        }
        
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 6. Mostrar menu de motivos de indisponibilidade
    printf("\n=== MOTIVO DA INDISPONIBILIDADE ===\n");
    printf("i - Inadequado (dimensões, acesso, etc.)\n");
    printf("o - Obras/manutenção\n");
    printf("r - Reservado (para funcionários, clientes especiais)\n");
    printf("m - Outros motivos\n");
    
    char motivo;
    printf("Escolha o motivo (i/o/r/m): ");
    if (scanf(" %c", &motivo) != 1) {
        printf("Entrada inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // 7. Validar o motivo escolhido
    motivo = tolower(motivo);
    if (motivo != 'i' && motivo != 'o' && motivo != 'r' && motivo != 'm') {
        printf("Motivo inválido!\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 8. Marcar lugar como indisponível
    parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].estado = motivo;
    parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].motivo = motivo;
    
    // 9. Descrição do motivo para mensagem ao utilizador
    char descricao[50];
    switch (motivo) {
        case 'i': strcpy(descricao, "Inadequado (dimensões, acesso, etc.)"); break;
        case 'o': strcpy(descricao, "Obras/manutenção"); break;
        case 'r': strcpy(descricao, "Reservado"); break;
        case 'm': strcpy(descricao, "Outros motivos"); break;
        default: strcpy(descricao, "Indisponível");
    }
    
    // 10. Confirmar operação ao utilizador
    printf("\nLugar '%s' marcado como indisponível.\n", codigoLugar);
    printf("Motivo: %s\n", descricao);
    
    // 11. Registar operação no sistema de logs
    char msg[150];
    snprintf(msg, sizeof(msg), "Lugar %s marcado como indisponível: %s", 
             codigoLugar, descricao);
    adicionarLog(LOG_INFO, msg);
    
    // 12. Salvar alterações no ficheiro do parque
    salvarParque();
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// Continuação do ficheiro parque.c

// -----------------------------------------------------------------------------
// FUNÇÃO: reverterIndisponivel() - FUNCIONALIDADE 7-b
// -----------------------------------------------------------------------------

/**
 * Reverte a situação de um lugar indisponível, tornando-o disponível.
 * Esta função implementa a funcionalidade 7-b do enunciado:
 * - Permite reverter a marcação de indisponibilidade
 * - O lugar volta a ficar disponível para estacionamento
 * - Mantém histórico do motivo anterior (se necessário)
 * - Valida se o lugar pode ser revertido
 * 
 * @note Apenas lugares marcados como indisponíveis podem ser revertidos.
 * @note Lugares ocupados não podem ser revertidos.
 * @note Requer confirmação do utilizador antes de proceder.
 */
void reverterIndisponivel() {
    limparEcra();
    printf("=========================================\n");
    printf("  REVERTER LUGAR INDISPONÍVEL            \n");
    printf("=========================================\n");
    
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado. Configure o parque primeiro.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    char codigoLugar[5];
    
    // 2. Solicitar código do lugar a reverter
    printf("Digite o código do lugar a reverter para disponível (ex: 1A01): ");
    if (scanf("%4s", codigoLugar) != 1) {
        printf("Entrada inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // 3. Procurar o lugar pelo código
    int lugarEncontrado = 0;
    int pisoIdx = -1, filaIdx = -1, lugarIdx = -1;
    
    for (int p = 0; p < parqueDinamico.numPisos && !lugarEncontrado; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso && !lugarEncontrado; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila && !lugarEncontrado; l++) {
                if (strcmp(parqueDinamico.parque[p][f][l].codigo, codigoLugar) == 0) {
                    lugarEncontrado = 1;
                    pisoIdx = p;
                    filaIdx = f;
                    lugarIdx = l;
                }
            }
        }
    }
    
    // 4. Verificar se o lugar foi encontrado
    if (!lugarEncontrado) {
        printf("Lugar '%s' não encontrado!\n", codigoLugar);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 5. Verificar estado atual do lugar
    char estadoAtual = parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].estado;
    
    if (estadoAtual == 'L') { // Livre
        printf("Lugar '%s' já está DISPONÍVEL!\n", codigoLugar);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    if (estadoAtual == 'O') { // Ocupado
        printf("Lugar '%s' está OCUPADO por um veículo!\n", codigoLugar);
        printf("Não pode ser revertido para disponível enquanto ocupado.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 6. Verificar se o lugar está realmente indisponível
    if (estadoAtual != 'i' && estadoAtual != 'o' && estadoAtual != 'r' && estadoAtual != 'm') {
        printf("Lugar '%s' não está marcado como indisponível!\n", codigoLugar);
        printf("Estado atual: %c\n", estadoAtual);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 7. Mostrar informação atual e solicitar confirmação
    printf("\nLugar '%s' está marcado como ", codigoLugar);
    
    // Descrever o motivo atual
    switch (estadoAtual) {
        case 'i': 
            printf("INADEQUADO (condições inadequadas)"); 
            break;
        case 'o': 
            printf("EM OBRAS (objeto de obras/manutenção)"); 
            break;
        case 'r': 
            printf("RESERVADO (uso exclusivo)"); 
            break;
        case 'm': 
            printf("INDISPONÍVEL (outros motivos)"); 
            break;
    }
    
    printf("\n\nDeseja reverter para DISPONÍVEL? (S/N): ");
    
    char confirmar;
    if (scanf(" %c", &confirmar) != 1) {
        printf("Entrada inválida!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    confirmar = toupper(confirmar);
    if (confirmar != 'S') {
        printf("Operação cancelada.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 8. Reverter para disponível
    parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].estado = 'L'; // Livre
    parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].motivo = ' '; // Limpar motivo
    
    printf("\nLugar '%s' revertido para DISPONÍVEL com sucesso.\n", codigoLugar);
    
    // 9. Registar operação no sistema de logs
    char msg[150];
    snprintf(msg, sizeof(msg), "Lugar %s revertido para disponível (estado anterior: %c)", 
             codigoLugar, estadoAtual);
    adicionarLog(LOG_INFO, msg);
    
    // 10. Salvar alterações no ficheiro do parque
    salvarParque();
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: alterarIndisponivel()
// -----------------------------------------------------------------------------

/**
 * Altera o motivo de indisponibilidade de um lugar.
 * Permite atualizar o motivo sem alterar o estado de indisponibilidade.
 * 
 * @note Útil para situações onde o motivo muda (ex: de 'obras' para 'reservado').
 * @note Mantém o lugar como indisponível, apenas altera a razão.
 * @note Requer confirmação do utilizador antes de proceder.
 * @note Oferece opção de cancelamento a qualquer momento.
 */
void alterarIndisponivel() {
    limparEcra();
    printf("=========================================\n");
    printf("  ALTERAR MOTIVO DE LUGAR INDISPONÍVEL   \n");
    printf("=========================================\n");
    
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado. Configure o parque primeiro.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    char codigoLugar[5];
    
    // 2. Solicitar código do lugar a alterar
    printf("Digite o código do lugar a alterar (ex: 1A01): ");
    if (scanf("%4s", codigoLugar) != 1) {
        printf("Entrada inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // 3. Procurar o lugar pelo código
    int lugarEncontrado = 0;
    int pisoIdx = -1, filaIdx = -1, lugarIdx = -1;
    
    for (int p = 0; p < parqueDinamico.numPisos && !lugarEncontrado; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso && !lugarEncontrado; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila && !lugarEncontrado; l++) {
                if (strcmp(parqueDinamico.parque[p][f][l].codigo, codigoLugar) == 0) {
                    lugarEncontrado = 1;
                    pisoIdx = p;
                    filaIdx = f;
                    lugarIdx = l;
                }
            }
        }
    }
    
    // 4. Verificar se o lugar foi encontrado
    if (!lugarEncontrado) {
        printf("Lugar '%s' não encontrado!\n", codigoLugar);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 5. Verificar se o lugar está indisponível
    char estadoAtual = parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].estado;
    
    if (estadoAtual != 'i' && estadoAtual != 'o' && estadoAtual != 'r' && estadoAtual != 'm') {
        printf("Lugar '%s' não está marcado como indisponível!\n", codigoLugar);
        
        // Mostrar estado atual
        if (estadoAtual == 'L') {
            printf("Estado atual: LIVRE\n");
        } else if (estadoAtual == 'O') {
            printf("Estado atual: OCUPADO\n");
        } else {
            printf("Estado atual: %c\n", estadoAtual);
        }
        
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 6. Mostrar estado atual do lugar
    printf("\nEstado atual do lugar '%s': ", codigoLugar);
    
    char descricaoAtual[50];
    switch (estadoAtual) {
        case 'i': 
            strcpy(descricaoAtual, "INADEQUADO (condições inadequadas)"); 
            break;
        case 'o': 
            strcpy(descricaoAtual, "EM OBRAS (objeto de obras/manutenção)"); 
            break;
        case 'r': 
            strcpy(descricaoAtual, "RESERVADO (uso exclusivo)"); 
            break;
        case 'm': 
            strcpy(descricaoAtual, "INDISPONÍVEL (outros motivos)"); 
            break;
    }
    printf("%s\n\n", descricaoAtual);
    
    // 7. Mostrar menu de novos motivos
    printf("=== NOVO MOTIVO DA INDISPONIBILIDADE ===\n");
    printf("i - Inadequado (dimensões, acesso, etc.)\n");
    printf("o - Obras/manutenção\n");
    printf("r - Reservado (para funcionários, clientes especiais)\n");
    printf("m - Outros motivos\n");
    printf("0 - Cancelar alteração\n");
    
    char novoMotivo;
    printf("Escolha o novo motivo (i/o/r/m/0): ");
    if (scanf(" %c", &novoMotivo) != 1) {
        printf("Entrada inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // 8. Processar escolha do utilizador
    if (novoMotivo == '0') {
        printf("Alteração cancelada.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 9. Validar novo motivo
    novoMotivo = tolower(novoMotivo);
    if (novoMotivo != 'i' && novoMotivo != 'o' && novoMotivo != 'r' && novoMotivo != 'm') {
        printf("Motivo inválido!\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 10. Verificar se o novo motivo é diferente do atual
    if (novoMotivo == estadoAtual) {
        printf("O novo motivo é igual ao motivo atual!\n");
        printf("Nenhuma alteração foi realizada.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 11. Alterar o motivo
    parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].estado = novoMotivo;
    parqueDinamico.parque[pisoIdx][filaIdx][lugarIdx].motivo = novoMotivo;
    
    // 12. Descrição do novo motivo para mensagem ao utilizador
    char descricaoNova[50];
    switch (novoMotivo) {
        case 'i': 
            strcpy(descricaoNova, "INADEQUADO"); 
            break;
        case 'o': 
            strcpy(descricaoNova, "EM OBRAS"); 
            break;
        case 'r': 
            strcpy(descricaoNova, "RESERVADO"); 
            break;
        case 'm': 
            strcpy(descricaoNova, "OUTROS MOTIVOS"); 
            break;
    }
    
    printf("\nMotivo do lugar '%s' alterado de:\n", codigoLugar);
    printf("  Antigo: %s\n", descricaoAtual);
    printf("  Novo: %s\n", descricaoNova);
    
    // 13. Registar operação no sistema de logs
    char msg[200];
    snprintf(msg, sizeof(msg), "Lugar %s: motivo alterado de %c (%s) para %c (%s)", 
             codigoLugar, estadoAtual, descricaoAtual, novoMotivo, descricaoNova);
    adicionarLog(LOG_INFO, msg);
    
    // 14. Salvar alterações no ficheiro do parque
    salvarParque();
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// Continuação do ficheiro parque.c

// -----------------------------------------------------------------------------
// FUNÇÃO: listarIndisponiveis()
// -----------------------------------------------------------------------------

/**
 * Lista todos os lugares atualmente marcados como indisponíveis.
 * Mostra para cada lugar:
 * - Código do lugar
 * - Motivo da indisponibilidade
 * - Descrição do motivo
 * 
 * @note Inclui estatísticas por motivo de indisponibilidade.
 * @note Formata a saída como tabela para fácil leitura.
 * @note Informa se não há lugares indisponíveis.
 * @note Regista a consulta no sistema de logs.
 */
void listarIndisponiveis() {
    limparEcra();
    printf("=========================================\n");
    printf("  LISTA DE LUGARES INDISPONÍVEIS         \n");
    printf("=========================================\n");
    
    // 1. Verificar se o parque está configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado. Configure o parque primeiro.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 2. Contadores para estatísticas
    int totalIndisponiveis = 0;
    int contadorPorMotivo[4] = {0}; // Índices: [0]=i, [1]=o, [2]=r, [3]=m
    
    // 3. Cabeçalho da tabela
    printf("\n%-8s %-8s %-15s %-20s\n", "PISO", "LUGAR", "ESTADO", "MOTIVO");
    printf("------------------------------------------------\n");
    
    // 4. Percorrer todo o parque procurando lugares indisponíveis
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                char estado = parqueDinamico.parque[p][f][l].estado;
                
                // 5. Verificar se é um lugar indisponível
                if (estado == 'i' || estado == 'o' || estado == 'r' || estado == 'm') {
                    totalIndisponiveis++;
                    
                    // 6. Contar por motivo para estatísticas
                    switch (estado) {
                        case 'i': contadorPorMotivo[0]++; break; // Inadequado
                        case 'o': contadorPorMotivo[1]++; break; // Obras
                        case 'r': contadorPorMotivo[2]++; break; // Reservado
                        case 'm': contadorPorMotivo[3]++; break; // Outros
                    }
                    
                    // 7. Descrição textual do motivo
                    char descricao[20];
                    switch (estado) {
                        case 'i': strcpy(descricao, "Inadequado"); break;
                        case 'o': strcpy(descricao, "Obras"); break;
                        case 'r': strcpy(descricao, "Reservado"); break;
                        case 'm': strcpy(descricao, "Outros motivos"); break;
                        default: strcpy(descricao, "Desconhecido");
                    }
                    
                    // 8. Mostrar linha da tabela com informações do lugar
                    printf("%-8d %-8s %-15c %-20s\n", 
                           p+1,                          // Número do piso (1-based)
                           parqueDinamico.parque[p][f][l].codigo,  // Código do lugar
                           estado,                       // Estado (i, o, r, m)
                           descricao);                   // Descrição do motivo
                }
            }
        }
    }
    
    // 9. Mostrar estatísticas gerais
    printf("\n=== ESTATÍSTICAS ===\n");
    printf("Total de lugares indisponíveis: %d\n", totalIndisponiveis);
    
    // 10. Mostrar distribuição por motivo
    if (totalIndisponiveis > 0) {
        printf("Distribuição por motivo:\n");
        printf("  Inadequado (i): %d lugar(es) (%.1f%%)\n", 
               contadorPorMotivo[0],
               (float)contadorPorMotivo[0] / totalIndisponiveis * 100);
        printf("  Obras (o): %d lugar(es) (%.1f%%)\n", 
               contadorPorMotivo[1],
               (float)contadorPorMotivo[1] / totalIndisponiveis * 100);
        printf("  Reservado (r): %d lugar(es) (%.1f%%)\n", 
               contadorPorMotivo[2],
               (float)contadorPorMotivo[2] / totalIndisponiveis * 100);
        printf("  Outros motivos (m): %d lugar(es) (%.1f%%)\n", 
               contadorPorMotivo[3],
               (float)contadorPorMotivo[3] / totalIndisponiveis * 100);
        
        // 11. Calcular percentagem do total do parque
        int capacidadeTotal = parqueDinamico.numPisos * 
                              parqueDinamico.numFilasPorPiso * 
                              parqueDinamico.numLugaresPorFila;
        
        if (capacidadeTotal > 0) {
            float percentagemTotal = (float)totalIndisponiveis / capacidadeTotal * 100;
            printf("\nPercentagem do parque indisponível: %.1f%%\n", percentagemTotal);
        }
    } else {
        printf("\nNão há lugares indisponíveis no parque.\n");
    }
    
    // 12. Registar consulta no sistema de logs
    char msg[100];
    snprintf(msg, sizeof(msg), "Listados %d lugares indisponíveis", totalIndisponiveis);
    adicionarLog(LOG_INFO, msg);
    
    printf("\nPressione Enter para continuar...");
    
    // 13. Limpar buffer de entrada
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: menuIndisponiveis() - MENU DE GESTÃO DE LUGARES INDISPONÍVEIS
// -----------------------------------------------------------------------------

/**
 * Menu principal para gestão de lugares indisponíveis.
 * Apresenta as opções de marcação, reversão, alteração e listagem.
 * Coordena as operações relacionadas com lugares indisponíveis.
 * 
 * @note Implementa todas as operações da funcionalidade 7 do enunciado.
 * @note Oferece interface unificada para todas as operações de indisponibilidade.
 * @note Valida todas as entradas do utilizador.
 * @note Regista a abertura e fecho do menu no sistema de logs.
 */
void menuIndisponiveis() {
    int opcao;
    
    do {
        limparEcra();
        printf("=========================================\n");
        printf("   GESTÃO DE LUGARES INDISPONÍVEIS       \n");
        printf("=========================================\n");
        printf("1 - Marcar lugar como indisponível\n");
        printf("2 - Reverter lugar para disponível\n");
        printf("3 - Listar lugares indisponíveis\n");
        printf("4 - Alterar motivo de lugar indisponível\n");
        printf("0 - Voltar ao menu principal\n");
        printf("=========================================\n");
        printf("Escolha: ");
        
        // 1. Ler e validar opção do utilizador
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            
            // Limpar buffer de entrada
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar();
            continue;
        }
        getchar();

        // 2. Processar opção escolhida
        switch (opcao) {
            case 1: 
                // Marcar lugar como indisponível
                marcarIndisponivel(); 
                break;
                
            case 2: 
                // Reverter lugar para disponível
                reverterIndisponivel(); 
                break;
                
            case 3: 
                // Listar todos os lugares indisponíveis
                listarIndisponiveis(); 
                break;
                
            case 4: 
                // Alterar motivo de lugar já indisponível
                alterarIndisponivel(); 
                break;
                
            case 0: 
                // Sair do menu
                adicionarLog(LOG_INFO, "Menu de gestão de lugares indisponíveis fechado");
                break;
                
            default: 
                // Opção inválida
                printf("Opção inválida!\n");
                adicionarLog(LOG_WARNING, "Opção inválida no menu de lugares indisponíveis");
                printf("Pressione Enter para continuar...");
                getchar();
        }
    } while (opcao != 0); // Continuar no menu até o utilizador escolher sair
}

// =====================================================================
// FUNÇÕES AUXILIARES
// =====================================================================

// Função para obter o lugar pelo código
Lugar* obterLugarPorCodigo(const char *codigo) {
    // Verificação de segurança: parque inicializado e código válido
    // IMPORTANTE: Conforme Normas Gerais, devemos validar parâmetros
    if (!parqueDinamico.parque || !codigo || strlen(codigo) != 4) {
        return NULL;  // Retorna NULL em caso de erro
    }
    
    // Extrair informações do código (ex: "1A01")
    // CONFORME: Nota 1 da Figura 2 - código tem 4 caracteres
    // Piso: primeiro carácter, convertido para índice 0-based
    int piso = codigo[0] - '0' - 1;  // Ex: '1' -> 0
    char filaChar = codigo[1];       // Ex: 'A'
    int fila = filaChar - 'A';       // Ex: 'A' -> 0
    int lugar = atoi(&codigo[2]) - 1; // Ex: "01" -> 0
    
    // Verificar limites
    // ATENÇÃO: Deveria usar constantes (MAX_PISOS, etc.) conforme Normas Gerais
    if (piso < 0 || piso >= parqueDinamico.numPisos ||
        fila < 0 || fila >= parqueDinamico.numFilasPorPiso ||
        lugar < 0 || lugar >= parqueDinamico.numLugaresPorFila) {
        return NULL;  // Código inválido
    }
    
    return &parqueDinamico.parque[piso][fila][lugar];
}

// Função para verificar se um lugar está livre
int lugarEstaDisponivel(const char *codigoLugar) {
    // Obtém o lugar usando a função auxiliar anterior
    Lugar *lugar = obterLugarPorCodigo(codigoLugar);
    
    // Verifica se o lugar existe
    if (!lugar) {
        return 0;  // Lugar não existe - retorna falso
    }
    
    // Verifica estado do lugar
    // '-' = livre (conforme funcionalidade 8)
    // 'X' = ocupado
    // 'i', 'o', 'r', 'm' = indisponível (funcionalidade 7-a)
    return (lugar->estado == 'L');
}

// Função para obter o primeiro lugar livre
int obterPrimeiroLugarLivre(char *codigoLugar) {
    // Verifica se o parque está configurado
    if (!parqueDinamico.parque) {
        return 0;  // Parque não configurado
    }
    
    // Percorre todos os lugares do parque
    // ESTRUTURA: piso -> fila -> lugar (tripla iteração)
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                // Verifica se o lugar está livre
                if (parqueDinamico.parque[p][f][l].estado == 'L') {
                    // Se foi fornecido buffer para código, formata-o
                    if (codigoLugar) {
                        // Formato: piso(1-5) + fila(A-Z) + lugar(01-50)
                        // Ex: 1A01, 2B15, 3C50
                        sprintf(codigoLugar, "%d%c%02d", p+1, 'A'+f, l+1);
                    }
                    return 1;  // Encontrou lugar livre
                }
            }
        }
    }
    
    return 0;  // Não há lugares livres
}

// Função para contar lugares por estado
void contarLugaresPorEstado(int *livres, int *ocupados, int *indisponiveis) {
    // Inicializa contadores (Normas Gerais: inicializar variáveis)
    if (!parqueDinamico.parque) {
        *livres = *ocupados = *indisponiveis = 0;
        return;
    }
    
    *livres = *ocupados = *indisponiveis = 0;
    
    // Percorre toda a estrutura do parque
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                // Classifica cada lugar conforme seu estado
                switch (parqueDinamico.parque[p][f][l].estado) {
                    case '-': 
                        (*livres)++;  // Lugar livre
                        break;
                    case 'X': 
                        (*ocupados)++;  // Lugar ocupado
                        break;
                    case 'i':  // condições inadequadas
                    case 'o':  // objeto de obras
                    case 'r':  // reservado
                    case 'm':  // outros motivos
                        (*indisponiveis)++;  // Lugar indisponível
                        break;
                }
            }
        }
    }
}

// Função para verificar integridade do parque
int verificarIntegridadeParque() {
    // Verifica se o parque foi configurado
    if (!parqueDinamico.parque) {
        return 0;  // Parque não configurado
    }
    
    int erros = 0;  // Contador de erros encontrados
    
    // Verifica cada lugar individualmente
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                // Verificação 1: Código do lugar está correto?
                char codigoEsperado[5];
                // Gera código esperado baseado na posição
                sprintf(codigoEsperado, "%d%c%02d", p+1, 'A'+f, l+1);
                
                // Compara com código armazenado
                if (strcmp(parqueDinamico.parque[p][f][l].codigo, codigoEsperado) != 0) {
                    printf("ERRO: Código incorreto no piso %d, fila %d, lugar %d\n", 
                           p+1, f+1, l+1);
                    printf("  Esperado: %s, Encontrado: %s\n", 
                           codigoEsperado, parqueDinamico.parque[p][f][l].codigo);
                    erros++;
                }
                
                // Verificação 2: Estado do lugar é válido?
                char estado = parqueDinamico.parque[p][f][l].estado;
                // Estados válidos conforme funcionalidades 7 e 8
                if (estado != '-' && estado != 'X' && 
                    estado != 'i' && estado != 'o' && estado != 'r' && estado != 'm') {
                    printf("ERRO: Estado inválido no lugar %s: %c\n", 
                           parqueDinamico.parque[p][f][l].codigo, estado);
                    erros++;
                }
            }
        }
    }
    
    // Relatório final
    if (erros == 0) {
        printf("Parque verificado com sucesso. Nenhum erro encontrado.\n");
    } else {
        printf("Encontrados %d erro(s) no parque.\n", erros);
    }
    
    return (erros == 0);  // Retorna 1 se não há erros, 0 caso contrário
}

// Função para exibir lugares disponíveis por piso
// IMPORTANTE: Conforme Introdução, deve estar sempre visível em menus
void exibirLugaresDisponiveis() {
    // Verifica se o parque foi configurado
    if (!parqueDinamico.parque || parqueDinamico.numPisos == 0) {
        printf("Parque não configurado.\n");
        return;
    }
    
    printf("=== LUGARES DISPONÍVEIS POR PISO ===\n");
    
    // Para cada piso, conta lugares livres
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        int livres = 0;
        
        // Percorre todas as filas e lugares do piso
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                // LUGAR LIVRE É 'L' (não '-')
                if (parqueDinamico.parque[p][f][l].estado == 'L') {
                    livres++;
                }
            }
        }
        
        // Exibe contagem para o piso atual
        printf("Piso %d: %d lugares livres\n", p+1, livres);
    }
    
    printf("====================================\n\n");
}