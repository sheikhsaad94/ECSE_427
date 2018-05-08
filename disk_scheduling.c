//Name: Sheikh Saad Salah Uddin
//McGill ID # 260569832

#include <stdio.h>  //for printf and scanf
#include <stdlib.h> //for malloc

#define LOW 0
#define HIGH 199
#define START 53

//compare function for qsort
//you might have to sort the request array
//use the qsort function 
//an argument to qsort function is a function that compares 2 quantities
//use this there.
int cmpfunc (const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

//function to swap 2 integers
void swap(int *a, int *b)
{
    if (*a != *b)
    {
        *a = (*a ^ *b);
        *b = (*a ^ *b);
        *a = (*a ^ *b);
        return;
    }
}

//Prints the sequence and the performance metric
void printSeqNPerformance(int *request, int numRequest)
{
    int i, last, acc = 0;
    last = START;
    printf("\n");
    printf("%d", START);
    for (i = 0; i < numRequest; i++)
    {
        printf(" -> %d", request[i]);
        acc += abs(last - request[i]);
        last = request[i];
    }
    printf("\nPerformance : %d\n", acc);
    return;
}

//access the disk location in FCFS
void accessFCFS(int *request, int numRequest)
{
    //simplest part of assignment
    printf("\n----------------\n");
    printf("FCFS :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SSTF
void accessSSTF(int *request, int numRequest)
{
    //SSTF logic
    int *diff = malloc(numRequest * sizeof(int)); //array to store the differences between initial head position and the requests
    int i = 0;
    int j = 0; //i and j are variables for comparing the ith and jth position of the diff array
    int a = 0; //a is used to update the diff array when the head position changes
    int currHeadPos = 0; //variable for current head position

    //calculate difference between each request and initial head position
    for (i = 0; i < numRequest; i++){
	diff[i] = abs(START - request[i]);
    }

    //rearrange request array according to SSTF
    for(i = 0; i < numRequest; i++){
        for(j = i+1; j < numRequest; j++){
	    if(diff[i] > diff[j]){ //if current difference is greater than next difference
                swap(&diff[i], &diff[j]); //swap current difference with next difference
		swap(&request[i], &request[j]); //swap current request with next request
	    }        
        }
        currHeadPos = request[i]; //set current head position
        for(a = i+1; a<numRequest; a++){ //update diff array with differences from current head position to find current shortest service time
            diff[a] = abs(currHeadPos-request[a]);
        }
    }

    printf("\n----------------\n");
    printf("SSTF :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in SCAN
void accessSCAN(int *request, int numRequest)
{
    //SCAN logic
    int newCnt = numRequest + 1; //new request array to include last track in head travel direction
    int* newRequest = realloc(request, newCnt * sizeof(int)); //reallocate request array size to include last track
    int i = 0;
    int j = 0; //i and j are variables for comparing the ith and jth position of the newRequest array
    if(START <= 99){ //if initial head position closer to LOW
        newRequest[numRequest] = LOW; //include LOW in newRequest array as last track
        qsort(newRequest, newCnt, sizeof(int), cmpfunc); //sort array from smallest to largest
	for(i = numRequest; i > 0; i--){ //move array towards LOW
	    for(j = i-1; j >= 0; j--){
                if(newRequest[i] < START){ //if head position less than ith request 
                    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request so that the array is rearranged in decreasing order towards 0 till head starts moving opposite
		}
	    }
	}
    }else{ //if head initially closer to HIGH
        newRequest[numRequest] = HIGH; //include HIGH in newRequest array as last track
        qsort(newRequest, newCnt, sizeof(int), cmpfunc); //sort array from smallest to largest
	for(i = 0; i < numRequest; i++){ //move towards HIGH
	    for(j = i+1; j <= numRequest; j++){
	        if(newRequest[i] < START){ //if head position less than ith request 
                    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request so that the array is rearranged in increasing order towards HIGH till head starts moving opposite
		}
            }
	}
    }

    printf("\n----------------\n");
    printf("SCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in CSCAN
void accessCSCAN(int *request, int numRequest)
{
    //CSCAN logic
    int newCnt; //new request array to include last and first track
    int* newRequest; //new request array if the original needs to be reallocated
    int i = 0;
    int j = 0; //i and j are variables for comparing the ith and jth position of the newRequest array
    if(START <= 99){ //if head initially closer to LOW
	qsort(request, numRequest, sizeof(int), cmpfunc); //sort request array
	if(request[numRequest-1]>START){ //check if head needs to move in opposite direction
	    newCnt = numRequest + 2; //new request array to include last and first track
	    newRequest = realloc(request, newCnt * sizeof(int)); //reallocate request array size to include last and first tracks
    	    newRequest[numRequest] = LOW; //first track
    	    newRequest[numRequest+1] = HIGH; //last track
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}else{ //if head does not need to change direction
	    newCnt = numRequest; 
	    newRequest = realloc(request, newCnt * sizeof(int)); 
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}

        for(i = newCnt-1; i > 0; i--){ //move towards LOW
	    for(j = i-1; j >= 0; j--){
	        if(newRequest[i] < START){ //if ith request less than initial head position
	            swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request so that the array is rearranged in decreasing order towards LOW
		}
	    }
	}
        for(i = 0; i < newCnt-1; i++){ //once LOW is reached
	    for(j = i+1; j <= newCnt-1; j++){
	        if(newRequest[i] > START){ 
		    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request so that remainder of array is rearranged in decreasing order from HIGH
		}
            }
        }
    }else{ //if head initially closer to HIGH

	qsort(request, numRequest, sizeof(int), cmpfunc); //sort request array
	if(request[0]<START){ //check if head needs to move in opposite direction
	    newCnt = numRequest + 2; //new request array to include last and first track
	    newRequest = realloc(request, newCnt * sizeof(int)); //reallocate request array size to include last and first tracks
    	    newRequest[numRequest] = LOW; //first track
    	    newRequest[numRequest+1] = HIGH; //last track
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}else{ //if head does not need to change direction
	    newCnt = numRequest; 
	    newRequest = realloc(request, newCnt * sizeof(int)); //reallocate
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}

        for(i = 0; i < newCnt-1; i++){ //move towards HIGH
	    for(j = i+1; j <= newCnt-1; j++){
	        if(newRequest[i] > START){ //if ith request greater than initial condition
                    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request so that the array is rearranged in increasing order towards HIGH
		}
	    }
	}
        for(i = newCnt-1; i > 0; i--){ //once HIGH is reached
	    for(j = i-1; j >=0; j--){
	        if(newRequest[i] > START){ 
		    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request so that remainder of array is rearranged in increasing order from LOW
		}
            }
        }
    }


    printf("\n----------------\n");
    printf("CSCAN :");
    printSeqNPerformance(newRequest, newCnt);
    printf("----------------\n");
    return;
}

//access the disk location in LOOK
void accessLOOK(int *request, int numRequest)
{
    //LOOK logic
    qsort(request, numRequest, sizeof(int), cmpfunc); //arrange request array from smallest to largest
    int i = 0;
    int j = 0; //i and j are variables for comparing the ith and jth position of the request array
    if(START <= 99){ //if initial head position closer to LOW
        for(i = numRequest-1; i > 0; i--){ //move towards LOW
	    for(j = i-1; j >= 0; j--){
	        if(request[i] < START){ //if ith request less than initial head position 
                    swap(&request[i], &request[j]); //swap ith request with jth request so that array is rearranged in decreasing order towards 0 till no more requests in this direction and the remainder of the requests in opposite direction are read in increasing order
		}
	    }
	}
    }else{ //if initial head position closer to HIGH
        for(i = 0; i < numRequest-1; i++){ //move towards HIGH
	    for(j = i+1; j <= numRequest-1; j++){
	        if(request[i] < START){ //if ith request less than initial head position
                    swap(&request[i], &request[j]); //swap ith request with jth request so that array is rearranged in increasing order towards HIGH till no more requests in this direction and the remainder of the requests in opposite direction and read in decreasing order
		}
            }
	}
    }

    printf("\n----------------\n");
    printf("LOOK :");
    printSeqNPerformance(request, numRequest);
    printf("----------------\n");
    return;
}

//access the disk location in CLOOK
void accessCLOOK(int *request, int numRequest)
{
    //CLOOK logic
    int newCnt; //new request array to include last or first track
    int* newRequest; //reallocate request array size to include last or first track
    int i = 0;
    int j = 0; //i and j are variables for comparing the ith and jth position of the newRequest array
    if(START <= 99){ //if head initialy closer to LOW
	qsort(request, numRequest, sizeof(int), cmpfunc); //sort request array
	if(request[numRequest-1]>START){ //check if head needs to move in opposite direction
	    newCnt = numRequest + 1; //new request array to include last and first track
	    newRequest = realloc(request, newCnt * sizeof(int)); //reallocate request array size to include last track
    	    newRequest[numRequest] = HIGH; //include HIGH
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}else{ //if head does not need to change direction
	    newCnt = numRequest; 
	    newRequest = realloc(request, newCnt * sizeof(int)); 
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}

	for(i = newCnt-1; i > 0; i--){ //move towards lowest request
            for(j = i-1; j >= 0; j--){
	        if(newRequest[i] < START){ //if ith request less than initial head position
		    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request to rearrange array in decreasing order towards the lowest request
		}
	    }
	}
        for(i = 0; i < newCnt-1; i++){ //once lowest request reached
            for(j = i+1; j <= newCnt-1; j++){ //move to HIGH
		if(newRequest[i] > START){ //if ith request greater than initial head position
		    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request to rearrange array in decreasing order towards the remaining requests, i.e. keep moving in same direction
		}
            }
        }
    }else{ //if head initialy closer to HIGH

	qsort(request, numRequest, sizeof(int), cmpfunc); //sort request array
	if(request[0]<START){ //check if head needs to move in opposite direction
	    newCnt = numRequest + 1; //new request array to include last and first track
	    newRequest = realloc(request, newCnt * sizeof(int)); //reallocate request array size to include first track
    	    newRequest[numRequest] = LOW; //include LOW
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}else{ //if head does not need to change direction
	    newCnt = numRequest; 
	    newRequest = realloc(request, newCnt * sizeof(int)); 
	    qsort(newRequest, newCnt, sizeof(int), cmpfunc);
	}

	for(i = 0; i < newCnt-1; i++){ //move towards highest request
            for(j = i+1; j <= newCnt-1; j++){
	        if(newRequest[i] > START){ //if ith request greater than initial head position
                    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request to rearrange array in increasing order towards the highest request
		}
	    }
	}
        for(i = newCnt-1; i > 0; i--){ //once highest request reached
	    for(j = i-1; j >=0; j--){ //move to LOW
	        if(newRequest[i] > START){ //if ith request greater than initial head position
		    swap(&newRequest[i], &newRequest[j]); //swap ith request with jth request to rearrange array in increasing order towards the remaining requests, i.e. keep moving in same direction
		}
            }
        }
    }

    printf("\n----------------\n");
    printf("CLOOK :");
    printSeqNPerformance(newRequest,newCnt);
    printf("----------------\n");
    return;
}

int main()
{
    int *request, numRequest, i,ans;

    //allocate memory to store requests
    printf("Enter the number of disk access requests : ");
    scanf("%d", &numRequest);
    request = malloc(numRequest * sizeof(int));

    printf("Enter the requests ranging between %d and %d\n", LOW, HIGH);
    for (i = 0; i < numRequest; i++)
    {
        scanf("%d", &request[i]);
    }

    printf("\nSelect the policy : \n");
    printf("----------------\n");
    printf("1\t FCFS\n");
    printf("2\t SSTF\n");
    printf("3\t SCAN\n");
    printf("4\t CSCAN\n");
    printf("5\t LOOK\n");
    printf("6\t CLOOK\n");
    printf("----------------\n");
    scanf("%d",&ans);

    switch (ans)
    {
    //access the disk location in FCFS
    case 1: accessFCFS(request, numRequest);
        break;

    //access the disk location in SSTF
    case 2: accessSSTF(request, numRequest);
        break;

    //access the disk location in SCAN
    case 3: accessSCAN(request, numRequest);
        break;

    //access the disk location in CSCAN
    case 4: accessCSCAN(request,numRequest);
        break;

    //access the disk location in LOOK
    case 5: accessLOOK(request,numRequest);
        break;

    //access the disk location in CLOOK
    case 6: accessCLOOK(request,numRequest);
        break;

    default:
        break;
    }
    return 0;
}
