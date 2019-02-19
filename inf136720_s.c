#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define MSGQ_ID 1234567
#define MAX_CLIENTS 1000

struct msgID {
long type;
int clientID;
} clientid[MAX_CLIENTS]; 

int clientsCount;
int messageSendIeration;

struct serverStruct {
    int mid;
} server;

struct msgbuf {
long type;
long hiddenType;
char text[1024];
} msg, messages[10];

int messagesCount = 0;

void addMessage(char message[], int type) {

    strcpy(messages[messagesCount].text, message);
    messages[messagesCount].hiddenType = type;
    messagesCount ++;
}

void createDefaults() {
    
    // create list of messages to send
    addMessage("Wygrana Polski w mistrzostwach o puchar ligi", 1);
    addMessage("Budowa wielojezycznego gimnazjum w Opolu", 2);
    addMessage("'Slepnac od swiatel' juz w telewizji!", 3);
    addMessage("Prezydent Poznania oglosil", 4);
    addMessage("Nowe znizki na transport publiczny w Poznaniu", 5);
    addMessage("Budowa elektrowni jadrowej", 6);
    addMessage("Otwarcie nowej galerii w centrum", 7);
    addMessage("'Kler' nominowany do Zlotych Lwow", 8);
    
}

void sendToAll(struct msgbuf msg) {
    for( int i = 0; i < clientsCount; i ++ ) { 
            msg.type = clientid[i].clientID;
            int id = msgsnd(server.mid, &msg, sizeof(msg) - sizeof(msg.type), 0);
            if (id == -1) {
                printf("error\n");
            }
        }
}

void sendMessage() {
    if(messageSendIeration < 6) {
        messageSendIeration ++;
    } else {
        messageSendIeration = 0;
        srand(time(NULL));
        int random = rand() % messagesCount;
        msg = messages[random];
        if(clientsCount > 0) {
            printf("Wysylam wiadomosc do wszystkich: %s\n", msg.text);
            sendToAll(msg);
        }
    }
}

void registerClient() {
    int id = msgrcv(server.mid, &(clientid[clientsCount]), sizeof(long) * MAX_CLIENTS, 1, IPC_NOWAIT); // type 1 - register users
    
    if(id != -1) {
        // successfully registered user, increase clients count
        clientsCount ++;
        
        // send back message with type 2
        printf("Zarejestrowano klienta nr: %d\n", clientid[clientsCount - 1].clientID);
        msg.type = 2;
        id = msgsnd(server.mid, &msg, 1024, 0);
        if(id == -1) {
            printf("error\n");
        }
        
    }
}

void initServer() {
    server.mid = msgget(MSGQ_ID, 0666 | IPC_CREAT);
    clientsCount = 0;
    messageSendIeration = 0;
}

void receiveMsgFromClients() {
    int id = msgrcv(server.mid, &msg, sizeof(struct msgbuf) - sizeof(long), 3, IPC_NOWAIT); 
    if(id!= -1) {
        printf("Otrzymano wiadomosc do roproszenia: %s\n", msg.text);
        sendToAll(msg);
    }
}

void sendAndRegister() {
    while(1) {
        registerClient();
        receiveMsgFromClients();
        sendMessage();
        sleep(2);
    }
}

int main(int argc, char *argv[]) {
    printf("\n --- SERWER --- \n");
    initServer();
    
    createDefaults();
    sendAndRegister();
    
    return 0;
}