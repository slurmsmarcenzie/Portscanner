#include <stdio.h>
#include <winsock2.h>
#include <stdlib.h>
#include <string.h>

#include <ws2tcpip.h>

void scanner(const char* ip, const char* fPort, const char* lPort) {

    // need this to start winsock
    WSADATA wsaData;
    int iResult;

    // initialising with version 2 2 which is common one apparently
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        fprintf(stderr, "Failed to initialize Winsock. Error code: %d\n", iResult);
        return;
    }


    // creating the struct (addrinfo) that holds all the members 
    struct addrinfo *result = NULL, *ptr = NULL, hints;
    //ZeroMemory is for windows to put memory bytes back to 0 (garbage clean up basically)
    ZeroMemory(&hints, sizeof(hints));
    // ai_ family is address family (holding IP)
    hints.ai_family = AF_INET;
    // ai_socktype is sockettype 
    hints.ai_socktype = SOCK_STREAM;
    // ai_protocol is type, UDP OR TCP IN OUR CASE TCP
    hints.ai_protocol = IPPROTO_TCP;


    // inbuilt function to put all the members infor into iresult
    // pointer for hints which goes to addrinfo
    // pointer for results to store result
    iResult = getaddrinfo(ip, fPort, &hints, &result);
    // error checking
    if (iResult != 0) {
        fprintf(stderr, "Error: getaddrinfo failed. Error code: %d\n", iResult);
        WSACleanup();
        return;
    }


    // for loop over pointer list, moves pointer to next in list if not empty
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // creates socket named ConnectSocket with each variable (IP, Socketype and Protocol from addrinfo struct)
        SOCKET ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);
        // if for if error occurs
        if (ConnectSocket == INVALID_SOCKET) {
            fprintf(stderr, "Error: Failed to create socket. Error code: %ld\n", WSAGetLastError());
            continue;
        }

        // connect to socket to addr and size convert
        iResult = connect(ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            continue;
        }
        // output so i know it connect to port number
        printf("Connected to server on port number %s\n", fPort);
        // close the connection
        closesocket(ConnectSocket);
    }

    // free up the addrinfo struct from result
    freeaddrinfo(result);
    // clean up memory 
    WSACleanup();
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Please enter IP, Port beginning and Port Ending Number\n");
        return 1;
    }


    // strings in C can't be changed cause const
    const char* firstIP = argv[1];
    const char* firstPort = argv[2];
    const char* lastPort = argv[3];


    // atoi converts string to int
    // it means ascii to interger (Atoi)
    int startPort = atoi(firstPort);
    int endPort = atoi(lastPort);

    // looping through port num
    for (int port = startPort; port <= endPort; ++port) {
        char portStr[6];
        // convert port num to string
        snprintf(portStr, sizeof(portStr), "%d", port);
        // calling out scanner function (void)
        scanner(firstIP, portStr, portStr);
    }

    // this has to be end of C program so ends properly
    return 0;
}
