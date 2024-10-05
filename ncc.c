#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "ncc_lib.h"
typedef struct{
	unsigned char opcode;
	int arc;
} Code;
int getStringArrayLength(char** array) {
    int length = 0;
    while (array[length] != NULL) {
        length++;
    }
    return length;
}

char** splitStringBySpaces(const char* str) {
    // ��ʼ������Ϊ10���ɸ�����Ҫ����
    int capacity = 10;
    char** result = malloc(capacity * sizeof(char*));
    int i = 0;

    // ʹ��strtok���������ַ����ָ�
    char* token = strtok((char*)str, " ");
    while (token != NULL) {
        // �����ǰ�������㣬��ӱ�����
        if (i >= capacity) {
            capacity *= 2;
            result = realloc(result, capacity * sizeof(char*));
        }

        // ��token���Ƶ��·�����ڴ��У�����ָ�����������
        result[i] = strdup(token);

        // ������ȡ��һ��token
        token = strtok(NULL, " ");
        i++;
    }

    // ����ʵ��ʹ�õ��������µ��������С
    result = realloc(result, (i + 1) * sizeof(char*));
    result[i] = NULL; // ������ĩβ���NULLָ�룬��ʾ����

    return result;
}
size_t count_byte(const char* str) {
    size_t bytes = 0;
    while (*str++) {
        bytes++;
    }
    return bytes;
}
Code* compiletobyte(char* token) {
	
	Code* rec = malloc(sizeof(Code));
	if (rec == NULL) {
	    // �����ڴ����ʧ�ܵ����
	    exit(EXIT_FAILURE);
	}
    if (strcmp(token, "<IO>") == 0) {
		rec->opcode = OP_IO;
		rec->arc = 0;
	}
	if (strstr(token, "//") != NULL) {
		rec->opcode = NOP;
		rec->arc = 0;
	}
	if (strstr(token, "ADD") != NULL) {
		rec->opcode = OP_ADD;
		rec->arc = 2;
	}
	if (strstr(token, "SUB") != NULL) {
		rec->opcode = OP_SUB;
		rec->arc = 2;
	}
	if (strstr(token, "MUL") != NULL) {
		rec->opcode = OP_MUL;
		rec->arc = 2;
	}
	if (strstr(token, "DIV") != NULL) {
		rec->opcode = OP_DIV;
		rec->arc = 2;
	}
	if (strstr(token, "PRT") != NULL) {
		rec->opcode = OP_PRT;
		rec->arc = 0;
	}
	if (strstr(token, "IPT") != NULL) {
		rec->opcode = OP_IPT;
		rec->arc = 0;
	}
	if (strstr(token, "PUSH") != NULL) {
		rec->opcode = OP_PUSH;
		rec->arc = 1;
	}
	return rec;
}

int main(int argc, char** argv){
	bool check = false;
	bool flash = false;
	int flash_on = 0;
	for(int i=1;i<argc;i++){
		if (strcmp(argv[i], "-v") == 0) {
			check = true;
		}
		if (strcmp(argv[i], "-s") == 0) {
			flash = true;
			flash_on = i + 1;
		}
	}
	if (check) {
		printf("%s -> %s \ncompile to byte\n", argv[1], argv[2]);
	}
	FILE* fin = fopen(argv[1], "r");
	FILE* fout = fopen(argv[2], "wb");
	if (fin == NULL || fout == NULL) {
		perror("Error opening file");
		return EXIT_FAILURE;
	}
	char line[1024];
	if (!flash) {
		for(int c = 0;c<512;c++){
			fputc(0x00, fout);
		}
	} else {
		char* mes = argv[flash_on];
		fputs(mes, fout);
		for(int c = 0;c<512 - count_byte(mes);c++){
			fputc(0x00, fout);
		}
	}
	
	while(fgets(line, sizeof(line), fin) != NULL) {
		Code* temp;
		temp = compiletobyte(line);
		char** arg = splitStringBySpaces(line);
		int argcount = getStringArrayLength(arg);
		if (argcount - 1 != temp->arc) {
			printf("On compile error: arguments error at %s.\nIn File %s\n", line, argv[1]);
			printf("because argcount must be %d, but your argcount is %d\n", temp->arc, argcount - 1);
			return COMPILE_ERROR;
		}
		
		fputc(temp->opcode, fout);
		if(check) {
			printf("FOUND OPCODE:%x\n", temp->opcode);
		}
		fputc(temp->arc, fout);
		if (temp->arc > 0) {
			for (int count = 1; count < argcount; count++) {
				unsigned char qian = atoi(arg[count]);
				if(strcmp(arg[count], "STACK") == 0){
					qian = 0xFF;
				}
				fputc(qian, fout);
			}
		}
		fputc(section, fout);
		for(int i=0;i<argcount;i++){
			free(arg[i]);
		}
		free(temp);
	}
	return EXIT_NORMAL;
}
