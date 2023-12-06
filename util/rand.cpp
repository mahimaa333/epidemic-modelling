#include<iostream>
#include<time.h>
#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#define POPULATION 10000  
#define NUMBERC 4     
#define TIME 1000     
#define INFECTEDP 0.1   
#define INFECTED 5   
#define RECOVEREDP 0.01   

void init_infect(char *arr, int tot_pop, int init_inf){
    int i, j;
    for(i=0; i<tot_pop; i++){
        arr[i] = 's';
    }
    for(i=0; i<init_inf; i++){
        j = rand()%tot_pop;
        while(arr[j] != 's'){
            j = rand()%tot_pop;
        }
        arr[j] = 'i';
    }
    return;
}

void pop_count(char *arr, int size, int &sus, int &inf, int &rec){
    sus = 0;
    inf = 0;
    rec = 0;
    for(int i=0; i<size; i++){
        if(arr[i] == 's')
            sus++;
        else if(arr[i] == 'i')
            inf++;
        else if(arr[i] == 'r')
            rec++;
    }
    return;
}

void run_update(char *arr, int size, int cont, float inf_prob, float rec_prob){
    int count[size] = {0};   
    int i, j;
    float p;    
    char curr[size]={'x'};  
    for(i=0; i<size; i++)
        curr[i] = arr[i];
    for(i=0; i<size; i++){
        
        if(arr[i] == 'i'){
            p = (float)rand()/RAND_MAX;
            if(p < rec_prob)
                arr[i] = 'r';
        }
        if(count[i] < cont){            
            
            j = rand()%size;
            while(count[j] >= cont || j == i)
                j = rand()%size;
            count[i]++;
            count[j]++;
            if(curr[i] != 'r'){
                p = (float)rand()/RAND_MAX;
                if(curr[i] == 'i' && curr[j] != 'r' && p < inf_prob)
                    arr[j] = 'i';
                else if(curr[i] == 's' && curr[j] == 'i' && p < inf_prob)
                    arr[i] = 'i';
            }            
        }
    }
    return;
}     

int main(){            

    srand(time(NULL));
    char state[POPULATION]={'x'};
    int s, i, r;
    int ind;          
    init_infect(state, POPULATION, INFECTED);        
    for(ind=0; ind<TIME; ind++){
        run_update(state, POPULATION, NUMBERC, INFECTEDP, RECOVEREDP);
        pop_count(state, POPULATION, s, i, r);        
        printf("%d\t%d\t%d\n", s, i, r);
    }
    return 0;
}
