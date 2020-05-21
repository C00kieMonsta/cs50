/*
 socket ? This is just two programs, objects, one is called a server and the other
 is called the client. The client tries to connect to the server on a certain port number (ex: 8080)
 If the server accepts the connection, then the server also connects to the client and they have 
 a line of code
*/

//
// server.c
//
// David J. Malan
// malan@harvard.edu
//

// feature test macro requirements 
// these are macros that you need to specify to get access to some functions
#define _GNU_SOURCE
#define _XOPEN_SOURCE 700
#define _XOPEN_SOURCE_EXTENDED

// limits on an HTTP request's size, based on Apache's
// http://httpd.apache.org/docs/2.2/mod/core.html
// These are just some limits that are allowed in various contexts for the HTTP requests
// that the browser allows to send me
#define LimitRequestFields 50
#define LimitRequestFieldSize 4094
#define LimitRequestLine 8190

// number of octets for buffered reads
// 1 octet = 1 byte = 8 bits
#define OCTETS 512

// header files
#include <arpa/inet.h>
#include <errno.h>
#include <limits.h>
#include <math.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

// types
typedef char octet;

// prototypes
bool connected(void);
bool error(unsigned short code);
void handler(int signal);
ssize_t load(void);
const char* lookup(const char* extension);
ssize_t parse(void);
void reset(void);
void start(short port, const char* path);
void stop(void);

/*
In general, using so much global variables is not a good idea but in this context we are 
using a technique called signal handeling which allows us to detect when the user types Ctr + c
for example to shut the server down
*/

// server's root - Global variable
char* root = NULL;

// file descriptor for sockets - Global variables
int cfd = -1, sfd = -1; // cfd = client file descriptor et sfd = server file descriptor 

// buffer for request - Global variable
octet* request = NULL;

// FILE pointer for files - Global variable
FILE* file = NULL;

// buffer for response-body - Global variable
octet* body = NULL;

int main(int argc, char* argv[])
{
    // a global variable defined in errno.h that's "set by system 
    // calls and some library functions [to a nonzero value]
    // in the event of an error to indicate what went wrong"
    // It has no type beacause the variable has been defined in a file 
    // we included.
    // Its purpose in life is to be initialised to a specific number 
    // when something goes wrong 
    errno = 0;

    // default to a random port
    int port = 0;

    // usage string - ce qu'on print quand ca va mal
    const char* usage = "Usage: server [-p port] /path/to/root";

    // a function that helps analyse/parse command-line arguments
    int opt;
    while ((opt = getopt(argc, argv, "hp:")) != -1)
    {
        switch (opt)
        {
            // -h  c'est ce qu'on met en flag
            case 'h':
                printf("%s\n", usage);
                return 0;

            // -p port
            case 'p':
                port = atoi(optarg);
                break;
        }
    }

    // error checking
    // ensure port is a non-negative short and path to server's root is specified
    if (port < 0 || port > SHRT_MAX || argv[optind] == NULL || strlen(argv[optind]) == 0)
    {
        // announce usage
        printf("%s\n", usage);

        // return 2 just like bash's builtins
        return 2;
    }

    // It's a finction that starts the web server
    start(port, argv[optind]);

    // listen for SIGINT (aka control-c)
    // this function says,if and when you hear ctrl + c from the keyboard, go ahead
    // and call a function called handler that's clean everything up and stop the server
    signal(SIGINT, handler);

    // accept connections one at a time
    // infinite loop
    while (true)
    {
        // reset server's state
        reset();

        // wait until client is connected
        // connected is a function that returns a boolean
        // but there something special with this function, it is a blocking call
        // it will sit there and wait until a user's browser tries to connect to this web server
        if (connected())
        {
            // parse client's HTTP request
            // the function parse, parses all of the octets, all of the bytes coming from a browser to the server
            // so that we can hand you back a value to one of those global variables that stores
            // all of the bytes in just the header of that request 
            ssize_t octets = parse();
            if (octets == -1)
            {
                continue;
            }

            // extract request's request-line
            // here we parse those headers we stored in the variable octets
            // we want to extract a subset of the information that we care about
            // we just need the request line, for example:
            // GET / HTTP/1.1
            // http://www.w3.org/Protocols/rfc2616/rfc2616-sec5.html
            const char* haystack = request;
            char* needle = strstr(haystack, "\r\n");
            if (needle == NULL)
            {
                error(400);
                continue;
            }
            else if (needle - haystack + 2 > LimitRequestLine)
            {
                error(414);
                continue;
            }
            
            // we copy these bytes into a variable called line which we have 
            // allocated on the STACK. We try to avoid calling the function "malloc"
            // beacause the user can easily stop the server with ctrl+c so we don't
            // want this code to suddenly interrupt because I might not have a chance 
            // to free something I have malloced
            
            // A mon avis needle - haystack est converti en nombre de caractères
            
            /* ********* */
            char line[needle - haystack + 2 + 1];
            /* ********* */
            
            strncpy(line, haystack, needle - haystack + 2);
            line[needle - haystack + 2] = '\0';

            // log request-line
            printf("%s", line);

            // Un serveur envoie des réponses et reçoit des requests 
            // donc il faut uniquement vérifier la validité d'une request
            // request-line   = method SP request-target SP HTTP-version CRLF
            // SP = single space && CRLF = Carriage Return Line Feed
            // CR = \r  et LF = \n
            
            /*
            
            GET /path/file?q=optional HTTP/1.1\r\n
            
            const char* haystack = request;
            char* needle = tout avant "\r\n" = strstr(haystack, "\r\n");
            
            le début de la request est au début de la memory adress "S" et la fin est "S + 36 bytes"
            mais la fin de needle est "S + 36 bytes - 2 bytes" parce qu'on ne tient pas compte du \r\n
            
            */
            
            // TODO: validate request-line
            // Avant de pouvoir valider il faut séparer le request en plusieurs bouts mais avant il
            // est préférable de faire une copie de line
            
            char lineCopy[needle - haystack + 2 + 1];
            strncpy(lineCopy, haystack, needle - haystack + 2);
            lineCopy[needle - haystack + 2] = '\0';
            
            char method[10];
            char requestTarget[60];
            char httpVersion[10];
            
            strcpy(method, strtok(lineCopy, " "));
            strcpy(requestTarget, strtok(NULL, " "));
            strcpy(httpVersion, strtok(NULL, " "));
            
            if (strcmp("GET", method) != 0)
            {
                error(405);
                continue;
            }
            
            if (requestTarget == NULL)
            {
                error(400);
                continue;
            }
            else if (requestTarget[0] != '/')
            {
                error(501);
                continue;
            }
            else if (strchr(requestTarget, '\"') != NULL)
            {
                error(400);
                continue;
            }
            
            if (strstr("HTTP/1.1", httpVersion) == NULL) // pas sûr !!!!!!!!!!
            {
                error(505);
                continue;
            }
            
            

            // TODO: extract query from request-target
            
            char* begin = strchr(requestTarget, '?');
            char* end = strchr(requestTarget, '\0');
            
            char query[(end - begin < 1) ? 1 : end - begin];
            
            if ((end - begin) < 1)
                strncpy(query, begin, 1);
            else if((end - begin) > 1)
                strncpy(query, begin + 1, end - begin);
            
            char absolutePath[begin - requestTarget + 1];
            
            strncpy(absolutePath, requestTarget, begin - requestTarget);
            
            // TODO: concatenate root and absolute-path
            
            char path[strlen(root) + strlen(absolutePath) + 1];
            
            strcpy(path, root);
            
            strcat(path, absolutePath);


            // TODO: ensure path exists 
            // I took this from a website
            
            if (access(path, F_OK) == -1)
            {
                error(404);
                continue;
            }
            
            // TODO: ensure path is readable
            // I took this from a website
            
            if (access(path, R_OK) == -1)
            {
                error(403);
                continue;
            }
 
            // TODO: extract path's extension
            
            end = strchr(absolutePath, '\0');
            char* point = strchr(absolutePath, '.');
            
            char extension[end - point];
            strncpy(extension, point + 1, end - point);
            

            // dynamic content - on check si l'extension est .php
            // si == 0 alors, php est égal a extension
            if (strcasecmp("php", extension) == 0)
            {
                // open pipe to PHP interpreter
                char* format = "QUERY_STRING=\"%s\" REDIRECT_STATUS=200 SCRIPT_FILENAME=\"%s\" php-cgi";
                char command[strlen(format) + (strlen(path) - 2) + (strlen(query) - 2) + 1];
                sprintf(command, format, query, path);
                file = popen(command, "r");
                if (file == NULL)
                {
                    error(500);
                    continue;
                }

                // load file
                ssize_t size = load();
                if (size == -1)
                {
                    error(500);
                    continue;
                }

                // subtract php-cgi's headers from body's size to get content's length
                /*
                
                  void *memmem(const void *haystack, size_t haystacklen, const void *needle, size_t needlelen);
                  
                  memmem finds the start of the first occurrence of the substring "needle" of length "needlelen"
                  in the memory area "haystack" of length "haystacklen".
                  
                */
                haystack = body;
                needle = memmem(haystack, size, "\r\n\r\n", 4);
                if (needle == NULL)
                {
                    error(500);
                    continue;
                }
                size_t length = size - (needle - haystack + 4);

                // respond to client
                if (dprintf(cfd, "HTTP/1.1 200 OK\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Connection: close\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Content-Length: %i\r\n", length) < 0)
                {
                    continue;
                }
                /*
                
                    ssize_t write(int fd, const void *buf, size_t count);
                    
                    write() writes up to count bytes from the buffer pointed buf 
                    to the file referred to by the file descriptor fd.
                    
                    fd = file descriptor = 0 / 1 / 2
                    
                    0 = Standard input
                    1 = Standard output
                    2 = Standard error
                
                */
                
                if (write(cfd, body, size) == -1)
                {
                    continue;
                }
            }

            // static content
            else
            {
                // look up file's MIME type
                const char* type = lookup(extension);
                if (type == NULL)
                {
                    error(501);
                    continue;
                }

                // open file
                file = fopen(path, "r");
                if (file == NULL)
                {
                    error(500);
                    continue;
                }

                // load file
                ssize_t length = load();
                if (length == -1)
                {
                    error(500);
                    continue;
                }

                // TODO: respond to client

                if (dprintf(cfd, "HTTP/1.1 200 OK\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Connection: close\r\n") < 0)
                {
                    continue;
                }
                if (dprintf(cfd, "Content-Length: %i\r\n", length) < 0)
                {
                    continue;
                }
                
                // I tooke this from a website
                
                if (dprintf(cfd, "Content-Type: %s\r\n\r\n", type) < 0)
                {
                    continue;
                }
                if (write(cfd, body, length) == -1)
                {
                    continue;
                }
            }
            
            // announce OK
            printf("\033[32m");
            printf("HTTP/1.1 200 OK");
            printf("\033[39m\n");
        }
    }
}

/**
 * Accepts a connection from a client, blocking (i.e., waiting) until one is heard.
 * Upon success, returns true; upon failure, returns false.
 */
bool connected(void)
{
    struct sockaddr_in cli_addr;
    memset(&cli_addr, 0, sizeof(cli_addr));
    socklen_t cli_len = sizeof(cli_addr);
    cfd = accept(sfd, (struct sockaddr*) &cli_addr, &cli_len);
    if (cfd == -1)
    {
        return false;
    }
    return true;
}

/**
 * Handles client errors (4xx) and server errors (5xx).
 */
bool error(unsigned short code)
{
    // ensure client's socket is open
    if (cfd == -1)
    {
        return false;
    }

    // ensure code is within range
    if (code < 400 || code > 599)
    {
        return false;
    }

    // determine Status-Line's phrase
    // http://www.w3.org/Protocols/rfc2616/rfc2616-sec6.html#sec6.1
    const char* phrase = NULL;
    switch (code)
    {
        case 400: phrase = "Bad Request"; break;
        case 403: phrase = "Forbidden"; break;
        case 404: phrase = "Not Found"; break;
        case 405: phrase = "Method Not Allowed"; break;
        case 413: phrase = "Request Entity Too Large"; break;
        case 414: phrase = "Request-URI Too Long"; break;
        case 418: phrase = "I'm a teapot"; break;
        case 500: phrase = "Internal Server Error"; break;
        case 501: phrase = "Not Implemented"; break;
        case 505: phrase = "HTTP Version Not Supported"; break;
    }
    if (phrase == NULL)
    {
        return false;
    }

    // template
    char* template = "<html><head><title>%i %s</title></head><body><h1>%i %s</h1></body></html>";
    char content[strlen(template) + 2 * ((int) log10(code) + 1 - 2) + 2 * (strlen(phrase) - 2) + 1];
    int length = sprintf(content, template, code, phrase, code, phrase);
    
    /*
        printf(); returns a integer, the number of characters it is displaying.So on success,
        return value is > 0 and on failure the return value is < 0
    */
    
    // respond with Status-Line
    // if < 0, drpintf failed and the code returns false
    if (dprintf(cfd, "HTTP/1.1 %i %s\r\n", code, phrase) < 0)
    {
        return false;
    }

    // respond with Connection header
    if (dprintf(cfd, "Connection: close\r\n") < 0)
    {
        return false;
    }

    // respond with Content-Length header
    if (dprintf(cfd, "Content-Length: %i\r\n", length) < 0)
    {
        return false;
    }

    // respond with Content-Type header
    if (dprintf(cfd, "Content-Type: text/html\r\n") < 0)
    {
        return false;
    }

    // respond with CRLF
    if (dprintf(cfd, "\r\n") < 0)
    {
        return false;
    }

    // respond with message-body
    if (write(cfd, content, length) == -1)
    {
        return false;
    }

    // announce Response-Line
    printf("\033[31m");
    printf("HTTP/1.1 %i %s", code, phrase);
    printf("\033[39m\n");

    return true;
}

/**
 * Loads file into message-body.
 */
ssize_t load(void)
{
    // ensure file is open
    if (file == NULL)
    {
        return -1;
    }

    // ensure body isn't already loaded
    if (body != NULL)
    {
        return -1;
    }

    // buffer for octets
    octet buffer[OCTETS];

    // read file
    ssize_t size = 0;
    while (true)
    {
        // try to read a buffer's worth of octets
        ssize_t octets = fread(buffer, sizeof(octet), OCTETS, file);

        // check for error
        if (ferror(file) != 0)
        {
            if (body != NULL)
            {
                free(body);
                body = NULL;
            }
            return -1;
        }

        // if octets were read, append to body
        if (octets > 0)
        {
            body = realloc(body, size + octets);
            if (body == NULL)
            {
                return -1;
            }
            memcpy(body + size, buffer, octets);
            size += octets;
        }

        // check for EOF
        if (feof(file) != 0)
        {
            break;
        }
    }
    return size;
}

/**
 * Handles signals.
 */
void handler(int signal)
{
    // control-c
    if (signal == SIGINT)
    {
        // ensure this isn't considered an error
        // (as might otherwise happen after a recent 404)
        errno = 0;

        // announce stop
        printf("\033[33m");
        printf("Stopping server\n");
        printf("\033[39m");

        // stop server
        stop();
    }
}

/**
 * Returns MIME type for supported extensions, else NULL.
 */
const char* lookup(const char* extension)
{
    // TODO
    if (extension == NULL)
        return NULL;
    
    
    if (strcasecmp(extension, "css") == 0)
        return "text/css";
        
    else if (strcasecmp(extension, "html") == 0)
        return "text/html";
        
    else if (strcasecmp(extension, "gif") == 0)
        return "image/gif";
        
    else if (strcasecmp(extension, "ico") == 0)
        return "image/x-icon";
        
    else if (strcasecmp(extension, "jpg") == 0)
        return "image/jpeg";
        
    else if (strcasecmp(extension, "js") == 0)
        return "text/javascript";
        
    else if (strcasecmp(extension, "png") == 0)
        return "image/png";
        
    return NULL;
}

/**
 * Parses an HTTP request.
 */
ssize_t parse(void)
{
    // ensure client's socket is open
    if (cfd == -1)
    {
        return -1;
    }

    // ensure request isn't already parsed
    if (request != NULL)
    {
        return -1;
    }

    // buffer for octets
    octet buffer[OCTETS];

    // parse request
    ssize_t length = 0;
    while (true)
    {
        // read from socket
        ssize_t octets = read(cfd, buffer, sizeof(octet) * OCTETS);
        if (octets == -1)
        {
            error(500);
            return -1;
        }

        // if octets have been read, remember new length
        if (octets > 0)
        {
            request = realloc(request, length + octets);
            if (request == NULL)
            {
                return -1;
            }
            memcpy(request + length, buffer, octets);
            length += octets;
        }

        // else if nothing's been read, socket's been closed
        else
        {
            return -1;
        }

        // search for CRLF CRLF
        int offset = (length - octets < 3) ? length - octets : 3;
        char* haystack = request + length - octets - offset;
        char* needle = memmem(haystack, request + length - haystack, "\r\n\r\n", 4);
        if (needle != NULL)
        {
            // trim to one CRLF and null-terminate
            length = needle - request + 2 + 1;
            request = realloc(request, length);
            if (request == NULL)
            {
                return -1;
            }
            request[length - 1] = '\0';
            break;
        }

        // if buffer's full and we still haven't found CRLF CRLF,
        // then request is too large
        if (length - 1 >= LimitRequestLine + LimitRequestFields * LimitRequestFieldSize)
        {
            error(413);
            return -1;
        }
    }
    return length;
}

/**
 * Resets server's state, deallocating any resources.
 */
void reset(void)
{
    // free response's body
    if (body != NULL)
    {
        free(body);
        body = NULL;
    }

    // close file
    if (file != NULL)
    {
        fclose(file);
        file = NULL;
    }

    // free request
    if (request != NULL)
    {
        free(request);
        request = NULL;
    }

    // close client's socket
    if (cfd != -1)
    {
        close(cfd);
        cfd = -1;
    }
}

/**
 * Starts server.
 */
void start(short port, const char* path)
{
    // path to server's root
    root = realpath(path, NULL);
    if (root == NULL)
    {
        stop();
    }

    // ensure root exists
    if (access(root, F_OK) == -1)
    {
        stop();
    }

    // ensure root is executable
    if (access(root, X_OK) == -1)
    {
        stop();
    }

    // announce root
    printf("\033[33m");
    printf("Using %s for server's root", root);
    printf("\033[39m\n");

    // create a socket
    sfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sfd == -1)
    {
        stop();
    }

    // allow reuse of address (to avoid "Address already in use")
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // assign name to socket
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(sfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1)
    {
        stop();
    }

    // listen for connections
    if (listen(sfd, SOMAXCONN) == -1)
    {
        stop();
    }

    // announce port in use
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
    if (getsockname(sfd, (struct sockaddr*) &addr, &addrlen) == -1)
    {
        stop();
    }
    printf("\033[33m");
    printf("Listening on port %i", ntohs(addr.sin_port));
    printf("\033[39m\n");
}

/**
 * Stop server, deallocating any resources.
 */
void stop(void)
{
    // preserve errno across this function's library calls
    int errsv = errno;

    // reset server's state
    reset();

    // free root, which was allocated by realpath
    if (root != NULL)
    {
        free(root);
    }

    // close server socket
    if (sfd != -1)
    {
        close(sfd);
    }

    // terminate process
    if (errsv == 0)
    {
        // success
        exit(0);
    }
    else
    {
        // announce error
        printf("\033[33m");
        printf("%s", strerror(errsv));
        printf("\033[39m\n");

        // failure
        exit(1);
    }
}
