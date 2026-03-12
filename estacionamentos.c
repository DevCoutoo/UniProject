/**
 * @file estacionamentos.c
 * @brief Módulo para gestão de operações de estacionamento
 * @brief Contém funções para persistência de dados, gestão da lista de estacionamentos e operações relacionadas
 * @author Rodrigo Couto
 * 
 * @note Este módulo implementa as funcionalidades 9 e 10 do enunciado, relacionadas com a persistência de dados
 * @note Validações: capacidade máxima de 5 pisos, 26 filas (A-Z), 50 lugares por fila (01-50)
 * @note Estruturas de dados: utiliza alocação dinâmica para otimizar uso de memória
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include "estacionamentos.h"
#include "datastructs.h"
#include "utils.h"
#include "parque.h"
#include "tarifas.h"
#include "logs.h"

/* Variáveis globais externas - declaradas em outros módulos */
extern ParqueDinamico parqueDinamico;  /* Estrutura que gerencia o estado do parque de estacionamento */
extern TarifasSistema sistema;         /* Estrutura que armazena as tarifas do sistema */

/**
 * @brief Obtém o próximo ID sequencial disponível para um novo estacionamento
 * 
 * Esta função calcula o próximo número de registo sequencial baseado no total
 * de estacionamentos existentes. É usada para garantir unicidade dos IDs.
 * 
 * @return int - Próximo ID disponível (sempre > 0)
 * 
 * @note Implementa o requisito do enunciado: "número sequencial de entrada (gerado automaticamente)"
 * @note Valor mínimo: 1
 */
int obterProximoId(void);

/**
 * @brief Salva todos os estacionamentos ativos no ficheiro binário
 * 
 * Esta função persiste o array completo de estacionamentos no ficheiro "estacionamentos.dat"
 * em formato binário. Salva apenas se houver dados válidos na lista.
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 9 do enunciado: persistência em ficheiros binários
 * @note Formato do ficheiro: binário, estrutura Estacionamento
 * @note Logs: registra sucesso/falha no sistema de logs
 * 
 * @warning Não salva se a lista estiver vazia ou nula
 */
void salvarEstacionamentos() {
    /* Verifica se existem estacionamentos para salvar */
    if (!listaEstacionamentos.estacionamentos || listaEstacionamentos.total == 0) {
        adicionarLog(LOG_WARNING, "Tentativa de salvar estacionamentos vazios");
        return;
    }
    
    /* Abre ficheiro para escrita binária */
    FILE *file = fopen("estacionamentos.dat", "wb");
    if (file == NULL) {
        adicionarLog(LOG_ERROR, "Erro ao abrir estacionamentos.dat para salvamento");
        return;
    }
    
    /* 
     * Bloco de escrita: salva todos os estacionamentos de uma vez
     * Estratégia mais eficiente que escrita individual
     */
    size_t escritos = fwrite(listaEstacionamentos.estacionamentos, 
                           sizeof(Estacionamento), 
                           listaEstacionamentos.total, 
                           file);
    fclose(file);
    
    /* Verifica se todos os registos foram escritos corretamente */
    if (escritos != (size_t)listaEstacionamentos.total) {
        char msgErro[150];  /* Buffer para mensagem de erro detalhada */
        snprintf(msgErro, sizeof(msgErro), 
                 "Erro ao salvar estacionamentos: escritos %zu de %d", 
                 escritos, listaEstacionamentos.total);
        adicionarLog(LOG_ERROR, msgErro);
    } else {
        char msgSucesso[100];  /* Buffer para mensagem de sucesso */
        snprintf(msgSucesso, sizeof(msgSucesso), 
                 "Estacionamentos salvos: %d registos", listaEstacionamentos.total);
        adicionarLog(LOG_INFO, msgSucesso);
    }
}

/**
 * @brief Salva todos os dados do sistema em formato binário
 * 
 * Função abrangente que persiste múltiplos componentes do sistema:
 * 1. Lista de estacionamentos
 * 2. Estado do parque
 * 3. Numeração de registos
 * 4. Tarifas (se implementado)
 * 
 * @return void
 * 
 * @note Implementa a funcionalidade 9 completa do enunciado
 * @note Deve ser chamada ao sair da aplicação (persistência obrigatória)
 * @note Ficheiros gerados: estacionamentos.dat, parque.dat, numeracao.dat
 * 
 * @see salvarEstacionamentos()
 * @see salvarParque()
 */
void salvarDadosBinarios() {
    char msg[200];  /* Buffer para mensagens de log */
    
    /* 1. Salvar estacionamentos se existirem */
    if (listaEstacionamentos.total > 0) {
        salvarEstacionamentos();
    }
    
    /* 2. Salvar estado atual do parque */
    salvarParque();
    
    /* 3. Salvar numeração de registos para manter sequência */
    FILE *fileNum = fopen("numeracao.dat", "wb");
    if (fileNum != NULL) {
        /* Calcula próximo ID baseado no total atual */
        int proximoId = listaEstacionamentos.total + 1;
        fwrite(&proximoId, sizeof(int), 1, fileNum);
        fclose(fileNum);
        adicionarLog(LOG_INFO, "Numeração de registos salva");
    }
    
    /* 4. Salvar tarifas (funcionalidade opcional - comentada) */
    /* salvarTarifasBinario(&sistema); */
    
    printf("✓ Todos os dados foram salvos em formato binário.\n");
}

/**
 * @brief Menu interativo para salvamento manual de dados
 * 
 * Apresenta um menu ao utilizador com opções de salvamento específico.
 * Permite controlo granular sobre quais dados persistir.
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 9: "sempre que o utilizador assim o entender"
 * @note Opções: 1-Estacionamentos, 2-Parque, 3-Tudo, 0-Cancelar
 * @note Valida entrada do utilizador para evitar erros
 * 
 * @warning Limpa buffer de entrada após scanf para evitar problemas
 */
void salvarDadosManual() {
    printf("\n=========================================\n");
    printf("      SALVAR DADOS MANUALMENTE          \n");
    printf("=========================================\n");
    printf("1. Salvar estacionamentos\n");
    printf("2. Salvar estado do parque\n");
    printf("3. Salvar tudo (recomendado)\n");
    printf("0. Cancelar\n");
    printf("Escolha: ");
    
    int opcao;  /* Variável para armazenar escolha do utilizador */
    if (scanf("%d", &opcao) != 1) {
        printf("Opção inválida!\n");
        /* Limpa buffer de entrada para evitar loops infinitos */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();  /* Remove newline do buffer */
    
    /* Estrutura de seleção para processar escolha do utilizador */
    switch (opcao) {
        case 1:
            salvarEstacionamentos();
            break;
        case 2:
            salvarParque();
            break;
        case 3:
            salvarDadosBinarios();
            break;
        case 0:
            printf("Operação cancelada.\n");
            break;
        default:
            printf("Opção inválida!\n");
    }
    
    printf("Pressione Enter para continuar...");
    getchar();
}

/**
 * @brief Executa salvamento automático dos dados ativos
 * 
 * Função chamada periodicamente ou por eventos para salvar dados
 * automaticamente. Verifica se há dados válidos antes de proceder.
 * 
 * @return void
 * 
 * @note Pode ser chamada por timer ou ao fechar aplicação
 * @note Salva apenas se houver dados ativos (veículos no parque)
 * @note Logs detalhados sobre quantidade de dados salvos
 * 
 * @see salvarDadosBinarios()
 */
void salvarDadosAutomatico() {
    adicionarLog(LOG_INFO, "Salvamento automático iniciado...");
    
    /* Conta veículos ativos para decisão de salvamento */
    int veiculosAtivos = 0;  /* Contador de veículos com status ativo */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        /* Verifica se estacionamento está ativo (veículo no parque) */
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
            veiculosAtivos++;
        }
    }
    
    /* 
     * Decisão de salvamento: só salva se houver dados
     * Evita criação de ficheiros vazios
     */
    if (listaEstacionamentos.total > 0 || veiculosAtivos > 0) {
        salvarDadosBinarios();
        
        char msg[150];  /* Buffer para mensagem informativa */
        snprintf(msg, sizeof(msg), 
                 "Salvamento automático concluído: %d registos totais, %d veículos ativos",
                 listaEstacionamentos.total, veiculosAtivos);
        adicionarLog(LOG_INFO, msg);
    } else {
        adicionarLog(LOG_INFO, "Nenhum dado para salvar automaticamente");
    }
}

/**
 * @brief Redimensiona a lista dinâmica de estacionamentos
 * 
 * Ajusta a capacidade do array dinâmico de estacionamentos usando realloc.
 * Mantém os dados existentes e atualiza a estrutura de controlo.
 * 
 * @param novaCapacidade Novo tamanho desejado para a lista (deve ser > 0)
 * @return int - 1 se sucesso, 0 se falha (alocação falhou)
 * 
 * @note Se nova capacidade < total atual, ajusta total (pode perder dados!)
 * @note Estratégia: duplica capacidade quando necessário (crescimento exponencial)
 * @note Valores válidos: novaCapacidade > 0
 * 
 * @warning Redução de capacidade pode truncar dados existentes
 */
int redimensionarListaEstacionamentos(int novaCapacidade) {
    /* Valida parâmetro de entrada */
    if (novaCapacidade <= 0) {
        adicionarLog(LOG_ERROR, "Tentativa de redimensionar para capacidade inválida");
        return 0;
    }
    
    /* 
     * Ajuste para redução: não podemos manter mais elementos que a capacidade
     * Se reduzir, perde-se os elementos excedentes
     */
    if (novaCapacidade < listaEstacionamentos.total) {
        listaEstacionamentos.total = novaCapacidade;
    }
    
    /* Realoca memória para novo tamanho */
    Estacionamento *novoArray = (Estacionamento *)realloc(
        listaEstacionamentos.estacionamentos, 
        novaCapacidade * sizeof(Estacionamento)
    );
    
    /* Verifica se alocação foi bem sucedida */
    if (!novoArray && novaCapacidade > 0) {
        adicionarLog(LOG_ERROR, "Erro ao redimensionar lista de estacionamentos");
        return 0;
    }
    
    /* Atualiza estrutura com novos valores */
    listaEstacionamentos.estacionamentos = novoArray;
    listaEstacionamentos.capacidade = novaCapacidade;
    
    char msg[100];  /* Buffer para mensagem informativa */
    snprintf(msg, sizeof(msg), "Lista de estacionamentos redimensionada para %d elementos", 
             novaCapacidade);
    adicionarLog(LOG_INFO, msg);
    
    return 1;
}

/**
 * @brief Liberta toda a memória alocada para a lista de estacionamentos
 * 
 * Função de limpeza que liberta o array dinâmico e reseta os contadores.
 * Deve ser chamada ao terminar a aplicação para evitar memory leaks.
 * 
 * @return void
 * 
 * @note Zera todos os campos da estrutura após libertação
 * @note Verifica se o ponteiro não é NULL antes de libertar
 * @note Importante para gestão correta de memória dinâmica
 * 
 * @warning Chama free() apenas se estacionamentos != NULL
 */
void libertarListaEstacionamentos() {
    /* Verifica se há memória alocada para libertar */
    if (listaEstacionamentos.estacionamentos) {
        free(listaEstacionamentos.estacionamentos);
        /* Reseta estrutura para estado inicial */
        listaEstacionamentos.estacionamentos = NULL;
        listaEstacionamentos.total = 0;
        listaEstacionamentos.capacidade = 0;
        adicionarLog(LOG_INFO, "Memória da lista de estacionamentos libertada");
    }
}

/**
 * @brief Adiciona um novo estacionamento à lista dinâmica
 * 
 * Insere um novo registo de estacionamento na lista, redimensionando
 * automaticamente se necessário. Atribui ID automático se não fornecido.
 * 
 * @param novo Estrutura Estacionamento com dados do veículo
 * @return int - 1 se inserção bem sucedida, 0 se falha
 * 
 * @note Atribui ID sequencial automático se novo.id <= 0
 * @note Redimensiona lista se capacidade insuficiente (crescimento exponencial)
 * @note Validações: matrícula, datas, lugar (conforme requisitos do enunciado)
 * 
 * @see redimensionarListaEstacionamentos()
 * @see obterProximoId()
 */
int adicionarEstacionamento(Estacionamento novo) {
    /* Verifica se precisa redimensionar (capacidade esgotada) */
    if (listaEstacionamentos.total >= listaEstacionamentos.capacidade) {
        /* Estratégia: duplica capacidade atual */
        int novaCapacidade = listaEstacionamentos.capacidade * 2;
        if (novaCapacidade <= 0) novaCapacidade = 100;  /* Capacidade inicial */
        
        if (!redimensionarListaEstacionamentos(novaCapacidade)) {
            adicionarLog(LOG_ERROR, "Não foi possível adicionar estacionamento - erro de redimensionamento");
            return 0;
        }
    }
    
    /* 
     * Lógica de atribuição de ID automático
     * Prioridade: 1. ID fornecido, 2. Último ID salvo, 3. Sequencial simples
     */
    if (novo.id <= 0) {
        /* Variável estática para manter último ID entre chamadas */
        static int ultimoIdSalvo = 0;
        
        /* Tenta carregar último ID do ficheiro de numeração */
        if (ultimoIdSalvo == 0) {
            FILE *file = fopen("numeracao.dat", "rb");
            if (file != NULL) {
                fread(&ultimoIdSalvo, sizeof(int), 1, file);
                fclose(file);
            }
        }
        
        /* Decisão de qual ID usar */
        if (ultimoIdSalvo > listaEstacionamentos.total) {
            novo.id = ultimoIdSalvo + 1;
            ultimoIdSalvo++;
        } else {
            novo.id = listaEstacionamentos.total + 1;
        }
    }
    
    /* Garantia final: se ID ainda inválido, usa sequencial simples */
    if (novo.id <= 0) {
        novo.id = listaEstacionamentos.total + 1;
    }
    
    /* Insere novo elemento no final da lista */
    listaEstacionamentos.estacionamentos[listaEstacionamentos.total] = novo;
    listaEstacionamentos.total++;
    
    /* Registra operação no sistema de logs */
    char msg[150];
    snprintf(msg, sizeof(msg), "Estacionamento adicionado: ID %d, Matrícula %s", 
             novo.id, novo.matricula);
    adicionarLog(LOG_INFO, msg);
    
    return 1;
}

/**
 * @brief Remove um estacionamento da lista usando seu ID como referência
 * 
 * Esta função localiza e remove um estacionamento baseado no ID fornecido.
 * Após a remoção, reorganiza o array para manter a continuidade e atualiza
 * os IDs dos elementos restantes. Pode redimensionar a lista se ficar
 * excessivamente vazia para otimizar uso de memória.
 * 
 * @param id ID do estacionamento a remover (deve estar entre 1 e total)
 * @return int - 1 se remoção bem sucedida, 0 se falha (ID inválido)
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: eliminação de registos
 * @note Atualiza IDs sequenciais após remoção para manter continuidade
 * @note Valores válidos: id >= 1 && id <= listaEstacionamentos.total
 * 
 * @warning A remoção altera os IDs dos elementos subsequentes
 * @see redimensionarListaEstacionamentos()
 */
int removerEstacionamentoPorId(int id) {
    /* Validação do parâmetro de entrada */
    if (id < 1 || id > listaEstacionamentos.total) {
        adicionarLog(LOG_WARNING, "Tentativa de remover estacionamento com ID inválido");
        return 0;
    }
    
    int index = id - 1;  /* Índice no array (IDs começam em 1, índices em 0) */
    Estacionamento removido = listaEstacionamentos.estacionamentos[index];
    
    /* 
     * Bloco de reorganização: move elementos para preencher o espaço vazio
     * Inicia no índice do elemento removido e vai até penúltimo elemento
     */
    for (int i = index; i < listaEstacionamentos.total - 1; i++) {
        /* Move elemento seguinte para posição atual */
        listaEstacionamentos.estacionamentos[i] = listaEstacionamentos.estacionamentos[i + 1];
        /* Atualiza ID para refletir nova posição (IDs são sequenciais) */
        listaEstacionamentos.estacionamentos[i].id = i + 1;
    }
    
    listaEstacionamentos.total--;  /* Atualiza contador total de elementos */
    
    /* 
     * Otimização de memória: redimensiona se lista estiver muito vazia
     * Condição: capacidade > 100 E ocupação < 25% da capacidade
     * Estratégia: reduz capacidade pela metade (mínimo 100)
     */
    if (listaEstacionamentos.capacidade > 100 && 
        listaEstacionamentos.total < listaEstacionamentos.capacidade / 4) {
        int novaCapacidade = listaEstacionamentos.capacidade / 2;
        if (novaCapacidade < 100) novaCapacidade = 100;
        redimensionarListaEstacionamentos(novaCapacidade);
    }
    
    /* Registra operação no sistema de logs */
    char msg[150];
    snprintf(msg, sizeof(msg), "Estacionamento removido: ID %d, Matrícula %s", 
             id, removido.matricula);
    adicionarLog(LOG_INFO, msg);
    
    return 1;
}

/**
 * @brief Localiza um estacionamento na lista usando seu ID
 * 
 * Retorna um ponteiro para a estrutura Estacionamento correspondente ao ID
 * fornecido. Acesso direto por índice (O(1)) já que IDs são sequenciais.
 * 
 * @param id ID do estacionamento a buscar (deve estar entre 1 e total)
 * @return Estacionamento* - Ponteiro para o estacionamento encontrado
 *                          NULL se ID inválido ou lista vazia
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: consulta de registos
 * @note Acesso direto: índice = id - 1 (complexidade O(1))
 * @note Não valida se estacionamento está ativo ou inativo
 * 
 * @warning Retorna ponteiro para dados internos - não modificar diretamente sem validação
 */
Estacionamento* buscarEstacionamentoPorId(int id) {
    /* Validação básica do parâmetro */
    if (id < 1 || id > listaEstacionamentos.total) {
        return NULL;
    }
    /* Retorna endereço direto (acesso por índice) */
    return &listaEstacionamentos.estacionamentos[id - 1];
}

/**
 * @brief Localiza um estacionamento pela matrícula do veículo
 * 
 * Percorre a lista de estacionamentos procurando por uma matrícula específica.
 * Pode filtrar apenas estacionamentos ativos (veículos ainda no parque).
 * 
 * @param matricula String com a matrícula a buscar (formato: XX-XX-XX)
 * @param apenasAtivos Flag para filtrar apenas estacionamentos ativos (1=filtrar, 0=todos)
 * @return Estacionamento* - Ponteiro para o primeiro estacionamento encontrado
 *                          NULL se não encontrado ou parâmetros inválidos
 * 
 * @note Implementa requisito do enunciado: busca por matrícula para operações de saída
 * @note Busca linear (O(n)) - adequado para listas de tamanho moderado
 * @note Validações: matrícula não nula, formato compatível com sistema
 * 
 * @see Estacionamento.ativo - Campo que indica se veículo ainda está no parque (1=sim, 0=não)
 */
Estacionamento* buscarEstacionamentoPorMatricula(const char *matricula, int apenasAtivos) {
    /* Validação de parâmetros de entrada */
    if (!matricula || !listaEstacionamentos.estacionamentos) {
        return NULL;
    }
    
    /* 
     * Busca linear através de todos os estacionamentos
     * Percorre array do início ao fim
     */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        /* Compara matrículas (case-sensitive) */
        if (strcmp(listaEstacionamentos.estacionamentos[i].matricula, matricula) == 0) {
            /* Aplica filtro de estacionamentos ativos se solicitado */
            if (apenasAtivos && listaEstacionamentos.estacionamentos[i].ativo != 1) {
                continue;  /* Pula estacionamentos inativos quando filtro ativo */
            }
            return &listaEstacionamentos.estacionamentos[i];
        }
    }
    
    return NULL;  /* Matrícula não encontrada */
}

/**
 * @brief Calcula o próximo ID sequencial disponível para novos estacionamentos
 * 
 * Retorna o número que deve ser atribuído ao próximo estacionamento,
 * baseado na quantidade atual de elementos na lista.
 * 
 * @return int - Próximo ID disponível (sempre total_atual + 1)
 * 
 * @note Implementa o requisito do enunciado: "número sequencial de entrada (gerado automaticamente)"
 * @note Não modifica nenhuma estrutura - apenas calcula valor
 * @note Valor mínimo retornado: 1 (se lista vazia)
 * 
 * @warning Esta função não garante unicidade se IDs forem reutilizados
 * @see adicionarEstacionamento() - função que realmente atribui IDs
 */
int obterProximoId() {
    /* Cálculo simples: próximo ID é sempre um a mais que o total atual */
    return listaEstacionamentos.total + 1;
}

// =====================================================================
// FUNÇÕES PRINCIPAIS
// =====================================================================

/**
 * @brief Carrega os estacionamentos a partir de ficheiros persistentes
 * 
 * Esta função implementa o carregamento inicial dos dados do sistema, seguindo
 * a ordem de prioridade:
 * 1. Primeiro tenta carregar do ficheiro binário "estacionamentos.dat" (formato rápido)
 * 2. Se não existir, tenta carregar do ficheiro texto "estacionamentos.txt"
 * 3. Se não existir nenhum, cria um ficheiro binário vazio
 * 
 * Também carrega a numeração sequencial de registos e atualiza o estado
 * do parque com os estacionamentos ativos.
 * 
 * @return void
 * 
 * @note Implementa a funcionalidade 2 do enunciado: carregamento de dados da execução anterior
 * @note Implementa requisitos da funcionalidade 1-b: leitura de ficheiros na inicialização
 * @note Processa conversão de TXT para binário automaticamente se necessário
 * @note Atualiza ocupação do parque em memória com veículos ainda estacionados
 * 
 * @warning Formato do TXT: espera cabeçalho e campos separados por ponto-e-vírgula
 * @see registrarErro() - para registar erros no ficheiro erros.txt conforme requisito
 */
void carregarEstacionamentos() {
    /* 1. CARREGAMENTO DA NUMERAÇÃO DE REGISTOS (para manter sequência entre execuções) */
    FILE *fileNum = fopen("numeracao.dat", "rb");
    if (fileNum != NULL) {
        int ultimoId = 0;  /* Variável para armazenar o último ID usado */
        if (fread(&ultimoId, sizeof(int), 1, fileNum) == 1) {
            /* Configurar para que o próximo ID seja o último + 1 */
            /* Isso mantém a numeração persistente entre execuções */
            adicionarLog(LOG_INFO, "Numeração de registos carregada");
        }
        fclose(fileNum);
    }

    /* 2. TENTATIVA DE CARREGAMENTO DO FICHEIRO BINÁRIO (formato nativo) */
    FILE *file = fopen("estacionamentos.dat", "rb");
    
    if (file != NULL) {
        /* Determinar tamanho do ficheiro para calcular número de registos */
        fseek(file, 0, SEEK_END);
        long tamanho = ftell(file);
        fseek(file, 0, SEEK_SET);
        
        int numEstacionamentos = tamanho / sizeof(Estacionamento);
        
        /* Valida se o ficheiro não está vazio */
        if (numEstacionamentos <= 0) {
            fclose(file);
            adicionarLog(LOG_WARNING, "Ficheiro de estacionamentos vazio");
            return;
        }
        
        /* Redimensionar lista dinâmica se capacidade atual for insuficiente */
        if (numEstacionamentos > listaEstacionamentos.capacidade) {
            if (!redimensionarListaEstacionamentos(numEstacionamentos + 50)) {
                adicionarLog(LOG_ERROR, "Erro ao redimensionar lista para carregar estacionamentos");
                fclose(file);
                return;
            }
        }
        
        /* Bloco de leitura: carrega todos os registos de uma vez */
        size_t lidos = fread(
            listaEstacionamentos.estacionamentos, 
            sizeof(Estacionamento), 
            numEstacionamentos, 
            file
        );
        fclose(file);
        
        /* Verifica integridade da leitura (se leu todos os registos esperados) */
        if (lidos != (size_t)numEstacionamentos) {
            char msg[150];
            snprintf(msg, sizeof(msg), 
                     "Erro na leitura do ficheiro binário: lidos %zu de %d", 
                     lidos, numEstacionamentos);
            
            /* Registra erro no ficheiro erros.txt conforme requisito do enunciado */
            registrarErro("Carregar Estacionamentos", 
                          "Erro de leitura binária", 
                          0,  /* Linha 0 para erros de ficheiro inteiro */
                          msg, 
                          1); /* Descartados os registros faltantes */
            
            adicionarLog(LOG_ERROR, msg);
        } else {
            listaEstacionamentos.total = lidos;
            char msg[100];
            snprintf(msg, sizeof(msg), "%d estacionamentos carregados do ficheiro binário", 
                     listaEstacionamentos.total);
            adicionarLog(LOG_INFO, msg);
        }
    } else {
        /* 3. FALHA NO BINÁRIO: TENTAR CARREGAR DO FICHEIRO TEXTO (backup/conversão) */
        FILE *txtFile = fopen("estacionamentos.txt", "r");
        
        if (txtFile != NULL) {
            adicionarLog(LOG_INFO, "Convertendo estacionamentos.txt para formato binário...");
            
            char linha[256];  /* Buffer para cada linha do ficheiro */
            int linhaNum = 0; /* Contador de linhas para mensagens de erro */
            int convertidos = 0; /* Contador de registos convertidos com sucesso */
            
            /* Ler e ignorar cabeçalho do ficheiro CSV/TXT */
            if (fgets(linha, sizeof(linha), txtFile) == NULL) {
                adicionarLog(LOG_ERROR, "Ficheiro de texto vazio ou erro de leitura");
                fclose(txtFile);
                return;
            }
            linhaNum++;
            
            /* Processar cada linha do ficheiro */
            while (fgets(linha, sizeof(linha), txtFile) != NULL) {
                linhaNum++;
                linha[strcspn(linha, "\n")] = 0; /* Remove newline */
                
                if (strlen(linha) == 0) continue; /* Ignora linhas vazias */
                
                Estacionamento novo;                /* Estrutura temporária para novo estacionamento */
                char matriculaComHifens[MAX_MATRICULA]; /* Buffer para matrícula com hífens */
                
                /* Tenta parsear a linha com formato esperado (14 campos) */
                if (sscanf(linha, "%d;%14[^;];%d-%d-%d %d:%d;%4[^;];%d-%d-%d %d:%d;%f;%d",
                        &novo.id,
                        matriculaComHifens,
                        &novo.entrada.ano, &novo.entrada.mes, &novo.entrada.dia,
                        &novo.entrada.hora, &novo.entrada.minuto,
                        novo.codigoLugar,
                        &novo.saida.ano, &novo.saida.mes, &novo.saida.dia,
                        &novo.saida.hora, &novo.saida.minuto,
                        &novo.valorPago,
                        &novo.ativo) == 14) {
                    
                    /* Processamento bem-sucedido - aplicar validações */
                    removerHifensMatricula(novo.matricula, matriculaComHifens);
                    verificarMatricula(novo.matricula);
                    verificarData(novo.entrada);
                    verificarData(novo.saida);
                    verificarCodigoLugar(novo.codigoLugar);
                    
                    /* Adiciona à lista em memória */
                    if (adicionarEstacionamento(novo)) {
                        convertidos++;
                    }
                } else {
                    /* ERRO NO FORMATO - REGISTRAR EM erros.txt conforme requisito */
                    char msg[150];
                    snprintf(msg, sizeof(msg), 
                            "Linha %d com formato inválido no TXT", linhaNum);
                    
                    registrarErro("Carregar Estacionamentos", 
                                "Formato inválido", 
                                linhaNum, 
                                linha, 
                                1);  /* Descartado = 1 (sim) */
                    
                    adicionarLog(LOG_WARNING, msg);
                }
            }
            
            fclose(txtFile);
            
            char msg[100];
            snprintf(msg, sizeof(msg), "Convertidos %d registos do TXT para binário", convertidos);
            adicionarLog(LOG_INFO, msg);
            
            /* 4. SALVAR NO FORMATO BINÁRIO PARA FUTURAS EXECUÇÕES */
            file = fopen("estacionamentos.dat", "wb");
            if (file != NULL) {
                fwrite(listaEstacionamentos.estacionamentos, sizeof(Estacionamento), 
                       listaEstacionamentos.total, file);
                fclose(file);
                adicionarLog(LOG_INFO, "Ficheiro binário estacionamentos.dat criado com sucesso");
            } else {
                adicionarLog(LOG_ERROR, "Erro ao criar ficheiro binário estacionamentos.dat");
            }
        } else {
            /* 5. NENHUM FICHEIRO EXISTENTE: CRIAR FICHEIRO BINÁRIO VAZIO */
            adicionarLog(LOG_INFO, "Criando novo ficheiro de estacionamentos vazio...");
            file = fopen("estacionamentos.dat", "wb");
            if (file != NULL) {
                fclose(file);
                adicionarLog(LOG_INFO, "Ficheiro estacionamentos.dat criado vazio");
            } else {
                adicionarLog(LOG_ERROR, "Erro ao criar ficheiro estacionamentos.dat");
            }
        }
    }
    
    /* 6. ATUALIZAR ESTADO DO PARQUE COM ESTACIONAMENTOS ATIVOS */
    adicionarLog(LOG_INFO, "Atualizando estado do parque com estacionamentos ativos...");
    
    int veiculosAtivos = 0;     /* Contador de veículos ainda no parque */
    int lugaresAtualizados = 0; /* Contador de lugares marcados como ocupados */
    
    /* Percorre todos os estacionamentos carregados */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        /* Verifica se o estacionamento está ativo (veículo ainda no parque) */
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
            veiculosAtivos++;
            
            /* Busca tripla para encontrar o lugar correspondente no parque */
            for (int p = 0; p < parqueDinamico.numPisos; p++) {
                for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
                    for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                        /* Gera código do lugar atual para comparação */
                        char codigoAtual[10];
                        sprintf(codigoAtual, "%d%c%02d", p+1, 'A'+f, l+1);
                        
                        /* Se encontrar o lugar, marca como ocupado (X) */
                        if (strcmp(codigoAtual, listaEstacionamentos.estacionamentos[i].codigoLugar) == 0) {
                            parqueDinamico.parque[p][f][l].estado = 'X';
                            strcpy(parqueDinamico.parque[p][f][l].codigo, 
                                   listaEstacionamentos.estacionamentos[i].codigoLugar);
                            lugaresAtualizados++;
                            break;
                        }
                    }
                }
            }
        }
    }
    
    /* Log final com estatísticas do carregamento */
    char msg[150];
    snprintf(msg, sizeof(msg), "Carregamento concluído: %d estacionamentos, %d ativos, %d lugares atualizados", 
             listaEstacionamentos.total, veiculosAtivos, lugaresAtualizados);
    adicionarLog(LOG_INFO, msg);
}

/**
 * @brief Inicializa todos os dados do sistema na memória
 * 
 * Função principal de inicialização que prepara as estruturas de dados
 * em memória e carrega informações persistentes.
 * Ordem de inicialização:
 * 1. Aloca memória para lista de estacionamentos se necessário
 * 2. Carrega estacionamentos (binário → texto → cria vazio)
 * 3. Carrega tarifas do sistema
 * 
 * @return void
 * 
 * @note Primeira função a ser chamada no arranque da aplicação
 * @note Implementa a fase de "instalação" da funcionalidade 1 do enunciado
 * @note Valida alocações de memória para evitar crashes
 * 
 * @warning Se falhar alocação de memória, o sistema não pode funcionar
 * @see carregarEstacionamentos()
 * @see carregarTarifas()
 */
void inicializarDados() {
    adicionarLog(LOG_INFO, "Iniciando carregamento de dados...");
    
    /* Inicializar lista dinâmica se não existir (primeira execução) */
    if (listaEstacionamentos.capacidade == 0) {
        listaEstacionamentos.capacidade = 100;  /* Capacidade inicial */
        listaEstacionamentos.total = 0;
        listaEstacionamentos.estacionamentos = malloc(listaEstacionamentos.capacidade * sizeof(Estacionamento));
        if (!listaEstacionamentos.estacionamentos) {
            adicionarLog(LOG_ERROR, "Erro ao alocar memória para lista de estacionamentos");
            return;
        }
        adicionarLog(LOG_INFO, "Lista de estacionamentos inicializada com capacidade 100");
    }
    
    /* Carregar dados persistentes */
    carregarEstacionamentos();
    carregarTarifas(&sistema);
}

/**
 * @brief Regista a entrada de um novo veículo no parque
 * 
 * Implementa o fluxo completo de entrada de um veículo:
 * 1. Valida matrícula
 * 2. Verifica disponibilidade de lugares
 * 3. Atribui lugar automático (política: primeiro livre encontrado)
 * 4. Regista data/hora (manual ou automática)
 * 5. Cria ticket de estacionamento
 * 6. Atualiza estruturas em memória e ficheiros
 * 
 * @return void
 * 
 * @note Implementa a funcionalidade 4-a do enunciado: registo de entrada
 * @note Implementa requisitos do ticket: mostra todas informações disponíveis
 * @note Política de atribuição: primeiro lugar livre encontrado (pode ser otimizada)
 * @note Valida: formato matrícula, disponibilidade lugares, datas válidas
 * 
 * @warning Interage com utilizador via console - valida todas as entradas
 * @see adicionarEstacionamento() - para adicionar à lista
 * @see salvarEstacionamentos() - para persistência imediata
 */
void registarEntrada() {
    /* Verifica se a lista de estacionamentos está inicializada */
    if (!listaEstacionamentos.estacionamentos) {
        /* Inicializar lista se não existir (fallback safety) */
        if (!redimensionarListaEstacionamentos(100)) {
            printf("Erro ao inicializar sistema de estacionamentos!\n");
            adicionarLog(LOG_ERROR, "Erro ao inicializar lista de estacionamentos");
            return;
        }
    }

    /* 1. LEITURA E VALIDAÇÃO DA MATRÍCULA */
    char matricula[MAX_MATRICULA];
    printf("Digite a matrícula do veículo: ");
    if (scanf("%14s", matricula) != 1) {
        printf("Entrada inválida!\n");
        /* Limpa buffer de entrada para evitar problemas */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();  /* Remove newline do buffer */
    
    verificarMatricula(matricula);
    
    /* 2. VERIFICAÇÃO DE LUGARES LIVRES NO PARQUE */
    int lugaresLivres = 0;
    for (int p = 0; p < parqueDinamico.numPisos; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                if (parqueDinamico.parque[p][f][l].estado =='L') {
                    lugaresLivres++;
                }
            }
        }
    }
    
    /* Se não há lugares, rejeita entrada */
    if (lugaresLivres == 0) {
        printf("Não há lugares disponíveis no parque!\n");
        adicionarLog(LOG_WARNING, "Tentativa de entrada com parque cheio");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* 3. BUSCA POR UM LUGAR LIVRE (política: primeiro encontrado) */
    int lugarEncontrado = 0;           /* Flag de controle da busca */
    int pisoEncontrado = -1;           /* Índice do piso do lugar livre */
    int filaEncontrada = -1;           /* Índice da fila do lugar livre */
    int lugarEncontradoIdx = -1;       /* Índice do lugar dentro da fila */
    char codigoLugar[5];               /* Buffer para código do lugar (ex: "1A01") */
    
    /* Busca tripla sequencial (piso → fila → lugar) */
    for (int p = 0; p < parqueDinamico.numPisos && !lugarEncontrado; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso && !lugarEncontrado; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila && !lugarEncontrado; l++) {
                if (parqueDinamico.parque[p][f][l].estado == 'L') {
                    lugarEncontrado = 1;
                    pisoEncontrado = p;
                    filaEncontrada = f;
                    lugarEncontradoIdx = l;
                    
                    /* Gera código do lugar no formato: Piso+Fila+Lugar (ex: "1A01") */
                    sprintf(codigoLugar, "%d%c%02d", p+1, 'A'+f, l+1);
                    break;
                }
            }
            if (lugarEncontrado) break;
        }
        if (lugarEncontrado) break;
    }

    /* Valida se realmente encontrou lugar (safety check) */
    if (!lugarEncontrado) {
        printf("Erro: Não foi possível encontrar um lugar livre.\n");
        adicionarLog(LOG_ERROR, "Não foi possível encontrar lugar livre apesar de contagem positiva");
        return;
    }

    /* 4. OBTENÇÃO DA DATA/HORA DE ENTRADA */
    int escolha;  /* Variável para escolha manual/automática */
    printf("Introduzir data e hora manualmente? (1-Sim | 2-Não [usar hora atual]): ");
    if (scanf("%d", &escolha) != 1) {
        printf("Entrada inválida!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();
    
    DataHora entrada;  /* Estrutura para armazenar data/hora de entrada */
    
    if (escolha == 1) {
        /* Entrada manual pelo utilizador */
        printf("Digite a data e hora de entrada (aaaa mm dd hh mm): ");
        if (scanf("%d %d %d %d %d", &entrada.ano, &entrada.mes, &entrada.dia, 
                  &entrada.hora, &entrada.minuto) != 5) {
            printf("Data/hora inválida!\n");
            adicionarLog(LOG_ERROR, "Data/hora de entrada inválida fornecida");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            return;
        }
        getchar();
        verificarData(entrada);
    } else {
        /* Entrada automática usando hora do sistema */
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        entrada.ano = tm_info->tm_year + 1900;
        entrada.mes = tm_info->tm_mon + 1;
        entrada.dia = tm_info->tm_mday;
        entrada.hora = tm_info->tm_hour;
        entrada.minuto = tm_info->tm_min;
    }

    /* 5. CRIAÇÃO DO REGISTO DE ESTACIONAMENTO */
    Estacionamento novoEstacionamento;
    novoEstacionamento.id = obterProximoId();
    strcpy(novoEstacionamento.matricula, matricula);
    strcpy(novoEstacionamento.codigoLugar, codigoLugar);
    novoEstacionamento.entrada = entrada;
    
    /* Inicializar campos de saída com zeros (ainda não saiu) */
    novoEstacionamento.saida.ano = 0;
    novoEstacionamento.saida.mes = 0;
    novoEstacionamento.saida.dia = 0;
    novoEstacionamento.saida.hora = 0;
    novoEstacionamento.saida.minuto = 0;
    
    novoEstacionamento.valorPago = 0.0f;
    novoEstacionamento.ativo = 1;  /* Marca como ativo (veículo no parque) */

    /* 6. ADIÇÃO À LISTA DINÂMICA */
    if (!adicionarEstacionamento(novoEstacionamento)) {
        printf("Erro ao adicionar estacionamento!\n");
        return;
    }
    
    /* 7. ATUALIZAÇÃO DO ESTADO DO PARQUE EM MEMÓRIA */
    if (pisoEncontrado >= 0 && filaEncontrada >= 0 && lugarEncontradoIdx >= 0) {
        parqueDinamico.parque[pisoEncontrado][filaEncontrada][lugarEncontradoIdx].estado = 'X';
        strcpy(parqueDinamico.parque[pisoEncontrado][filaEncontrada][lugarEncontradoIdx].codigo, 
               codigoLugar);
    }
    
    /* 8. PERSISTÊNCIA IMEDIATA DOS DADOS */
    salvarEstacionamentos();  /* Salva lista de estacionamentos */
    salvarParque();           /* Salva estado atualizado do parque */

    /* 9. EMISSÃO DO TICKET (conforme requisito do enunciado) */
    printf("\n=========================================\n");
    printf("           TICKET DE ENTRADA            \n");
    printf("=========================================\n");
    printf("ID: %d\n", novoEstacionamento.id);
    printf("Matrícula: %s\n", novoEstacionamento.matricula);
    printf("Lugar: %s\n", novoEstacionamento.codigoLugar);
    printf("Entrada: %04d-%02d-%02d %02d:%02d\n",
           entrada.ano, entrada.mes, entrada.dia,
           entrada.hora, entrada.minuto);
    printf("=========================================\n");
    printf("Pressione Enter para continuar........");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

/**
 * @brief Regista a saída de um veículo do parque de estacionamento
 * 
 * Esta função implementa o fluxo completo de saída de um veículo:
 * 1. Verifica existência de veículos ativos
 * 2. Solicita e valida ID do veículo
 * 3. Permite entrada manual ou automática da data/hora de saída
 * 4. Calcula o valor a pagar com base nas tarifas
 * 5. Liberta o lugar ocupado no parque
 * 6. Emite ticket de saída com valor calculado
 * 7. Atualiza estruturas de dados e ficheiros persistentes
 * 
 * @return void
 * 
 * @note Implementa a funcionalidade 6 do enunciado: registo de saída do veículo
 * @note Implementa requisitos de cálculo de tarifas conforme especificações do PDF
 * @note Validações: ID válido, veículo ativo, data saída > data entrada
 * @note Cálculo de valor: usa função calcularValor() que implementa as regras de tarifário
 * 
 * @warning Valida se data de saída é posterior à entrada para evitar erros temporais
 * @warning Verifica se o veículo já saiu anteriormente (ativo == 0)
 * @see calcularValor() - função que implementa o algoritmo complexo de tarifas
 */
void registarSaida() {
    /* Verifica se há estacionamentos registados no sistema */
    if (listaEstacionamentos.total == 0) {
        printf("Nao ha estacionamentos registados.\n");
        adicionarLog(LOG_WARNING, "Tentativa de registar saída sem estacionamentos registados");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* Busca por veículos ativos (com status ativo == 1) */
    int encontrado = 0;  /* Flag para indicar se há veículos no parque */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
            encontrado = 1;
            break;
        }
    }
    
    /* Se não encontrar veículos ativos, informa utilizador e retorna */
    if (!encontrado) {
        printf("Nao ha veiculos ativos no parque.\n");
        adicionarLog(LOG_WARNING, "Tentativa de registar saída sem veículos ativos");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* 1. SOLICITAÇÃO E VALIDAÇÃO DO ID DO VEÍCULO */
    int id;  /* ID do veículo a sair (fornecido pelo utilizador) */
    printf("Digite o ID do veiculo que ira sair: ");
    if (scanf("%d", &id) != 1) {
        printf("Entrada inválida!\n");
        /* Limpa buffer de entrada */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();  /* Remove newline do buffer */
    
    /* Validação do ID: deve estar entre 1 e total de estacionamentos */
    if (id < 1 || id > listaEstacionamentos.total) {
        printf("ID invalido.\n");
        char msg[100];  /* Buffer para mensagem de log */
        snprintf(msg, sizeof(msg), "ID inválido fornecido para saída: %d", id);
        adicionarLog(LOG_WARNING, msg);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* 2. OBTENÇÃO DO PONTEIRO PARA O ESTACIONAMENTO */
    Estacionamento *e = buscarEstacionamentoPorId(id);
    if (!e) {
        printf("Erro ao buscar estacionamento!\n");
        return;
    }
    
    /* Verifica se o veículo já saiu anteriormente (ativo == 0) */
    if (e->ativo == 0) {
        printf("Veiculo ja saiu do parque.\n");
        char msg[100];
        snprintf(msg, sizeof(msg), "Tentativa de registar saída para veículo já saído: ID %d", id);
        adicionarLog(LOG_WARNING, msg);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* 3. OBTENÇÃO DA DATA/HORA DE SAÍDA */
    int escolha;  /* Variável para escolha manual/automática */
    printf("Introduzir data e hora manualmente? (1-Sim | 2-Nao): ");
    if (scanf("%d", &escolha) != 1) {
        printf("Entrada inválida!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();

    DataHora saida;  /* Estrutura para armazenar data/hora de saída */
    
    if (escolha == 1) {
        /* Entrada manual pelo utilizador */
        printf("Digite a data de saida (aaaa mm dd hh mm): ");
        if (scanf("%d %d %d %d %d", &saida.ano, &saida.mes, &saida.dia, 
                  &saida.hora, &saida.minuto) != 5) {
            printf("Data/hora invalida!\n");
            adicionarLog(LOG_ERROR, "Data/hora de saída inválida fornecida");
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            printf("Pressione Enter para continuar...");
            getchar();
            return;
        }
        getchar();
        verificarData(saida);
    } else {
        /* Entrada automática usando hora do sistema */
        time_t t = time(NULL);
        struct tm *tm_info = localtime(&t);
        saida.ano = tm_info->tm_year + 1900;
        saida.mes = tm_info->tm_mon + 1;
        saida.dia = tm_info->tm_mday;
        saida.hora = tm_info->tm_hour;
        saida.minuto = tm_info->tm_min;
    }

    /* 4. VALIDAÇÃO TEMPORAL: SAÍDA DEVE SER POSTERIOR À ENTRADA */
    if (compararDatas(saida, e->entrada) <= 0) {
        printf("ERRO: Data de saída deve ser posterior à data de entrada!\n");
        adicionarLog(LOG_ERROR, "Data de saída anterior à entrada");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* 5. ATUALIZAÇÃO DO REGISTRO DE ESTACIONAMENTO */
    e->saida = saida;                         /* Atualiza data/hora de saída */
    e->valorPago = calcularValor(e, &sistema); /* Calcula tarifa baseada no tempo */
    e->ativo = 0;                             /* Marca como inativo (veículo saiu) */

    /* 6. LIBERTAÇÃO DO LUGAR NO PARQUE (marca como disponível 'L') */
    int lugarLiberado = 0;  /* Flag para controlar término da busca */
    for (int p = 0; p < parqueDinamico.numPisos && !lugarLiberado; p++) {
        for (int f = 0; f < parqueDinamico.numFilasPorPiso && !lugarLiberado; f++) {
            for (int l = 0; l < parqueDinamico.numLugaresPorFila && !lugarLiberado; l++) {
                /* Compara código do lugar para encontrar o correto */
                if (strcmp(parqueDinamico.parque[p][f][l].codigo, e->codigoLugar) == 0) {
                    parqueDinamico.parque[p][f][l].estado = 'L'; /* Marca como disponível */
                    lugarLiberado = 1;
                    break;
                }
            }
            if (lugarLiberado) break;
        }
        if (lugarLiberado) break;
    }

    /* 7. PERSISTÊNCIA IMEDIATA DOS DADOS ATUALIZADOS */
    salvarEstacionamentos();

    /* 8. EMISSÃO DO TICKET DE SAÍDA (conforme requisito do enunciado) */
    printf("\n=========================================\n");
    printf("           TICKET DE SAIDA              \n");
    printf("=========================================\n");
    printf("ID: %d\n", e->id);
    printf("Matrícula: %s\n", e->matricula);
    printf("Lugar: %s\n", e->codigoLugar);
    printf("Entrada: %04d-%02d-%02d %02d:%02d\n",
           e->entrada.ano, e->entrada.mes, e->entrada.dia,
           e->entrada.hora, e->entrada.minuto);
    printf("Saída: %04d-%02d-%02d %02d:%02d\n",
           e->saida.ano, e->saida.mes, e->saida.dia,
           e->saida.hora, e->saida.minuto);
    printf("Valor a pagar: %.2f€\n", e->valorPago);
    printf("=========================================\n");
    
    /* Aguarda confirmação do utilizador antes de continuar */
    printf("\nPressione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    
    /* 9. SALVA ESTADO ATUALIZADO DO PARQUE */
    salvarParque();
}

/**
 * @brief Lista todos os tickets de estacionamento registados no sistema
 * 
 * Exibe uma listagem completa de todos os estacionamentos, incluindo:
 * - Veículos atualmente no parque (ativo == 1)
 * - Veículos que já saíram (ativo == 0)
 * - Formata matrículas com hífens para melhor legibilidade
 * - Mostra datas de saída apenas se o veículo já saiu
 * - Indica estado atual (DENTRO/SAIU)
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: consulta de registos
 * @note Implementa requisitos de formatação: matrículas com hífens na exibição
 * @note Mostra informações completas conforme especificado no enunciado
 * 
 * @warning Listagem pode ser longa se houver muitos registos
 * @note Para melhor usabilidade, pode ser implementado paginação (funcionalidade 10)
 * 
 * @see adicionarHifensMatricula() - formata matrícula para exibição
 * @see limparEcra() - limpa terminal antes de mostrar listagem
 */
void listarTodosTickets() {
    limparEcra();  /* Limpa terminal para melhor visualização */
    printf("\n=== LISTA DE TODOS OS TICKETS DE ESTACIONAMENTO ===\n");
    
    /* Verifica se há tickets para listar */
    if (listaEstacionamentos.total == 0) {
        printf("Nao ha tickets registrados.\n");
    } else {
        /* Itera por todos os estacionamentos na lista */
        for (int i = 0; i < listaEstacionamentos.total; i++) {
            Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
            char matriculaExibicao[MAX_MATRICULA];  /* Buffer para matrícula formatada */
            
            /* Converte matrícula para formato de exibição COM hífens */
            adicionarHifensMatricula(matriculaExibicao, e->matricula);
            
            /* Exibe informações fixas: ID, matrícula, lugar, entrada */
            printf("ID: %d | Matrícula: %s | Lugar: %s | Entrada: %04d-%02d-%02d %02d:%02d | ",
                   e->id, matriculaExibicao, e->codigoLugar,
                   e->entrada.ano, e->entrada.mes, e->entrada.dia,
                   e->entrada.hora, e->entrada.minuto);
            
            /* Exibe saída apenas se veículo já saiu (ativo == 0 e saída válida) */
            if (e->ativo == 0 && e->saida.ano != 0) {
                printf("Saída: %04d-%02d-%02d %02d:%02d | ",
                       e->saida.ano, e->saida.mes, e->saida.dia,
                       e->saida.hora, e->saida.minuto);
            } else {
                printf("Saída: - | ");  /* Placeholder para veículos ainda no parque */
            }
            
            /* Exibe valor pago e estado atual (DENTRO ou SAIU) */
            printf("Valor Pago: %.2f | Estado: %s\n",
                   e->valorPago,
                   e->ativo == 1 ? "DENTRO" : "SAIU");
        }
    }
    
    /* Pausa para permitir leitura da listagem */
    printf("\nPressione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();  /* Captura o Enter pressionado */
}

/**
 * @brief Consulta um ticket de estacionamento específico pelo seu ID
 * 
 * Esta função permite ao utilizador visualizar todas as informações
 * detalhadas de um ticket de estacionamento específico, identificado
 * pelo seu ID único. Mostra dados completos incluindo matrícula formatada,
 * lugar ocupado, data/hora de entrada e saída (se aplicável), valor pago
 * e estado atual.
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: consulta de registos
 * @note Formata a matrícula para exibição com hífens para melhor legibilidade
 * @note Mostra informações diferentes consoante o estado do veículo (ativo/inativo)
 * 
 * @warning Valida se o ID está dentro do intervalo válido (1 a total)
 * @warning Verifica se o ticket existe antes de mostrar informações
 * 
 * @see buscarEstacionamentoPorId() - para localizar o ticket pelo ID
 * @see adicionarHifensMatricula() - para formatação da matrícula
 */
void consultarTicket() {
    /* Verifica se existem tickets registados no sistema */
    if (listaEstacionamentos.total == 0) {
        printf("Não há tickets registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* Solicitação e validação do ID do ticket a consultar */
    int id;  /* ID do ticket fornecido pelo utilizador */
    printf("Digite o ID do ticket a consultar (1 a %d): ", listaEstacionamentos.total);
    if (scanf("%d", &id) != 1 || id < 1 || id > listaEstacionamentos.total) {
        printf("ID inválido!\n");
        /* Limpa buffer de entrada para evitar problemas */
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();  /* Remove newline do buffer */

    /* Busca do ticket pelo ID fornecido */
    Estacionamento *e = buscarEstacionamentoPorId(id);
    
    /* Verifica se o ticket foi encontrado */
    if (!e) {
        printf("Ticket com ID %d não encontrado.\n", id);
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* Exibição do cabeçalho do ticket */
    printf("\n=== TICKET ID: %d ===\n", e->id);
    
    /* Formatação da matrícula para exibição (com hífens) */
    char matriculaExibicao[MAX_MATRICULA];  /* Buffer para matrícula formatada */
    adicionarHifensMatricula(matriculaExibicao, e->matricula);
    
    /* Exibição dos dados fixos do ticket */
    printf("Matrícula: %s\n", matriculaExibicao);
    printf("Lugar: %s\n", e->codigoLugar);
    printf("Entrada: %04d-%02d-%02d %02d:%02d\n",
           e->entrada.ano, e->entrada.mes, e->entrada.dia,
           e->entrada.hora, e->entrada.minuto);
    
    /* Exibição condicional dos dados de saída */
    if (e->ativo == 0 && e->saida.ano != 0) {
        /* Veículo já saiu: mostra dados de saída e valor pago */
        printf("Saída: %04d-%02d-%02d %02d:%02d\n",
               e->saida.ano, e->saida.mes, e->saida.dia,
               e->saida.hora, e->saida.minuto);
        printf("Valor Pago: %.2f€\n", e->valorPago);
    } else {
        /* Veículo ainda no parque: indica que ainda não saiu */
        printf("Saída: (ainda no parque)\n");
    }
    /* Exibe estado atual do veículo (DENTRO ou SAIU) */
    printf("Estado: %s\n", e->ativo == 1 ? "DENTRO" : "SAIU");

    /* Pausa para permitir leitura das informações */
    printf("\nPressione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

/**
 * @brief Remove permanentemente um ticket de estacionamento do sistema
 * 
 * Esta função permite a remoção completa de um ticket de estacionamento
 * do sistema. Inclui múltiplas validações e confirmações do utilizador.
 * Se o veículo ainda estiver no parque (ativo), liberta o lugar ocupado.
 * Atualiza todos os ficheiros persistentes após a remoção.
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: eliminação de registos
 * @note Requer confirmação explícita do utilizador antes de proceder à remoção
 * @note Liberta o lugar no parque se o veículo ainda estivesse estacionado
 * @note Reescreve completamente o ficheiro binário após remoção
 * 
 * @warning Operação irreversível - remove permanentemente o registo
 * @warning Se o veículo estiver no parque, liberta o lugar (muda estado para 'L')
 * 
 * @see buscarEstacionamentoPorId() - para localizar o ticket
 * @see removerEstacionamentoPorId() - para remover da lista em memória
 * @see salvarParque() - para atualizar estado do parque no disco
 */
void removerTicket() {
    /* Verifica se existem tickets para remover */
    if (listaEstacionamentos.total == 0) {
        printf("Não há tickets de estacionamento registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* Solicitação e validação do ID do ticket a remover */
    int id;  /* ID do ticket a remover */
    printf("Digite o ID do ticket a remover (1 a %d): ", listaEstacionamentos.total);
    if (scanf("%d", &id) != 1 || id < 1 || id > listaEstacionamentos.total) {
        printf("ID inválido!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    getchar(); /* Limpar buffer do newline */
    
    /* Busca do ticket pelo ID fornecido */
    Estacionamento *e = buscarEstacionamentoPorId(id);
    if (!e) {
        printf("Ticket não encontrado!\n");
        return;
    }
    
    /* Exibição dos dados do ticket para confirmação do utilizador */
    printf("\n=== TICKET SELECIONADO ===\n");
    printf("ID: %d\n", e->id);
    
    char matriculaExibicao[MAX_MATRICULA];  /* Buffer para matrícula formatada */
    adicionarHifensMatricula(matriculaExibicao, e->matricula);
    
    printf("Matrícula: %s\n", matriculaExibicao);
    printf("Lugar: %s\n", e->codigoLugar);
    printf("Entrada: %04d-%02d-%02d %02d:%02d\n",
           e->entrada.ano, e->entrada.mes, e->entrada.dia,
           e->entrada.hora, e->entrada.minuto);
    printf("Estado: %s\n", e->ativo == 1 ? "DENTRO" : "SAIU");
    
    /* Confirmação explícita do utilizador para evitar remoções acidentais */
    printf("\nTem certeza que deseja REMOVER este ticket?\n");
    printf("1 - Sim, remover permanentemente\n");
    printf("2 - Não, cancelar\n");
    printf("Escolha: ");
    
    int escolha;  /* Variável para armazenar escolha do utilizador */
    if (scanf("%d", &escolha) != 1) {
        printf("Escolha inválida!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    getchar();
    
    /* Se utilizador não confirmar, cancela a operação */
    if (escolha != 1) {
        printf("Remoção cancelada.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }
    
    /* Registo da operação no sistema de logs */
    char msg[150];
    snprintf(msg, sizeof(msg), "Ticket removido: ID %d, Matrícula %s, Lugar %s", 
            e->id, matriculaExibicao, e->codigoLugar);
    adicionarLog(LOG_INFO, msg);
    
    /* Se o veículo ainda está no parque, liberta o lugar ocupado */
    if (e->ativo == 1) {
        int lugarLiberado = 0;  /* Flag para controlar término da busca */
        /* Busca tripla pelo lugar no parque */
        for (int p = 0; p < parqueDinamico.numPisos && !lugarLiberado; p++) {
            for (int f = 0; f < parqueDinamico.numFilasPorPiso && !lugarLiberado; f++) {
                for (int l = 0; l < parqueDinamico.numLugaresPorFila && !lugarLiberado; l++) {
                    if (strcmp(parqueDinamico.parque[p][f][l].codigo, e->codigoLugar) == 0) {
                        parqueDinamico.parque[p][f][l].estado = 'L';  /* Marca como disponível */
                        lugarLiberado = 1;
                        break;
                    }
                }
                if (lugarLiberado) break;
            }
            if (lugarLiberado) break;
        }
    }
    
    /* Remove o ticket da lista em memória */
    if (!removerEstacionamentoPorId(id)) {
        printf("Erro ao remover ticket!\n");
        return;
    }
    
    /* Reescreve o ficheiro binário completo com a lista atualizada */
    FILE *file = fopen("estacionamentos.dat", "wb");
    if (file != NULL) {
        /* Escrita sequencial de todos os tickets restantes */
        for (int i = 0; i < listaEstacionamentos.total; i++) {
            fwrite(&listaEstacionamentos.estacionamentos[i], sizeof(Estacionamento), 1, file);
        }
        fclose(file);
        printf("Ticket removido permanentemente com sucesso.\n");
    } else {
        printf("Erro ao atualizar o ficheiro de estacionamentos.\n");
        adicionarLog(LOG_ERROR, "Erro ao atualizar ficheiro após remoção de ticket");
    }
    
    /* Salva o estado atualizado do parque (se lugar foi libertado) */
    salvarParque();
    
    /* Pausa para confirmar operação */
    printf("Pressione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

/**
 * @brief Permite alterar dados de um ticket de estacionamento existente
 * 
 * Esta função permite modificar informações de um ticket de estacionamento,
 * com comportamentos diferentes consoante o estado do veículo:
 * - Tickets fechados (veículo saiu): apenas permite alterar a matrícula
 * - Tickets ativos (veículo no parque): permite alterar matrícula ou lugar
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: alteração de registos
 * @note Comportamento diferenciado consoante estado do ticket (ativo/inativo)
 * @note Para alteração de lugar: valida disponibilidade e atualiza estado do parque
 * @note Atualiza ficheiros persistentes após alteração
 * 
 * @warning Alteração de lugar só é possível se houver lugares livres
 * @warning Para tickets fechados, apenas matrícula pode ser alterada
 * 
 * @see buscarEstacionamentoPorId() - para localizar o ticket
 * @see verificarMatricula() - para validar nova matrícula
 * @see verificarCodigoLugar() - para validar novo lugar
 * @see salvarParque() - para atualizar estado do parque
 */
void alterarTicket() {
    /* Verifica se existem tickets para alterar */
    if (listaEstacionamentos.total == 0) {
        printf("Não há tickets de estacionamento registados.\n");
        printf("Pressione Enter para continuar...");
        getchar();
        return;
    }

    /* Solicitação do ID do ticket a alterar */
    int id;  /* ID do ticket a alterar */
    printf("Digite o ID do ticket a alterar (1 a %d): ", listaEstacionamentos.total);
    if (scanf("%d", &id) != 1) {
        printf("Entrada inválida!\n");
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    /* Busca do ticket pelo ID fornecido */
    Estacionamento *e = buscarEstacionamentoPorId(id);

    /* Verifica se o ticket foi encontrado */
    if (!e) {
        printf("Ticket com ID %d não encontrado.\n", id);
        int c;
        while ((c = getchar()) != '\n' && c != EOF);
        return;
    }
    
    getchar(); /* Limpa o buffer do newline */
    
    /* Registo do início da operação de alteração */
    char msgInicio[100];
    snprintf(msgInicio, sizeof(msgInicio), "Início da alteração do ticket ID %d", id);
    adicionarLog(LOG_INFO, msgInicio);
    
    /* Verifica o estado do ticket para determinar operações permitidas */
    if (e->ativo == 0) {
        /* TICKET FECHADO: veículo já saiu, só pode alterar matrícula */
        printf("Este ticket já foi fechado (saída registada). Apenas é possível alterar a matrícula.\n");
        char novaMatricula[MAX_MATRICULA];  /* Buffer para nova matrícula */
        printf("Digite a nova matrícula (atual: %s): ", e->matricula);
        scanf("%14s", novaMatricula);
        
        /* Validação da nova matrícula */
        verificarMatricula(novaMatricula);
        
        /* Registo da alteração para possível rollback */
        char matriculaAntiga[MAX_MATRICULA];
        strcpy(matriculaAntiga, e->matricula);
        strcpy(e->matricula, novaMatricula);
        
        /* Log da alteração realizada */
        char msg[150];
        snprintf(msg, sizeof(msg), "Ticket %d: Matrícula alterada de '%s' para '%s'", 
                id, matriculaAntiga, novaMatricula);
        adicionarLog(LOG_INFO, msg);
    } else {
        /* TICKET ATIVO: veículo ainda no parque, pode alterar matrícula ou lugar */
        char opcao;  /* Variável para escolha do tipo de alteração */
        printf("Ticket ativo. Deseja alterar a matrícula (M) ou o lugar (L)? ");
        scanf(" %c", &opcao);
        opcao = toupper(opcao);
        getchar();

        if (opcao == 'M') {
            /* ALTERAÇÃO DE MATRÍCULA */
            char novaMatricula[MAX_MATRICULA];
            printf("Digite a nova matrícula (atual: %s): ", e->matricula);
            scanf("%14s", novaMatricula);
            
            /* Validação da nova matrícula */
            verificarMatricula(novaMatricula);
            
            /* Registo da alteração */
            char matriculaAntiga[MAX_MATRICULA];
            strcpy(matriculaAntiga, e->matricula);
            strcpy(e->matricula, novaMatricula);
            
            /* Log da alteração */
            char msg[150];
            snprintf(msg, sizeof(msg), "Ticket %d: Matrícula alterada de '%s' para '%s'", 
                    id, matriculaAntiga, novaMatricula);
            adicionarLog(LOG_INFO, msg);
        } else if (opcao == 'L') {
            /* ALTERAÇÃO DE LUGAR: mostra lugares disponíveis primeiro */
            printf("Lugares disponíveis:\n");
            int lugaresLivres = 0;  /* Contador de lugares livres */
            /* Lista todos os lugares livres no parque */
            for (int p = 0; p < parqueDinamico.numPisos; p++) {
                for (int f = 0; f < parqueDinamico.numFilasPorPiso; f++) {
                    for (int l = 0; l < parqueDinamico.numLugaresPorFila; l++) {
                        if (parqueDinamico.parque[p][f][l].estado == 'L') {
                            printf("  %s\n", parqueDinamico.parque[p][f][l].codigo);
                            lugaresLivres++;
                        }
                    }
                }
            }

            /* Verifica se há lugares livres para onde mover o veículo */
            if (lugaresLivres == 0) {
                printf("Não há lugares livres. Não é possível alterar o lugar.\n");
                adicionarLog(LOG_WARNING, "Tentativa de alterar lugar sem lugares livres disponíveis");
            } else {
                char novoLugar[5];  /* Buffer para código do novo lugar */
                printf("Digite o novo lugar (atual: %s): ", e->codigoLugar);
                scanf("%4s", novoLugar);
                getchar();
                
                /* Validação do formato do novo lugar */
                verificarCodigoLugar(novoLugar);

                /* Verifica se o novo lugar existe e está livre */
                int encontrado = 0;  /* Flag para controlar término da busca */
                for (int p = 0; p < parqueDinamico.numPisos && !encontrado; p++) {
                    for (int f = 0; f < parqueDinamico.numFilasPorPiso && !encontrado; f++) {
                        for (int l = 0; l < parqueDinamico.numLugaresPorFila && !encontrado; l++) {
                            if (strcmp(parqueDinamico.parque[p][f][l].codigo, novoLugar) == 0) {
                                encontrado = 1;
                                /* Verifica se o lugar está disponível */
                                if (parqueDinamico.parque[p][f][l].estado == 'L') {
                                    /* Liberta o lugar antigo */
                                    for (int p2 = 0; p2 < parqueDinamico.numPisos; p2++) {
                                        for (int f2 = 0; f2 < parqueDinamico.numFilasPorPiso; f2++) {
                                            for (int l2 = 0; l2 < parqueDinamico.numLugaresPorFila; l2++) {
                                                if (strcmp(parqueDinamico.parque[p2][f2][l2].codigo, e->codigoLugar) == 0) {
                                                    parqueDinamico.parque[p2][f2][l2].estado = 'L';
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                    /* Ocupa o novo lugar */
                                    parqueDinamico.parque[p][f][l].estado = 'X';
                                    
                                    /* Atualiza código do lugar no ticket */
                                    char lugarAntigo[5];
                                    strcpy(lugarAntigo, e->codigoLugar);
                                    strcpy(e->codigoLugar, novoLugar);
                                    
                                    /* Log da alteração de lugar */
                                    char msg[150];
                                    snprintf(msg, sizeof(msg), "Ticket %d: Lugar alterado de '%s' para '%s'", 
                                            id, lugarAntigo, novoLugar);
                                    adicionarLog(LOG_INFO, msg);
                                    
                                    printf("Lugar alterado com sucesso.\n");
                                } else {
                                    printf("Lugar não está disponível.\n");
                                    adicionarLog(LOG_WARNING, "Tentativa de mover para lugar ocupado");
                                }
                                break;
                            }
                        }
                        if (encontrado) break;
                    }
                    if (encontrado) break;
                }

                /* Se não encontrou o lugar especificado */
                if (!encontrado) {
                    printf("Lugar não encontrado.\n");
                    adicionarLog(LOG_WARNING, "Tentativa de mover para lugar inexistente");
                }
            }
        } else {
            /* Opção inválida selecionada */
            printf("Opção inválida.\n");
            adicionarLog(LOG_WARNING, "Opção inválida na alteração de ticket");
        }
    }

    /* Atualiza o ficheiro binário com as alterações realizadas */
    FILE *file = fopen("estacionamentos.dat", "wb");
    if (file != NULL) {
        fwrite(listaEstacionamentos.estacionamentos, sizeof(Estacionamento), 
               listaEstacionamentos.total, file);
        fclose(file);
        printf("Ticket atualizado no ficheiro.\n");
    } else {
        printf("Erro ao abrir o ficheiro de estacionamentos.\n");
        adicionarLog(LOG_ERROR, "Erro ao atualizar ficheiro após alteração de ticket");
    }

    /* Se houve alteração de lugar, atualiza o ficheiro do parque */
    salvarParque();

    /* Pausa para confirmar operação */
    printf("Pressione Enter para continuar...");
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
    getchar();
}

/**
 * @brief Menu de gestão de tickets (consultar/remover/alterar)
 * 
 * Apresenta um menu interativo que permite ao utilizador realizar operações
 * sobre tickets de estacionamento. O menu roda em loop até que o utilizador
 * escolha sair. Inclui tratamento robusto de entradas inválidas.
 * 
 * @return void
 * 
 * @note Implementa parte da funcionalidade 4-b do enunciado: interface para gestão de registos
 * @note Menu em loop: permanece ativo até seleção da opção 0 (voltar)
 * @note Validação de entrada: verifica se scanf conseguiu ler um inteiro
 * @note Logging: registra operações no sistema de logs
 * 
 * @warning Limpa buffer de entrada após scanf para evitar problemas em iterações seguintes
 * @see consultarTicket(), removerTicket(), alterarTicket() - funções chamadas pelo menu
 */
void menuTicket() {
    int opcao;  /* Variável para armazenar escolha do utilizador */
    do {
        limparEcra();  /* Limpa terminal para melhor apresentação */
        printf("=========================================\n");
        printf("       CONSULTAR/REMOVER/ALTERAR        \n");
        printf("=========================================\n");
        printf("1 - Consultar Ticket\n");
        printf("2 - Remover Ticket\n");
        printf("3 - Alterar Ticket\n");
        printf("0 - Voltar\n");
        printf("Escolha: ");
        
        /* Validação da entrada: verifica se foi lido um inteiro */
        if (scanf("%d", &opcao) != 1) {
            printf("Entrada inválida!\n");
            /* Limpa buffer de entrada para evitar loops infinitos */
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
            getchar();
            continue;
        }
        getchar();  /* Remove newline do buffer */

        /* Estrutura de seleção para processar escolha do utilizador */
        switch (opcao) {
            case 1: 
                consultarTicket(); 
                break;
            case 2: 
                removerTicket(); 
                break;
            case 3: 
                alterarTicket(); 
                break;
            case 0: 
                /* Registra no log que o menu foi fechado */
                adicionarLog(LOG_INFO, "Menu de tickets fechado");
                break;
            default: 
                /* Tratamento de opção inválida */
                printf("Opção inválida!\n");
                adicionarLog(LOG_WARNING, "Opção inválida no menu de tickets");
                printf("Pressione Enter para continuar...");
                getchar();
        }
    } while (opcao != 0);  /* Loop até utilizador escolher sair (opção 0) */
}

// =====================================================================
// FUNÇÕES AUXILIARES
// =====================================================================

/**
 * @brief Conta o número de estacionamentos ativos (veículos no parque)
 * 
 * Percorre a lista de estacionamentos e conta quantos têm o campo ativo = 1.
 * 
 * @return int - Número de veículos atualmente estacionados no parque
 * 
 * @note Utilizado para mostrar estatísticas em tempo real no menu principal
 * @note Campo ativo: 1 = veículo no parque, 0 = veículo já saiu
 * 
 * @see listaEstacionamentos.estacionamentos[i].ativo
 */
int obterNumeroEstacionamentosAtivos() {
    int count = 0;  /* Contador de estacionamentos ativos */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
            count++;
        }
    }
    return count;
}

/**
 * @brief Calcula o valor total arrecadado com estacionamentos finalizados
 * 
 * Soma o campo valorPago de todos os estacionamentos que já foram finalizados
 * (ativo = 0). Estacionamentos ativos não contam para este total.
 * 
 * @return float - Valor total em euros arrecadado pelo parque
 * 
 * @note Considera apenas estacionamentos concluídos (veículos que já saíram)
 * @note Formato monetário: euros com duas casas decimais
 * 
 * @see Estacionamento.valorPago - campo que armazena valor pago pelo estacionamento
 */
float obterValorTotalArrecadado() {
    float total = 0.0f;  /* Acumulador do valor total */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        if (listaEstacionamentos.estacionamentos[i].ativo == 0) {
            total += listaEstacionamentos.estacionamentos[i].valorPago;
        }
    }
    return total;
}

/**
 * @brief Verifica se uma matrícula já está estacionada no parque
 * 
 * Percorre a lista de estacionamentos ativos para verificar se uma
 * matrícula específica já se encontra estacionada (ativo = 1).
 * 
 * @param matricula String com a matrícula a verificar (sem hífens)
 * @return int - 1 se a matrícula já está estacionada, 0 caso contrário
 * 
 * @note Usada para prevenir entradas duplicadas do mesmo veículo
 * @note Comparação case-sensitive: matrículas devem estar no mesmo formato
 * 
 * @warning Não verifica matrículas em estacionamentos já finalizados
 */
int matriculaJaEstacionada(const char *matricula) {
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        if (listaEstacionamentos.estacionamentos[i].ativo == 1 &&
            strcmp(listaEstacionamentos.estacionamentos[i].matricula, matricula) == 0) {
            return 1;  /* Matrícula encontrada entre os ativos */
        }
    }
    return 0;  /* Matrícula não encontrada ou veículo já saiu */
}

/**
 * @brief Lista todos os estacionamentos ativos (veículos no parque)
 * 
 * Exibe uma listagem formatada de todos os veículos que estão atualmente
 * estacionados no parque. Inclui ID, matrícula (com hífens), lugar e
 * data/hora de entrada.
 * 
 * @return void
 * 
 * @note Utilizada para operações de gestão e monitorização em tempo real
 * @note Formata matrículas com hífens para melhor legibilidade
 * 
 * @see adicionarHifensMatricula() - função de formatação de matrículas
 */
void listarEstacionamentosAtivos() {
    printf("\n=== ESTACIONAMENTOS ATIVOS ===\n");
    
    int encontrados = 0;  /* Contador de estacionamentos ativos encontrados */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
            Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
            char matriculaExibicao[MAX_MATRICULA];  /* Buffer para matrícula formatada */
            adicionarHifensMatricula(matriculaExibicao, e->matricula);
            
            printf("ID: %d | Matrícula: %s | Lugar: %s | Entrada: %04d-%02d-%02d %02d:%02d\n",
                   e->id, matriculaExibicao, e->codigoLugar,
                   e->entrada.ano, e->entrada.mes, e->entrada.dia,
                   e->entrada.hora, e->entrada.minuto);
            encontrados++;
        }
    }
    
    /* Mensagem apropriada consoante o resultado da listagem */
    if (encontrados == 0) {
        printf("Não há veículos estacionados no momento.\n");
    } else {
        printf("Total: %d veículo(s)\n", encontrados);
    }
}

/**
 * @brief Obtém estatísticas básicas sobre os estacionamentos
 * 
 * Calcula e retorna através de ponteiros várias estatísticas:
 * - Total de estacionamentos registados
 * - Número de estacionamentos ativos
 * - Número de estacionamentos finalizados
 * - Valor total arrecadado
 * 
 * @param total Ponteiro para armazenar total de estacionamentos
 * @param ativos Ponteiro para armazenar número de estacionamentos ativos
 * @param finalizados Ponteiro para armazenar número de estacionamentos finalizados
 * @param valorTotal Ponteiro para armazenar valor total arrecadado
 * 
 * @return void
 * 
 * @note Função utilitária para relatórios e dashboards
 * @note Todos os parâmetros são de saída (escrita)
 */
void obterEstatisticasBasicas(int *total, int *ativos, int *finalizados, float *valorTotal) {
    *total = listaEstacionamentos.total;  /* Total de registos */
    *ativos = 0;    /* Inicializa contador de ativos */
    *finalizados = 0;  /* Inicializa contador de finalizados */
    *valorTotal = 0.0f;  /* Inicializa acumulador de valor */
    
    /* Percorre todos os estacionamentos para calcular estatísticas */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        if (listaEstacionamentos.estacionamentos[i].ativo == 1) {
            (*ativos)++;  /* Incrementa contador de ativos */
        } else {
            (*finalizados)++;  /* Incrementa contador de finalizados */
            *valorTotal += listaEstacionamentos.estacionamentos[i].valorPago;
        }
    }
}

/**
 * @brief Verifica a integridade dos dados de estacionamentos
 * 
 * Realiza uma série de verificações de consistência nos dados:
 * 1. Verifica se a lista está inicializada
 * 2. Valida sequência de IDs
 * 3. Valida formato de matrículas
 * 4. Valida formato de códigos de lugar
 * 5. Valida datas (entrada e saída)
 * 
 * @return int - 1 se dados são consistentes, 0 se foram encontrados erros
 * 
 * @note Implementa requisitos de robustez do enunciado: verificação de dados
 * @note Útil para diagnóstico e manutenção do sistema
 * @note Registra problemas encontrados mas não os corrige automaticamente
 * 
 * @warning Verificação de datas: alerta para anos suspeitos (fora de 2000-2100)
 */
int verificarIntegridadeEstacionamentos() {
    /* Verifica se a lista de estacionamentos está inicializada */
    if (!listaEstacionamentos.estacionamentos) {
        printf("Lista de estacionamentos não inicializada!\n");
        return 0;
    }
    
    int erros = 0;  /* Contador de erros encontrados */
    
    /* Verificação individual de cada estacionamento */
    for (int i = 0; i < listaEstacionamentos.total; i++) {
        Estacionamento *e = &listaEstacionamentos.estacionamentos[i];
        
        /* 1. Verificação da sequência de IDs */
        if (e->id != i + 1) {
            printf("ERRO: ID incorreto no índice %d (ID: %d, esperado: %d)\n", 
                   i, e->id, i + 1);
            erros++;
        }
        
        /* 2. Verificação do formato da matrícula */
        if (strlen(e->matricula) < 6 || strlen(e->matricula) > 8) {
            printf("ERRO: Matrícula inválida no ID %d: %s\n", e->id, e->matricula);
            erros++;
        }
        
        /* 3. Verificação do formato do código do lugar (deve ser 4 caracteres) */
        if (strlen(e->codigoLugar) != 4) {
            printf("ERRO: Código de lugar inválido no ID %d: %s\n", e->id, e->codigoLugar);
            erros++;
        }
        
        /* 4. Verificação básica das datas */
        if (e->entrada.ano < 2000 || e->entrada.ano > 2100) {
            printf("AVISO: Ano de entrada suspeito no ID %d: %d\n", e->id, e->entrada.ano);
            /* Aviso mas não conta como erro */
        }
        
        /* 5. Verificação de consistência temporal (se já saiu) */
        if (e->ativo == 0 && e->saida.ano != 0) {
            if (compararDatas(e->saida, e->entrada) <= 0) {
                printf("ERRO: Data de saída anterior à entrada no ID %d\n", e->id);
                erros++;
            }
        }
    }
    
    /* Retorna resultado da verificação */
    if (erros == 0) {
        printf("Dados de estacionamentos verificados com sucesso.\n");
        return 1;
    } else {
        printf("Encontrados %d erro(s) nos dados de estacionamentos.\n", erros);
        return 0;
    }
}