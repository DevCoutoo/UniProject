#ifndef ESTACIONAMENTO_H
#define ESTACIONAMENTO_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// DECLARAÇÃO DE VARIÁVEIS GLOBAIS EXTERNAS
// -----------------------------------------------------------------------------

/**
 * Variável global que contém a lista dinâmica de todos os estacionamentos.
 * Esta lista é acedida por múltiplos módulos do sistema.
 * NOTA: Uso de variável global é permitido neste contexto conforme a arquitetura do projeto.
 */
extern ListaEstacionamentos listaEstacionamentos;

// -----------------------------------------------------------------------------
// FUNÇÕES PARA PERSISTÊNCIA DE DADOS
// -----------------------------------------------------------------------------

/**
 * Salva todos os estacionamentos da memória para o ficheiro "Estacionamentos.txt".
 * Esta função deve ser chamada apenas uma vez ao sair da aplicação ou quando
 * o utilizador solicitar gravação manual.
 * @note Formato do ficheiro: ver Figura 2 do enunciado.
 */
void salvarEstacionamentos();

/**
 * Carrega os estacionamentos do ficheiro "Estacionamentos.txt" para a memória.
 * Esta função é chamada durante a inicialização da aplicação.
 * @note Registos sem hora de saída são considerados veículos ainda no parque.
 */
void carregarEstacionamentos();

/**
 * Inicializa todas as estruturas de dados do sistema.
 * Esta função deve ser chamada no arranque da aplicação para preparar
 * as estruturas em memória antes de qualquer operação.
 */
void inicializarDados();

// -----------------------------------------------------------------------------
// FUNÇÕES PARA GESTÃO DE ENTRADAS E SAÍDAS
// -----------------------------------------------------------------------------

/**
 * Regista a entrada de um veículo no parque de estacionamento.
 * Esta função:
 * 1. Valida se há lugares disponíveis
 * 2. Atribui um lugar ao veículo (conforme política definida)
 * 3. Atualiza o estado do parque
 * 4. Mostra o ticket de estacionamento no ecrã
 * @note Corresponde à funcionalidade 4-a do enunciado.
 */
void registarEntrada();

/**
 * Regista a saída de um veículo do parque de estacionamento.
 * Esta função:
 * 1. Calcula o valor a pagar conforme o tarifário
 * 2. Atualiza o registo do estacionamento
 * 3. Liberta o lugar ocupado
 * 4. Mostra o recibo de pagamento
 * @note Corresponde à funcionalidade 6 do enunciado.
 */
void registarSaida();

// -----------------------------------------------------------------------------
// FUNÇÕES PARA GESTÃO DE TICKETS/ESTACIONAMENTOS
// -----------------------------------------------------------------------------

/**
 * Lista todos os tickets de estacionamento armazenados no sistema.
 * Inclui tanto veículos que já saíram como os que ainda estão no parque.
 * Implementa paginação conforme funcionalidade 10 do enunciado.
 */
void listarTodosTickets();

/**
 * Consulta um ticket específico pelo seu número de entrada (ID).
 * Mostra toda a informação disponível sobre o estacionamento.
 * @note Corresponde à funcionalidade 4-b do enunciado.
 */
void consultarTicket();

/**
 * Remove um ticket/estaionamento do sistema pelo seu ID.
 * Esta operação só é permitida em situações específicas (ex: entrada errada).
 * @note Requer validação cuidadosa para não corromper dados.
 */
void removerTicket();

/**
 * Altera os dados de um estacionamento existente.
 * Permite correção de erros em registos (ex: matrícula incorreta).
 * Após alteração, mostra o novo ticket atualizado.
 * @note Corresponde à funcionalidade 4-b do enunciado.
 */
void alterarTicket();

/**
 * Menu principal para gestão de tickets/estacionamentos.
 * Apresenta as opções de consulta, alteração e remoção.
 * Esta função coordena as operações de gestão de estacionamentos.
 */
void menuTicket();

/**
 * Obtém o próximo ID sequencial para um novo estacionamento.
 * Garante que cada estacionamento tem um identificador único.
 * @return Próximo ID disponível (número inteiro > 0)
 */
int obterProximoId();

// -----------------------------------------------------------------------------
// FUNÇÕES PARA PERSISTÊNCIA BINÁRIA
// -----------------------------------------------------------------------------

/**
 * Salva todos os dados em memória para ficheiros binários.
 * Esta função é chamada obrigatoriamente ao sair da aplicação.
 * @note Corresponde à funcionalidade 9 do enunciado.
 */
void salvarDadosBinarios();

/**
 * Salva os dados em memória para ficheiros quando o utilizador solicita.
 * Permite ao utilizador gravar manualmente o estado atual do sistema.
 */
void salvarDadosManual();

/**
 * Salva automaticamente os dados em memória para ficheiros.
 * Pode ser chamada periodicamente ou em eventos específicos.
 * @note Funcionalidade de autosalvamento para prevenção de perda de dados.
 */
void salvarDadosAutomatico();

// -----------------------------------------------------------------------------
// NOVAS FUNÇÕES PARA GESTÃO DE MEMÓRIA DINÂMICA
// -----------------------------------------------------------------------------

/**
 * Redimensiona a lista dinâmica de estacionamentos para uma nova capacidade.
 * Esta função é usada internamente quando a lista atinge a capacidade atual.
 * @param novaCapacidade Nova capacidade desejada (deve ser maior que a atual)
 * @return 1 se o redimensionamento foi bem sucedido, 0 em caso de erro
 */
int redimensionarListaEstacionamentos(int novaCapacidade);

/**
 * Liberta toda a memória alocada para a lista de estacionamentos.
 * Esta função deve ser chamada antes de terminar a aplicação.
 * @note Previne memory leaks.
 */
void libertarListaEstacionamentos();

/**
 * Adiciona um novo estacionamento à lista dinâmica.
 * A lista é redimensionada automaticamente se necessário.
 * @param novo Estrutura Estacionamento preenchida com os dados do novo registo
 * @return ID do estacionamento adicionado se sucesso, 0 em caso de erro
 */
int adicionarEstacionamento(Estacionamento novo);

/**
 * Remove um estacionamento da lista pelo seu ID.
 * Esta operação marca o estacionamento como inativo ou remove-o fisicamente.
 * @param id ID do estacionamento a remover
 * @return 1 se a remoção foi bem sucedida, 0 se o ID não foi encontrado
 */
int removerEstacionamentoPorId(int id);

/**
 * Busca um estacionamento na lista pelo seu ID.
 * @param id ID do estacionamento a buscar
 * @return Ponteiro para o estacionamento se encontrado, NULL se não encontrado
 */
Estacionamento* buscarEstacionamentoPorId(int id);

#endif