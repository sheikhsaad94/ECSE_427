//Name: Sheikh Saad Salah Uddin
//McGill ID # 260569832

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

int p; //number of processes
int r; //number of resource types
int *availRsrcs; //1D array of max no. of each resource type
int **maxClaim; //2D array of max resources each process can claim
int **holdArr; //2D array of how many processes each array is holding
int **needArr; //2D array of how many resources each process needs to finish

//function to display information on available resources
void printAvail(){

	int i;
	printf("Available resources: ");
	for(i=0; i<r; i++){
		printf("R%d:%d ", i, availRsrcs[i]);
	}
	printf("\n");
	
}

//request vector generator
int * reqGen(int procNo, int * needArray){ //takes process number and array of resources needed to terminate process
	int i = 0;
	int *req = (int *)malloc(p * sizeof(int)); //request vector
	printf("Request vector for process %d: ", procNo);
	for(i = 0; i<r; i++){
		req[i] = (rand()%(1+needArray[i]));
		printf("%d ", req[i]); //to see request vector
	}
	printf("\n");
	return req; 
}

//fault simulator
void *faultSim(void* fltPID){
	int probab = 0; //variable for fault probability. Can be either 0 or 1 
	int rSel;
	printf("Fault simulator thread created.\n");
	while(1){ //run idefinitely
		probab = rand()%2; //randomly select 0 or 1 (50% probability)
		if(probab == 0){
			rSel = rand()%(1+(r-1)); //randomly select a resource
			if(availRsrcs[rSel]>0){ //if there is a resource
				availRsrcs[rSel] = availRsrcs[rSel] - 1;
				printf("Fault introduced in resource %d. Value decreased from %d to %d.\n", rSel, availRsrcs[rSel]+1, availRsrcs[rSel]);
				printf("\n");
			}	
		}
		sleep(10); //sleep for 10 seconds
	}
}

//Deadlock Checker
void *deadlockChk(void* deadlkChkID){
	printf("Deadlock checker thread created.\n");
	int i = 0;
	int j = 0;
	int notEnough; //variable which increments everytime a resource is not enough for all of the processes
	int deadlock; //variable to indicate deadlock
	while(1){ //run idefinitely
		for(j=0; j<r; j++){
			for(i=0; i<p; i++){
				if(needArr[i][j]>availRsrcs[j]){
					notEnough++;
				}
			}
		}
		if(notEnough >= p){ //if needs for all processes are higher than availability of at least one particular resource 
			deadlock = 1;
			printf("Deadlock detected.\n");
			printAvail();
			notEnough = 0;
			exit(0); //exit when deadlock detected
		}else{
			deadlock = 0;
			notEnough = 0; //reset value of notEnough if no deadlock detected
		}
		sleep(10); //sleep for 10 seconds
	}
}


//Safe state checker
int isSafe(int procNo){
	int i;
	int j;
	int safe;
	int *work = (int *)malloc(p * sizeof(int));; //temporary 1D array of max no. of available resources
	
	//initialize to current available resources
	for(i=0; i<r; i++){
		work[i] = availRsrcs[i];
	}

	int *finish = (int *)malloc(p * sizeof(int)); //temporary 1D array to see if all processes can finish executing
	for(i=0; i<p; i++){
		finish[i] = 0; //finish is false for all processes to start with
	}
	
	//starts by checking safety of allocation to process being considered
	//if this is not done then checking always done from process 0, and resources
	//might not be allocated even if it is safe
	for(j=0; j<r; j++){ 
		if(needArr[procNo][j] <= work[j]){
			work[j] = work[j] + holdArr[procNo][j];
			finish[procNo] = 1;
		}else if(needArr[procNo][j] >= work[j]){
			finish[procNo] = 0;
			break;
		}
	}


	//for processes less than considered process number
	//find process such that finish[i] = false and need[i][j]<=work[j]
	for(i=0; i<procNo; i++){
		for(j = 0; j<r; j++){
			if(needArr[i][j] <= work[j]){
				work[j] = work[j] + holdArr[i][j];
				finish[i] = 1;
			}else if(needArr[i][j] >= work[j]){
				finish[i] = 0;
				break;
			}
		}
	}

	//for processes less than considered process number
	//find process such that finish[i] = false and need[i][j]<=work[j]
	for(i=procNo+1; i<p; i++){
		for(j = 0; j<r; j++){
			if(needArr[i][j] <= work[j]){
				work[j] = work[j] + holdArr[i][j];
				finish[i] = 1;
			}else if(needArr[i][j] >= work[j]){
				finish[i] = 0;
				break;
			}
		}
	}

	for(i=0; i<p; i++){
		if(finish[i] == 1){
			safe = 1;
		}else{
			safe = 0;
			i = p;
		}
	}
	return safe;

}

//Banker's algorithm
void bankersAlg(int procID, int *reqVec){
	int i;
	int safe;
	for(i=0; i<r; i++){
		if(reqVec[i]>needArr[procID][i]){
			printf("Requested amount of resource type %d is more than is needed.\n", i);
			break;
		}else if(reqVec[i]>availRsrcs[i]){
			printf("Requested amount of resource type %d is more than what is available.\n", i);
			break;
		}else{
			//provisional allocation
			availRsrcs[i] = availRsrcs[i] - reqVec[i];
			holdArr[procID][i] = holdArr[procID][i] + reqVec[i];
			needArr[procID][i] = needArr[procID][i] - reqVec[i];
			
			//IsSafe
			safe = isSafe(procID);
			if(safe != 1){
				//deallocate if not safe
				availRsrcs[i] = availRsrcs[i] + reqVec[i];
				holdArr[procID][i] = holdArr[procID][i] - reqVec[i];
				needArr[procID][i] = needArr[procID][i] + reqVec[i];
				printf("Unsafe to allocate resource %d to process %d. Cancelling.\n", i, procID);
			}else if(safe == 1){
				printf("System is safe. Allocating resource %d.\n", i);
			}
		}
	}
	printf("\n");
}

//process simulator
void *procSim(void* procID){
	int procNo = *(int *)procID; //process number
	printf("Process %d created.\n", procNo);
	int * reqRsrcs; //requested resources
	//Banker's algorithm
	int i;
	int a, b; //variables for checking whether process will terminate
	for(i=0; i<r; i++){
		while(needArr[procNo][i] != 0){
			reqRsrcs = reqGen(procNo, needArr[procNo]);
			bankersAlg(procNo, reqRsrcs);
			printAvail;
			
			//sleep for 3 seconds if process is not done
			for(a=0; a<r; a++){
				if(needArr[procNo][a] != 0){
					b = 1;
				}
			}
			if(b == 1){
				sleep(3);
			}
			b = 0;
		}
	}
	for(i=0; i<r; i++){ //release resources after finished executing
		availRsrcs[i] = availRsrcs[i] + holdArr[procNo][i];
		holdArr[procNo][i] = 0;
		needArr[procNo][i] = 0;
	}	
	printf("Process %d finished and resources released.\n", procNo);
	printf("\n");
}

int main(){
	time_t now;	
	srand((unsigned int)time(&now)); //seed

	int i, j;

	printf("Enter number of processes: "); //get number of processes from user
	scanf("%d", &p);

	printf("Enter number of resource types: "); //get number of resource types
	scanf("%d", &r);
	
	availRsrcs = (int *)malloc(p * sizeof(int));

	printf("Enter number of available resources for each type: \n"); //get max no. of each resource type
	for(i=0; i<r; i++){
		printf("Resource %d: ", i);
		scanf("%d", &availRsrcs[i]);
	}

	maxClaim = (int **)malloc(p * sizeof(int *));
	holdArr = (int **)malloc(p * sizeof(int *));
	needArr = (int **)malloc(p * sizeof(int *));
	for(i = 0; i<p; i++){
		maxClaim[i] = (int *)malloc(r * sizeof(int));
		holdArr[i] = (int *)malloc(r * sizeof(int));
		needArr[i] = (int *)malloc(r * sizeof(int));
	}
	
	printf("Enter maximum resource claims for processes: \n");
	for(i = 0; i<p; i++){
		printf("Process %d:\n", i);
		for(j=0; j<r; j++){
			printf("Resource type %d: ", j);
			scanf("%d", &maxClaim[i][j]); //get max claim of each resource type for each process	
		}
	}
	
	for(i=0; i<p; i++){
		for(j=0; j<r; j++){
			needArr[i][j] = maxClaim[i][j];
		}
	}

	printf("\n");

	//print max claim table
	printf("The maximum claim table is:\n");
	for(i=0; i<p; i++){
		printf("Process %d: ", i);
		for(j=0; j<r; j++){
			printf("R%d:%d ", j, maxClaim[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	//print initial allocation table
	printf("The initial allocation table is:\n");
	for(i=0; i<p; i++){
		printf("Process %d: ", i);
		for(j=0; j<r; j++){
			printf("R%d:%d ", j, holdArr[i][j]);
		}
		printf("\n");
	}

	printf("\n");

	//creating fault simulator thread
	pthread_t fltPIDs[1];
	if((pthread_create(&fltPIDs[1], NULL, faultSim, NULL)) != 0){
		printf("Fault simulator thread creation failed.\n");
	}

	//creating deadlock checking thread
	pthread_t deadlkChkID[1];
	if((pthread_create(&deadlkChkID[1], NULL, deadlockChk, NULL)) != 0){
		printf("Deadlock checker thread creation failed.\n");
	}


	//create threads simulating processes
	pthread_t procIDs[p];
	int *proc_ID[p];
	for(i = 0; i<p; i++){
		proc_ID[i] = malloc(sizeof(int));
		*proc_ID[i] = i;
		if((pthread_create(&procIDs[i], NULL, procSim, proc_ID[i])) != 0){
			printf("Process thread %d creation failed.\n", i);
		}
	}
	
	//join threads after done executing
	for(i = 0; i<p; i++){
		if(pthread_join(procIDs[i], NULL)){
			perror("Join error.\n");
		}
	}

	//join fault simulating thread
	if(pthread_join(fltPIDs[1], NULL)){
		perror("Join error.\n");
	}

	//join deadlock checking thread
	if(pthread_join(deadlkChkID[1], NULL)){
		perror("Join error.\n");
	}
	
	pthread_exit(NULL);	

	return 0;	
}

