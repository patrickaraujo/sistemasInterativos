#include<stdio.h>
#include<conio.h>

int main(){
	int a[10], b[10], x[10];
	int waiting[10], turnaround[10], completion[10];
	int i, j, smallest, count = 0, time, n;
	double avg = 0, tt = 0, end;
	clrscr();
	printf("\nEntre com o numero de processos: ");
	scanf("%d",&n);
	for(i = 0; i < n; i++){
		printf("\Entre com o tempo de chegada dos processos %d: ", i + 1);
		scanf("%d",&a[i]);
	}
	for(i = 0; i < n; i++){
		printf("\nEntre com as duracoes dos processos %d: ", i + 1);
		scanf("d", &b[i]);
	}
	for(i = 0; i < n; i++)
		x[i]=b[i];
	b[9] = 9999;
	// printf("time => numero do processo");
	for(time = 0; count !=n ; time++){
		smallest = 0;
		for(i = 0; i < n; i++){
			if(a[i] <= time && b[i] < b[smallest] && b[i] > 0)
				smallest = i;
		}
			b[smallest]--;
			// printf("\n%d => p%d",time+1,smallest);
			if(b[smallest] == 0){
				count++;
				end = time+1;
				completion[smallest] = end;
				waiting[smallest] = end - a[smallest] - x[smallest];
				turnaround[smallest] = end - a[smallest];
				// printf("\n %d %d", smallest, wt[smallest], ttp[smallest]);
			}
	}
	printf("pID \t T. Duracao \t T. Chegada \t T. Espera \t T.Resposta \t T.Conclusao");
	for(i = 0; i < n; i++){
		printf("\n %d \t %d \t %d\t\t%d \t\t%d\t\t%d", i+1,x[i],a[i],waiting[i],turnaround[i],completion[i]);
		avg = avg + waiting[i];
		tt = tt + turnaround[i];
	}
	printf("\n %If %If", avg, tt);
	printf("\n\Tempo medio de espera = %lf\n", avg/n);
	printf("Tempo medio de resposta = %lf",tt/n);
	getch();
	return 0;
}
