#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include <ctime>


#pragma warning(disable:4996)

#define SET_SIZE 1000000

//#define STRINGIFY_INT(int) #(int)

void printArray(char* arr[], int len) {
	for (int i = 0; i < len; i++)
		printf("%s\n", arr[i]);
}

//
char* B[SET_SIZE + 1] = {};

void CountingSort(char* arr[], int sort_idx) {
	int C[27] = {};//1 Extra for 'NULL' top priority.

	int optimal_str_len = sort_idx + 1;//just the right one.
	
	for (int i = SET_SIZE - 1; i >= 0; i--) {
		if (strlen(arr[i]) < optimal_str_len) {
			C[0]++;
		}
		else {
			C[arr[i][sort_idx] - 'a' + 1]++;
		}
	}
	//done counting total.

	for (int i = 1; i < 27; i++)
		C[i] = C[i - 1] + C[i];//update.

	for (int i = SET_SIZE - 1; i >= 0; i--) {
		if (strlen(arr[i]) < optimal_str_len) {
			B[C[0]] = arr[i];
			C[0]--;
		}
		else {
			B[C[arr[i][sort_idx] - 'a' + 1]] = arr[i];
			C[arr[i][sort_idx] - 'a' + 1]--;
		}
	}
	//done organizing 'em!

	for (int i = 1; i <= SET_SIZE; i++)
		arr[i - 1] = B[i];//give it back to A.
}

void radixSort(char* arr[], int len) {
	for (int i = len - 1; i >= 0; i--) {
		CountingSort(arr, i);
	}
}


char* dataSet[SET_SIZE];

int main()
{

	FILE* input = fopen("1000000.txt", "rt");

	for (int i = 0; i<SET_SIZE; i++) {
		char* str = (char*)malloc(16);//15 charac. + NULL.
		fscanf(input, "%s", str);
		//fgets(str, WORD_SIZE, input);
		dataSet[i] = str;
	}

	fclose(input);

	//printArray(dataSet, SET_SIZE);

	puts("<<");

	clock_t begin = clock();
	radixSort(dataSet, 15);
	clock_t end = clock();

	puts("<<");

	double elapsed_sec = (double)(end - begin) / CLOCKS_PER_SEC;
	printf("%f SECONDS elapsed.\n", elapsed_sec);

	//printArray(dataSet, SET_SIZE);

	FILE* output = fopen("1000000_cpp_Sorted.txt", "wt");

	for (int i = 0; i<SET_SIZE; i++) {
		//char* str = (char*)malloc(16);//15 charac. + NULL.
		//fscanf(input, "%s", str);
		//fgets(str, WORD_SIZE, input);
		fprintf(output, "%s\n", dataSet[i]);
	}

	fclose(output);
	//END!
	return 0;
}