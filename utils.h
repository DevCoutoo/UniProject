#ifndef UTILS_H
#define UTILS_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// CONSTANTES PARA PAGINAÇÃO
// -----------------------------------------------------------------------------

/**
 * Define o número padrão de linhas a mostrar por página em listagens.
 * Este valor é usado quando não é especificado um valor diferente.
 * Implementa a funcionalidade 10 do enunciado (mecanismo de avanço página a página).
 */
#define LINHAS_POR_PAGINA 20

// -----------------------------------------------------------------------------
// STRUCT PARA GESTÃO DE PAGINAÇÃO
// -----------------------------------------------------------------------------

/**
 * Estrutura para gerir a paginação em listagens de dados.
 * Utilizada para controlar a navegação em listas longas que são apresentadas
 * em múltiplas páginas para melhor legibilidade.
 */
typedef struct {
    int paginaAtual;    // Número da página atualmente visível (começa em 1)
    int totalPaginas;   // Número total de páginas disponíveis
    int totalItens;     // Número total de itens na listagem
    int itensPorPagina; // Número de itens a mostrar por página
} Paginacao;

// -----------------------------------------------------------------------------
// FUNÇÕES PARA PAGINAÇÃO
// -----------------------------------------------------------------------------

/**
 * Inicializa uma estrutura de paginação com os parâmetros fornecidos.
 * 
 * @param p Ponteiro para a estrutura Paginacao a inicializar
 * @param totalItens Número total de itens a paginar
 * @param itensPorPagina Número de itens a mostrar por página
 * 
 * @note Calcula automaticamente o número total de páginas com base nos itens.
 */
void inicializarPaginacao(Paginacao *p, int totalItens, int itensPorPagina);

/**
 * Mostra uma página específica de uma listagem de dados.
 * Implementa o mecanismo de navegação página a página conforme
 * funcionalidade 10 do enunciado.
 * 
 * @param p Ponteiro para a estrutura Paginacao com estado atual
 * @param mostrarItem Função de callback que sabe como mostrar um item específico
 * @param data Dados a serem passados para a função de callback
 * @return 1 se a operação foi bem sucedida, 0 em caso de erro ou cancelamento
 * 
 * @note Oferece opções de navegação:
 *       - Próxima página
 *       - Página anterior (se implementado - extra valorizado)
 *       - Página específica (se implementado - extra valorizado)
 *       - Última página (se implementado - extra valorizado)
 */
int mostrarPagina(Paginacao *p, void (*mostrarItem)(int index, void *data), void *data);

// -----------------------------------------------------------------------------
// FUNÇÕES DE UTILIDADE GERAL
// -----------------------------------------------------------------------------

/**
 * Limpa o ecrã do terminal para uma apresentação mais limpa.
 * Esta função melhora a experiência do utilizador ao remover conteúdo anterior.
 * 
 * @note A implementação pode usar comandos específicos do sistema operativo
 *       (ex: "cls" no Windows, "clear" no Linux/Mac).
 */
void limparEcra();

// -----------------------------------------------------------------------------
// FUNÇÕES PARA FORMATAÇÃO DE MATRÍCULAS
// -----------------------------------------------------------------------------

/**
 * Formata uma matrícula adicionando hífens no formato padrão.
 * Converte de "AB12CD" para "AB-12-CD".
 * 
 * @param destino String de destino onde será colocada a matrícula formatada
 * @param origem String de origem com a matrícula sem formatação
 * 
 * @note Garante que a string de destino tem espaço suficiente (15 caracteres).
 */
void adicionarHifensMatricula(char *destino, const char *origem);

/**
 * Remove hífens de uma matrícula formatada.
 * Converte de "AB-12-CD" para "AB12CD".
 * Útil para armazenamento compacto ou comparações.
 * 
 * @param destino String de destino onde será colocada a matrícula sem hífens
 * @param origem String de origem com a matrícula formatada
 */
void removerHifensMatricula(char *destino, const char *origem);

// -----------------------------------------------------------------------------
// FUNÇÕES PARA MANIPULAÇÃO DE DATAS
// -----------------------------------------------------------------------------

/**
 * Compara duas datas/horas para determinar a ordem temporal.
 * 
 * @param d1 Primeira data/hora a comparar
 * @param d2 Segunda data/hora a comparar
 * @return -1 se d1 < d2, 0 se d1 == d2, 1 se d1 > d2
 * 
 * @note A comparação considera ano, mês, dia, hora e minuto.
 */
int compararDatas(DataHora d1, DataHora d2);

/**
 * Ajusta uma data/hora após incrementar um dia, considerando mudanças de mês e ano.
 * Útil para cálculos que envolvem períodos que cruzam limites de mês/ano.
 * 
 * @param dh Ponteiro para a estrutura DataHora a ajustar
 */
void ajustarDataAposIncrementoDia(DataHora *dh);

/**
 * Arredonda uma data/hora para o próximo intervalo de 15 minutos.
 * Implementa a regra de cálculo do tarifário:
 * - Entrada: arredonda para os 15 minutos seguintes
 * - Saída: arredonda para os 15 minutos seguintes
 * Exemplo: 8:10 → 8:15, 9:55 → 10:00
 * 
 * @param dh Ponteiro para a estrutura DataHora a arredondar
 * 
 * @note Esta função é essencial para o cálculo correto do valor a pagar.
 */
void arredondarPara15Minutos(DataHora *dh);

/**
 * Calcula a diferença em minutos entre duas datas/horas.
 * 
 * @param entrada Data/hora de entrada
 * @param saida Data/hora de saída
 * @return Diferença em minutos (sempre positiva ou zero)
 * 
 * @note Assume que saida >= entrada.
 */
int calcularDiferencaMinutos(DataHora entrada, DataHora saida);

/**
 * Verifica se duas datas/horas referem-se ao mesmo dia.
 * 
 * @param d1 Primeira data/hora
 * @param d2 Segunda data/hora
 * @return 1 se for o mesmo dia, 0 caso contrário
 * 
 * @note Compara apenas ano, mês e dia (ignora hora e minuto).
 */
int mesmoDia(DataHora d1, DataHora d2);

/**
 * Calcula o número de dias entre duas datas/horas.
 * Considera dias completos e incompletos conforme tarifário.
 * 
 * @param entrada Data/hora de entrada
 * @param saida Data/hora de saída
 * @return Número de dias a considerar para tarifa T4
 * 
 * @note Implementa a regra: número de dias = soma dos dias completos e incompletos
 */
int contarDias(DataHora entrada, DataHora saida);

// -----------------------------------------------------------------------------
// FUNÇÃO PARA CÁLCULO DE VALOR DE ESTACIONAMENTO
// -----------------------------------------------------------------------------

/**
 * Calcula o valor a pagar por um estacionamento com base no tarifário.
 * Implementa todas as regras de cálculo especificadas no enunciado:
 * 
 * a) 8:00 às 21:59 - T1 €/hora (calculado em períodos de 15 min)
 * b) 22:00 às 7:59 - T2 €/hora (calculado em períodos de 15 min)
 * c) Dia completo (sem mudança de dia) - T3 €
 * d) Vários dias (>=2 mudanças de dia) - T4 €/dia
 * 
 * Regras especiais:
 * 1. Arredonda entrada/saída para múltiplos de 15 minutos
 * 2. Se valor horário > dia completo (sem mudar de dia), aplica T3
 * 3. Para vários dias, conta todos os dias (completos e incompletos)
 * 
 * @param e Ponteiro para o estacionamento a calcular
 * @param sistema Ponteiro para as tarifas vigentes
 * @return Valor a pagar em euros
 * 
 * @note Esta é uma das funções mais complexas e críticas do sistema.
 */
float calcularValor(Estacionamento *e, TarifasSistema *sistema);

#endif