#ifndef PARQUE_H
#define PARQUE_H

#include "datastructs.h"

// -----------------------------------------------------------------------------
// DECLARAÇÃO DE VARIÁVEIS GLOBAIS EXTERNAS
// -----------------------------------------------------------------------------

/**
 * Variável global que contém a estrutura dinâmica do parque de estacionamento.
 * Representa a configuração atual do parque com todos os pisos, filas e lugares.
 * Esta variável é acedida por múltiplos módulos do sistema.
 */
extern ParqueDinamico parqueDinamico;

// -----------------------------------------------------------------------------
// FUNÇÕES PARA CONFIGURAÇÃO E INICIALIZAÇÃO DO PARQUE
// -----------------------------------------------------------------------------

/**
 * Configura os parâmetros do parque de estacionamento.
 * Esta função é chamada durante a "fase de instalação" da aplicação
 * (funcionalidade 1-a do enunciado):
 * - Permite especificar o número de pisos (máximo 5)
 * - Permite especificar o número de filas por piso (máximo 26)
 * - Permite especificar o número de lugares por fila (máximo 50)
 * 
 * @note Os valores são validados contra os máximos definidos nas constantes.
 */
void configurarParque();

/**
 * Inicializa a estrutura do parque com base na configuração definida.
 * Esta função:
 * - Aloca memória para a estrutura 3D do parque
 * - Inicializa todos os lugares como livres ('L')
 * - Atribui códigos únicos a cada lugar (ex: "1A01", "1A02", ...)
 * - Configura o estado inicial do parque
 * 
 * @note Deve ser chamada após configurarParque() ou ao carregar configuração salva.
 */
void inicializarParque();

/**
 * Salva a configuração atual do parque para ficheiro.
 * Esta função persiste os parâmetros do parque (pisos, filas, lugares)
 * para que possam ser recuperados em futuras execuções da aplicação.
 * 
 * @note Os dados são guardados em formato binário conforme funcionalidade 9.
 */
void salvarParque();

/**
 * Altera a configuração do parque após a instalação inicial.
 * Permite ajustar os parâmetros do parque sem perder dados existentes
 * (quando possível).
 * 
 * @note Requer validação cuidadosa para não afetar lugares ocupados.
 * @note Pode requerer redistribuição de veículos estacionados.
 */
void alterarParque();

// -----------------------------------------------------------------------------
// FUNÇÕES PARA VISUALIZAÇÃO DO PARQUE
// -----------------------------------------------------------------------------

/**
 * Imprime o mapa de ocupação de um piso específico do parque.
 * Esta função implementa a funcionalidade 8 do enunciado:
 * - Mostra a ocupação do piso de forma explícita
 * - Usa símbolos: 'X' para ocupado, '-' para livre, letra para indisponível
 * - Formata a visualização para fácil leitura
 * 
 * @note O utilizador pode selecionar qual piso visualizar.
 * @note Inclui legenda explicativa dos símbolos usados.
 */
void imprimirMapaPiso();

/**
 * Exibe o número de lugares disponíveis em cada piso.
 * Esta função apresenta a informação que deve estar sempre visível
 * conforme funcionalidade 3 do enunciado.
 * 
 * @note A informação é apresentada de forma clara e atualizada em tempo real.
 * @note Facilita a decisão dos automobilistas na escolha do piso.
 */
void exibirLugaresDisponiveis();

// -----------------------------------------------------------------------------
// FUNÇÕES PARA GESTÃO DE LUGARES INDISPONÍVEIS
// -----------------------------------------------------------------------------

/**
 * Marca um lugar como indisponível e regista o motivo.
 * Esta função implementa a funcionalidade 7-a do enunciado:
 * - Permite especificar lugares não disponíveis
 * - Regista o motivo: 'i' (condições inadequadas), 'o' (obras),
 *   'r' (reservado), 'm' (outros motivos)
 * - Atualiza o estado do lugar na estrutura do parque
 * 
 * @note O lugar marcado como indisponível não pode ser atribuído a veículos.
 */
void marcarIndisponivel();

/**
 * Reverte a situação de um lugar indisponível, tornando-o disponível.
 * Esta função implementa a funcionalidade 7-b do enunciado:
 * - Permite reverter a marcação de indisponibilidade
 * - O lugar volta a ficar disponível para estacionamento
 * - Mantém histórico do motivo anterior (se necessário)
 * 
 * @note Apenas lugares marcados como indisponíveis podem ser revertidos.
 */
void reverterIndisponivel();

/**
 * Altera o motivo de indisponibilidade de um lugar.
 * Permite atualizar o motivo sem alterar o estado de indisponibilidade.
 * 
 * @note Útil para situações onde o motivo muda (ex: de 'obras' para 'reservado').
 */
void alterarIndisponivel();

/**
 * Lista todos os lugares atualmente marcados como indisponíveis.
 * Mostra para cada lugar:
 * - Código do lugar
 * - Motivo da indisponibilidade
 * - Data/hora da marcação (se registada)
 * 
 * @note Inclui opções de filtragem por motivo ou piso.
 */
void listarIndisponiveis();

/**
 * Menu principal para gestão de lugares indisponíveis.
 * Apresenta as opções de marcação, reversão, alteração e listagem.
 * Coordena as operações relacionadas com lugares indisponíveis.
 */
void menuIndisponiveis();

// -----------------------------------------------------------------------------
// NOVAS FUNÇÕES PARA GESTÃO DE MEMÓRIA DINÂMICA DO PARQUE
// -----------------------------------------------------------------------------

/**
 * Aloca memória para a estrutura dinâmica do parque.
 * Esta função cria a matriz 3D que representa o parque:
 * - Primeira dimensão: pisos
 * - Segunda dimensão: filas por piso
 * - Terceira dimensão: lugares por fila
 * 
 * @param pisos Número de pisos a alocar (1-5)
 * @param filas Número de filas por piso a alocar (1-26)
 * @param lugares Número de lugares por fila a alocar (1-50)
 * 
 * @note Utiliza alocação dinâmica para eficiência de memória.
 * @note Inicializa todos os lugares como livres com códigos apropriados.
 */
void alocarParque(int pisos, int filas, int lugares);

/**
 * Liberta toda a memória alocada para a estrutura do parque.
 * Esta função deve ser chamada antes de terminar a aplicação
 * ou antes de realocar o parque com diferentes dimensões.
 * 
 * @note Previne memory leaks ao libertar todos os níveis da matriz 3D.
 */
void libertarParque();

/**
 * Redimensiona a estrutura do parque mantendo dados existentes quando possível.
 * Permite alterar as dimensões do parque após a criação inicial.
 * 
 * @param novosPisos Novo número de pisos
 * @param novasFilas Novo número de filas por piso
 * @param novosLugares Novo número de lugares por fila
 * 
 * @note Se as novas dimensões são menores, verifica se há lugares ocupados
 *       nas áreas a remover.
 * @note Se as novas dimensões são maiores, inicializa os novos lugares como livres.
 */
void redimensionarParque(int novosPisos, int novasFilas, int novosLugares);

#endif