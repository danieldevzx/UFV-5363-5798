#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Definição da estrutura que armazena informações sobre as instruções
typedef struct {
    const char *mnemonico;      // O mnemônico da instrução
    const char *opcode;         // O código de operação da instrução
    const char *funct3;         // O campo funct3 da instrução
    const char *funct7;         // O campo funct7 da instrução (se existir)
    const char *tipo;           // Tipo da instrução: "R", "I", "S" ou "B"
} InformacaoInstrucao;

// Definição das informações para cada instrução
const InformacaoInstrucao instrucoes[] = {
    {"lb", "0000011", "000", "", "I"},
    {"sb", "0100011", "000", "", "S"},
    {"add", "0110011", "000", "0000000", "R"},
    {"sub", "0110011", "000", "0100000", "R"},
    {"and", "0110011", "111", "0000000", "R"},
    {"or", "0110011", "110", "0000000", "R"},
    {"xor", "0110011", "100", "0000000", "R"},
    {"addi", "0010011", "000", "", "I"},
    {"andi", "0010011", "111", "", "I"},
    {"ori", "0010011", "110", "", "I"},
    {"sll", "0110011", "001", "0000000", "R"},
    {"srl", "0110011", "101", "0000000", "R"},
    {"bne", "1100011", "001", "", "B"},
    {"beq", "1100011", "000", "", "B"}
};

// Função para obter o número do registro a partir de sua representação em string
int obter_numero_registro(const char* reg) {
    return atoi(reg + 1);
}

// Função para converter um número inteiro para sua representação binária em string
void inteiro_para_binario(int num, char *binario, int n) {
    for (int i = n - 1; i >= 0; i--) {
        binario[i] = (num & 1) + '0';
        num >>= 1;
    }
    binario[n] = '\0';
}

// Função para obter o imediato da instrução e convertê-lo para sua representação binária em string
char* obter_imediato(const char* imediato, const char* tipo) {
    long valor = strtol(imediato, NULL, 10);
    char *binario = malloc(13); // Considerando que o imediato tem no máximo 12 bits

    if (strcmp(tipo, "I") == 0 || strcmp(tipo, "S") == 0 || strcmp(tipo, "B") == 0) {
        inteiro_para_binario(valor, binario, 12);
    } else {
        free(binario);
        return NULL;
    }

    return binario;
}

// Função principal para montar a instrução binária
char* montar(const char* instrucao) {
    char opcode[8], rs1[6], rs2[6], rd[6], imediato[13] = "", funct3[4], funct7[8] = "";
    char instrucao_binaria[33];
    instrucao_binaria[0] = '\0'; // Inicialização da string

    const char* delimitadores = " ,()";
    char* token = strtok((char*)instrucao, delimitadores);

    const InformacaoInstrucao *info = NULL;
    for (size_t i = 0; i < sizeof(instrucoes) / sizeof(instrucoes[0]); ++i) {
        if (strcmp(token, instrucoes[i].mnemonico) == 0) {
            info = &instrucoes[i];
            break;
        }
    }

    if (info == NULL) {
        return NULL; // Se a instrução não for reconhecida, retorna NULL
    }

    strcpy(opcode, info->opcode);

    token = strtok(NULL, delimitadores);
    if (token == NULL) {
        return NULL; // Se não houver mais tokens, retorna NULL
    }

    if (strcmp(info->tipo, "R") == 0) {
        strcpy(rd, rs2);
        strcpy(rs2, rs1);
        strcpy(rs1, funct7);
        strcpy(funct7, "");
    } else if (strcmp(info->tipo, "I") == 0) {
        strcpy(rd, rs1);
        strcpy(rs1, imediato);
        strcpy(imediato, "");
    } else if (strcmp(info->tipo, "S") == 0 || strcmp(info->tipo, "B") == 0) {
        strcpy(rd, rs2);
        strcpy(rs2, rs1);
        strcpy(rs1, imediato);
        strcpy(imediato, "");
    }

    strcpy(funct3, info->funct3);

    if (info->funct7[0] != '\0') {
        strcpy(funct7, info->funct7);
    }

    if (info->tipo[0] != 'R') {
        char* imediato_token = token;
        token = strtok(NULL, delimitadores);
        if (token == NULL) {
            return NULL;
        }
        inteiro_para_binario(obter_numero_registro(token), rs1, 5);

        char *imediato_binario = obter_imediato(imediato_token, info->tipo);
        if (imediato_binario == NULL) {
            return NULL;
        }
        strcpy(imediato, imediato_binario);
        free(imediato_binario);
    } else {
        inteiro_para_binario(obter_numero_registro(token), rd, 5);
        token = strtok(NULL, delimitadores);
        if (token == NULL) {
            return NULL;
        }
        inteiro_para_binario(obter_numero_registro(token), rs1, 5);
        token = strtok(NULL, delimitadores);
        if (token == NULL) {
            return NULL;
        }
        inteiro_para_binario(obter_numero_registro(token), rs2, 5);
    }

    strcat(instrucao_binaria, funct7); // Funct7
    strcat(instrucao_binaria, rs2); // rs2
    strcat(instrucao_binaria, rs1); // rs1
    strcat(instrucao_binaria, funct3); // Funct3
    strcat(instrucao_binaria, rd); // rd
    strcat(instrucao_binaria, opcode); // Opcode

    if (info->tipo[0] != 'R') {
        strcat(instrucao_binaria, imediato); // Immediate
    }

    return strdup(instrucao_binaria);
}

// Função para processar as instruções em modo interativo
void processar_modo_interativo() {
    printf("Modo interativo. Digite uma instrução por linha. Pressione Enter após cada instrução.\n");
    printf("Para sair, pressione Enter sem digitar nada.\n");

    char instrucao[256];
    while (fgets(instrucao, sizeof(instrucao), stdin) != NULL && instrucao[0] != '\n') {
        char* instrucao_binaria = montar(instrucao);
        if (instrucao_binaria != NULL) {
            printf("%s\n", instrucao_binaria);
            free(instrucao_binaria);
        } else {
            printf("Erro ao montar a instrução: %s\n", instrucao);
        }
    }
}

// Função para processar as instruções em modo arquivo
void processar_modo_arquivo(const char* arquivo_entrada, const char* arquivo_saida) {
    FILE* entrada = fopen(arquivo_entrada, "r");
    FILE* saida = fopen(arquivo_saida, "w");

    char instrucao[256];
    while (fgets(instrucao, sizeof(instrucao), entrada) != NULL) {
        char* instrucao_binaria = montar(instrucao);
        if (instrucao_binaria != NULL) {
            fprintf(saida, "%s\n", instrucao_binaria);
            printf("%s\n", instrucao_binaria);
            free(instrucao_binaria);
        } else {
            printf("Erro ao montar a instrução: %s\n", instrucao);
        }
    }

    fclose(entrada);
    fclose(saida);
}

// Função principal
int main(int argc, char *argv[]) {
    if (argc == 1) {
        processar_modo_interativo();
    } else if (argc == 3) {
        processar_modo_arquivo(argv[1], argv[2]);
    } else {
        printf("Uso correto: %s <arquivo_entrada.asm> <arquivo_saida>\n", argv[0]);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
