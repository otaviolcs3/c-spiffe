#include "dial.h"
#include "spiffetls/src/mode.h"
#include "spiffetls/src/option.h"
#include "spiffetls/tlsconfig/src/config.h"
#include <unistd.h>

static int createSocket(in_port_t port)
{
    struct sockaddr_in address = { .sin_family = AF_INET,
                                   .sin_addr.s_addr = htonl(INADDR_ANY),
                                   .sin_port = htons(port) };

    const int sockfd = socket(/*IPv4*/ AF_INET, /*TCP*/ SOCK_STREAM, /*IP*/ 0);
    if(sockfd < 0) {
        // could not create socket
        return -1;
    }

    const int bind_ret
        = bind(sockfd, (const struct sockaddr *) &address, sizeof address);
    if(bind_ret < 0) {
        // could not bind socket
        return -1;
    }

    const int listen_ret = listen(sockfd, /*backlog*/ 1);
    if(listen_ret < 0) {
        // could not listen from socket
        return -1;
    }

    return sockfd;
}

static SSL_CTX *createTLSContext()
{
    const SSL_METHOD *method = TLS_method();
    SSL_CTX *ctx = SSL_CTX_new(method);

    return ctx;
}

SSL *spiffetls_ListenWithMode(in_port_t port, in_addr_t addr,
                              spiffetls_ListenMode *mode,
                              spiffetls_listenConfig *config, err_t *err)
{
    if(!mode->unneeded_source) {
        workloadapi_X509Source *source = mode->source;
        if(!source) {
            source = workloadapi_NewX509Source(NULL, err);

            if(*err) {
                goto error;
            }
        }
        mode->source = source;
        mode->bundle = x509bundle_SourceFromSource(source);
        mode->svid = x509svid_SourceFromSource(source);
    }

    SSL_CTX *tls_config
        = config->base_TLS_conf ? config->base_TLS_conf : createTLSContext();

    switch(mode->mode) {
    case TLS_SERVER_MODE:
        tlsconfig_HookTLSServerConfig(tls_config, mode->svid, NULL);
        break;
    case MTLS_SERVER_MODE:
        tlsconfig_HookMTLSServerConfig(tls_config, mode->svid, mode->bundle,
                                       mode->authorizer, NULL);
        break;
    case MTLS_WEBSERVER_MODE:
    default:
        // unknown mode
        *err = ERROR1;
        goto error;
    }

    const int sockfd
        = config->listener_fd > 0 ? config->listener_fd : createSocket(port);
    if(sockfd < 0) {
        // could not create socket with given address and port
        *err = ERROR2;
        goto error;
    }
    
    struct sockaddr_in addr;
    socklen_t len;
    const int clientfd = accept(sockfd, (struct sockaddr *) &addr, &len);
    if(clientfd < 0) {
        // could not accept client
        *err = ERROR3;
        goto error;
    }

    SSL *conn = SSL_new(tls_config);
    SSL_set_fd(conn, clientfd);
    SSL_set_accept_state(conn);

    if(SSL_accept(conn) != 1) {
        // could not build a SSL session
        SSL_shutdown(conn);
        SSL_free(conn);
        close(sockfd);
        *err = ERROR2;
        goto error;
    }
    // successful handshake
    return conn;

error:
    return NULL;
}