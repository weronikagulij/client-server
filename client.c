#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define MSGQ_ID 1234567
#define MAX_MESSAGES 100
#define MAX_TYPES 1000

struct client {
    int mypid;
    int mid;
} client;

struct msgID {
    long type;
    int clientID;
} clientid; 

struct msgbuf {
    long type;
    long hiddenType;
    char text[1024];
} msg, *messages;

int *msgCount;
int subscribedTypes[MAX_TYPES];

void registerProcess() {
    client.mid = msgget(MSGQ_ID, 0666 | IPC_CREAT);
    clientid.clientID = client.mypid;
    clientid.type = 1; // type 1 is used for communication client - server, sends number of registered client 
    int id = msgsnd(client.mid, &clientid, 1024, 0);
    
    if (id == -1) {
        printf("error");
    }
    
    id = msgrcv(client.mid, &msg, 1024, 2, 0); 
    if(id != -1) {
        printf("Zostales zarejestrowany w systemie!\nTwoj numer: %d\n", client.mypid);
    }
}

void subscribeForMessages() {
    // client is listening to message queue with its type
    int id;
    while(1) {
        id = msgrcv(client.mid, &messages[msgCount[0]], (sizeof(struct msgbuf) - sizeof(long)), client.mypid, 0); 

        if(id != -1) {
            // if message hidden type equals 1 in subscribed types - show 
            if(subscribedTypes[messages[msgCount[0]].hiddenType] == 1) {
                printf("Odebrno wiadomosc: %ld - %s\n",messages[msgCount[0]].hiddenType, messages[msgCount[0]].text);
            }
            
            // if 2 in subscribed types - keep
            else if(subscribedTypes[messages[msgCount[0]].hiddenType] == 2 ) {
                msgCount[0]++;
                printf("\n");
            }
            
        } else {
            printf("error\n");
        }
    }
}

void loginClient() {
    char login[100], password[100];
    printf("\n ---- PANEL LOGOWANIA ----\n");
    printf("Podaj login: ");
    fgets( login, 100, stdin );
    printf("Podaj haslo: ");
    fgets( password, 100, stdin );
    printf( "\nWitaj w systemie, %s\n", login);
}

void setSubscribedTypes(int setTypeTo) {
    char c[100];
    fgets(c, 100, stdin);
    
    int nr = atoi(c);
    while (nr > MAX_TYPES) {
        printf("numer jest zly! Podaj liczbe z zakresu 0 - %d", MAX_TYPES);
        fgets(c, 100, stdin);
        nr = atoi(c);
    }
    
    int type;
    for(int i = 0; i < nr; i ++ ) {
        printf("Podaj subskrybowany typ: ");
        fgets(c, 100, stdin);
        type = atoi(c);
        if(type > 0 && type <= MAX_TYPES) {
            subscribedTypes[type] = setTypeTo;
        }
    }
}

void initClient() {
    // init message queue
    loginClient();
    client.mid = msgget(MSGQ_ID, 0666 | IPC_CREAT);    
    client.mypid = getpid();
    for(int i = 0; i < MAX_TYPES;i ++ ) {
        subscribedTypes[i] = 0;
    }
    
    printf("---- WYBIERZ SUBSKTYPCJE ----\n");
    
    printf("Wiadomosci przychodzace od razu - ile typow chcesz subskrybowac?\n");
    setSubscribedTypes(1);
    
    printf("Wiadomosci przychodzace na polecenie - ile typow chcesz subskrybowac?\n");
    setSubscribedTypes(2);
    
    msgCount[0] = 0;
}

void showSubscriptions() {
    printf(" \n --- SUBSKRYBCJE SYNCHRONICZNE --- \n");
    for(int i = 0; i < MAX_TYPES; i ++ )
        if(subscribedTypes[i] == 1)
            printf("%d ", i);
    
        
    printf(" \n --- SUBSKRYBCJE ASYNCHRONICZNE --- \n");
    for(int i = 0; i < MAX_TYPES; i ++ )
        if(subscribedTypes[i] == 2)
            printf("%d ", i);
}

void getMessages() {
    printf("\n --- WIADOMOSCI W SKRZYNCE - %d --- \n", msgCount[0]);
    for(int i = 0; i < msgCount[0]; i++) {
        printf("%ld - %s\n", messages[i].hiddenType, messages[i].text);
        strcpy(messages[i].text, "");
        messages[i].hiddenType = 0;
    }
    
    msgCount[0] = 0;
}

void sendMessage() {
    char messageText[1024], number[100];
    
    printf("WYSYLANIE NOWEJ WIADOMOSCI\n");
    printf("Podaj tresc: ");
    fgets(messageText, 1024, stdin);
    
    printf("Podaj typ: ");
    fgets(number, 100, stdin);
    
    int nr = atoi(number);
    while(nr < 1 || nr > MAX_TYPES) {
        printf("Zly typ! Typ musi sie miescic w zakresie: %d - %d\n", 1, MAX_TYPES);
        fgets(number, 100, stdin);
        nr = atoi(number);
    }
    
    strcpy(msg.text, messageText);
    msg.hiddenType = nr;
    msg.type = 3;
    printf("wysylanie %ld - %s", msg.hiddenType, msg.text); 
    
    int id = msgsnd(client.mid, &msg, sizeof(msg) - sizeof(msg.type), 0);
    if (id == -1) {
        printf("error\n");
    }
}

void exitProgram () {
    
    msgctl(client.mid, IPC_RMID, NULL);
}

void showMenu() {
    char c[100];
    printf("\nWybierz, co chcesz zrobic:\n");
    printf("1 - Wyswietl subskrybowane typy\n");
    printf("2 - Odbierz wiadomosci\n");
    printf("3 - Wyslij wiadomosc\n");
    printf("4 - Wyjdz\n");
    
    fgets(c, 100, stdin);
    if(strlen(c) > 2) {
        printf("Podano zla wartosc!\n");
        showMenu();
    }
    
    else if(c[0] == '1') {
        showSubscriptions();
        showMenu();
    }
    
    else if(c[0] == '2') {
        getMessages();
        showMenu();
    }
    
    else if(c[0] == '3') {
        sendMessage();
        showMenu(); 
    }
    
     else if(c[0] == '4')
         exitProgram();
    
    else {
        printf("Podano zla wartosc!\n");
        showMenu();
    }
}

int main(int argc, char *argv[]) {
    int shmidClients = shmget(0x1285, sizeof(struct msgbuf)*MAX_MESSAGES, 0644 | IPC_CREAT);
    messages = (struct msgbuf*) shmat(shmidClients, (void*)0, 0);
    
    int shmidCount = shmget(0x1432, sizeof(int)*1, 0644 | IPC_CREAT);
    msgCount = (int*) shmat(shmidCount, (void*)0, 0);
    
    
    initClient();
    registerProcess();
    
     if(fork() == 0) {
         showMenu();
     } else {
         subscribeForMessages();
         
     }
    
    shmdt(msgCount);
    shmctl(shmidCount, IPC_RMID, NULL);

    shmdt(messages);
    shmctl(shmidClients, IPC_RMID, NULL);
    return 0;
}
