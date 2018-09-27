#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void apresenta_matriz(float matriz[4][3000]){
	for(int j=0; j<3000; j++){
		for(int i=0; i<4; i++){
			printf(" %f |", matriz[i][j]);
		}
		printf("\n\n");
	}
}
int main(){
	FILE * pont;
	srand(time(0));
	int i, j, data;
	char sensor[256], nome[256];
	float matriz[4][3000], valor;
	printf("Realizado por: ");
	fgets(nome, 256, stdin);
	printf("\nTipo do sensor: ");
	fgets(sensor, 256, stdin);
	printf("Data (ddmmaa): ");
	scanf("%d", &data);
	for(i=0; i<4; i++){
		for(j=0; j<3000; j++){
			valor = (rand()%20001);
			matriz[i][j] = valor/10000;
		}
	}
	pont = fopen("Dados_Q1.dat", "w");
	fprintf(pont, "\nRealizado por: %s",nome );
	fprintf(pont, "\nTipo de Sensor: %s", sensor );
	fprintf(pont, "\nData (ddmmaa): %d\n", data);
	for(int j=0; j<3000; j++){
		for(int i=0; i<4; i++){
			fprintf(pont, " %f |", matriz[i][j]);
		}
		printf("\n\n");
	}
    fclose(pont);
	apresenta_matriz(matriz);
	return 0;
}