#ifndef FICHEIROS_H
#define FICHEIROS_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// FUNÇÕES PARA EXPORTAÇÃO DE DADOS
// -----------------------------------------------------------------------------

/**
 * Gera e salva uma listagem em formato de ficheiro de texto (.txt).
 * Esta função implementa parte da funcionalidade 10 do enunciado:
 * - Permite ao utilizador especificar o nome do ficheiro de saída
 * - Inclui um cabeçalho descritivo com informações sobre a listagem
 * - Formata os dados de forma legível para leitura com editores de texto simples
 * 
 * @note O ficheiro gerado pode conter diferentes tipos de listagens:
 *       - Lista de todos os estacionamentos
 *       - Lista de estacionamentos ativos
 *       - Lista de estacionamentos por período
 *       A escolha do conteúdo depende do contexto de chamada da função.
 */
void gerarTXT();

/**
 * Exporta dados para um ficheiro CSV (Comma-Separated Values).
 * Esta função implementa o extra E3 do enunciado:
 * - Permite ao utilizador escolher o separador (normalmente ',' ou ';')
 * - Inclui linha de cabeçalho com descrição dos campos
 * - Permite especificar o nome do ficheiro de saída
 * - Exporta dados de estacionamentos dentro de um intervalo de datas
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
void guardarCSV();

#endif