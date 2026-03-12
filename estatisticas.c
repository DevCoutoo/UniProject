// estatisticas.c - Módulo de Estatísticas e Relatórios
// Autor: [Seu Nome]
// Data: [Data]
// Descrição: Contém funções para geração de estatísticas e relatórios
//           conforme funcionalidade 11 e extras E1, E2, E3 do enunciado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>
#include "estatisticas.h"
#include "datastructs.h"
#include "utils.h"
#include "parque.h"
#include "estacionamentos.h"
#include "tarifas.h"

// -----------------------------------------------------------------------------
// DECLARAÇÕES EXTERNAS DE VARIÁVEIS GLOBAIS
// -----------------------------------------------------------------------------

/**
 * Estrutura dinâmica do parque de estacionamento.
 * Usada para calcular estatísticas de ocupação por piso.
 */
extern ParqueDinamico parqueDinamico;

/**
 * Lista dinâmica de todos os estacionamentos.
 * Usada para análise estatística de dados históricos.
 */
extern ListaEstacionamentos listaEstacionamentos;

/**
 * Sistema de tarifas atual.
 * Usado para referência em cálculos de valores.
 */
extern TarifasSistema sistema;

// -----------------------------------------------------------------------------
// FUNÇÃO: mostrarEstatisticasGerais()
// -----------------------------------------------------------------------------

/**
 * Mostra estatísticas gerais sobre a utilização do parque.
 * Esta função apresenta uma visão geral do sistema com:
 * - Total de registos de estacionamento
 * - Veículos atualmente no parque
 * - Veículos que já saíram
 * - Valor total arrecadado
 * - Média por veículo
 * 
 * @note Funcionalidade opcional que fornece uma visão geral do sistema.
 * @note Utiliza a estrutura dinâmica listaEstacionamentos.
 */
void mostrarEstatisticasGerais() {
    limparEcra();
    printf("=== ESTATÍSTICAS GERAIS ===\n");
    
    // Verificar se existem dados
    if (listaEstacionamentos.total == 0) {
        printf("Não existem estacionamentos registados.\n");
    } else {
        int veiculosAtivos = 0;      // Contador de veículos ainda no parque
        int veiculosFinalizados = 0; // Contador de veículos que já saíram
        float valorTotal = 0.0f;     // Acumulador do valor total arrecadado
        
        // Analisar todos os estacionamentos
        for (int i = 0; i < listaEstacionamentos.total; i++) {
            if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
                // Veículo ainda está ativo (no parque)
                veiculosAtivos++;
            } else {
                // Veículo já saiu
                veiculosFinalizados++;
                valorTotal += listaEstacionamentos.estacionamentos[i].valorPago;
            }
        }
        
        // Apresentar estatísticas
        printf("Total de registos: %d\n", listaEstacionamentos.total);
        printf("Veículos atualmente no parque: %d\n", veiculosAtivos);
        printf("Veículos que já saíram: %d\n", veiculosFinalizados);
        printf("Valor total arrecadado: €%.2f\n", valorTotal);
        
        // Calcular média (evitar divisão por zero)
        if (veiculosFinalizados > 0) {
            printf("Média por veículo: €%.2f\n", valorTotal / veiculosFinalizados);
        } else {
            printf("Média por veículo: €0.00\n");
        }
    }
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: mostrarEstatisticasPorPiso()
// -----------------------------------------------------------------------------

/**
 * Mostra estatísticas detalhadas por piso do parque.
 * Para cada piso, apresenta:
 * - Número de lugares livres/ocupados/indisponíveis
 * - Percentagem de ocupação
 * - Distribuição por estado
 * 
 * @note Utiliza a estrutura dinâmica parqueDinamico.
 * @note Útil para análise da distribuição de veículos no parque.
 */
void mostrarEstatisticasPorPiso() {
    limparEcra();
    printf("=== ESTATÍSTICAS POR PISO ===\n");
    
    // Verificar se o parque está configurado
    if (parqueDinamico.numPisos == 0) {
        printf("Parque não configurado.\n");
    } else {
        // Analisar cada piso individualmente
        for (int p = 0; p < parqueDinamico.numPisos; p++) {
            int lugaresLivres = 0;
            int lugaresOcupados = 0;
            int lugaresIndisponiveis = 0;
            
            // Contar lugares por estado neste piso
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
                        default:  // Qualquer outro estado considerado indisponível
                            lugaresIndisponiveis++;
                            break;
                    }
                }
            }
            
            // Calcular totais e percentagens
            int totalLugares = lugaresLivres + lugaresOcupados + lugaresIndisponiveis;
            
            printf("\nPISO %d:\n", p + 1);
            printf("  Lugares livres: %d (%.1f%%)\n", 
                   lugaresLivres,
                   totalLugares > 0 ? (float)lugaresLivres / totalLugares * 100 : 0);
            printf("  Lugares ocupados: %d (%.1f%%)\n", 
                   lugaresOcupados,
                   totalLugares > 0 ? (float)lugaresOcupados / totalLugares * 100 : 0);
            printf("  Lugares indisponíveis: %d (%.1f%%)\n", 
                   lugaresIndisponiveis,
                   totalLugares > 0 ? (float)lugaresIndisponiveis / totalLugares * 100 : 0);
        }
    }
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: gerarGraficoBarras() - EXTRA E1
// -----------------------------------------------------------------------------

/**
 * Gera e exibe um gráfico de barras com dados de saídas por hora.
 * Esta função implementa o extra E1 do enunciado:
 * - Lista veículos que saíram num determinado dia
 * - Agrupa por hora dentro de um período especificado pelo utilizador
 * - Mostra número total de veículos e valor total por hora
 * 
 * @note O gráfico é apresentado no formato ASCII no terminal.
 * @note Oferece opções de visualização e exportação de dados.
 * @note Calcula estatísticas adicionais como horas de pico.
 */
void gerarGraficoBarras() {
    limparEcra();
    printf("=========================================\n");
    printf("      ESTATÍSTICAS POR HORA (EXTRA E1)   \n");
    printf("=========================================\n");
    
    // Verificar se existem dados para análise
    if (listaEstacionamentos.total == 0) {
        printf("Não existem estacionamentos registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Inicializar array de estatísticas por hora (0-23)
    EstatisticaHora estatisticas[24] = {0};
    
    // Inicializar estrutura para cada hora
    for (int i = 0; i < 24; i++) {
        estatisticas[i].hora = i;
        estatisticas[i].totalVeiculos = 0;
        estatisticas[i].valorTotal = 0.0f;
    }
    
    // Coletar dados dos estacionamentos finalizados
    int veiculosProcessados = 0;
    float valorTotalGeral = 0.0f;
    
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
        
        // Apenas considerar veículos que já saíram (com data de saída válida)
        if (e->ativo == 0 && e->saida.ano != 0) {
            int horaSaida = e->saida.hora;
            
            // Validar hora (deve estar entre 0 e 23)
            if (horaSaida >= 0 && horaSaida < 24) {
                // Atualizar estatísticas da hora correspondente
                estatisticas[horaSaida].totalVeiculos++;
                estatisticas[horaSaida].valorTotal += e->valorPago;
                
                // Atualizar contadores gerais
                veiculosProcessados++;
                valorTotalGeral += e->valorPago;
            }
        }
    }
    
    // Verificar se há dados suficientes
    if (veiculosProcessados == 0) {
        printf("Não existem veículos que já tenham saído do parque.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Encontrar valores máximos para escalar o gráfico
    int maxVeiculos = 0;
    float maxValor = 0.0f;
    
    for (int i = 0; i < 24; i++) {
        if (estatisticas[i].totalVeiculos > maxVeiculos) {
            maxVeiculos = estatisticas[i].totalVeiculos;
        }
        if (estatisticas[i].valorTotal > maxValor) {
            maxValor = estatisticas[i].valorTotal;
        }
    }
    
    // Se máximo for 0, não há dados significativos
    if (maxVeiculos == 0) {
        printf("Não há dados suficientes para gerar o gráfico.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Menu de opções de visualização
    printf("\nEscolha o tipo de gráfico:\n");
    printf("1 - Número de veículos por hora\n");
    printf("2 - Valor arrecadado por hora\n");
    printf("3 - Ambos (lado a lado)\n");
    printf("Escolha: ");
    
    int opcao;
    scanf("%d", &opcao);
    getchar(); // Limpar buffer
    
    // Validar opção
    if (opcao < 1 || opcao > 3) {
        printf("Opção inválida!\n");
        getchar();
        return;
    }
    
    // Mostrar cabeçalho do gráfico
    limparEcra();
    printf("=========================================\n");
    printf("         GRÁFICO DE BARRAS POR HORA      \n");
    printf("=========================================\n");
    
    // Explicar legenda conforme opção escolhida
    if (opcao == 1) {
        // Apenas veículos
        printf("LEGENDA: Cada '■' representa aproximadamente %.1f veículos\n", 
               maxVeiculos / 50.0); // 50 é a largura máxima do gráfico
        printf("\nHORA | QUANTIDADE | GRÁFICO (0-%d veículos)\n", maxVeiculos);
    } else if (opcao == 2) {
        // Apenas valor
        printf("LEGENDA: Cada '■' representa aproximadamente €%.2f\n", 
               maxValor / 50.0);
        printf("\nHORA |   VALOR    | GRÁFICO (€0-€%.2f)\n", maxValor);
    } else {
        // Ambos lado a lado
        printf("LEGENDA: Esquerda: veículos | Direita: valor (€)\n");
        printf("        Cada '■' = ~%.1f veículos ou €%.2f\n", 
               maxVeiculos / 25.0, maxValor / 25.0); // 25 para cada gráfico
        printf("\nHORA | VEÍCULOS | GRÁFICO VEÍCULOS | VALOR (€) | GRÁFICO VALOR\n");
    }
    
    // Linha separadora
    printf("-----|-----------|-------------------|----------|----------------\n");
    
    // Gerar cada linha do gráfico (uma para cada hora)
    for (int hora = 0; hora < 24; hora++) {
        int barraVeiculos = 0;
        int barraValor = 0;
        
        // Calcular tamanho das barras (proporcional ao máximo)
        if (maxVeiculos > 0) {
            barraVeiculos = (int)((float)estatisticas[hora].totalVeiculos / maxVeiculos * 50);
        }
        if (maxValor > 0) {
            barraValor = (int)((float)estatisticas[hora].valorTotal / maxValor * 50);
        }
        
        // Imprimir linha conforme opção escolhida
        if (opcao == 1) {
            // Apenas veículos
            printf(" %02dh | %9d | ", hora, estatisticas[hora].totalVeiculos);
            for (int j = 0; j < barraVeiculos; j++) {
                printf("■");
            }
            printf("\n");
            
        } else if (opcao == 2) {
            // Apenas valor
            printf(" %02dh | %9.2f | ", hora, estatisticas[hora].valorTotal);
            for (int j = 0; j < barraValor; j++) {
                printf("■");
            }
            printf("\n");
            
        } else {
            // Ambos lado a lado (limitar a 25 caracteres cada)
            barraVeiculos = barraVeiculos > 25 ? 25 : barraVeiculos;
            barraValor = barraValor > 25 ? 25 : barraValor;
            
            printf(" %02dh | %8d | ", hora, estatisticas[hora].totalVeiculos);
            
            // Gráfico de veículos
            for (int j = 0; j < barraVeiculos; j++) printf("■");
            for (int j = barraVeiculos; j < 25; j++) printf(" "); // Espaços para alinhar
            
            printf(" | %8.2f | ", estatisticas[hora].valorTotal);
            
            // Gráfico de valor
            for (int j = 0; j < barraValor; j++) printf("■");
            
            printf("\n");
        }
    }
    
    // Estatísticas gerais
    printf("\n=== RESUMO ESTATÍSTICO ===\n");
    printf("Total de veículos analisados: %d\n", veiculosProcessados);
    printf("Valor total arrecadado: €%.2f\n", valorTotalGeral);
    
    // Identificar horas de pico
    int horaPicoVeiculos = 0; // Hora com mais veículos
    int horaPicoValor = 0;    // Hora com maior valor arrecadado
    
    for (int i = 1; i < 24; i++) {
        if (estatisticas[i].totalVeiculos > estatisticas[horaPicoVeiculos].totalVeiculos) {
            horaPicoVeiculos = i;
        }
        if (estatisticas[i].valorTotal > estatisticas[horaPicoValor].valorTotal) {
            horaPicoValor = i;
        }
    }
    
    printf("\nHora de pico (mais saídas): %02dh (%d veículos)\n", 
           horaPicoVeiculos, estatisticas[horaPicoVeiculos].totalVeiculos);
    printf("Hora de pico (maior valor): %02dh (€%.2f)\n", 
           horaPicoValor, estatisticas[horaPicoValor].valorTotal);
    
    // Opção para exportar dados
    printf("\nDeseja exportar estes dados para um ficheiro? (S/N): ");
    char exportar;
    scanf(" %c", &exportar);
    getchar();
    
    if (toupper(exportar) == 'S') {
        char nomeFicheiro[100];
        printf("Nome do ficheiro [estatisticas_hora.txt]: ");
        fgets(nomeFicheiro, 100, stdin);
        nomeFicheiro[strcspn(nomeFicheiro, "\n")] = 0; // Remover newline
        
        // Usar nome padrão se entrada estiver vazia
        if (strlen(nomeFicheiro) == 0) {
            strcpy(nomeFicheiro, "estatisticas_hora.txt");
        }
        
        // Criar e escrever no ficheiro
        FILE *file = fopen(nomeFicheiro, "w");
        if (file != NULL) {
            // Cabeçalho
            fprintf(file, "ESTATÍSTICAS POR HORA - SISTEMA DE ESTACIONAMENTO\n");
            fprintf(file, "================================================\n\n");
            
            // Dados em formato CSV
            fprintf(file, "Hora;Veiculos;Valor(€)\n");
            for (int i = 0; i < 24; i++) {
                fprintf(file, "%02d;%d;%.2f\n", 
                        i, estatisticas[i].totalVeiculos, estatisticas[i].valorTotal);
            }
            
            // Resumo
            fprintf(file, "\nRESUMO:\n");
            fprintf(file, "Total de veículos: %d\n", veiculosProcessados);
            fprintf(file, "Valor total: %.2f€\n", valorTotalGeral);
            fprintf(file, "Hora de pico (veículos): %02dh (%d veículos)\n", 
                    horaPicoVeiculos, estatisticas[horaPicoVeiculos].totalVeiculos);
            fprintf(file, "Hora de pico (valor): %02dh (%.2f€)\n", 
                    horaPicoValor, estatisticas[horaPicoValor].valorTotal);
            
            fclose(file);
            printf("Dados exportados para '%s'\n", nomeFicheiro);
        } else {
            printf("Erro ao criar o ficheiro.\n");
        }
    }
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// Continuação do ficheiro estatisticas.c

// -----------------------------------------------------------------------------
// FUNÇÃO: gerarTabelaDinamica() - EXTRA E2
// -----------------------------------------------------------------------------

/**
 * Gera e exibe uma tabela dinâmica com dados de saídas por dia/mês.
 * Esta função implementa o extra E2 do enunciado:
 * - Lista veículos que saíram num intervalo de datas especificado
 * - Agrupa dados por mês e dia
 * - Mostra número total de saídas e valor total arrecadado
 * - Oferece opções de filtro e exportação
 * 
 * @note A tabela permite análise temporal dos dados.
 * @note Oferece visualização completa ou filtrada por mês específico.
 */
void gerarTabelaDinamica() {
    limparEcra();
    printf("=========================================\n");
    printf("       TABELA DINÂMICA (EXTRA E2)        \n");
    printf("=========================================\n");
    
    // Verificar se existem dados para análise
    if (listaEstacionamentos.total == 0) {
        printf("Não existem estacionamentos registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Array com nomes abreviados dos meses (português)
    char *nomesMeses[] = {
        "JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
        "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"
    };
    
    // Estrutura para armazenar dados por dia (máximo: 366 dias num ano bissexto)
    #define MAX_DIAS 366
    DadosTabelaDinamica dadosDias[MAX_DIAS];
    int numDiasUnicos = 0;  // Contador de dias com dados
    
    // Estrutura para totais por mês
    TotalMes totaisMeses[12] = {0};
    int mesesPresentes[12] = {0};  // Array para marcar meses com dados (0 = sem dados, 1 = com dados)
    
    // Inicializar estruturas de dados
    for (int i = 0; i < MAX_DIAS; i++) {
        dadosDias[i].dia = 0;
        dadosDias[i].numSaidas = 0;
        dadosDias[i].valorTotal = 0.0f;
    }
    
    // Coletar dados dos estacionamentos finalizados
    int veiculosProcessados = 0;
    
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
        
        // Apenas considerar veículos que já saíram (com data de saída válida)
        if (e->ativo == 0 && e->saida.ano != 0) {
            int mes = e->saida.mes - 1;  // Converter para índice 0-based
            int dia = e->saida.dia;
            
            // Buscar se este dia já está registado nos dados
            int diaEncontrado = 0;
            for (int j = 0; j < numDiasUnicos && !diaEncontrado; j++) {
                if (strcmp(dadosDias[j].mes, nomesMeses[mes]) == 0 && 
                    dadosDias[j].dia == dia) {
                    // Dia já existe - atualizar estatísticas
                    dadosDias[j].numSaidas++;
                    dadosDias[j].valorTotal += e->valorPago;
                    diaEncontrado = 1;
                }
            }
            
            if (!diaEncontrado) {
                // Novo dia - adicionar à estrutura
                strcpy(dadosDias[numDiasUnicos].mes, nomesMeses[mes]);
                dadosDias[numDiasUnicos].dia = dia;
                dadosDias[numDiasUnicos].numSaidas = 1;
                dadosDias[numDiasUnicos].valorTotal = e->valorPago;
                numDiasUnicos++;
            }
            
            // Atualizar totais do mês correspondente
            totaisMeses[mes].totalSaidas++;
            totaisMeses[mes].totalValor += e->valorPago;
            strcpy(totaisMeses[mes].mes, nomesMeses[mes]);
            mesesPresentes[mes] = 1;  // Marcar mês como presente
            
            veiculosProcessados++;
        }
    }
    
    // Verificar se há dados suficientes
    if (veiculosProcessados == 0) {
        printf("Não existem veículos que já tenham saído do parque.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Ordenar dados por mês e dia (bubble sort simples)
    for (int i = 0; i < numDiasUnicos - 1; i++) {
        for (int j = 0; j < numDiasUnicos - i - 1; j++) {
            // Converter mês para número para ordenação (JAN=0, FEV=1, etc.)
            int mes1 = -1, mes2 = -1;
            for (int m = 0; m < 12; m++) {
                if (strcmp(dadosDias[j].mes, nomesMeses[m]) == 0) mes1 = m;
                if (strcmp(dadosDias[j+1].mes, nomesMeses[m]) == 0) mes2 = m;
            }
            
            // Ordenar primeiro por mês (cronológico)
            if (mes1 > mes2) {
                DadosTabelaDinamica temp = dadosDias[j];
                dadosDias[j] = dadosDias[j+1];
                dadosDias[j+1] = temp;
            }
            // Se mesmo mês, ordenar por dia
            else if (mes1 == mes2 && dadosDias[j].dia > dadosDias[j+1].dia) {
                DadosTabelaDinamica temp = dadosDias[j];
                dadosDias[j] = dadosDias[j+1];
                dadosDias[j+1] = temp;
            }
        }
    }
    
    // Menu de opções de visualização
    printf("\nEscolha o tipo de visualização:\n");
    printf("1 - Todos os meses\n");
    printf("2 - Mês específico\n");
    printf("Escolha: ");
    
    int opcao;
    if (scanf("%d", &opcao) != 1 || (opcao != 1 && opcao != 2)) {
        printf("Opção inválida!\n");
        getchar();
        return;
    }
    getchar();
    
    // Variáveis para filtro por mês
    char mesEscolhido[4] = "";
    int mesNum = -1;
    
    if (opcao == 2) {
        // Solicitar mês específico ao utilizador
        printf("\nEscolha o mês (JAN, FEV, MAR, ABR, MAI, JUN, JUL, AGO, SET, OUT, NOV, DEZ): ");
        scanf("%3s", mesEscolhido);
        getchar();
        
        // Converter entrada para maiúsculas
        for (int i = 0; i < 3; i++) {
            mesEscolhido[i] = toupper(mesEscolhido[i]);
        }
        
        // Validar mês informado
        mesNum = -1;
        for (int i = 0; i < 12; i++) {
            if (strcmp(mesEscolhido, nomesMeses[i]) == 0) {
                mesNum = i;
                break;
            }
        }
        
        if (mesNum == -1) {
            printf("Mês inválido!\n");
            printf("Pressione Enter para continuar...");
            getchar();
            return;
        }
        
        // Verificar se há dados para o mês escolhido
        if (mesesPresentes[mesNum] == 0) {
            printf("Não há dados para o mês %s.\n", mesEscolhido);
            printf("Pressione Enter para continuar...");
            getchar();
            return;
        }
    }
    
    // Mostrar tabela
    limparEcra();
    printf("======================================================\n");
    printf("               TABELA DINÂMICA (EXTRA E2)             \n");
    printf("======================================================\n");
    
    // Indicar filtro aplicado
    if (opcao == 2) {
        printf("Mês filtrado: %s\n", mesEscolhido);
    }
    
    // Cabeçalho da tabela principal
    printf("\n%-8s | %-8s | %-12s | %-15s\n", 
           "MÊS", "DIA", "Nº SAÍDAS", "VALOR TOTAL (€)");
    printf("----------|----------|--------------|-----------------\n");
    
    // Exibir dados ordenados
    float valorTotalGeral = 0.0f;
    int saidasTotalGeral = 0;
    char mesAtual[4] = "";
    int primeiroDoMes = 1;  // Flag para controlar separadores entre meses
    
    for (int i = 0; i < numDiasUnicos; i++) {
        // Aplicar filtro se necessário
        if (opcao == 2 && strcmp(dadosDias[i].mes, mesEscolhido) != 0) {
            continue;
        }
        
        // Verificar se mudou de mês para exibir linha separadora
        if (strcmp(dadosDias[i].mes, mesAtual) != 0) {
            if (!primeiroDoMes) {
                // Exibir linha separadora entre meses
                printf("----------|----------|--------------|-----------------\n");
            }
            strcpy(mesAtual, dadosDias[i].mes);
            primeiroDoMes = 0;
        }
        
        // Exibir linha da tabela
        printf("%-8s | %-8d | %-12d | %-15.2f\n",
               dadosDias[i].mes,
               dadosDias[i].dia,
               dadosDias[i].numSaidas,
               dadosDias[i].valorTotal);
        
        // Atualizar totais gerais
        saidasTotalGeral += dadosDias[i].numSaidas;
        valorTotalGeral += dadosDias[i].valorTotal;
    }
    
    // Tabela de totais por mês
    printf("\n========== TOTAIS POR MÊS ==========\n");
    printf("%-8s | %-12s | %-15s\n", "MÊS", "TOTAL SAÍDAS", "VALOR TOTAL (€)");
    printf("----------|--------------|-----------------\n");
    
    for (int i = 0; i < 12; i++) {
        if (mesesPresentes[i] == 1) {
            // Aplicar filtro se necessário
            if (opcao == 2 && i != mesNum) {
                continue;
            }
            
            printf("%-8s | %-12d | %-15.2f\n",
                   nomesMeses[i],
                   totaisMeses[i].totalSaidas,
                   totaisMeses[i].totalValor);
        }
    }
    
    // Estatísticas gerais e análises
    printf("\n========== ESTATÍSTICAS GERAIS ==========\n");
    printf("Total de veículos analisados: %d\n", veiculosProcessados);
    printf("Total de saídas registadas: %d\n", saidasTotalGeral);
    printf("Valor total arrecadado: €%.2f\n", valorTotalGeral);
    
    if (veiculosProcessados > 0) {
        printf("Média por veículo: €%.2f\n", valorTotalGeral / veiculosProcessados);
        
        // Encontrar dia com mais saídas
        int maxSaidas = 0;
        char diaMaxMes[4] = "";
        int diaMaxDia = 0;
        float valorMaxDia = 0.0f;
        
        for (int i = 0; i < numDiasUnicos; i++) {
            if (dadosDias[i].numSaidas > maxSaidas) {
                maxSaidas = dadosDias[i].numSaidas;
                strcpy(diaMaxMes, dadosDias[i].mes);
                diaMaxDia = dadosDias[i].dia;
                valorMaxDia = dadosDias[i].valorTotal;
            }
        }
        
        if (maxSaidas > 0) {
            printf("\nDia com mais saídas: %s dia %d\n", 
                   diaMaxMes, diaMaxDia);
            printf("  Nº de saídas: %d\n", maxSaidas);
            printf("  Valor arrecadado: €%.2f\n", valorMaxDia);
        }
    }
    
    // Opção para exportar dados para ficheiro
    printf("\nDeseja exportar estes dados para um ficheiro? (S/N): ");
    char exportar;
    scanf(" %c", &exportar);
    getchar();
    
    if (toupper(exportar) == 'S') {
        char nomeFicheiro[100];
        printf("Nome do ficheiro [tabela_dinamica.txt]: ");
        fgets(nomeFicheiro, 100, stdin);
        nomeFicheiro[strcspn(nomeFicheiro, "\n")] = 0;  // Remover newline
        
        // Usar nome padrão se entrada estiver vazia
        if (strlen(nomeFicheiro) == 0) {
            strcpy(nomeFicheiro, "tabela_dinamica.txt");
        }
        
        // Criar ficheiro e escrever dados
        FILE *file = fopen(nomeFicheiro, "w");
        if (file != NULL) {
            // Cabeçalho do ficheiro
            fprintf(file, "TABELA DINÂMICA - SISTEMA DE ESTACIONAMENTO\n");
            fprintf(file, "===========================================\n\n");
            
            // Indicar filtro se aplicado
            if (opcao == 2) {
                fprintf(file, "Mês: %s\n\n", mesEscolhido);
            }
            
            // Dados por dia (formato CSV para fácil importação)
            fprintf(file, "DADOS POR DIA:\n");
            fprintf(file, "MÊS\tDIA\tSAÍDAS\tVALOR(€)\n");
            
            for (int i = 0; i < numDiasUnicos; i++) {
                // Aplicar filtro se necessário
                if (opcao == 2 && strcmp(dadosDias[i].mes, mesEscolhido) != 0) {
                    continue;
                }
                
                fprintf(file, "%s\t%d\t%d\t%.2f\n",
                       dadosDias[i].mes,
                       dadosDias[i].dia,
                       dadosDias[i].numSaidas,
                       dadosDias[i].valorTotal);
            }
            
            // Totais por mês
            fprintf(file, "\n\nTOTAIS POR MÊS:\n");
            fprintf(file, "MÊS\tSAÍDAS\tVALOR(€)\n");
            
            for (int i = 0; i < 12; i++) {
                if (mesesPresentes[i] == 1) {
                    // Aplicar filtro se necessário
                    if (opcao == 2 && i != mesNum) {
                        continue;
                    }
                    
                    fprintf(file, "%s\t%d\t%.2f\n",
                           nomesMeses[i],
                           totaisMeses[i].totalSaidas,
                           totaisMeses[i].totalValor);
                }
            }
            
            // Estatísticas gerais
            fprintf(file, "\n\nESTATÍSTICAS GERAIS:\n");
            fprintf(file, "Total de veículos analisados: %d\n", veiculosProcessados);
            fprintf(file, "Total de saídas registadas: %d\n", saidasTotalGeral);
            fprintf(file, "Valor total arrecadado: %.2f€\n", valorTotalGeral);
            
            if (veiculosProcessados > 0) {
                fprintf(file, "Média por veículo: %.2f€\n", 
                       valorTotalGeral / veiculosProcessados);
            }
            
            fclose(file);
            printf("Dados exportados para '%s'\n", nomeFicheiro);
        } else {
            printf("Erro ao criar o ficheiro.\n");
        }
    }
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: mostrarEstatisticasPorPeriodo()
// -----------------------------------------------------------------------------

/**
 * Mostra estatísticas para um período específico definido pelo utilizador.
 * Esta função permite análise temporal detalhada de entrada/saída de veículos.
 * 
 * @note Funcionalidade avançada para análise temporal.
 * @note Calcula taxas de ocupação e identifica dias de pico.
 * @note Oferece estatísticas diárias detalhadas.
 */
void mostrarEstatisticasPorPeriodo() {
    limparEcra();
    printf("=========================================\n");
    printf("       ESTATÍSTICAS POR PERÍODO          \n");
    printf("=========================================\n");
    
    // Verificar se existem dados para análise
    if (listaEstacionamentos.total == 0) {
        printf("Não existem estacionamentos registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Solicitar período de análise ao utilizador
    DataHora dataInicio, dataFim;
    
    printf("Digite a data inicial do período (dd mm aaaa): ");
    if (scanf("%d %d %d", &dataInicio.dia, &dataInicio.mes, &dataInicio.ano) != 3) {
        printf("Data inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    printf("Digite a data final do período (dd mm aaaa): ");
    if (scanf("%d %d %d", &dataFim.dia, &dataFim.mes, &dataFim.ano) != 3) {
        printf("Data inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // Configurar horas para incluir dias completos (00:00 a 23:59)
    dataInicio.hora = 0;
    dataInicio.minuto = 0;
    dataFim.hora = 23;
    dataFim.minuto = 59;
    
    // Inicializar variáveis para estatísticas do período
    int totalEntradas = 0;      // Veículos que entraram no período
    int totalSaidas = 0;        // Veículos que saíram no período
    float valorTotal = 0.0f;    // Valor arrecadado no período
    int veiculosAtivosNoPeriodo = 0;  // Veículos que entraram e ainda estão no parque
    
    // Arrays para estatísticas diárias detalhadas
    int entradasPorDia[32] = {0};    // Índice 1-31 para dias do mês
    int saidasPorDia[32] = {0};
    float valorPorDia[32] = {0.0f};
    
    // Processar todos os estacionamentos
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
        
        // Verificar se a entrada ocorreu dentro do período analisado
        if (compararDatas(e->entrada, dataInicio) >= 0 && 
            compararDatas(e->entrada, dataFim) <= 0) {
            totalEntradas++;
            
            // Contabilizar por dia específico
            if (e->entrada.dia >= 1 && e->entrada.dia <= 31) {
                entradasPorDia[e->entrada.dia]++;
            }
        }
        
        // Verificar se a saída ocorreu dentro do período (apenas para veículos que já saíram)
        if (e->ativo == 0 && e->saida.ano != 0) {
            if (compararDatas(e->saida, dataInicio) >= 0 && 
                compararDatas(e->saida, dataFim) <= 0) {
                totalSaidas++;
                valorTotal += e->valorPago;
                
                // Contabilizar por dia específico
                if (e->saida.dia >= 1 && e->saida.dia <= 31) {
                    saidasPorDia[e->saida.dia]++;
                    valorPorDia[e->saida.dia] += e->valorPago;
                }
            }
        }
        
        // Contabilizar veículos ativos que entraram antes ou durante o período
        // e ainda estão no parque no final do período
        if (e->ativo == 1) {
            // Se o veículo entrou antes do fim do período e não saiu
            if (compararDatas(e->entrada, dataFim) <= 0) {
                veiculosAtivosNoPeriodo++;
            }
        }
    }
    
    // Calcular número de dias no período (considerando datas completas)
    int diasNoPeriodo = 1;  // Mínimo 1 dia
    
    // Usar funções de tempo para cálculo preciso
    struct tm inicio_tm = {0};
    struct tm fim_tm = {0};
    
    inicio_tm.tm_year = dataInicio.ano - 1900;
    inicio_tm.tm_mon = dataInicio.mes - 1;
    inicio_tm.tm_mday = dataInicio.dia;
    
    fim_tm.tm_year = dataFim.ano - 1900;
    fim_tm.tm_mon = dataFim.mes - 1;
    fim_tm.tm_mday = dataFim.dia;
    
    time_t inicio_time = mktime(&inicio_tm);
    time_t fim_time = mktime(&fim_tm);
    
    if (inicio_time != -1 && fim_time != -1) {
        double diferenca = difftime(fim_time, inicio_time);
        diasNoPeriodo = (int)(diferenca / (60 * 60 * 24)) + 1;
        if (diasNoPeriodo < 1) diasNoPeriodo = 1;
    }
    
    // Apresentar resultados
    limparEcra();
    printf("=========================================\n");
    printf("  ESTATÍSTICAS DO PERÍODO SELECIONADO    \n");
    printf("=========================================\n");
    printf("Período: %02d/%02d/%04d a %02d/%02d/%04d\n",
           dataInicio.dia, dataInicio.mes, dataInicio.ano,
           dataFim.dia, dataFim.mes, dataFim.ano);
    printf("Dias no período: %d\n\n", diasNoPeriodo);
    
    // Resumo geral
    printf("=== RESUMO GERAL ===\n");
    printf("Total de entradas: %d\n", totalEntradas);
    printf("Total de saídas: %d\n", totalSaidas);
    printf("Valor total arrecadado: €%.2f\n", valorTotal);
    printf("Veículos ativos no final do período: %d\n\n", veiculosAtivosNoPeriodo);
    
    // Médias diárias
    if (diasNoPeriodo > 0) {
        printf("Média diária:\n");
        printf("  Entradas: %.1f\n", (float)totalEntradas / diasNoPeriodo);
        printf("  Saídas: %.1f\n", (float)totalSaidas / diasNoPeriodo);
        printf("  Valor arrecadado: €%.2f\n\n", valorTotal / diasNoPeriodo);
    }
    
    // Atividade detalhada por dia
    printf("=== ATIVIDADE POR DIA ===\n");
    printf("DIA  | ENTRADAS | SAÍDAS | VALOR (€)\n");
    printf("-----|----------|--------|----------\n");
    
    int diasComAtividade = 0;
    for (int dia = 1; dia <= 31; dia++) {
        if (entradasPorDia[dia] > 0 || saidasPorDia[dia] > 0) {
            printf("%4d | %8d | %6d | %9.2f\n",
                   dia, entradasPorDia[dia], saidasPorDia[dia], valorPorDia[dia]);
            diasComAtividade++;
        }
    }
    
    if (diasComAtividade == 0) {
        printf("Nenhuma atividade registada nos dias deste período.\n");
    }
    
    // Estatísticas avançadas e análises
    printf("\n=== ESTATÍSTICAS ADICIONAIS ===\n");
    
    // Identificar dia com mais entradas
    int maxEntradas = 0, diaMaxEntradas = 0;
    for (int dia = 1; dia <= 31; dia++) {
        if (entradasPorDia[dia] > maxEntradas) {
            maxEntradas = entradasPorDia[dia];
            diaMaxEntradas = dia;
        }
    }
    if (diaMaxEntradas > 0) {
        printf("Dia com mais entradas: %02d (%d entradas)\n", diaMaxEntradas, maxEntradas);
    }
    
    // Identificar dia com mais saídas
    int maxSaidas = 0, diaMaxSaidas = 0;
    for (int dia = 1; dia <= 31; dia++) {
        if (saidasPorDia[dia] > maxSaidas) {
            maxSaidas = saidasPorDia[dia];
            diaMaxSaidas = dia;
        }
    }
    if (diaMaxSaidas > 0) {
        printf("Dia com mais saídas: %02d (%d saídas, €%.2f)\n", 
               diaMaxSaidas, maxSaidas, valorPorDia[diaMaxSaidas]);
    }
    
    // Identificar dia com maior faturação
    float maxValor = 0.0f;
    int diaMaxValor = 0;
    for (int dia = 1; dia <= 31; dia++) {
        if (valorPorDia[dia] > maxValor) {
            maxValor = valorPorDia[dia];
            diaMaxValor = dia;
        }
    }
    if (diaMaxValor > 0) {
        printf("Dia com maior faturação: %02d (€%.2f)\n", diaMaxValor, maxValor);
    }
    
    // Calcular taxa de ocupação média (simplificada)
    if (totalEntradas > 0 && diasNoPeriodo > 0) {
        // Calcular capacidade total do parque
        int capacidadeTotal = parqueDinamico.numPisos * 
                              parqueDinamico.numFilasPorPiso * 
                              parqueDinamico.numLugaresPorFila;
        
        if (capacidadeTotal > 0) {
            // Taxa de ocupação = (total de entradas) / (dias × capacidade) × 100
            float ocupacaoMedia = (float)totalEntradas / (diasNoPeriodo * capacidadeTotal) * 100;
            printf("Taxa de ocupação média: %.1f%%\n", ocupacaoMedia);
        }
    }
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// Continuação do ficheiro estatisticas.c

// -----------------------------------------------------------------------------
// FUNÇÃO: exportarEstatisticas() - EXTRA E3 (parte)
// -----------------------------------------------------------------------------

/**
 * Exporta dados estatísticos para ficheiros de texto.
 * Esta função implementa parte do extra E3 do enunciado:
 * - Permite exportar diferentes tipos de estatísticas
 * - Cria ficheiros com timestamps para versionamento
 * - Formata dados em formato CSV para fácil importação
 * 
 * @note Oferece opções de exportação: gerais, por hora, tabela dinâmica, ou todas.
 * @note Inclui timestamps nos nomes dos ficheiros para rastreabilidade.
 * @note Formato CSV permite importação em Excel, Google Sheets, etc.
 */
void exportarEstatisticas() {
    limparEcra();
    printf("=========================================\n");
    printf("         EXPORTAR ESTATÍSTICAS           \n");
    printf("=========================================\n");
    
    // Verificar se existem dados para exportar
    if (listaEstacionamentos.total == 0) {
        printf("Não existem estacionamentos registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Menu de tipos de exportação
    int opcao;
    printf("\nEscolha o tipo de estatísticas a exportar:\n");
    printf("1 - Estatísticas Gerais\n");
    printf("2 - Estatísticas por Hora (Extra E1)\n");
    printf("3 - Tabela Dinâmica (Extra E2)\n");
    printf("4 - Todas as estatísticas (completo)\n");
    printf("Escolha: ");
    
    if (scanf("%d", &opcao) != 1 || opcao < 1 || opcao > 4) {
        printf("Opção inválida!\n");
        
        // Limpar buffer de entrada
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        getchar();
        return;
    }
    getchar();
    
    // Solicitar nome base para os ficheiros
    char nomeBase[100];
    printf("\nDigite o nome base para os ficheiros [estatisticas]: ");
    fgets(nomeBase, 100, stdin);
    nomeBase[strcspn(nomeBase, "\n")] = 0;  // Remover newline
    
    // Usar nome padrão se entrada estiver vazia
    if (strlen(nomeBase) == 0) {
        strcpy(nomeBase, "estatisticas");
    }
    
    // Obter data/hora atual para timestamp nos nomes dos ficheiros
    time_t agora = time(NULL);
    struct tm *tm_info = localtime(&agora);
    
    // Processar conforme opção escolhida
    if (opcao == 1 || opcao == 4) {
        // Exportar estatísticas gerais
        char nomeFicheiro[150];
        sprintf(nomeFicheiro, "%s_gerais_%04d%02d%02d.txt", 
                nomeBase, tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
        
        FILE *file = fopen(nomeFicheiro, "w");
        if (file != NULL) {
            // Cabeçalho do ficheiro
            fprintf(file, "ESTATÍSTICAS GERAIS - SISTEMA DE ESTACIONAMENTO\n");
            fprintf(file, "Exportado em: %04d-%02d-%02d %02d:%02d:%02d\n\n",
                    tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                    tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
            
            // Informações de configuração do parque
            fprintf(file, "=== CONFIGURAÇÃO DO PARQUE ===\n");
            fprintf(file, "Pisos: %d\n", parqueDinamico.numPisos);
            fprintf(file, "Filas por piso: %d\n", parqueDinamico.numFilasPorPiso);
            fprintf(file, "Lugares por fila: %d\n", parqueDinamico.numLugaresPorFila);
            fprintf(file, "Capacidade total: %d lugares\n\n", 
                    parqueDinamico.numPisos * parqueDinamico.numFilasPorPiso * parqueDinamico.numLugaresPorFila);
            
            // Contar lugares por estado atual
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
            
            int totalLugares = lugaresLivres + lugaresOcupados + lugaresIndisponiveis;
            
            // Estado atual do parque
            fprintf(file, "=== ESTADO ATUAL DO PARQUE ===\n");
            fprintf(file, "Lugares livres: %d (%.1f%%)\n", lugaresLivres,
                    totalLugares > 0 ? (float)lugaresLivres / totalLugares * 100 : 0);
            fprintf(file, "Lugares ocupados: %d (%.1f%%)\n", lugaresOcupados,
                    totalLugares > 0 ? (float)lugaresOcupados / totalLugares * 100 : 0);
            fprintf(file, "Lugares indisponíveis: %d (%.1f%%)\n\n", lugaresIndisponiveis,
                    totalLugares > 0 ? (float)lugaresIndisponiveis / totalLugares * 100 : 0);
            
            // Estatísticas dos estacionamentos
            int veiculosAtivos = 0, veiculosFinalizados = 0;
            float valorTotal = 0.0f, valorMaximo = 0.0f, valorMinimo = 999999.0f;
            char matriculaMax[MAX_MATRICULA] = "";
            char matriculaMin[MAX_MATRICULA] = "";
            
            // Analisar todos os estacionamentos
            for (int i = 0; i < listaEstacionamentos.total; i++) {
                if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
                    veiculosAtivos++;
                } else {
                    veiculosFinalizados++;
                    float valorAtual = listaEstacionamentos.estacionamentos[i].valorPago;
                    valorTotal += valorAtual;
                    
                    // Encontrar valores máximo e mínimo
                    if (valorAtual > valorMaximo) {
                        valorMaximo = valorAtual;
                        strcpy(matriculaMax, listaEstacionamentos.estacionamentos[i].matricula);
                    }
                    if (valorAtual < valorMinimo && valorAtual > 0) {
                        valorMinimo = valorAtual;
                        strcpy(matriculaMin, listaEstacionamentos.estacionamentos[i].matricula);
                    }
                }
            }
            
            // Escrever estatísticas de estacionamento
            fprintf(file, "=== ESTATÍSTICAS DE ESTACIONAMENTO ===\n");
            fprintf(file, "Total de registos: %d\n", listaEstacionamentos.total);
            fprintf(file, "Veículos atualmente no parque: %d\n", veiculosAtivos);
            fprintf(file, "Veículos que já saíram: %d\n", veiculosFinalizados);
            fprintf(file, "Valor total arrecadado: €%.2f\n", valorTotal);
            
            if (veiculosFinalizados > 0) {
                fprintf(file, "Média por veículo: €%.2f\n", valorTotal / veiculosFinalizados);
                
                // Estatísticas de extremos (máximo e mínimo)
                if (valorMaximo > 0) {
                    char matriculaExibicao[MAX_MATRICULA];
                    adicionarHifensMatricula(matriculaExibicao, matriculaMax);
                    fprintf(file, "Valor mais alto: €%.2f (Matrícula: %s)\n", 
                            valorMaximo, matriculaExibicao);
                }
                if (valorMinimo < 999999.0f) {
                    char matriculaExibicao[MAX_MATRICULA];
                    adicionarHifensMatricula(matriculaExibicao, matriculaMin);
                    fprintf(file, "Valor mais baixo: €%.2f (Matrícula: %s)\n", 
                            valorMinimo, matriculaExibicao);
                }
            }
            
            // Informações sobre tarifas atuais
            fprintf(file, "\n=== TARIFAS ATUAIS ===\n");
            fprintf(file, "T1 (Diurna): €%.2f por hora (8:00-22:00)\n", sistema.tarifaT1.valor);
            fprintf(file, "T2 (Noturna): €%.2f por hora (22:00-8:00)\n", sistema.tarifaT2.valor);
            fprintf(file, "T3 (Dia completo): €%.2f máximo por dia\n", sistema.tarifaT3.valor);
            fprintf(file, "T4 (Múltiplos dias): €%.2f por dia\n", sistema.tarifaT4.valor);
            
            fclose(file);
            printf("Estatísticas gerais exportadas para: %s\n", nomeFicheiro);
        } else {
            printf("Erro ao criar ficheiro %s\n", nomeFicheiro);
        }
    }
    
    if (opcao == 2 || opcao == 4) {
        // Exportar estatísticas por hora (Extra E1)
        char nomeFicheiro[150];
        sprintf(nomeFicheiro, "%s_por_hora_%04d%02d%02d.txt", 
                nomeBase, tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
        
        // Coletar dados de saídas por hora
        EstatisticaHora estatisticas[24] = {0};
        int veiculosProcessados = 0;
        float valorTotalGeral = 0.0f;
        
        // Inicializar array de horas
        for (int i = 0; i < 24; i++) {
            estatisticas[i].hora = i;
        }
        
        // Coletar dados dos estacionamentos
        for (int i = 0; i < listaEstacionamentos.total; i++) {
            Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
            
            // Apenas veículos que já saíram
            if (e->ativo == 0 && e->saida.ano != 0) {
                int horaSaida = e->saida.hora;
                
                // Validar hora (0-23)
                if (horaSaida >= 0 && horaSaida < 24) {
                    estatisticas[horaSaida].totalVeiculos++;
                    estatisticas[horaSaida].valorTotal += e->valorPago;
                    
                    veiculosProcessados++;
                    valorTotalGeral += e->valorPago;
                }
            }
        }
        
        // Se houver dados, criar ficheiro
        if (veiculosProcessados > 0) {
            FILE *file = fopen(nomeFicheiro, "w");
            if (file != NULL) {
                // Cabeçalho
                fprintf(file, "ESTATÍSTICAS POR HORA - SISTEMA DE ESTACIONAMENTO\n");
                fprintf(file, "Exportado em: %04d-%02d-%02d %02d:%02d:%02d\n\n",
                        tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
                
                // Dados em formato CSV (Excel-friendly)
                fprintf(file, "HORA;VEICULOS;VALOR(€);PERCENTUAL_VEICULOS;PERCENTUAL_VALOR\n");
                
                for (int i = 0; i < 24; i++) {
                    // Calcular percentuais
                    float percentualVeiculos = veiculosProcessados > 0 ? 
                        (float)estatisticas[i].totalVeiculos / veiculosProcessados * 100 : 0;
                    float percentualValor = valorTotalGeral > 0 ? 
                        estatisticas[i].valorTotal / valorTotalGeral * 100 : 0;
                    
                    fprintf(file, "%02d;%d;%.2f;%.1f;%.1f\n",
                            i, estatisticas[i].totalVeiculos, estatisticas[i].valorTotal,
                            percentualVeiculos, percentualValor);
                }
                
                // Identificar horas de pico
                int horaPicoVeiculos = 0; // Hora com mais veículos
                int horaPicoValor = 0;    // Hora com maior valor
                
                for (int i = 1; i < 24; i++) {
                    if (estatisticas[i].totalVeiculos > estatisticas[horaPicoVeiculos].totalVeiculos) {
                        horaPicoVeiculos = i;
                    }
                    if (estatisticas[i].valorTotal > estatisticas[horaPicoValor].valorTotal) {
                        horaPicoValor = i;
                    }
                }
                
                // Resumo estatístico
                fprintf(file, "\n=== RESUMO ===\n");
                fprintf(file, "Total de veículos analisados: %d\n", veiculosProcessados);
                fprintf(file, "Valor total arrecadado: €%.2f\n", valorTotalGeral);
                fprintf(file, "Hora de pico (mais saídas): %02dh (%d veículos)\n", 
                        horaPicoVeiculos, estatisticas[horaPicoVeiculos].totalVeiculos);
                fprintf(file, "Hora de pico (maior valor): %02dh (€%.2f)\n", 
                        horaPicoValor, estatisticas[horaPicoValor].valorTotal);
                
                fclose(file);
                printf("Estatísticas por hora exportadas para: %s\n", nomeFicheiro);
            } else {
                printf("Erro ao criar ficheiro %s\n", nomeFicheiro);
            }
        }
    }
    
    if (opcao == 3 || opcao == 4) {
        // Exportar tabela dinâmica (Extra E2)
        char nomeFicheiro[150];
        sprintf(nomeFicheiro, "%s_tabela_dinamica_%04d%02d%02d.txt", 
                nomeBase, tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday);
        
        // Coletar dados para tabela dinâmica
        char *nomesMeses[] = {
            "JAN", "FEV", "MAR", "ABR", "MAI", "JUN",
            "JUL", "AGO", "SET", "OUT", "NOV", "DEZ"
        };
        
        DadosTabelaDinamica dadosDias[366];  // Máximo dias num ano bissexto
        int numDiasUnicos = 0;
        TotalMes totaisMeses[12] = {0};
        int mesesPresentes[12] = {0};
        int veiculosProcessados = 0;
        
        // Processar estacionamentos para coletar dados diários
        for (int i = 0; i < listaEstacionamentos.total; i++) {
            Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
            
            // Apenas veículos que já saíram
            if (e->ativo == 0 && e->saida.ano != 0) {
                int mes = e->saida.mes - 1;  // Converter para 0-based
                int dia = e->saida.dia;
                
                // Verificar se este dia já está registado
                int diaEncontrado = 0;
                for (int j = 0; j < numDiasUnicos && !diaEncontrado; j++) {
                    if (strcmp(dadosDias[j].mes, nomesMeses[mes]) == 0 && 
                        dadosDias[j].dia == dia) {
                        // Dia já existe - atualizar
                        dadosDias[j].numSaidas++;
                        dadosDias[j].valorTotal += e->valorPago;
                        diaEncontrado = 1;
                    }
                }
                
                if (!diaEncontrado) {
                    // Novo dia - adicionar
                    strcpy(dadosDias[numDiasUnicos].mes, nomesMeses[mes]);
                    dadosDias[numDiasUnicos].dia = dia;
                    dadosDias[numDiasUnicos].numSaidas = 1;
                    dadosDias[numDiasUnicos].valorTotal = e->valorPago;
                    numDiasUnicos++;
                }
                
                // Atualizar totais mensais
                totaisMeses[mes].totalSaidas++;
                totaisMeses[mes].totalValor += e->valorPago;
                strcpy(totaisMeses[mes].mes, nomesMeses[mes]);
                mesesPresentes[mes] = 1;
                
                veiculosProcessados++;
            }
        }
        
        // Se houver dados, criar ficheiro
        if (veiculosProcessados > 0) {
            FILE *file = fopen(nomeFicheiro, "w");
            if (file != NULL) {
                // Cabeçalho
                fprintf(file, "TABELA DINÂMICA - SISTEMA DE ESTACIONAMENTO\n");
                fprintf(file, "Exportado em: %04d-%02d-%02d %02d:%02d:%02d\n\n",
                        tm_info->tm_year + 1900, tm_info->tm_mon + 1, tm_info->tm_mday,
                        tm_info->tm_hour, tm_info->tm_min, tm_info->tm_sec);
                
                // Dados por dia (formato CSV)
                fprintf(file, "=== DADOS POR DIA ===\n");
                fprintf(file, "MES;DIA;SAIDAS;VALOR(€)\n");
                
                for (int i = 0; i < numDiasUnicos; i++) {
                    fprintf(file, "%s;%d;%d;%.2f\n",
                           dadosDias[i].mes, dadosDias[i].dia,
                           dadosDias[i].numSaidas, dadosDias[i].valorTotal);
                }
                
                // Totais por mês
                fprintf(file, "\n=== TOTAIS POR MÊS ===\n");
                fprintf(file, "MES;SAIDAS;VALOR(€);MEDIA_POR_SAIDA(€)\n");
                
                float valorTotalGeral = 0.0f;
                int saidasTotalGeral = 0;
                
                for (int i = 0; i < 12; i++) {
                    if (mesesPresentes[i] == 1) {
                        // Calcular média por saída neste mês
                        float mediaPorSaida = totaisMeses[i].totalSaidas > 0 ?
                            totaisMeses[i].totalValor / totaisMeses[i].totalSaidas : 0;
                        
                        fprintf(file, "%s;%d;%.2f;%.2f\n",
                               nomesMeses[i], totaisMeses[i].totalSaidas,
                               totaisMeses[i].totalValor, mediaPorSaida);
                        
                        // Acumular totais gerais
                        valorTotalGeral += totaisMeses[i].totalValor;
                        saidasTotalGeral += totaisMeses[i].totalSaidas;
                    }
                }
                
                // Resumo geral
                fprintf(file, "\n=== RESUMO GERAL ===\n");
                fprintf(file, "Total de veículos analisados: %d\n", veiculosProcessados);
                fprintf(file, "Total de saídas registadas: %d\n", saidasTotalGeral);
                fprintf(file, "Valor total arrecadado: €%.2f\n", valorTotalGeral);
                
                if (saidasTotalGeral > 0) {
                    fprintf(file, "Média por saída: €%.2f\n", valorTotalGeral / saidasTotalGeral);
                }
                
                // Identificar mês com mais atividade
                int maxSaidas = 0;
                char mesMaxSaidas[4] = "";
                float valorMaxMes = 0.0f;
                
                for (int i = 0; i < 12; i++) {
                    if (totaisMeses[i].totalSaidas > maxSaidas) {
                        maxSaidas = totaisMeses[i].totalSaidas;
                        strcpy(mesMaxSaidas, nomesMeses[i]);
                        valorMaxMes = totaisMeses[i].totalValor;
                    }
                }
                
                if (maxSaidas > 0) {
                    fprintf(file, "\nMês com mais atividade: %s\n", mesMaxSaidas);
                    fprintf(file, "  Saídas: %d\n", maxSaidas);
                    fprintf(file, "  Valor: €%.2f\n", valorMaxMes);
                }
                
                fclose(file);
                printf("Tabela dinâmica exportada para: %s\n", nomeFicheiro);
            } else {
                printf("Erro ao criar ficheiro %s\n", nomeFicheiro);
            }
        }
    }
    
    // Resumo da operação
    if (opcao == 4) {
        printf("\nTodas as estatísticas foram exportadas com sucesso!\n");
    }
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: estatisticas() - MENU PRINCIPAL DE ESTATÍSTICAS
// -----------------------------------------------------------------------------

/**
 * Menu principal para o módulo de estatísticas.
 * Apresenta todas as opções disponíveis de análise e relatórios.
 * Coordena a navegação entre as diferentes funcionalidades estatísticas.
 * 
 * @note Este é o ponto de entrada para o módulo de estatísticas.
 * @note Implementa funcionalidade 11 do enunciado (pelo menos uma listagem).
 * @note Inclui extras E1, E2, E3 como opções específicas.
 */
void estatisticas() {
    int opcao;
    
    do {
        limparEcra();
        printf("=========================================\n");
        printf("          ESTATÍSTICAS DO SISTEMA        \n");
        printf("=========================================\n");

        // Mostrar lugares disponíveis conforme funcionalidade 3
        exibirLugaresDisponiveis();

        // Menu de opções de estatísticas
        printf("\n1 - Estatísticas Gerais\n");
        printf("2 - Estatísticas por Piso\n");
        printf("3 - Estatísticas por Hora (Gráfico - Extra E1)\n");
        printf("4 - Tabela Dinâmica (Extra E2)\n");
        printf("5 - Estatísticas por Período\n");
        printf("6 - Exportar Estatísticas (Extra E3)\n");
        printf("0 - Voltar ao Menu Principal\n");
        printf("=========================================\n");
        printf("Escolha: ");
        
        // Ler e validar opção
        if (scanf("%d", &opcao) != 1) {
            printf("Opção inválida!\n");
            
            // Limpar buffer de entrada
            while (getchar() != '\n');
            continue;
        }
        getchar();  // Limpar newline
        
        // Processar opção escolhida
        switch (opcao) {
            case 1: 
                mostrarEstatisticasGerais(); 
                break;
            case 2: 
                mostrarEstatisticasPorPiso(); 
                break;
            case 3: 
                gerarGraficoBarras();  // Extra E1
                break;
            case 4: 
                gerarTabelaDinamica(); // Extra E2
                break;
            case 5: 
                mostrarEstatisticasPorPeriodo(); 
                break;
            case 6: 
                exportarEstatisticas(); // Extra E3 (parte)
                break;
            case 0: 
                break;  // Sair do menu
            default: 
                printf("Opção inválida! Pressione Enter...");
                getchar();
        }
    } while (opcao != 0);  // Continuar até o utilizador escolher sair
}