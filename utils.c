// utils.c - Módulo de Funções Utilitárias
// Autor: Rodrigo Couto
// Descrição: Contém funções auxiliares para formatação, cálculos
//           de datas, validações e sistema de paginação conforme
//           especificado nas funcionalidades 10 e cálculo de tarifas.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "utils.h"
#include "datastructs.h"

// -----------------------------------------------------------------------------
// FUNÇÃO: limparEcra()
// -----------------------------------------------------------------------------

/**
 * Limpa o ecrã do terminal para uma apresentação mais limpa.
 * Esta função melhora a experiência do utilizador ao remover conteúdo anterior.
 * A implementação é específica para cada sistema operativo.
 * 
 * @note Usa comandos específicos:
 *       - Windows: "cls"
 *       - Linux/Mac/Unix: "clear"
 */
void limparEcra() {
    #ifdef _WIN32
        system("cls");          // Comando para Windows
    #else
        system("clear");        // Comando para Linux/Mac/Unix
    #endif
}

// -----------------------------------------------------------------------------
// FUNÇÕES PARA FORMATAÇÃO DE MATRÍCULAS
// -----------------------------------------------------------------------------

/**
 * Formata uma matrícula adicionando hífens no formato padrão.
 * Converte matrículas sem hífens para o formato legível com hífens.
 * 
 * @param destino String de destino onde será colocada a matrícula formatada
 * @param origem String de origem com a matrícula sem formatação
 * 
 * @note Suporta três formatos principais:
 *       - 6 caracteres: AABBCC → AA-BB-CC
 *       - 7 caracteres: AABBCCD → AA-BB-CCD
 *       - 8 caracteres: AABBCCDD → AA-BB-CC-DD
 * @note Garante que a string de destino não excede MAX_MATRICULA caracteres.
 */
void adicionarHifensMatricula(char *destino, const char *origem) {
    int len = strlen(origem);
    
    if (len == 6) { 
        // Formato: AABBCC → AA-BB-CC
        snprintf(destino, MAX_MATRICULA, "%c%c-%c%c-%c%c", 
                origem[0], origem[1], origem[2], origem[3], origem[4], origem[5]);
    } 
    else if (len == 7) { 
        // Formato: AABBCCD → AA-BB-CCD
        snprintf(destino, MAX_MATRICULA, "%c%c-%c%c-%c%c%c", 
                origem[0], origem[1], origem[2], origem[3], origem[4], origem[5], origem[6]);
    }
    else if (len == 8) { 
        // Formato: AABBCCDD → AA-BB-CC-DD
        snprintf(destino, MAX_MATRICULA, "%c%c-%c%c-%c%c-%c%c", 
                origem[0], origem[1], origem[2], origem[3], origem[4], origem[5], origem[6], origem[7]);
    }
    else {
        // Formato não reconhecido, copiar sem alterações
        strncpy(destino, origem, MAX_MATRICULA - 1);
        destino[MAX_MATRICULA - 1] = '\0';
    }
}

/**
 * Remove hífens de uma matrícula formatada.
 * Converte matrículas com hífens para o formato compacto sem hífens.
 * Útil para armazenamento eficiente ou comparações.
 * 
 * @param destino String de destino onde será colocada a matrícula sem hífens
 * @param origem String de origem com a matrícula formatada com hífens
 * 
 * @note Converte todos os caracteres para maiúsculas.
 * @note Remove apenas o caractere '-' (hífen).
 */
void removerHifensMatricula(char *destino, const char *origem) {
    int j = 0;
    // Percorrer string de origem e copiar apenas caracteres não hífen
    for (int i = 0; origem[i] != '\0' && j < MAX_MATRICULA - 1; i++) {
        if (origem[i] != '-') {
            destino[j++] = toupper(origem[i]);  // Converter para maiúsculas
        }
    }
    destino[j] = '\0';  // Terminar string
}

// -----------------------------------------------------------------------------
// FUNÇÕES PARA MANIPULAÇÃO E COMPARAÇÃO DE DATAS
// -----------------------------------------------------------------------------

/**
 * Compara duas datas/horas para determinar a ordem temporal.
 * 
 * @param d1 Primeira data/hora a comparar
 * @param d2 Segunda data/hora a comparar
 * @return Valor negativo se d1 < d2, 0 se d1 == d2, positivo se d1 > d2
 * 
 * @note A comparação considera:
 *       1. Ano
 *       2. Mês
 *       3. Dia
 *       4. Hora
 *       5. Minuto
 * @note Funciona como strcmp() mas para estruturas DataHora.
 */
int compararDatas(DataHora d1, DataHora d2) {
    if (d1.ano != d2.ano)
        return d1.ano - d2.ano;
    if (d1.mes != d2.mes)
        return d1.mes - d2.mes;
    if (d1.dia != d2.dia)
        return d1.dia - d2.dia;
    if (d1.hora != d2.hora)
        return d1.hora - d2.hora;
    return d1.minuto - d2.minuto;
}

/**
 * Ajusta uma data/hora após incrementar um dia, considerando mudanças de mês e ano.
 * Esta função é usada quando operações de data ultrapassam limites mensais.
 * 
 * @param dh Ponteiro para a estrutura DataHora a ajustar
 * 
 * @note Considera anos bissextos para fevereiro.
 * @note Atualiza dia, mês e ano conforme necessário.
 */
void ajustarDataAposIncrementoDia(DataHora *dh) {
    // Array com dias por mês (índice 0 = janeiro)
    int diasPorMes[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Verificar se o ano é bissexto
    int anoBissexto = 0;
    if (dh->ano % 4 == 0) {
        if (dh->ano % 100 == 0) {
            if (dh->ano % 400 == 0) {
                anoBissexto = 1;
            }
        } else {
            anoBissexto = 1;
        }
    }
    
    // Ajustar fevereiro para anos bissextos
    if (anoBissexto) {
        diasPorMes[1] = 29;
    }
    
    // Se o dia excede os dias do mês atual, avançar para o próximo mês
    if (dh->dia > diasPorMes[dh->mes - 1]) {
        dh->dia = 1;
        dh->mes++;
        
        // Se passou de dezembro, avançar para o próximo ano
        if (dh->mes > 12) {
            dh->mes = 1;
            dh->ano++;
        }
    }
}

/**
 * Arredonda uma data/hora para o próximo intervalo de 15 minutos.
 * Implementa a regra de cálculo do tarifário especificada no enunciado:
 * - Entrada: arredonda para os 15 minutos seguintes
 * - Saída: arredonda para os 15 minutos seguintes
 * 
 * @param dh Ponteiro para a estrutura DataHora a arredondar
 * 
 * @note Exemplos:
 *       8:10 → 8:15
 *       9:55 → 10:00
 *       23:50 → 00:00 (próximo dia)
 */
void arredondarPara15Minutos(DataHora *dh) {
    int minutos = dh->minuto;
    int resto = minutos % 15;  // Quanto falta para o próximo múltiplo de 15
    
    if (resto > 0) {
        // Arredondar para cima para o próximo múltiplo de 15
        dh->minuto = minutos + (15 - resto);
        
        // Se passou de 59 minutos, ajustar hora
        if (dh->minuto >= 60) {
            dh->minuto -= 60;
            dh->hora += 1;
            
            // Se passou de 23 horas, ajustar dia
            if (dh->hora >= 24) {
                dh->hora = 0;
                dh->dia += 1;
                ajustarDataAposIncrementoDia(dh);  // Ajustar mês/ano se necessário
            }
        }
    }
    // Se resto == 0, já está em múltiplo de 15, não fazer nada
}

/**
 * Calcula a diferença em minutos entre duas datas/horas.
 * 
 * @param entrada Data/hora de entrada
 * @param saida Data/hora de saída
 * @return Diferença em minutos (sempre positiva ou zero)
 * 
 * @note Assume que saida >= entrada.
 * @note Usa funções time.h para cálculos precisos considerando meses/anos.
 */
int calcularDiferencaMinutos(DataHora entrada, DataHora saida) {
    // Converter estruturas DataHora para struct tm (biblioteca time.h)
    struct tm entrada_tm = {0};
    struct tm saida_tm = {0};
    
    entrada_tm.tm_year = entrada.ano - 1900;  // tm_year conta a partir de 1900
    entrada_tm.tm_mon = entrada.mes - 1;      // tm_mon: 0 = janeiro
    entrada_tm.tm_mday = entrada.dia;
    entrada_tm.tm_hour = entrada.hora;
    entrada_tm.tm_min = entrada.minuto;
    
    saida_tm.tm_year = saida.ano - 1900;
    saida_tm.tm_mon = saida.mes - 1;
    saida_tm.tm_mday = saida.dia;
    saida_tm.tm_hour = saida.hora;
    saida_tm.tm_min = saida.minuto;
    
    // Converter para time_t (segundos desde epoch)
    time_t entrada_time = mktime(&entrada_tm);
    time_t saida_time = mktime(&saida_tm);
    
    // Verificar conversão bem sucedida
    if (entrada_time == -1 || saida_time == -1) {
        return 0;  // Retornar 0 em caso de erro
    }
    
    // Calcular diferença em segundos e converter para minutos
    double diferenca = difftime(saida_time, entrada_time);
    return (int)(diferenca / 60);  // Converter segundos para minutos
}

/**
 * Verifica se duas datas/horas referem-se ao mesmo dia.
 * 
 * @param d1 Primeira data/hora
 * @param d2 Segunda data/hora
 * @return 1 se for o mesmo dia, 0 caso contrário
 * 
 * @note Compara apenas ano, mês e dia (ignora hora e minuto).
 */
int mesmoDia(DataHora d1, DataHora d2) {
    return (d1.dia == d2.dia && d1.mes == d2.mes && d1.ano == d2.ano);
}

/**
 * Calcula o número de dias entre duas datas/horas.
 * Considera dias completos e incompletos conforme especificação do tarifário.
 * 
 * @param entrada Data/hora de entrada
 * @param saida Data/hora de saída
 * @return Número de dias a considerar para tarifa T4
 * 
 * @note Regra: número de dias = soma dos dias completos e incompletos
 * @note Sempre retorna pelo menos 1 dia
 */
int contarDias(DataHora entrada, DataHora saida) {
    // Converter para struct tm (só data, ignorando hora/minuto)
    struct tm entrada_tm = {0};
    struct tm saida_tm = {0};
    
    entrada_tm.tm_year = entrada.ano - 1900;
    entrada_tm.tm_mon = entrada.mes - 1;
    entrada_tm.tm_mday = entrada.dia;
    
    saida_tm.tm_year = saida.ano - 1900;
    saida_tm.tm_mon = saida.mes - 1;
    saida_tm.tm_mday = saida.dia;
    
    // Converter para time_t
    time_t entrada_time = mktime(&entrada_tm);
    time_t saida_time = mktime(&saida_tm);
    
    if (entrada_time == -1 || saida_time == -1) {
        return 1;  // Retornar 1 dia como fallback em caso de erro
    }
    
    // Calcular diferença em segundos e converter para dias
    double diferenca = difftime(saida_time, entrada_time);
    int dias = (int)(diferenca / (60 * 60 * 24));  // Segundos → dias
    
    // Garantir pelo menos 1 dia
    return dias >= 1 ? dias : 1;
}

// -----------------------------------------------------------------------------
// FUNÇÃO: calcularValor() - CÁLCULO DE TARIFAS
// -----------------------------------------------------------------------------

/**
 * Calcula o valor a pagar por um estacionamento com base no tarifário.
 * Implementa todas as regras de cálculo especificadas no enunciado.
 * 
 * @param e Ponteiro para o estacionamento a calcular
 * @param sistema Ponteiro para as tarifas vigentes
 * @return Valor a pagar em euros
 * 
 * @note Implementa todas as regras:
 *       a) 8:00-21:59: T1 €/hora (15 min)
 *       b) 22:00-7:59: T2 €/hora (15 min)
 *       c) Dia completo (sem mudança de dia): T3 €
 *       d) Vários dias (≥2 mudanças): T4 €/dia
 * 
 * @note Regras especiais:
 *       1. Arredonda entrada/saída para múltiplos de 15 minutos
 *       2. Se valor horário > dia completo (sem mudar de dia), aplica T3
 *       3. Para vários dias, conta todos os dias (completos e incompletos)
 */
float calcularValor(Estacionamento *e, TarifasSistema *sistema) {
    // Validação de parâmetros
    if (e == NULL || sistema == NULL) {
        return 0.0f;
    }
    
    // Criar cópias das datas para não alterar originais
    DataHora entrada = e->entrada;
    DataHora saida = e->saida;
    
    // 1. Arredondar datas conforme regra do enunciado
    arredondarPara15Minutos(&entrada);
    arredondarPara15Minutos(&saida);
    
    // 2. Verificar se está no mesmo dia
    if (mesmoDia(entrada, saida)) {
        // CASO A: Estacionamento dentro do mesmo dia
        
        // Calcular duração total em minutos
        int duracaoMinutos = calcularDiferencaMinutos(entrada, saida);
        int periodos15min = (duracaoMinutos + 14) / 15;  // Arredondamento para cima
        
        // Inicializar acumuladores para cada tarifa
        float valorT1 = 0.0f;  // Tarifa diurna (8:00-22:00)
        float valorT2 = 0.0f;  // Tarifa noturna (22:00-8:00)
        
        // Simular passagem do tempo em intervalos de 15 minutos
        DataHora atual = entrada;
        int minutosRestantes = duracaoMinutos;
        
        while (minutosRestantes > 0) {
            // Determinar duração deste período (máximo 15 minutos)
            int minutosNoPeriodo = (minutosRestantes > 15) ? 15 : minutosRestantes;
            
            // Determinar em qual tarifa este período se enquadra
            int horaMinutos = atual.hora * 100 + atual.minuto;  // Formato HHMM
            
            if (horaMinutos >= 800 && horaMinutos <= 2159) {
                // Período diurno: aplica tarifa T1
                // T1.valor é por hora, dividir por 4 para 15 minutos
                valorT1 += (sistema->tarifaT1.valor / 4.0f) * (minutosNoPeriodo / 15.0f);
            } else {
                // Período noturno: aplica tarifa T2
                valorT2 += (sistema->tarifaT2.valor / 4.0f) * (minutosNoPeriodo / 15.0f);
            }
            
            // Avançar no tempo
            minutosRestantes -= minutosNoPeriodo;
            atual.minuto += minutosNoPeriodo;
            
            // Ajustar hora/dia se necessário
            if (atual.minuto >= 60) {
                atual.minuto -= 60;
                atual.hora += 1;
                if (atual.hora >= 24) {
                    atual.hora = 0;
                }
            }
        }
        
        // Calcular valor total pelo método horário
        float valorHorario = valorT1 + valorT2;
        
        // Aplicar regra especial: se valor horário > T3, usar T3 (dia completo)
        if (valorHorario > sistema->tarifaT3.valor) {
            return sistema->tarifaT3.valor;
        }
        
        return valorHorario;
        
    } else {
        // CASO B: Estacionamento com mudança de dia
        
        // Contar número total de dias
        int numDias = contarDias(entrada, saida);
        
        if (numDias == 1) {
            // CASO B1: Mudou de dia mas ainda é considerado 1 dia
            // (ex: entra 23:50, sai 00:10)
            
            // Calcular valor por método horário (mesma lógica acima)
            int duracaoMinutos = calcularDiferencaMinutos(entrada, saida);
            
            DataHora atual = entrada;
            int minutosRestantes = duracaoMinutos;
            float valorTotal = 0.0f;
            
            while (minutosRestantes > 0) {
                int minutosNoPeriodo = (minutosRestantes > 15) ? 15 : minutosRestantes;
                int horaMinutos = atual.hora * 100 + atual.minuto;
                
                if (horaMinutos >= 800 && horaMinutos <= 2159) {
                    valorTotal += (sistema->tarifaT1.valor / 4.0f) * (minutosNoPeriodo / 15.0f);
                } else {
                    valorTotal += (sistema->tarifaT2.valor / 4.0f) * (minutosNoPeriodo / 15.0f);
                }
                
                minutosRestantes -= minutosNoPeriodo;
                atual.minuto += minutosNoPeriodo;
                if (atual.minuto >= 60) {
                    atual.minuto -= 60;
                    atual.hora += 1;
                    if (atual.hora >= 24) {
                        atual.hora = 0;
                        atual.dia += 1;
                    }
                }
            }
            
            return valorTotal;
            
        } else {
            // CASO B2: Vários dias (≥2 mudanças de dia)
            // Aplicar tarifa T4 por dia
            return numDias * sistema->tarifaT4.valor;
        }
    }
}

// -----------------------------------------------------------------------------
// FUNÇÕES DE PAGINAÇÃO (Funcionalidade 10)
// -----------------------------------------------------------------------------

/**
 * Inicializa uma estrutura de paginação com os parâmetros fornecidos.
 * 
 * @param p Ponteiro para a estrutura Paginacao a inicializar
 * @param totalItens Número total de itens a paginar
 * @param itensPorPagina Número de itens a mostrar por página
 * 
 * @note Calcula automaticamente o número total de páginas.
 * @note Define a página atual como 1 (primeira página).
 */
void inicializarPaginacao(Paginacao *p, int totalItens, int itensPorPagina) {
    p->paginaAtual = 1;                    // Começar na primeira página
    p->totalItens = totalItens;            // Total de itens na listagem
    p->itensPorPagina = itensPorPagina;    // Itens por página (ex: 20)
    
    // Calcular total de páginas (arredondamento para cima)
    p->totalPaginas = (totalItens + itensPorPagina - 1) / itensPorPagina;
}

/**
 * Mostra uma página específica de uma listagem de dados.
 * Implementa o mecanismo de navegação página a página conforme
 * funcionalidade 10 do enunciado, com extras opcionais.
 * 
 * @param p Ponteiro para a estrutura Paginacao com estado atual
 * @param mostrarItem Função de callback que sabe como mostrar um item específico
 * @param data Dados a serem passados para a função de callback
 * @return 1 para continuar mostrando páginas, 0 para sair
 * 
 * @note Oferece opções de navegação (extras valorizados):
 *       - (N) Próxima página
 *       - (P) Página anterior (extra)
 *       - (F) Primeira página (extra)
 *       - (U) Última página (extra)
 *       - (E) Página específica (extra)
 *       - (S) Sair da paginação
 */
int mostrarPagina(Paginacao *p, void (*mostrarItem)(int index, void *data), void *data) {
    // Mostrar cabeçalho com informação da página atual
    printf("\n=== Página %d de %d ===\n", p->paginaAtual, p->totalPaginas);
    
    // Calcular intervalo de itens a mostrar nesta página
    int inicio = (p->paginaAtual - 1) * p->itensPorPagina;
    int fim = inicio + p->itensPorPagina;
    if (fim > p->totalItens) fim = p->totalItens;  // Ajustar última página
    
    // Mostrar cada item da página atual usando a função callback
    for (int i = inicio; i < fim; i++) {
        mostrarItem(i, data);
    }
    
    // Mostrar menu de navegação
    printf("\nNavegação: (N) Próxima, (P) Anterior, (F) Primeira, (U) Última, (E) Específica, (S) Sair\n");
    printf("Escolha: ");
    
    // Ler opção do utilizador
    char opcao;
    scanf(" %c", &opcao);  // Espaço antes de %c para ignorar whitespace
    opcao = toupper(opcao);
    
    // Processar opção escolhida
    switch (opcao) {
        case 'N':  // Próxima página
            if (p->paginaAtual < p->totalPaginas) {
                p->paginaAtual++;
            } else {
                printf("Já está na última página.\n");
            }
            break;
            
        case 'P':  // Página anterior (extra)
            if (p->paginaAtual > 1) {
                p->paginaAtual--;
            } else {
                printf("Já está na primeira página.\n");
            }
            break;
            
        case 'F':  // Primeira página (extra)
            p->paginaAtual = 1;
            break;
            
        case 'U':  // Última página (extra)
            p->paginaAtual = p->totalPaginas;
            break;
            
        case 'E':  // Página específica (extra)
            printf("Digite o número da página (1 a %d): ", p->totalPaginas);
            int novaPagina;
            if (scanf("%d", &novaPagina) == 1 && novaPagina >= 1 && novaPagina <= p->totalPaginas) {
                p->paginaAtual = novaPagina;
            } else {
                printf("Página inválida.\n");
            }
            break;
            
        case 'S':  // Sair da paginação
            return 0;  // Indica que o utilizador quer sair
            
        default:
            printf("Opção inválida.\n");
    }
    
    // Limpar buffer de entrada para próxima iteração
    getchar();
    
    return 1;  // Continuar mostrando páginas
}