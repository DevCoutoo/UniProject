// logs.c - Módulo de Gestão de Logs e Validação
// Autor: Rodrigo Couto
// Descrição: Contém funções para registo de logs, validação de dados
//           e tratamento de erros conforme especificado nas secções
//           "Desenvolvimento Faseado e Resiliência da App" e 
//           "Documentação do Código" do enunciado.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "logs.h"
#include "parque.h"

// -----------------------------------------------------------------------------
// DECLARAÇÕES EXTERNAS DE VARIÁVEIS GLOBAIS
// -----------------------------------------------------------------------------

/**
 * Variáveis globais para configuração do parque.
 * Usadas na verificação de consistência de dados.
 * @note Estas variáveis são declaradas e inicializadas noutro módulo.
 */
extern int numPisos, numFilasPorPiso, numLugaresPorFila;

/**
 * Estrutura dinâmica do parque.
 * Usada para verificar a consistência dos dados do parque.
 */
extern ParqueDinamico parqueDinamico; 

// -----------------------------------------------------------------------------
// FUNÇÃO: adicionarLog()
// -----------------------------------------------------------------------------

/**
 * Adiciona uma entrada ao sistema de logs.
 * Esta função regista mensagens no ficheiro "sistema_estacionamento.log"
 * para auditoria e diagnóstico de problemas.
 * Se o tipo for LOG_ERROR, também regista no ficheiro "erros.txt".
 * 
 * @param tipo Tipo de log (LOG_INFO, LOG_WARNING, LOG_ERROR)
 * @param mensagem Texto descritivo da operação ou erro a registar
 * 
 * @note Cada entrada de log inclui automaticamente:
 *       - Data e hora atual (formato: AAAA-MM-DD HH:MM:SS)
 *       - Tipo de log (INFO, AVISO, ERRO)
 *       - Mensagem fornecida
 */
void adicionarLog(int tipo, const char *mensagem) {
    // Abrir ficheiro de logs em modo append (adição)
    FILE *logFile = fopen("sistema_estacionamento.log", "a");
    if (!logFile) return;  // Se não conseguir abrir, terminar silenciosamente
    
    // Obter data e hora atual
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    
    // Converter tipo numérico para string descritiva
    const char *tipoStr;
    switch (tipo) {
        case LOG_INFO: tipoStr = "INFO"; break;
        case LOG_WARNING: tipoStr = "AVISO"; break;
        case LOG_ERROR: tipoStr = "ERRO"; break;
        default: tipoStr = "DESCONHECIDO";
    }
    
    // Escrever entrada formatada no ficheiro de logs
    fprintf(logFile, "[%04d-%02d-%02d %02d:%02d:%02d] [%s] %s\n",
            local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
            local->tm_hour, local->tm_min, local->tm_sec,
            tipoStr, mensagem);
    
    // Fechar ficheiro
    fclose(logFile);
    
    // Se for erro, registar também no ficheiro específico de erros
    // (implementa parte da funcionalidade c) da secção de resiliência)
    if (tipo == LOG_ERROR) {
        FILE *erroFile = fopen("erros.txt", "a");
        if (erroFile) {
            fprintf(erroFile, "[%04d-%02d-%02d %02d:%02d:%02d] [ERRO] %s\n",
                    local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
                    local->tm_hour, local->tm_min, local->tm_sec,
                    mensagem);
            fclose(erroFile);
        }
    }
}

// -----------------------------------------------------------------------------
// FUNÇÃO: listarLogs()
// -----------------------------------------------------------------------------

/**
 * Lista todas as entradas de log armazenadas no sistema.
 * Esta função apresenta os logs do ficheiro "sistema_estacionamento.log"
 * no ecrã, permitindo ao administrador verificar a atividade do sistema.
 * 
 * @note A função mostra todos os logs disponíveis, sem paginação.
 * @note Se o ficheiro não existir, informa que não há logs.
 */
void listarLogs() {
    // Tentar abrir ficheiro de logs para leitura
    FILE *logFile = fopen("sistema_estacionamento.log", "r");
    if (!logFile) {
        printf("Não existem logs registados.\n");
        return;
    }
    
    // Mostrar cabeçalho
    printf("\n=== LOGS DO SISTEMA ===\n");
    
    // Ler e mostrar cada linha do ficheiro
    char linha[256];
    while (fgets(linha, sizeof(linha), logFile)) {
        printf("%s", linha);
    }
    
    // Fechar ficheiro e mostrar rodapé
    fclose(logFile);
    printf("=======================\n");
    
    printf("\nPressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: limparLogs()
// -----------------------------------------------------------------------------

/**
 * Limpa todos os registos de log do sistema.
 * Esta função remove o ficheiro "sistema_estacionamento.log",
 * eliminando todo o histórico de logs.
 * 
 * @note Deve ser usada com cautela, pois elimina histórico de auditoria.
 * @note Não é implementada confirmação do utilizador nesta versão.
 */
void limparLogs() {
    // Tentar remover ficheiro de logs
    if (remove("sistema_estacionamento.log") == 0) {
        printf("Logs limpos com sucesso.\n");
    } else {
        printf("Não foi possível limpar os logs.\n");
    }
    
    printf("Pressione Enter para continuar...");
    getchar();
}

// -----------------------------------------------------------------------------
// FUNÇÃO: verificarConsistenciaDados()
// -----------------------------------------------------------------------------

/**
 * Verifica a consistência dos dados em memória.
 * Esta função realiza verificações de integridade sobre as estruturas
 * de dados para detetar possíveis corrupções ou inconsistências.
 * 
 * @return Número de problemas encontrados (0 = dados consistentes)
 * 
 * @note Verificações realizadas:
 *       1. Configuração básica do parque
 *       2. Códigos de lugares válidos
 *       3. (Poderia incluir mais verificações em versões futuras)
 */
int verificarConsistenciaDados() {
    printf("\n=== VERIFICAÇÃO DE CONSISTÊNCIA ===\n");
    
    int problemas = 0;  // Contador de problemas encontrados
    
    // 1. Verificar configuração básica do parque
    // O parque deve ter pelo menos 1 piso, 1 fila e 1 lugar
    if (parqueDinamico.numPisos == 0 || parqueDinamico.numFilasPorPiso == 0 || parqueDinamico.numLugaresPorFila == 0) {
        printf("ERRO: Parque não configurado corretamente.\n");
        printf("      Pisos: %d, Filas: %d, Lugares: %d\n",
               parqueDinamico.numPisos, parqueDinamico.numFilasPorPiso, parqueDinamico.numLugaresPorFila);
        problemas++;
    }
    
    // 2. Verificar lugares duplicados ou inválidos
    // Percorrer toda a estrutura 3D do parque
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                // Verificar se o lugar tem código atribuído
                if (strlen(parqueDinamico.parque[p][f][l].codigo) == 0) {
                    printf("AVISO: Lugar [Piso %d][Fila %d][Lugar %d] sem código.\n", p, f, l);
                    problemas++;
                }
            }
        }
    }
    
    // 3. Mostrar resumo da verificação
    printf("Verificação concluída. %d problema(s) encontrado(s).\n", problemas);
    printf("\nPressione Enter para continuar...");
    getchar();
    
    return problemas;
}

// -----------------------------------------------------------------------------
// FUNÇÃO: verificarMatricula()
// -----------------------------------------------------------------------------

/**
 * Verifica se uma matrícula tem formato válido.
 * Esta função valida o comprimento da matrícula e adiciona um log
 * se o formato for suspeito.
 * 
 * @param matricula String com a matrícula a validar
 * 
 * @note Esta é uma validação básica. Em versões futuras poderia:
 *       - Verificar caracteres alfanuméricos
 *       - Verificar formato específico (XX-XX-XX ou XX-XX-XXX)
 */
void verificarMatricula(const char *matricula) {
    // Verificar comprimento da matrícula
    // Comprimento típico: 6-8 caracteres (sem hífens)
    if (strlen(matricula) < 6 || strlen(matricula) > 8) {
        adicionarLog(LOG_WARNING, "Matrícula com formato suspeito");
    }
}

// -----------------------------------------------------------------------------
// FUNÇÃO: verificarData()
// -----------------------------------------------------------------------------

/**
 * Verifica se uma data/hora é válida.
 * Esta função valida os campos individuais da data e adiciona logs
 * apropriados se encontrar problemas.
 * 
 * @param data Estrutura DataHora a validar
 * 
 * @note Validações realizadas:
 *       - Ano entre 2000 e 2100 (AVISO se fora)
 *       - Mês entre 1 e 12 (ERRO se inválido)
 *       - Dia entre 1 e 31 (AVISO se inválido)
 * @note Não valida dias específicos por mês ou anos bissextos nesta versão.
 */
void verificarData(DataHora data) {
    // Validar ano (intervalo razoável para a aplicação)
    if (data.ano < 2000 || data.ano > 2100) {
        adicionarLog(LOG_WARNING, "Data com ano fora do intervalo normal");
    }
    
    // Validar mês (1-12, erro se inválido)
    if (data.mes < 1 || data.mes > 12) {
        adicionarLog(LOG_ERROR, "Data com mês inválido");
    }
    
    // Validar dia (1-31, aviso se inválido)
    if (data.dia < 1 || data.dia > 31) {
        adicionarLog(LOG_WARNING, "Data com dia inválido");
    }
    
    // Nota: Não valida combinações específicas (ex: 31 de fevereiro)
    // nem horas/minutos nesta versão básica
}

// -----------------------------------------------------------------------------
// FUNÇÃO: verificarCodigoLugar()
// -----------------------------------------------------------------------------

/**
 * Verifica se um código de lugar é válido.
 * Esta função valida o comprimento do código do lugar.
 * 
 * @param codigo String com o código do lugar (ex: "3C05")
 * 
 * @note Verificação básica de comprimento (deve ser 4 caracteres).
 * @note Em versões futuras poderia validar:
 *       - Formato: dígito + letra + dois dígitos
 *       - Existência do lugar no parque
 */
void verificarCodigoLugar(const char *codigo) {
    // Verificar comprimento do código (deve ser 4: 1D + 1L + 2D)
    if (strlen(codigo) != 4) {
        adicionarLog(LOG_ERROR, "Código de lugar com tamanho inválido");
    }
}

// -----------------------------------------------------------------------------
// FUNÇÃO: registrarErro()
// -----------------------------------------------------------------------------

/**
 * Regista um erro detetado durante a leitura de ficheiros no arranque.
 * Esta função implementa o requisito da alínea c) da secção
 * "Desenvolvimento Faseado e Resiliência da App":
 * - Erros detetados na leitura dos ficheiros .txt devem ser registados
 *   num ficheiro "erros.txt"
 * 
 * @param operacao Nome da operação onde ocorreu o erro (ex: "lerEstacionamentos")
 * @param erro Descrição do erro detetado (ex: "formato de data inválido")
 * @param linha Número da linha do ficheiro onde o erro foi detetado
 * @param registro Conteúdo da linha problemática (pode ser NULL)
 * @param descartado Indica se o registo foi descartado (1) ou mantido (0)
 * 
 * @note Cada entrada no ficheiro "erros.txt" contém:
 *       1. Data e hora do erro
 *       2. Operação em causa
 *       3. Descrição do erro
 *       4. Linha do ficheiro (se aplicável)
 *       5. Conteúdo do registo (se disponível)
 *       6. Indicação se foi descartado
 *       7. Separador visual
 */
void registrarErro(const char *operacao, const char *erro, int linha, const char *registro, int descartado) {
    // Abrir ficheiro de erros em modo append (adição)
    FILE *erroFile = fopen("erros.txt", "a");
    
    // Se não conseguir abrir, tentar criar o ficheiro
    if (!erroFile) {
        erroFile = fopen("erros.txt", "w");
        if (!erroFile) {
            // Se nem criação for possível, mostrar erro crítico no stderr
            fprintf(stderr, "CRÍTICO: Não foi possível criar ficheiro erros.txt\n");
            return;
        }
        fclose(erroFile);
        // Reabrir em modo append
        erroFile = fopen("erros.txt", "a");
        if (!erroFile) return;  // Se ainda falhar, terminar
    }
    
    // Obter data e hora atual
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    
    // Escrever entrada formatada no ficheiro de erros
    
    // 1. Data e hora do erro
    fprintf(erroFile, "[%04d-%02d-%02d %02d:%02d:%02d]\n", 
            local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
            local->tm_hour, local->tm_min, local->tm_sec);
    
    // 2. Operação onde ocorreu o erro
    fprintf(erroFile, "Operação: %s\n", operacao);
    
    // 3. Descrição do erro
    fprintf(erroFile, "Erro: %s\n", erro);
    
    // 4. Linha do ficheiro afetada (se especificada)
    if (linha > 0) {
        fprintf(erroFile, "Linha afetada: %d\n", linha);
    } else {
        fprintf(erroFile, "Elemento afetado: Não especificado\n");
    }
    
    // 5. Conteúdo do registo problemático (se disponível)
    if (registro && strlen(registro) > 0) {
        fprintf(erroFile, "Registro: %s\n", registro);
    } else {
        fprintf(erroFile, "Registro: (vazio)\n");
    }
    
    // 6. Indicação se o registo foi descartado
    fprintf(erroFile, "Descartado: %s\n", descartado ? "SIM" : "NÃO");
    
    // 7. Separador visual para facilitar leitura
    fprintf(erroFile, "----------------------------------------\n\n");
    
    // Fechar ficheiro
    fclose(erroFile);
}