
#include <stdio.h>
#include <stdlib.h>

#define NOP 0x00
#define OP_ADD 0x02
#define OP_SUB 0x03
#define OP_MUL 0x04
#define OP_DIV 0x05
#define OP_PRT 0x06
#define OP_IPT 0x07
#define OP_PUSH 0x08
#define SPECIAL_VALUE 0xFF
#define CODE_SEGMENT_END 0x2E

typedef struct {
    unsigned char opcode;
    unsigned char param1;
    unsigned char param2;
} Instruction;

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <bytecode.asl>\n", argv[0]);
        return 1;
    }
    
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }
    unsigned char mes = 0x00;
    for(int Flash=0;Flash < 512;Flash++){
		fread(&mes, 1, 1, file);
		if (mes != 0x00) {
			printf("%c", mes);		
		}
	}
	printf("\n");
	printf("----------EXECUTE START----------\n");
    int temp = 0; // ��ʼ���ݴ�������tempΪ0
    unsigned char buffer[4]; // ���ڶ�ȡopcode�Ϳ��ܵĲ���
    size_t bytesRead;
    long offset = 0; // ����fseek��ƫ����
    

    while (1) {
        // ��ȡ������
        if (fread(buffer, 1, 1, file) != 1) {
            // �ļ��������ȡ����
            if (feof(file)) {
                break; // ��������
            } else {
                perror("Error reading opcode");
                fclose(file);
                return 1;
            }
        }

        Instruction inst;
        inst.opcode = buffer[0];

        if (inst.opcode == CODE_SEGMENT_END) {
            // ����ν�������ӡ���з���������ȡ��һ�������
            inst.opcode = NOP;
            continue;
        }

        switch (inst.opcode) {
            case NOP:
                break;
            case OP_ADD:
            case OP_SUB:
            case OP_MUL:
            case OP_DIV:
                // ��ȡ��������
                fread(&inst.param1, 1, 1, file);
                if (fread(&inst.param1, 1, 1, file) != 1 ||
                    fread(&inst.param2, 1, 1, file) != 1) {
                    fprintf(stderr, "Error reading parameters for opcode 0x%02X\n", inst.opcode);
                    fclose(file);
                    return 1;
                }
                // ��������ֵ0xFF�������ݴ�����ֵ
                if (inst.param1 == SPECIAL_VALUE) inst.param1 = temp;
                if (inst.param2 == SPECIAL_VALUE) inst.param2 = temp;

                // ִ��ָ������ݴ���ֵ
                switch (inst.opcode) {
                    case OP_ADD: temp = inst.param1 + inst.param2; break;
                    case OP_SUB: temp = inst.param1 - inst.param2; break;
                    case OP_MUL: temp = inst.param1 * inst.param2; break;
                    case OP_DIV:
                        if (inst.param2 == 0) {
                            fprintf(stderr, "Error: division by zero\n");
                            fclose(file);
                            return 1;
                        }
                        temp = inst.param1 / inst.param2;
                        break;
                }
                break;
            case OP_PRT:
                // ��ӡ�ݴ���ֵ
                printf("%d\n", temp);
                break;
            case OP_IPT:
                // �������벢д���ݴ���
                if (scanf("%d", &temp) != 1) {
                    fprintf(stderr, "Error reading input for OP_IPT\n");
                    fclose(file);
                    return 1;
                }
                break;
            default: printf("Unknown opcode. run failed.\n"); return 1; break;
        }
        
    }
    printf("----------EXECUTE SUCCESS----------\n");
}

        
