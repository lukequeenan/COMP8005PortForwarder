#include "eventLoop.h"
#define MAXBUFFSIZE 2048


/*
 -- FUNCTION: void newConnectionTasks(int socketFD)
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: TBD
 --
 -- RETURNS: TBD
 --
 -- NOTES:
 -- This function will perform any maintenance task upon recieving a new connection
 -- For for example creating socketpair and adding them to hashmap
 */
void newConnectionTasks(int socketFD) {
    //may want to establish connection to server here if using the socket pair example
   //then add that new socket to the epoll event monitor
   //probably will use this function to setup the hashmap entry.
}

/*
 -- FUNCTION: closedConnectionTasks
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: TBD
 --
 -- RETURNS: TBD
 --
 -- NOTES:
 -- Handles socket closing operations ie delete from hashmap and close socket pair
 */

void closedConnectionTasks(int socketFD) {
    //find its socket pair from hashmap and close it
    //need to determine
}

/*
 -- FUNCTION: startServer
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void startServer(int port,int (*fnPtr)(int, char*, int))
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- Entry point for starting the epoll server.  give it a port and it will listen for connections
 -- The function pointer is the function called upon reading data.
 */
void startServer(int port,int (*fnPtr)(int, char*, int)) {
    struct epoll_event *events;
    int socketFD;
    int epollFD;
    socketFD = validateSocket(port);
    bindandListenSocket(socketFD);
    epollFD = createEPoll();
    setEPollSocket(epollFD, socketFD, &events);
    eventLoop(socketFD, epollFD, events, fnPtr);
    close(socketFD);
}

/*
 -- FUNCTION: readDataFromSocket
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int readDataFromSocket(int socketFD, int (*fnPtr)(int, char*, int))
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- reads all data that is currently on the socket
 -- maybe be handicapped and need to be readdressed this function may read beyond one packet.
 */
int readDataFromSocket(int socketFD, int (*fnPtr)(int, char*, int)) {
    ssize_t length = 0;
    char *buf;
    buf = malloc(sizeof(char) * MAXBUFFSIZE);
    if (buf == NULL) {
        perror("MALLOC");
        abort();
    }


    while (1) {
        ssize_t count;
        count = recv(socketFD, &buf[length], MAXBUFFSIZE - length, 0);
        length += count;
        if (length == MAXBUFFSIZE) {
            printf("hope this never is reached");
            //some error condition here
        }
        switch (count) {
        case -1:
            if (errno != EAGAIN) {
                perror ("read");
                return 1;
            }
            if (length != 0) {
                return fnPtr(socketFD, buf, length);
            }
            //not sure what to do here so falling throught
        case 0:
            return 1;
        default:
            continue;
        }
    }
    return 0;
}

/*
 -- FUNCTION: readDataFromSocket
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int readDataFromSocket(int socketFD, int (*fnPtr)(int, char*, int))
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- reads all data that is currently on the socket
 -- maybe be handicapped and need to be readdressed this function may read beyond one packet.
 */
void makeNonBlockingSocket (int socketDescriptor) {
    int flags, result;
    flags = fcntl (socketDescriptor, F_GETFL, 0);
    if (flags == -1) {
        perror ("fcntl");
        abort ();
    }

    flags |= O_NONBLOCK;
    result = fcntl (socketDescriptor, F_SETFL, flags);
    if (result == -1) {
        perror ("fcntl");
        abort ();
    }
    return;
}

/*
 -- FUNCTION: getAddressResult
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int getAddressResult(int port, struct addrinfo **result)
 --
 -- RETURNS: 0 on success -1 on failure
 --
 -- NOTES:
 -- used for testing purposes prints out the socket data to console when called
 */
int getAddressResult(int port, struct addrinfo **result) {
    struct addrinfo hints;
    int returnValue;
    char sPort[6];
    memset (&hints, 0, sizeof (struct addrinfo));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    sprintf(sPort, "%d",port);
    returnValue = getaddrinfo (NULL, sPort, &hints, result);
    if (returnValue != 0) {
        fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (returnValue));
        return -1;
    }
    return 0;
}

/*
 -- FUNCTION: createAndBind
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int createAndBind(int port)
 --
 -- RETURNS: a socket descriptor on success -1 on failure
 --
 -- NOTES:
 -- Accepts all the incoming new sockets and sets them as nonblocking.
 */
int createAndBind(int port) {

    struct addrinfo *result, *rp;
    int socketFD;

    if (getAddressResult(port, &result) == -1) { return -1; }

    for (rp = result; rp != NULL; rp = rp->ai_next) {
        socketFD = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (socketFD == -1) {
            continue;
        }
        if (bind (socketFD, rp->ai_addr, rp->ai_addrlen) == 0) {
            /* We managed to bind successfully! */
            break;
        }
        close (socketFD);
    }

    if (rp == NULL) {
        fprintf (stderr, "Could not bind\n");
        return -1;
    }
    freeaddrinfo (result);
    return socketFD;
}

/*
 -- FUNCTION: processIncomingNewSocket
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void processIncomingNewSocket(int socketFD, int epollFD)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- upon a new connection manipulates the socket so it works with epoll
 -- makes it non blocking and adds registers it with epoll for future monitoring
 */
void processIncomingNewSocket(int socketFD, int epollFD) {
    while (1) {
        struct epoll_event event;
        struct sockaddr in_addr;
        socklen_t in_len;
        int infd, epollSuccess, connectResult;
        char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
        in_len = sizeof(in_addr);
        infd = accept (socketFD, &in_addr, &in_len);
        if (infd == -1) {
            if ((errno == EAGAIN) ||
                    (errno == EWOULDBLOCK)) {
                /* We have processed all incoming
                   connections. */
                break;
            } else {
                perror ("accept");
                break;
            }
        }
        connectResult = getnameinfo (&in_addr, in_len,
                         hbuf, sizeof hbuf,
                         sbuf, sizeof sbuf,
                         NI_NUMERICHOST | NI_NUMERICSERV);
        if (connectResult == 0) {
            printf("Accepted connection on descriptor %d "
                   "(host=%s, port=%s)\n", infd, hbuf, sbuf);
            newConnectionTasks(infd);
        }
        makeNonBlockingSocket (infd);

        event.data.fd = infd;
        event.events = EPOLLIN | EPOLLET;
        epollSuccess = epoll_ctl (epollFD, EPOLL_CTL_ADD, infd, &event);
        if (epollSuccess == -1) {
            perror ("epoll_ctl");
            abort ();
        }
    }
}

/*
 -- FUNCTION: eventLoop
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void eventLoop(int socketFD, int epollFD, struct epoll_event *events, int (*fnPtr)(int, char*, int))
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- the primary loop for the function. loops on the epoll_wait system call
 */
void eventLoop(int socketFD, int epollFD, struct epoll_event *events, int (*fnPtr)(int, char*, int)) {
    while (TRUE) {
        int n, i;

        n = epoll_wait (epollFD, events, MAXEVENTS, -1);
        for (i = 0; i < n; i++) {
            if ((events[i].events & EPOLLERR) ||
                    (events[i].events & EPOLLHUP) ||
                    (!(events[i].events & EPOLLIN))) {
                /* An error has occured on this fd, or the socketFD is not
                   ready for reading (why were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
            } else if (socketFD == events[i].data.fd) {
                processIncomingNewSocket(socketFD, epollFD);
                continue;
            } else {
                //Read all data from socket and do something with it
                if (readDataFromSocket(events[i].data.fd, fnPtr)) {
                    printf ("Closed connection on descriptor %d\n",
                            events[i].data.fd);
                    close (events[i].data.fd);
                }
            }
        }
    }
    free (events);
}

/*
 -- FUNCTION: validateSocket
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int validateSocket(int port)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for createAndBind ie error checking
 */
int validateSocket(int port) {
    int socketFD = createAndBind (port);
    if (socketFD == -1) {
        abort ();
    }
    return socketFD;
}

/*
 -- FUNCTION: bindandListenSocket
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void bindandListenSocket(int socketFD)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for the listen system call
 */
void bindandListenSocket(int socketFD) {
    int result;
    makeNonBlockingSocket (socketFD);

    result = listen (socketFD, SOMAXCONN);
    if (result == -1) {
        perror ("listen");
        abort ();
    }
}

/*
 -- FUNCTION: createEPoll
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: int createEPoll()
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for the epoll_create1 call
 */
int createEPoll() {
    int epollFD = epoll_create1 (0);
    if (epollFD == -1) {
        perror ("epoll_create");
        abort ();
    }
    return epollFD;
}

/*
 -- FUNCTION: setEPollSocket
 --
 -- DATE: March 13, 2012
 --
 -- REVISIONS: (Date and Description)
 --
 -- DESIGNER: Warren Voelkl
 --
 -- PROGRAMMER: Warren Voelkl
 --
 -- INTERFACE: void setEPollSocket(int epollFD, int socketFD, struct epoll_event **pevents)
 --
 -- RETURNS: void
 --
 -- NOTES:
 -- wrapper for the epoll_ctl system call
 */
void setEPollSocket(int epollFD, int socketFD, struct epoll_event **pevents) {

    struct epoll_event event;
    int result;

    event.data.fd = socketFD;
    event.events = EPOLLIN | EPOLLET;
    result = epoll_ctl (epollFD, EPOLL_CTL_ADD, socketFD, &event);
    if (result == -1) {
        perror ("epoll_ctl");
        abort ();
    }

    /* Buffer where events are returned */
    *pevents = calloc (MAXEVENTS, sizeof event);
}

