//
// Created by Christoph on 09.09.20.
//

#ifndef POWERWALL_OPENTRACKSERVER_H
#define POWERWALL_OPENTRACKSERVER_H


class OpentrackServer {
public:
    explicit OpentrackServer(unsigned short port = 4242);
    ~OpentrackServer(){};

    bool update();

    int sockfd;
    double currentPose[6];
};

#endif //POWERWALL_OPENTRACKSERVER_H
