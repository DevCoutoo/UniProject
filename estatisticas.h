#ifndef ESTATISTICAS_H
#define ESTATISTICAS_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// FUNÇÕES PARA ESTATÍSTICAS E RELATÓRIOS
// -----------------------------------------------------------------------------

/**
 * Mostra estatísticas gerais sobre a utilização do parque.
 * Inclui informações como:
 * - Total de lugares disponíveis/ocupados/indisponíveis
 * - Total de veículos que entraram/saíram
 * - Valor total arrecadado
 * - Taxa de ocupação média
 * @note Funcionalidade opcional que fornece uma visão geral do sistema.
 */
void mostrarEstatisticasGerais();

/**
 * Mostra estatísticas detalhadas por piso do parque.
 * Para cada piso, apresenta:
 * - Número de lugares disponíveis/ocupados/indisponíveis
 * - Taxa de ocupação do piso
 * - Valor arrecadado no piso (se aplicável)
 * @note Útil para análise da distribuição de veículos no parque.
 */
void mostrarEstatisticasPorPiso();

/**
 * Gera e exibe um gráfico de barras com dados de saídas por hora.
 * Esta função implementa o extra E1 do enunciado:
 * - Lista veículos que saíram num determinado dia
 * - Agrupa por hora dentro de um período especificado pelo utilizador
 * - Mostra número total de veículos e valor total por hora
 * @note O gráfico é apresentado no formato ASCII no terminal.
 */
void gerarGraficoBarras();

/**
 * Gera e exibe uma tabela dinâmica com dados de saídas por dia/mês.
 * Esta função implementa o extra E2 do enunciado:
 * - Lista veículos que saíram num intervalo de datas especificado
 * - Agrupa dados por mês e dia
 * - Mostra número total de saídas e valor total arrecadado
 * @note A tabela permite análise temporal dos dados.
 */
void gerarTabelaDinamica();

/**
 * Mostra estatísticas para um período específico definido pelo utilizador.
 * Permite ao utilizador especificar:
 * - Data/hora inicial
 * - Data/hora final
 * A função apresenta:
 * - Número de entradas/saídas no período
 * - Valor total arrecadado
 * - Média de tempo de estacionamento
 * @note Funcionalidade avançada para análise temporal.
 */
void mostrarEstatisticasPorPeriodo();

/**
 * Exporta dados estatísticos para um ficheiro CSV.
 * Esta função implementa parte do extra E3 do enunciado:
 * - Permite ao utilizador escolher o separador (',' ou ';')
 * - Inclui linha de cabeçalho descritiva
 * - O nome do ficheiro é especificado pelo utilizador
 * @note Facilita a importação de dados para outras aplicações (ex: Excel).
 */
void exportarEstatisticas();

/**
 * Menu principal para o módulo de estatísticas.
 * Apresenta todas as opções disponíveis de análise e relatórios.
 * Coordena a navegação entre as diferentes funcionalidades estatísticas.
 * @note Este é o ponto de entrada para o módulo de estatísticas.
 */
void estatisticas();

#endif