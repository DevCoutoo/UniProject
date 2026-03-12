// ficheiros.c - Módulo de Exportação de Dados
// Autor: Rodrigo Couto
// Descrição: Contém funções para exportar dados do sistema para formatos
//           de ficheiro texto (TXT) e CSV, conforme especificado nas
//           funcionalidades 10 e E3 do enunciado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "ficheiros.h"
#include "datastructs.h"
#include "utils.h"
#include "estacionamentos.h"  // Adicionado para aceder a listaEstacionamentos
#include "tarifas.h"

// -----------------------------------------------------------------------------
// DECLARAÇÕES EXTERNAS DE VARIÁVEIS GLOBAIS
// -----------------------------------------------------------------------------

/**
 * Variável global que contém a estrutura dinâmica do parque.
 * Declarada externamente para aceder à configuração do parque.
 */
extern ParqueDinamico parqueDinamico;

/**
 * Variável global que contém a lista dinâmica de todos os estacionamentos.
 * Declarada externamente para exportar os dados de estacionamento.
 */
extern ListaEstacionamentos listaEstacionamentos;

/**
 * Variável global que contém as tarifas do sistema.
 * Declarada externamente para referência em cálculos de valor.
 */
extern TarifasSistema sistema;

// -----------------------------------------------------------------------------
// FUNÇÃO: gerarTXT()
// -----------------------------------------------------------------------------

/**
 * Gera e salva uma listagem em formato de ficheiro de texto (.txt).
 * Esta função implementa parte da funcionalidade 10 do enunciado:
 * - Permite ao utilizador especificar o nome do ficheiro de saída
 * - Inclui um cabeçalho descritivo com informações sobre a listagem
 * - Formata os dados de forma legível para leitura com editores de texto simples
 * 
 * Funcionalidades implementadas:
 * - Exportação completa de todos os estacionamentos
 * - Formatação de matrículas com hífens para melhor legibilidade
 * - Cabeçalho descritivo com explicação dos campos
 * - Inclusão de observação sobre o estado do veículo
 * 
 * @note O ficheiro gerado pode ser lido por qualquer editor de texto simples.
 * @note Formato de saída: ver Figura 2 do enunciado (adaptado).
 */
void gerarTXT() {
    limparEcra();
    printf("=========================================\n");
    printf("     GERAR FICHEIRO DE TEXTO (TXT)       \n");
    printf("=========================================\n");
    
    // Abrir ficheiro para escrita
    FILE *file = fopen("estacionamentos.txt", "w");
    if (!file) {
        printf("Erro ao criar ficheiro: estacionamentos.txt\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Escrever cabeçalho formatado com larguras fixas
    // Formato: <numE>\t<matricula>\t<anoE>\t<mesE>\t<diaE>\t<horaE>\t<minuteE>\t<lugar>\t...
    fprintf(file, "%-6s\t%-12s\t%-6s\t%-4s\t%-4s\t%-5s\t%-7s\t%-8s\t%-6s\t%-4s\t%-4s\t%-5s\t%-6s\t%-10s\t%s\n",
            "<numE>", "<matricula>", "<anoE>", "<mesE>", "<diaE>", "<horaE>", "<minuteE>", "<lugar>",
            "<anoS>", "<mesS>", "<diaS>", "<horaS>", "<minS>", "<valorPago>", "<Obs>");
    
    int registosExportados = 0;
    
    // Percorrer todos os estacionamentos na lista dinâmica
    for(int i = 0; i < listaEstacionamentos.total; i++) {
        // Obter ponteiro para o estacionamento atual
        Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
        
        // Converter matrícula SEM hífens para COM hífens para melhor legibilidade no TXT
        char matriculaComHifens[MAX_MATRICULA];
        adicionarHifensMatricula(matriculaComHifens, e->matricula);
        
        // Determinar observação baseada no estado do veículo
        char obs[30];
        if (e->ativo) {
            strcpy(obs, "Veículo no parque");
        } else if (e->saida.ano == 0) {
            strcpy(obs, "Sem saída registada");
        } else {
            strcpy(obs, "Saída registada");
        }

        // Escrever linha formatada no ficheiro
        // Nota: Matrícula é escrita COM hífens no ficheiro TXT
        fprintf(file, "%-6d\t%-10s\t%-6d\t%-4d\t%-4d\t%-5d\t%-7d\t%-8s\t%-6d\t%-4d\t%-4d\t%-5d\t%-6d\t%-10.2f\t%s\n",
                e->id,
                matriculaComHifens,
                e->entrada.ano, e->entrada.mes, e->entrada.dia, e->entrada.hora, e->entrada.minuto,
                e->codigoLugar,
                e->saida.ano, e->saida.mes, e->saida.dia, e->saida.hora, e->saida.minuto,
                e->valorPago,
                obs);
        
        registosExportados++;
    }
    
    // Fechar ficheiro
    fclose(file);
    
    // Mostrar relatório da operação
    printf("Ficheiro estacionamentos.txt gerado com sucesso.\n");
    printf("Registos exportados: %d\n", registosExportados);
    
    if (registosExportados == 0) {
        printf("AVISO: Ficheiro gerado vazio (sem registos).\n");
    }
    
    // Explicar estrutura do ficheiro gerado
    printf("\nEstrutura do ficheiro TXT:\n");
    printf("1. <numE>: ID do estacionamento\n");
    printf("2. <matricula>: Matrícula do veículo (com hífens)\n");
    printf("3. <anoE>, <mesE>, <diaE>, <horaE>, <minuteE>: Data/hora de entrada\n");
    printf("4. <lugar>: Código do lugar ocupado\n");
    printf("5. <anoS>, <mesS>, <diaS>, <horaS>, <minS>: Data/hora de saída\n");
    printf("6. <valorPago>: Valor pago (€)\n");
    printf("7. <Obs>: Observação (estado do veículo)\n");
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: guardarCSV()
// -----------------------------------------------------------------------------

/**
 * Exporta dados para um ficheiro CSV (Comma-Separated Values).
 * Esta função implementa o extra E3 do enunciado:
 * - Permite ao utilizador escolher o separador (normalmente ',' ou ';')
 * - Inclui linha de cabeçalho com descrição dos campos
 * - Permite especificar o nome do ficheiro de saída
 * - Exporta dados de estacionamentos dentro de um intervalo de datas
 * - Ordena por valor pago (decrescente)
 * 
 * @note Formato CSV é ideal para importação em outras aplicações como:
 *       - Microsoft Excel
 *       - Google Sheets
 *       - Base de dados
 *       - Ferramentas de análise de dados
 * 
 * @note Esta função complementa a funcionalidade 10, oferecendo uma
 *       alternativa ao formato TXT para facilitar interoperabilidade.
 */
void guardarCSV() {
    limparEcra();
    printf("=========================================\n");
    printf("       EXPORTAÇÃO PARA FICHEIRO CSV      \n");
    printf("=========================================\n");
    
    // Verificar se existem dados para exportar
    if (listaEstacionamentos.total == 0) {
        printf("Não existem estacionamentos registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // Variáveis de configuração da exportação
    int opcaoFiltro = 2;          // Padrão: sem filtro (2 = Não)
    int opcaoData = 1;            // Padrão: filtrar por data de entrada (1)
    DataHora dataInicio = {0};    // Data inicial para filtro
    DataHora dataFim = {0};       // Data final para filtro
    char separador[3] = ";";      // Separador padrão: ponto e vírgula
    char nomeFicheiro[100] = "estacionamentos.csv";  // Nome padrão
    
    // 1. Perguntar sobre filtro de datas
    printf("Deseja filtrar por datas? (1-Sim | 2-Não): ");
    if (scanf("%d", &opcaoFiltro) == 1 && opcaoFiltro == 1) {
        getchar();  // Limpar buffer do teclado
        
        // Escolha do tipo de data para filtro
        printf("Filtrar por data de: (1-Entrada | 2-Saída): ");
        if (scanf("%d", &opcaoData) != 1 || (opcaoData != 1 && opcaoData != 2)) {
            printf("Opção inválida. Usando data de entrada por padrão.\n");
            opcaoData = 1;
        }
        getchar();
        
        // Obter data inicial do utilizador
        printf("Data inicial (dd mm aaaa): ");
        if (scanf("%d %d %d", &dataInicio.dia, &dataInicio.mes, &dataInicio.ano) != 3) {
            printf("Data inválida. Cancelando filtro.\n");
            opcaoFiltro = 2;  // Desativar filtro
        }
        getchar();
        
        if (opcaoFiltro == 1) {
            // Obter data final do utilizador
            printf("Data final (dd mm aaaa): ");
            if (scanf("%d %d %d", &dataFim.dia, &dataFim.mes, &dataFim.ano) != 3) {
                printf("Data inválida. Cancelando filtro.\n");
                opcaoFiltro = 2;
            }
            getchar();
            
            // Configurar horas para inclusão completa dos dias
            // Início do dia: 00:00, Fim do dia: 23:59
            dataInicio.hora = 0;
            dataInicio.minuto = 0;
            dataFim.hora = 23;
            dataFim.minuto = 59;
        }
    } else {
        getchar();  // Limpar buffer mesmo quando resposta é Não
    }
    
    // 2. Escolha do separador CSV
    printf("Separador a usar (',' ou ';') [padrão: ;]: ");
    fgets(separador, 3, stdin);
    if (separador[0] == '\n' || (separador[0] != ',' && separador[0] != ';')) {
        separador[0] = ';';  // Usar separador padrão se entrada inválida
    }
    separador[1] = '\0';  // Garantir terminação de string
    
    // 3. Obter nome do ficheiro do utilizador
    printf("Nome do ficheiro [padrão: estacionamentos.csv]: ");
    fgets(nomeFicheiro, 100, stdin);
    nomeFicheiro[strcspn(nomeFicheiro, "\n")] = 0;  // Remover newline
    
    // Usar nome padrão se entrada estiver vazia
    if (strlen(nomeFicheiro) == 0) {
        strcpy(nomeFicheiro, "estacionamentos.csv");
    }
    
    // Garantir que o ficheiro tem extensão .csv
    if (strstr(nomeFicheiro, ".csv") == NULL) {
        strcat(nomeFicheiro, ".csv");
    }
    
    // Abrir ficheiro para escrita
    FILE *file = fopen(nomeFicheiro, "w");
    if (!file) {
        printf("Erro ao criar ficheiro %s\n", nomeFicheiro);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    // 4. Escrever cabeçalho do CSV
    // Formato: Campo1<separador>Campo2<separador>...<separador>CampoN\n
    fprintf(file, "ID%cMatricula%cLugar%cData_Entrada%cHora_Entrada%cData_Saida%cHora_Saida%cDuracao_Minutos%cValor_Pago\n",
            separador[0], separador[0], separador[0], separador[0], 
            separador[0], separador[0], separador[0], separador[0]);
    
    // 5. Preparar array de índices para ordenação
    // Alocar memória para armazenar índices dos estacionamentos a exportar
    int *indices = malloc(listaEstacionamentos.total * sizeof(int));
    if (indices == NULL) {
        printf("Erro de memória ao preparar exportação.\n");
        fclose(file);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    int count = 0;              // Contador de registos filtrados
    int registosExportados = 0; // Contador de registos exportados
    
    // Fase 1: Filtrar estacionamentos conforme critérios especificados
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        // Apenas exportar estacionamentos finalizados (com saída registada)
        // Estacionamentos ativos (veículos ainda no parque) são ignorados
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) continue;
        if (listaEstacionamentos.estacionamentos[i].saida.ano == 0) continue;
        
        // Aplicar filtro de datas se ativado
        if (opcaoFiltro == 1) {
            DataHora dataVerificar;
            if (opcaoData == 1) {
                // Filtrar por data de entrada
                dataVerificar = listaEstacionamentos.estacionamentos[i].entrada;
            } else {
                // Filtrar por data de saída
                dataVerificar = listaEstacionamentos.estacionamentos[i].saida;
            }
            
            // Verificar se data está dentro do intervalo especificado
            // dataVerificar deve ser >= dataInicio E <= dataFim
            if (compararDatas(dataVerificar, dataInicio) < 0) continue;
            if (compararDatas(dataVerificar, dataFim) > 0) continue;
        }
        
        // Adicionar índice à lista de exportação
        indices[count] = i;
        count++;
    }
    
    // Fase 2: Ordenar por valorPago (decrescente) usando Bubble Sort
    // Implementação simples adequada para o volume de dados esperado
    for (int i = 0; i < count - 1; i++) {
        for (int j = 0; j < count - i - 1; j++) {
            // Comparar valores pagos dos estacionamentos nos índices j e j+1
            if (listaEstacionamentos.estacionamentos[indices[j]].valorPago < 
                listaEstacionamentos.estacionamentos[indices[j+1]].valorPago) {
                // Trocar índices para ordenação decrescente
                int temp = indices[j];
                indices[j] = indices[j+1];
                indices[j+1] = temp;
            }
        }
    }
    
    // Fase 3: Escrever dados filtrados e ordenados no ficheiro CSV
    for (int i = 0; i < count; i++) {
        // Obter ponteiro para o estacionamento atual
        Estacionamento *e = &listaEstacionamentos.estacionamentos[indices[i]];
        
        // Calcular duração total do estacionamento em minutos
        int duracao = calcularDiferencaMinutos(e->entrada, e->saida);
        
        // Converter matrícula para exibição com hífens (mais legível)
        char matriculaComHifens[MAX_MATRICULA];
        adicionarHifensMatricula(matriculaComHifens, e->matricula);
        
        // Escrever linha formatada no ficheiro CSV
        // Formato: ID;Matricula;Lugar;AAAA-MM-DD;HH:MM;AAAA-MM-DD;HH:MM;Minutos;Valor
        fprintf(file, "%d%c%s%c%s%c%04d-%02d-%02d%c%02d:%02d%c%04d-%02d-%02d%c%02d:%02d%c%d%c%.2f\n",
                e->id, separador[0],
                matriculaComHifens, separador[0],
                e->codigoLugar, separador[0],
                e->entrada.ano, e->entrada.mes, e->entrada.dia, separador[0],
                e->entrada.hora, e->entrada.minuto, separador[0],
                e->saida.ano, e->saida.mes, e->saida.dia, separador[0],
                e->saida.hora, e->saida.minuto, separador[0],
                duracao, separador[0],
                e->valorPago);
        
        registosExportados++;
    }
    
    // Fase 4: Libertar recursos e fechar ficheiro
    fclose(file);
    free(indices);
    
    // Mostrar relatório detalhado da exportação
    printf("\n=========================================\n");
    printf("EXPORTAÇÃO CSV CONCLUÍDA COM SUCESSO\n");
    printf("=========================================\n");
    printf("Ficheiro gerado: %s\n", nomeFicheiro);
    printf("Registos exportados: %d\n", registosExportados);
    printf("Separador utilizado: '%s'\n", separador);
    printf("Ordenação: Valor pago (decrescente)\n");
    
    // Mostrar informações sobre filtros aplicados
    if (opcaoFiltro == 1) {
        printf("Filtro aplicado: Data de %s\n", 
               opcaoData == 1 ? "ENTRADA" : "SAÍDA");
        printf("Período: %02d/%02d/%04d a %02d/%02d/%04d\n",
               dataInicio.dia, dataInicio.mes, dataInicio.ano,
               dataFim.dia, dataFim.mes, dataFim.ano);
    }
    
    // Explicar estrutura do ficheiro CSV gerado
    printf("\nEstrutura do CSV:\n");
    printf("1. ID: Identificador único do estacionamento\n");
    printf("2. Matricula: Matrícula do veículo (com hífens)\n");
    printf("3. Lugar: Código do lugar ocupado (ex: 3C05)\n");
    printf("4. Data_Entrada: Data de entrada (formato AAAA-MM-DD)\n");
    printf("5. Hora_Entrada: Hora de entrada (formato HH:MM)\n");
    printf("6. Data_Saida: Data de saída (formato AAAA-MM-DD)\n");
    printf("7. Hora_Saida: Hora de saída (formato HH:MM)\n");
    printf("8. Duracao_Minutos: Tempo total de estacionamento em minutos\n");
    printf("9. Valor_Pago: Total pago pelo estacionamento (€)\n");
    
    printf("\nPressione Enter para continuar...");
    getchar();
}