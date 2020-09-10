//
// Created by Christoph on 09.09.20.
//

#include "OpentrackServer.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/filio.h>
#include <sys/ioctl.h>
#include <assert.h>

OpentrackServer::OpentrackServer(unsigned short port) {
// Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) {
        perror("socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr{};

    memset(&servaddr, 0, sizeof(servaddr));


    // Filling server information
    servaddr.sin_family    = AF_INET; // IPv4
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    // Bind the socket with the server address
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,
              sizeof(servaddr)) < 0 )
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

//    sendto(sockfd, (const char *)hello, strlen(hello),
//           MSG_CONFIRM, (const struct sockaddr *) &cliaddr,
//           len);
//    printf("Hello message sent.\n");
}

bool OpentrackServer::update() {
    socklen_t len, n;
    struct sockaddr_in cliaddr{};

    memset(&cliaddr, 0, sizeof(cliaddr));
    len = sizeof(cliaddr);  //len is value/resuslt

    const static unsigned int pkgLen = sizeof(double[6]);
    double buffer[6];

    bool new_data = false;
    while (ioctl(sockfd, FIONREAD, &n) == 0 && n >= pkgLen) {
        auto received = recvfrom(sockfd, (char *)buffer, pkgLen,
                                 MSG_WAITALL, ( struct sockaddr *) &cliaddr,
                                 &len);
        assert(received == pkgLen);
        memcpy(currentPose, buffer, pkgLen);
        new_data = true;
    }

    //printf("Client : [%f, %f, %f], [%f, %f, %f]\n", currentPose[0], currentPose[1], currentPose[2], currentPose[3], currentPose[4], currentPose[5]);
    return new_data;
}
