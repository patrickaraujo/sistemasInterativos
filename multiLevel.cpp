#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>

int queue;
int queue1[10],queue2[10], queue3[10], n;
int q1 = 0, q2 = 0, q3 = 0 ,cnt = 0;
int t = 0;
//Process information
struct process
{
	int priority, burst_time, rem_burst_time;
	int wt;
};

//Check function for checking whether the all the process is finished or not
bool check(struct process *prc)
{
	int cd = 0, i;
	int avg_wait_time = 0, avg_turnaround_time = 0;
   for(i = 0; i< n ;i++)
   {
   	if(prc[i].rem_burst_time == 0)
   	cd++;
   }
   if(cd == n)
   {
	   //Printing the result
   		printf("\n\n*****************************************************\n");
   		printf("\t\t\tRESULT\n");
   		printf("*****************************************************\n");
   		printf("Process \tWaiting time \tTurnaround time\n");
   		for( i = 0 ;i <n; i++)
   		{
			printf("   %d\t\t    %d\t\t    %d\n", i+1, prc[i].wt, prc[i].wt + prc[i].burst_time);
			avg_wait_time += prc[i].wt;
			avg_turnaround_time += prc[i].wt + prc[i].burst_time;
		}
		printf("\n\nAverage waiting time is = %f\n", avg_wait_time/ (float)n );
		printf("Average turnaround time is = %f\n", avg_turnaround_time/(float)n);
		printf("\n~~~~~~~~~~~~~~~~~~~~~THE END~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
   		exit(0);
   }
   else
   return false;
}
//Sorting queue 2 based on priority of process
void sort(struct process *prc)
{
	int i,j,temp;
	if(q2 != 1)
	{
		for(i = 0; i< q2-1; i++)
		{
			for( j = 0 ;j <q2 - i - 1; j++)
			{
				if(prc[queue2[j]].priority > prc[queue2[j+1]].priority )
				{
					temp = queue2[j];
					queue2[j] = queue2[j+1];
					queue2[j+1] = temp;
				}
			}
		}
	}
}
//Applying Round Robin in  every process in queue until completion of 10s in this queue
void RR(struct process *prc )
{
int flag = 0, q = 0;
while (1)
    {
 		int i= 0;
        for (i = 0 ; i < q1; ++i)
        {
            if (prc[queue1[i]].rem_burst_time > 0)
            {
                if (prc[queue1[i]].rem_burst_time >= 4)
                {
        			if(q + 4 > 10 )
        			{
        				prc[queue1[i]].rem_burst_time -= 10 - q;
        				t = t + (10 - q);
						PS(prc);
						q = 0;
						i = 0;
						break;
				}
                    			t += 4;
 					q += 4;
                    			prc[queue1[i]].rem_burst_time -= 4;
                }
                else
                {
                	t = t + prc[queue1[i]].rem_burst_time;
 		   	q = q + prc[queue1[i]].rem_burst_time;
                    	prc[queue1[i]].wt = t - prc[queue1[i]].burst_time;
                    	prc[queue1[i]].rem_burst_time = 0;
                }
            }
             	if(q == 10 || prc[queue1[0]].rem_burst_time == 0 && prc[queue1[1]].rem_burst_time == 0)
                {
                	q = 0 ;
                	i = 0;
                	if(!check(prc))
                	PS(prc);
                	break;
		}
	     }
	}
}

//Applying priority scheduling in every process until completion of 10s in this queue
void PS(struct process *prc )
{
	int q = 0 ,i;
	sort(prc);
	while(1)
	{
		for( i = 0; i < q2 ; ++i)
		{
			if(prc[queue2[i]].rem_burst_time > 0)
			{
				if(prc[queue2[i]].rem_burst_time < 10)
				{
					if(q + prc[queue2[i]].rem_burst_time > 10)
					{
						prc[queue2[i]].rem_burst_time  -= 10 - q;
						t = t + (10 - q);
						q = 10;
						if(!check(prc))
						FCFS(prc);
						q = 0;
						i = 0;
						break;
					}
					q = q + prc[queue2[i]].rem_burst_time ;
					t = t + prc[queue2[i]].rem_burst_time ;
					prc[queue2[i]].wt = t - prc[queue2[i]].burst_time ;
					prc[queue2[i]].rem_burst_time  = 0;
				}
				else
				{
					q = q + 10;
					t = t + 10;
					prc[queue2[i]].rem_burst_time -= 10;
					if(!check(prc))
					FCFS(prc);
					q = 0;
					i = 0;
					break;
				}
			}
			if(q == 10 || prc[queue2[i]].rem_burst_time == 0 )
			{
				if(!check(prc))
				FCFS(prc);
				q = 0;
				i = 0;
				break;
			}
		}

	}
}
//Applying priority scheduling in every process until completion of 10s in this queue
void FCFS(struct process *prc )
{
	int q = 0, i;
	while(1)
	{
		for(i = 0 ;i< q3; ++i)
		{
			if(prc[queue3[i]].rem_burst_time > 10 )
			{
				q += 10;
				t = t + 10;
				prc[queue3[i]].rem_burst_time -= 10;
				if(!check(prc))
				RR(prc);
				q = 0;
				i = 0;
				break;
			}
			else
			{
				if(q + prc[queue3[i]].rem_burst_time > 10)
					{
						prc[queue3[i]].rem_burst_time  -= 10 - q;
						t = t + (10 - q);
						q = 10;
						if(!check(prc))
						FCFS(prc);
						q = 0;
						i = 0;
						break;
					}
				q = q + prc[queue3[i]].rem_burst_time;
				t += prc[queue3[i]].rem_burst_time;
				prc[queue3[i]].wt = t - prc[queue3[i]].burst_time;
				prc[queue3[i]].rem_burst_time = 0;
			}
				if(q == 10 || prc[queue3[0]].rem_burst_time == 0 )
				{
					q = 0 ;
					i = 0;
					if(!check(prc))
					RR(prc);
					break;
				}
		}
	}
}
int main()
{
	int i, j,sum = 0, twt = 0, f ;
	int prq[3][2];
	printf("Enter range of priority of queues:\n");
	for(i = 0; i< 3; i++)
	{
		printf("Queue %d:",i+1);
		for(j = 0; j<2; j++ )
		scanf("%d", &prq[i][j]);
	}
	printf("\nEnter total number of process:");
	scanf("%d", &n);
	struct process p[n];
	printf("Enter process's PRIORITY and BURST TIME (space separated):\n");
	for(i = 0; i < n; i++ )
	{
		printf("Process %d:" ,i + 1);
		scanf("%d%d", &p[i].priority, &p[i].burst_time);
		p[i].rem_burst_time = p[i].burst_time;
	}
	printf("\n\nYour entred data is\n");
	printf("Process \tPriority \tBurst time\n");
	for(i = 0; i < n; i++ )
	{
		printf("   %d\t\t    %d\t\t    %d\n", i+1, p[i].priority,p[i].burst_time);
	}
	int  m = 0,h;
	h = n;

	//Putting all the process in the queues based on there priority
			while(h--)
			{
				m = m %n;
				if(p[m].priority >= prq[0][0] &&  p[m].priority <= prq[0][1])
				{
					queue1[q1] = m;
					++q1;
					++cnt;
				}
				if(p[m].priority >= prq[1][0] &&  p[m].priority <= prq[1][1] )
				{
					queue2[q2] = m;
					++q2;
					++cnt;
				}
				if(p[m].priority >= prq[2][0] &&  p[m].priority <= prq[2][1] )
				{
					queue3[q3] = m;
					++q3;
					++cnt;
				}
				++m;
				if(cnt == n )
				break;
			}
			RR(p);			//calling Roind robin for first time
	return 0;
}
