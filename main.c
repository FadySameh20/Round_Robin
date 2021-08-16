#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    char*name;
    int starting_time;
    int remaining_time;
} Process;

typedef struct Node
{
    Process data;
    struct Node*next;
} Node;

typedef struct
{
    Node*head;
} Queue;

Queue* initialize()
{
    Queue *q = malloc(sizeof(Queue));
    if(q == NULL)
    {
        printf("Out of memory");
        exit(-1);
    }
    q->head=NULL;
    return q;
}

int isEmpty(Queue*q)
{
    if(q->head==NULL)
        return 1;
    else
        return 0;
}

Process dequeue(Queue*q)
{
    if(q->head)
    {
        Node*temp = q->head;
        Process p =  temp->data;
        q->head = q->head->next;
        free(temp);
        return p;
    }
    else
    {
        printf("Nothing to dequeue !!");
        exit(-1);
    }
}

void enqueue(Queue*q, Process val)
{
    Node*n = malloc(sizeof(Node));
    if(n == NULL)
    {
        printf("Out of memory");
        exit(-1);
    }
    n->data=val;
    n->next = NULL;
    if(q->head == NULL)  //If the queue is empty
        q->head = n;
    else
    {
        Node*temp = q->head;
        while(temp->next)  //To get the tail of the queue
            temp = temp->next;
        temp->next = n;
    }
}

void destroy(Queue*q)
{
    Node*temp;
    while(q->head)  //Destroy all nodes
    {
        temp = q->head;
        q->head = q->head->next;
        free(temp);
    }
    free(q);  //Destroy the queue
}

void RoundRobin(char*filename)
{
    FILE*fptr;
    fptr = fopen(filename,"r");
    if(!fptr)
    {
        printf("File doesn't exist\n");
        exit(-1);
    }
    Queue*q=initialize();
    int i,j,count=0,n=0,totalTime=0,timeQuantum=1;
    char c;
    while((c=fgetc(fptr))!=EOF)
    {
        if(c=='\n')
            n++;
    }
    int remain=n;
    Process p[n];
    Process idle;
    char str[10]="idle";
    idle.name=malloc(strlen(str)+1);
    if(idle.name == NULL)
    {
        printf("Out of memory");
        exit(-1);
    }
    strcpy(idle.name,str);
    idle.starting_time=idle.remaining_time=0;
    fseek(fptr,22,SEEK_SET);  //To get the watchingSlots
    fgets(str,10,fptr);
    char*token=strtok(str,"\n");
    int watchingSlots=atoi(token);
    for(i=0; i<n && !feof(fptr); i++)
    {
        fscanf(fptr,"%s %d %d",str,&p[i].starting_time,&p[i].remaining_time);  //To get each process with its details
        p[i].name = malloc(strlen(str)+1);
        if(p[i].name == NULL)
        {
            printf("Out of memory");
            exit(-1);
        }
        strcpy(p[i].name,str);
    }
    for(i=0; i<n-1; i++) //To sort the processes according to the their starting_time
    {
        for(j=i+1; j<n; j++)
        {
            if(p[i].starting_time>p[j].starting_time)
            {
                Process temp = p[i];
                p[i]=p[j];
                p[j]=temp;
            }
        }
    }
    while(p[0].starting_time > totalTime && count<watchingSlots)  //Enqueue idle until it,s turn for the starting_time of the first process
    {
        enqueue(q,idle);
        totalTime+=timeQuantum;
        count++;
    }
    i=0;
    while(remain!=0)
    {
        if(p[i].remaining_time<=timeQuantum && p[i].remaining_time>0)
        {
            enqueue(q,p[i]);
            totalTime+=p[i].remaining_time;
            p[i].remaining_time=0;
            count++;
            remain--;
        }
        else if(p[i].remaining_time>0)  //Process should enter only of the remaining_time>timeQuantum
        {
            //Can,t enter if the remaining_time=0
            enqueue(q,p[i]);
            totalTime+=timeQuantum;
            p[i].remaining_time -= timeQuantum;
            count++;
        }
        if(i!=n-1 && p[i+1].starting_time <= totalTime)  //Check whether the starting_time of the next process has the turn or not yet
            i++;                                         // (i!=n-1) in order not to check if it is the last process
        else
        {
            int x;
            for(x=0; x<i; x++)
            {
                if(p[x].remaining_time != 0)
                    i=x;   //Go back to the processes that started but not finished yet
            }
            if(p[i].remaining_time==0)  //If all processes ended or the starting_time of the next ones hasn't come yet, then it is idle
            {
                enqueue(q,idle);
                totalTime+=timeQuantum;
                count++;
            }
        }
    }
    while(count<watchingSlots) //If all processes finished, then the remaining will be idle
    {
        enqueue(q,idle);
        totalTime+=timeQuantum;
        count++;
    }
    printf("\n");
    Process process;
    i=0;
    count=0;
    while(!isEmpty(q) && count++<watchingSlots)
    {
        process=dequeue(q);
        if(process.remaining_time>timeQuantum)
        {
            printf("%s\t(%d-->%d)",process.name,i,i+timeQuantum);
            i+=timeQuantum;
        }
        else
        {
            if(!strcmp(process.name,"idle"))  //if it is idle
            {
                printf("%s\t(%d-->%d)",process.name,i,i+timeQuantum);
                i+=timeQuantum;
            }
            else  //if the remaining_time <= timeQuantum
            {
                printf("%s\t(%d-->%d) %s aborts",process.name,i,i+process.remaining_time,process.name);
                i+=process.remaining_time;
            }
        }
        printf("\n");
    }
    printf("stop\n\n");
    for(i=0;i<n;i++)
        free(p[i].name);
    free(idle.name);
    destroy(q);
    fclose(fptr);
}

int main()
{
    char filename[261];
    puts("Enter file name or Ctrl+Z to exit:");
    puts("----------------------------------");
    while(fgets(filename, 260, stdin) != NULL)
    {
        filename[strlen(filename)-1]='\0';
        if(fopen(filename,"r"))
            RoundRobin(filename);
        else
        {
            puts("File Not Found!");
            puts("----------------------------------");
        }
        puts("Enter file name or Ctrl+Z to exit:");
        puts("----------------------------------");
    }
    return 0;
}
