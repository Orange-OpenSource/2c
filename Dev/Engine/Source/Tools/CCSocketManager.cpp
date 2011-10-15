/*-----------------------------------------------------------
 * 2C - Cross Platform 3D Application Framework
 *-----------------------------------------------------------
 * Copyright © 2011 – 2011 France Telecom
 * This software is distributed under the Apache 2.0 license,
 * see the "license.txt" file for more details.
 *-----------------------------------------------------------
 * File Name   : CCSocketManager.cpp
 *-----------------------------------------------------------
 */

#include "CCDefines.h"
#include "CCSocketManager.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#ifndef Q_OS_WIN
#ifdef Q_OS_WIN
    #ifdef _WIN32_WINNT
        #undef _WIN32_WINNT
        #define _WIN32_WINNT 0x501
    #endif
    #include <ws2tcpip.h>

    #define _STDCALL_SUPPORTED
    #pragma comment( lib, "ws2_32.lib" )
#else
    #include <fcntl.h>
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <arpa/inet.h>
    #include <netdb.h>
#endif

#define SERVER_PORT "8080"

// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }
    
    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}


bool udpListenerOpen(int &sockfd)
{
    struct addrinfo hints, *servinfo, *p;
    
    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;    // set to AF_INET to force IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;    // use my IP
    
    int rv = getaddrinfo( NULL, SERVER_PORT, &hints, &servinfo );
    if( rv != 0 ) 
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( rv ) );
        return false ;
    }
    
    // loop through all the results and bind to the first we can
    for( p = servinfo; p != NULL; p = p->ai_next ) 
    {
        sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
        if( sockfd == -1) 
        {
            perror( "listener: socket" );
            continue;
        }
        
#ifdef Q_OS_WIN
        u_long iMode = 1;
        int result = ioctlsocket( sockfd, FIONBIO, &iMode );
        ASSERT( result == 0 );
#else
        fcntl( sockfd, F_SETFL, O_NONBLOCK );
#endif
        
        if( bind( sockfd, p->ai_addr, p->ai_addrlen ) == -1 )
        {
            close( sockfd );
            perror( "listener: bind" );
            continue;
        }
        
        break;
    }
    
    if( p == NULL )
    {
        fprintf( stderr, "listener: failed to bind socket\n" );
        return false;
    }
    
    freeaddrinfo( servinfo );
    return true;
}

const int udpListenerReceive(char *buffer, const int length, const int sockfd)
{
    int numbytes;
    struct sockaddr_storage their_addr;
    socklen_t addr_len;
    //char s[INET6_ADDRSTRLEN];
    
    //printf( "listener: waiting to recvfrom...\n" );
    
    addr_len = sizeof their_addr;
    numbytes = recvfrom( sockfd, buffer, length-1 , 0, (struct sockaddr*)&their_addr, &addr_len );
    if( numbytes == -1 )
    {
        //perror( "recvfrom" );
        return numbytes;
    }
    
#ifndef Q_OS_WIN
    //printf( "listener: got packet from %s\n", inet_ntop( their_addr.ss_family, get_in_addr((struct sockaddr *)&their_addr), s, sizeof s ) );
#endif
    //printf( "listener: packet is %d bytes long\n", numbytes );
    //buffer[numbytes] = '\0';
    //printf( "listener: packet contains \"%s\"\n", buffer );
    return numbytes;
}

void udpListenerClose(const int sockfd)
{
    close( sockfd );
}


bool udpListenerSelect()
{       
    fd_set master;    // master file descriptor list
    fd_set read_fds;  // temp file descriptor list for select()
    int fdmax;        // maximum file descriptor number
    
    int listener;     // listening socket descriptor
    int newfd;        // newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // client address
    socklen_t addrlen;
    
    char buf[256];    // buffer for client data
    int nbytes;
    
    char remoteIP[INET6_ADDRSTRLEN];
    
    int yes = 1;        // for setsockopt() SO_REUSEADDR, below
    int i, j, rv;
    
    struct addrinfo hints, *ai, *p;
    
    FD_ZERO( &master );    // clear the master and temp sets
    FD_ZERO( &read_fds );
    
    // get us a socket and bind it
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    if( ( rv = getaddrinfo( NULL, SERVER_PORT, &hints, &ai ) ) != 0 )
    {
        fprintf( stderr, "selectserver: %s\n", gai_strerror( rv ) );
        exit( 1 );
    }
    
    for( p=ai; p!=NULL; p=p->ai_next )
    {
        listener = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
        
        if( listener < 0 )
        {
            continue;
        }
        
        // lose the pesky "address already in use" error message
        setsockopt( listener, SOL_SOCKET, SO_REUSEADDR, (char*)&yes, sizeof(int) );
        
        if( bind( listener, p->ai_addr, p->ai_addrlen ) < 0 )
        {
            close( listener );
            continue;
        }
        
        break;
    }
    
    // if we got here, it means we didn't get bound
    if( p == NULL )
    {
        fprintf( stderr, "selectserver: failed to bind\n" );
        exit( 2 );
    }
    
    freeaddrinfo( ai ); // all done with this
    
    // listen
    if( listen(listener, 10 ) == -1 )
    {
        perror( "listen" );
        exit( 3 );
    }
    
    // add the listener to the master set
    FD_SET( listener, &master );
    
    // keep track of the biggest file descriptor
    fdmax = listener; // so far, it's this one
    
    // main loop
    for( ;; )
    {
        read_fds = master; // copy it
        if( select( fdmax+1, &read_fds, NULL, NULL, NULL ) == -1 )
        {
            perror( "select" );
            exit( 4 );
        }
        
        // run through the existing connections looking for data to read
        for( i=0; i<=fdmax; ++i )
        {
            if( FD_ISSET( i, &read_fds ) )
            {
                // we got one!!
                if( i == listener )
                {
                    // handle new connections
                    addrlen = sizeof remoteaddr;
                    newfd = accept( listener, (struct sockaddr*)&remoteaddr, &addrlen );
                    
                    if( newfd == -1 )
                    {
                        perror( "accept" );
                    }
                    else
                    {
                        FD_SET( newfd, &master ); // add to master set
                        if( newfd > fdmax )
                        {
                            // keep track of the max
                            fdmax = newfd;
                        }
                        
#ifndef Q_OS_WIN
                        printf( "selectserver: new connection from %s on socket %d\n",
                               inet_ntop(remoteaddr.ss_family, get_in_addr((struct sockaddr*)&remoteaddr), remoteIP, INET6_ADDRSTRLEN), newfd );
#endif
                    }
                }
                else
                {
                    // handle data from a client
                    if( ( nbytes = recv( i, buf, sizeof buf, 0 ) ) <= 0 )
                    {
                        // got error or connection closed by client
                        if( nbytes == 0 )
                        {
                            // connection closed
                            printf( "selectserver: socket %d hung up\n", i );
                        }
                        else
                        {
                            perror( "recv" );
                        }
                        close( i ); // bye!
                        FD_CLR( i, &master ); // remove from master set
                    }
                    else
                    {
                        // we got some data from a client
                        for( j=0; j<=fdmax; ++j )
                        {
                            // send to everyone!
                            if( FD_ISSET( j, &master ) )
                            {
                                // except the listener and ourselves
                                if( j != listener && j != i )
                                {
                                    if( send( j, buf, nbytes, 0 ) == -1 )
                                    {
                                        perror( "send" );
                                    }
                                }
                            }
                        }
                    }
                } // END handle data from client
            } // END got new incoming connection
        } // END looping through file descriptors
    } // END for(;;)--and you thought it would never end!
    
    return 0;
}


int udpTalker(const char *hostname, const char *message)
{
    int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    
    memset( &hints, 0, sizeof hints );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    
    rv = getaddrinfo( hostname, SERVER_PORT, &hints, &servinfo );
    if( rv != 0 ) 
    {
        fprintf( stderr, "getaddrinfo: %s\n", gai_strerror( rv ) );
        return 1;
    }
    
    // loop through all the results and make a socket
    for( p = servinfo; p != NULL; p = p->ai_next )
    {
        sockfd = socket( p->ai_family, p->ai_socktype, p->ai_protocol );
        if( sockfd == -1 )
        {
            perror( "talker: socket" );
            continue;
        }
        break;
    }
    
    if( p == NULL )
    {
        fprintf( stderr, "talker: failed to bind socket\n" );
        return 2;
    }
    
    for( int i=0; i<1; ++i )
    {
        numbytes = sendto( sockfd, message, strlen( message ), 0, p->ai_addr, p->ai_addrlen );
        if( numbytes == -1 )
        {
            perror( "talker: sendto" );
            exit( 1 );
        }
    }
    
    freeaddrinfo( servinfo );
    
    //printf( "talker: sent %s %d bytes to %s\n", message, numbytes, hostname );
    close( sockfd );
    
    return 0;
}
#endif
