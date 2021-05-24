#include <stdio.h>
#include <stdlib.h>

#define MEMORY_LEN 512

#define BINARY_FMT "%c%c%c%c%c%c%c%c"
#define BIN(x) \
	(x & 0b10000000 ? '1':'0'), \
	(x & 0b01000000 ? '1':'0'), \
	(x & 0b00100000 ? '1':'0'), \
	(x & 0b00010000 ? '1':'0'), \
	(x & 0b00001000 ? '1':'0'), \
	(x & 0b00000100 ? '1':'0'), \
	(x & 0b00000010 ? '1':'0'), \
	(x & 0b00000001 ? '1':'0')
	
unsigned char Memory[MEMORY_LEN];
FILE *f = NULL;
char c = 0;
char ShouldExit = 0;
int AccessPtr = 0;
char Bit = 1;
int LoopStart = 0;
int LoopEnd = 0;
int CopyValue = 0;

int ParseChar(void)
{
	switch (c)
	{
		case '#':
			c = fgetc(f);
			while (c != '\n' && c != EOF)
			{
				c = fgetc(f);
			}
			break;
		case 'C':
			CopyValue = Memory[AccessPtr];
			break;
		case 'P':
			Memory[AccessPtr] = CopyValue;
			break;
		case ']':
			AccessPtr = (AccessPtr + 1) % MEMORY_LEN;
			break;
		case '[':
			AccessPtr--;
			if (AccessPtr < 0) AccessPtr = MEMORY_LEN - 1; 
			break;
		case '<':
			Bit <<= 1;
			Bit = Bit == 0 ? 1 : Bit;
			break;
		case '>':
			Bit >>= 1;
			Bit = Bit == 0 ? 1 << 7 : Bit;
			break;
		case '^':
			Memory[AccessPtr] = Memory[AccessPtr] ^ Bit;
			break;
		case '&':
			Memory[AccessPtr] = 
				Memory[AccessPtr] | (Memory[AccessPtr] & Bit);
			break;
		case '|':
			Memory[AccessPtr] = Memory[AccessPtr] | Bit;
			break;
		case '~':
			Memory[AccessPtr] = ~(Memory[AccessPtr]);
			break;
		case '+':
			AccessPtr += 2;
			Memory[AccessPtr] = Memory[AccessPtr-2] + Memory[AccessPtr-1];
			break;
		case '-':
			AccessPtr += 2;
			Memory[AccessPtr] = Memory[AccessPtr-2] - Memory[AccessPtr-1];
			break;
		case '*':
			AccessPtr += 2;
			Memory[AccessPtr] = Memory[AccessPtr-2] * Memory[AccessPtr-1];
			break;
		case '/':
			AccessPtr += 2;
			Memory[AccessPtr] = Memory[AccessPtr-2] / Memory[AccessPtr-1];
			break;
		case '%':
			AccessPtr += 2;
			Memory[AccessPtr] = Memory[AccessPtr-2] % Memory[AccessPtr-1];
			break;
		case 'e':
			AccessPtr += 2;
			Memory[AccessPtr] = 
				(Memory[AccessPtr-2] == Memory[AccessPtr-1]) ? 1 : 0;
			break;
		case 'g':
			AccessPtr += 2;
			Memory[AccessPtr] = 
				(Memory[AccessPtr-2] > Memory[AccessPtr-1]) ? 1 : 0;
			break;
		case 'G':
			AccessPtr += 2;
			Memory[AccessPtr] = 
				(Memory[AccessPtr-2] >= Memory[AccessPtr-1]) ? 1 : 0;
			break;
		case 'l':
			AccessPtr += 2;
			Memory[AccessPtr] = 
				(Memory[AccessPtr-2] < Memory[AccessPtr-1]) ? 1 : 0;
			break;
		case 'L':
			AccessPtr += 2;
			Memory[AccessPtr] = 
				(Memory[AccessPtr-2] <= Memory[AccessPtr-1]) ? 1 : 0;
			break;
		case 'n':
			printf("%i", (int)((char)Memory[AccessPtr]));
			break;
		case 'p':
			printf("%c", Memory[AccessPtr]);
			break;
		case 'd':
			printf("\n%i|" BINARY_FMT "|"  BINARY_FMT "|%i|%c\n",
					AccessPtr,
					BIN(Bit),
					BIN((char)(Memory[AccessPtr])),
					(int)(Memory[AccessPtr]),
					Memory[AccessPtr]);
			break;
		case 'i':
			Memory[AccessPtr] = fgetc(stdin);
			break;
		case '{':
			if (Memory[AccessPtr] != 0)
			{
				LoopStart = ftell(f)-1;
				Memory[AccessPtr] = Memory[AccessPtr] -1;
			}
			else
			{
				fseek(f, LoopEnd, SEEK_SET);
			}
			break;
		case '}':
			LoopEnd = ftell(f);
			fseek(f, LoopStart, SEEK_SET);
			break;
		case 'r':
			return 1;
		default:
		case '\r':
		case '\n':
		case '\t':
		case ' ':
			break;
	}
	return 0;
}

int main(int argc, char **argv)
{


	if (argc == 1) { return 1; }
	f = fopen(argv[1], "r");
	if (f ==  NULL) { return 1; }

	for (int i = 0; i < MEMORY_LEN; i++)
	{
		Memory[i] = 0;
	}

	while ((c = fgetc(f)) != EOF && !ShouldExit)
	{
		if (ParseChar())
			break;
	}

	return (int)((char)Memory[AccessPtr]);
}
