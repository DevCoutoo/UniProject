#ifndef LOGS_H
#define LOGS_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// CONSTANTES PARA TIPOS DE LOG
// -----------------------------------------------------------------------------

/**
 * Define o tipo de log como INFORMATIVO.
 * Usado para registar operações normais do sistema que são importantes
 * para auditoria e monitorização.
 */
#define LOG_INFO 0

/**
 * Define o tipo de log como AVISO.
 * Usado para situações que não impedem o funcionamento do sistema,
 * mas que merecem atenção (ex: dados incompletos, valores no limite).
 */
#define LOG_WARNING 1

/**
 * Define o tipo de log como ERRO.
 * Usado para situações que afetam o funcionamento normal do sistema
 * ou que indicam problemas que requerem intervenção.
 */
#define LOG_ERROR 2

// -----------------------------------------------------------------------------
// FUNÇÕES PARA GESTÃO DE LOGS E REGISTOS DE ERROS
// -----------------------------------------------------------------------------

/**
 * Adiciona uma entrada ao sistema de logs.
 * Esta função regista mensagens no ficheiro de logs para auditoria
 * e diagnóstico de problemas.
 * 
 * @param tipo Tipo de log (LOG_INFO, LOG_WARNING, LOG_ERROR)
 * @param mensagem Texto descritivo da operação ou erro a registar
 * 
 * @note Cada entrada de log inclui automaticamente:
 *       - Data e hora atual
 *       - Tipo de log
 *       - Mensagem fornecida
 *       - Identificação do módulo/função (se disponível)
 */
void adicionarLog(int tipo, const char *mensagem);

/**
 * Lista todas as entradas de log armazenadas no sistema.
 * Esta função apresenta os logs no ecrã, geralmente com formatação
 * que diferencia os tipos de mensagem (info, aviso, erro).
 * 
 * @note Pode incluir opções de filtragem por tipo ou período temporal.
 * @note Implementa paginação conforme funcionalidade 10 do enunciado.
 */
void listarLogs();

/**
 * Limpa todos os registos de log do sistema.
 * Esta função remove todas as entradas do ficheiro de logs.
 * 
 * @note Deve ser usada com cautela, pois elimina histórico de auditoria.
 * @note Pode exigir confirmação do utilizador antes de executar.
 */
void limparLogs();

/**
 * Verifica a consistência dos dados em memória.
 * Esta função realiza verificações de integridade sobre as estruturas
 * de dados para detetar possíveis corrupções ou inconsistências.
 * 
 * @return 1 se os dados são consistentes, 0 se foram encontrados problemas
 * 
 * @note Verificações típicas incluem:
 *       - IDs sequenciais sem lacunas ou duplicados
 *       - Referências a lugares existentes
 *       - Datas válidas (entrada anterior a saída)
 *       - Valores monetários positivos
 */
int verificarConsistenciaDados();

// -----------------------------------------------------------------------------
// FUNÇÕES DE VALIDAÇÃO DE DADOS
// -----------------------------------------------------------------------------

/**
 * Verifica se uma matrícula tem formato válido.
 * Esta função valida a matrícula de acordo com os formatos esperados.
 * 
 * @param matricula String com a matrícula a validar
 * 
 * @note Formatos aceites podem incluir:
 *       - Formato antigo: XX-XX-XX
 *       - Formato novo: XX-XX-XXX
 *       - Formato internacional (sem hífens)
 * @note A função adiciona um log se a validação falhar.
 */
void verificarMatricula(const char *matricula);

/**
 * Verifica se uma data/hora é válida.
 * Esta função valida a consistência temporal dos campos de data.
 * 
 * @param data Estrutura DataHora a validar
 * 
 * @note Verificações realizadas:
 *       - Mês entre 1 e 12
 *       - Dia apropriado para o mês (considerando anos bissextos)
 *       - Hora entre 0 e 23
 *       - Minuto entre 0 e 59
 *       - Ano razoável (ex: >= 2023)
 * @note A função adiciona um log se a validação falhar.
 */
void verificarData(DataHora data);

/**
 * Verifica se um código de lugar é válido.
 * Esta função valida o formato e a existência do lugar no parque.
 * 
 * @param codigo String com o código do lugar (ex: "3C05")
 * 
 * @note Validações realizadas:
 *       - Formato: dígito + letra + dois dígitos
 *       - Piso existente no parque
 *       - Fila existente no piso
 *       - Lugar existente na fila
 * @note A função adiciona um log se a validação falhar.
 */
void verificarCodigoLugar(const char *codigo);

// -----------------------------------------------------------------------------
// FUNÇÃO PARA REGISTO DE ERROS EM FICHEIRO
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
 *       - Data e hora do erro
 *       - Operação em causa
 *       - Descrição do erro
 *       - Linha do ficheiro
 *       - Conteúdo do registo (se disponível)
 *       - Indicação se foi descartado
 */
void registrarErro(const char *operacao, const char *erro, int linha, const char *registro, int descartado);

#endif