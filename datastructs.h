#ifndef DATASTRUCTS_H
#define DATASTRUCTS_H

// -----------------------------------------------------------------------------
// CONSTANTES GERAIS
// -----------------------------------------------------------------------------

/**
 * Define o número máximo de pisos que o parque pode ter.
 * Valor fixo conforme especificado no enunciado (máximo de 5 pisos).
 */
#define MAX_PISOS 5

/**
 * Define o número máximo de filas por piso.
 * Valor fixo conforme especificado (máximo de 26 filas, de A a Z).
 */
#define MAX_FILAS 26

/**
 * Define o número máximo de lugares por fila.
 * Valor fixo conforme especificado (máximo de 50 lugares, de 1 a 50).
 */
#define MAX_LUGARES 50

/**
 * Define o tamanho máximo para o campo de matrícula do veículo.
 * Inclui espaço para caracteres e terminador nulo.
 */
#define MAX_MATRICULA 15

/**
 * Define o valor máximo para o campo 'motivo' de um lugar indisponível.
 * Valores possíveis: 'i' (condições inadequadas), 'o' (obras), 
 * 'r' (reservado), 'm' (outros motivos).
 */
#define MAX_TIPO_INDISP 'm'

/**
 * Valor padrão para a tarifa T1 (período diurno: 8:00 às 21:59).
 * Este valor pode ser substituído pela leitura do ficheiro Tarifario.txt.
 */
#define TARIFA_T1_PADRAO 0.60

/**
 * Valor padrão para a tarifa T2 (período noturno: 22:00 às 7:59).
 * Este valor pode ser substituído pela leitura do ficheiro Tarifario.txt.
 */
#define TARIFA_T2_PADRAO 0.30

/**
 * Valor padrão para a tarifa T3 (dia completo).
 * Este valor pode ser substituído pela leitura do ficheiro Tarifario.txt.
 */
#define TARIFA_T3_PADRAO 8.00

/**
 * Valor padrão para a tarifa T4 (vários dias).
 * Este valor pode ser substituído pela leitura do ficheiro Tarifario.txt.
 */
#define TARIFA_T4_PADRAO 6.00

// -----------------------------------------------------------------------------
// STRUCT PARA DATA E HORA
// -----------------------------------------------------------------------------

/**
 * Estrutura para armazenar data e hora completas.
 * Utilizada para registar entrada, saída e períodos de tarifação.
 */
typedef struct {
    int dia;     // Dia do mês (1-31)
    int mes;     // Mês do ano (1-12)
    int ano;     // Ano (ex: 2025)
    int hora;    // Hora do dia (0-23)
    int minuto;  // Minuto da hora (0-59)
} DataHora;

// -----------------------------------------------------------------------------
// STRUCT PARA TARIFAS
// -----------------------------------------------------------------------------

/**
 * Estrutura que representa uma tarifa do sistema.
 * Cada tarifa tem um tipo, código, período de aplicação e valor.
 */
typedef struct {
    char tipoTarifa;    // 'H' para horária, 'D' para diária
    char codigo[4];     // Código da tarifa (ex: "CT1", "CT2", "CT3", "CT4")
    DataHora inicio;    // Hora de início de aplicação da tarifa
    DataHora fim;       // Hora de fim de aplicação da tarifa
    float valor;        // Valor da tarifa em euros
} Tarifa;

/**
 * Estrutura que agrupa todas as tarifas do sistema.
 * Facilita o acesso às diferentes tarifas disponíveis.
 */
typedef struct {
    Tarifa tarifaT1;  // Tarifa horária diurna (8:00-22:00)
    Tarifa tarifaT2;  // Tarifa horária noturna (22:00-8:00)
    Tarifa tarifaT3;  // Tarifa dia completo
    Tarifa tarifaT4;  // Tarifa vários dias
} TarifasSistema;

// -----------------------------------------------------------------------------
// STRUCT PARA LUGARES DO PARQUE
// -----------------------------------------------------------------------------

/**
 * Estrutura que representa um lugar de estacionamento no parque.
 * Cada lugar tem um código único, estado atual e motivo se indisponível.
 */
typedef struct {
    char codigo[5];  // Código do lugar (ex: "3C05" = piso 3, fila C, lugar 05)
    char estado;     // Estado: 'L' (livre), 'O' (ocupado), 'I' (indisponível)
    char motivo;     // Motivo da indisponibilidade: 'i', 'o', 'r', 'm' (ver MAX_TIPO_INDISP)
} Lugar;

// -----------------------------------------------------------------------------
// STRUCT PARA ESTACIONAMENTOS
// -----------------------------------------------------------------------------

/**
 * Estrutura que representa um registo de estacionamento.
 * Contém toda a informação de um veículo estacionado no parque.
 */
typedef struct {
    int id;                     // Número sequencial único da entrada (gerado automaticamente)
    char matricula[MAX_MATRICULA];  // Matrícula do veículo
    char codigoLugar[5];        // Código do lugar atribuído
    DataHora entrada;           // Data e hora de entrada
    DataHora saida;             // Data e hora de saída (se já saiu)
    float valorPago;            // Valor pago pelo estacionamento (0 se ainda não saiu)
    int ativo;                  // Indica se o veículo ainda está no parque (1 = ativo, 0 = inativo)
} Estacionamento;

// -----------------------------------------------------------------------------
// STRUCTS PARA ESTATÍSTICAS E RELATÓRIOS
// -----------------------------------------------------------------------------

/**
 * Estrutura para estatísticas por hora (usada no extra E1 - Gráfico).
 * Agrupa dados de veículos que saíram numa determinada hora.
 */
typedef struct {
    int hora;            // Hora do dia (0-23)
    int totalVeiculos;   // Número total de veículos que saíram nessa hora
    float valorTotal;    // Valor total pago pelos veículos que saíram nessa hora
} EstatisticaHora;

/**
 * Estrutura para dados da tabela dinâmica (usada no extra E2 - Tabela Dinâmica).
 * Armazena dados de saídas por dia para geração de relatórios agregados.
 */
typedef struct {
    char mes[4];        // Mês (ex: "Jan", "Fev", "Mar")
    int dia;            // Dia do mês (1-31)
    int numSaidas;      // Número de veículos que saíram nesse dia
    float valorTotal;   // Valor total pago nesse dia
} DadosTabelaDinamica;

/**
 * Estrutura para totais mensais (usada no extra E2 - Tabela Dinâmica).
 * Agrega dados de saídas por mês para sumarização.
 */
typedef struct {
    char mes[4];        // Mês (ex: "Jan", "Fev", "Mar")
    int totalSaidas;    // Total de veículos que saíram no mês
    float totalValor;   // Valor total pago no mês
} TotalMes;

// -----------------------------------------------------------------------------
// NOVAS STRUCTS PARA MEMÓRIA DINÂMICA
// -----------------------------------------------------------------------------

/**
 * Estrutura para lista dinâmica de estacionamentos.
 * Permite armazenar um número variável de registos de estacionamento,
 * redimensionando-se automaticamente conforme necessário.
 * Utiliza alocação dinâmica para maior eficiência de memória.
 */
typedef struct {
    Estacionamento *estacionamentos;  // Array dinâmico de estacionamentos
    int total;                        // Número atual de estacionamentos na lista
    int capacidade;                   // Capacidade atual do array (pode ser aumentada)
} ListaEstacionamentos;

/**
 * Estrutura para representar o parque de estacionamento com memória dinâmica.
 * Utiliza uma matriz 3D alocada dinamicamente para representar:
 * - Pisos
 * - Filas por piso
 * - Lugares por fila
 * Esta abordagem é mais eficiente em termos de memória para parques de tamanho variável.
 */
typedef struct {
    Lugar ***parque;           // Matriz 3D: parque[piso][fila][lugar]
    int numPisos;              // Número real de pisos do parque (1-5)
    int numFilasPorPiso;       // Número real de filas por piso (1-26)
    int numLugaresPorFila;     // Número real de lugares por fila (1-50)
} ParqueDinamico;

#endif