#ifndef TARIFAS_H
#define TARIFAS_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// DECLARAÇÃO DE VARIÁVEL GLOBAL EXTERNA
// -----------------------------------------------------------------------------

/**
 * Variável global que contém todas as tarifas configuradas no sistema.
 * Esta estrutura agrupa as quatro tarifas disponíveis:
 * - tarifaT1: Tarifa horária diurna (8:00-22:00)
 * - tarifaT2: Tarifa horária noturna (22:00-8:00)
 * - tarifaT3: Tarifa de dia completo
 * - tarifaT4: Tarifa de vários dias
 * 
 * @note Os valores podem ser carregados do ficheiro Tarifario.txt ou
 *       usar os valores padrão definidos em datastructs.h.
 */
extern TarifasSistema sistema;

// -----------------------------------------------------------------------------
// FUNÇÕES PARA GESTÃO DE TARIFAS
// -----------------------------------------------------------------------------

/**
 * Salva as tarifas atuais em formato binário para persistência.
 * Esta função implementa parte da funcionalidade 9 do enunciado,
 * permitindo que as tarifas configuradas sejam mantidas entre
 * execuções da aplicação.
 * 
 * @param sistema Ponteiro para a estrutura TarifasSistema a ser salva
 * @return 1 se a operação foi bem sucedida, 0 em caso de erro
 * 
 * @note O ficheiro binário criado permite recuperação rápida e eficiente
 *       dos dados de tarifação.
 */
int salvarTarifasBinario(TarifasSistema *sistema);

/**
 * Carrega as tarifas do ficheiro Tarifario.txt para memória.
 * Esta função implementa parte da funcionalidade 1-b do enunciado:
 * - Lê o ficheiro Tarifario.txt durante a inicialização da aplicação
 * - Valida o formato de cada linha conforme Figura 1 do enunciado
 * - Armazena os dados na estrutura TarifasSistema em memória
 * - Usa valores padrão se o ficheiro não existir ou tiver erros
 * 
 * @param sistema Ponteiro para a estrutura TarifasSistema a ser preenchida
 * 
 * @note Formato esperado do ficheiro (ver Figura 1):
 *       <TpTarifa>\t<CodTarifa>\t<Horalnf>\t<HoraSup>\t<ValorHora>
 *       Exemplo: H\tCT1\t08:00\t22:00\t0.60€
 * 
 * @note Se ocorrerem erros na leitura, estes são registados no ficheiro
 *       "erros.txt" conforme requisito da secção de resiliência da app.
 */
void carregarTarifas(TarifasSistema *sistema);

/**
 * Permite alterar os valores das tarifas do sistema.
 * Esta função implementa o requisito do enunciado que indica que
 * os valores das tarifas devem poder ser facilmente alterados no futuro.
 * 
 * Oferece as seguintes opções:
 * 1. Alterar tarifa T1 (horária diurna: 8:00-22:00)
 * 2. Alterar tarifa T2 (horária noturna: 22:00-8:00)
 * 3. Alterar tarifa T3 (dia completo)
 * 4. Alterar tarifa T4 (vários dias)
 * 5. Restaurar valores padrão
 * 
 * @note As alterações são validadas para garantir valores positivos
 *       e coerência entre tarifas (ex: T3 deve ser maior que T1 para 8 horas).
 * @note Após alteração, as novas tarifas são automaticamente salvas
 *       em formato binário para persistência.
 */
void alterarTarifas();

#endif