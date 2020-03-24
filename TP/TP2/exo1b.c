
#include <signal.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <string.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/types.h> 
#include <unistd.h> 
  
#define FILLED 0 
#define Ready 1 
#define NotReady -1 
  
struct memory { 
    char buff[100]; 
    int status, pid1, pid2; 
}; 
  
struct memory* shmptr; 
  
void handler(int signum) 
{   
    if (signum == SIGUSR1) { 
        printf("Message recu: "); 
        puts(shmptr->buff); 
    } 
} 
  
int main() 
{   
    // Recuperation du PID
    int pid = getpid(); 
    int shmid; 
    // La cle est la meme que le client
    int key = 12345; 
    shmid = shmget(key, sizeof(struct memory), IPC_CREAT | 0666); 
    shmptr = (struct memory*)shmat(shmid, NULL, 0); 
    shmptr->pid1 = pid; 
    shmptr->status = NotReady; 
    signal(SIGUSR1, handler); 
    while (1) { 
        while (shmptr->status != Ready) 
            continue; 
        sleep(1);   
        // Cela empeche le serveur de communiquer
        shmptr->status = FILLED;   
        kill(shmptr->pid2, SIGUSR2); 
    } 
    shmdt((void*)shmptr); 
    shmctl(shmid, IPC_RMID, NULL); 
    return 0; 
} 
