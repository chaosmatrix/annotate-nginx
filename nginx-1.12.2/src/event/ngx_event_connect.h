
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#ifndef _NGX_EVENT_CONNECT_H_INCLUDED_
#define _NGX_EVENT_CONNECT_H_INCLUDED_


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>


#define NGX_PEER_KEEPALIVE           1
#define NGX_PEER_NEXT                2
#define NGX_PEER_FAILED              4

// Annotate:
//  * active connection
typedef struct ngx_peer_connection_s  ngx_peer_connection_t;

typedef ngx_int_t (*ngx_event_get_peer_pt)(ngx_peer_connection_t *pc,
    void *data);
typedef void (*ngx_event_free_peer_pt)(ngx_peer_connection_t *pc, void *data,
    ngx_uint_t state);
typedef void (*ngx_event_notify_peer_pt)(ngx_peer_connection_t *pc,
    void *data, ngx_uint_t type);
typedef ngx_int_t (*ngx_event_set_peer_session_pt)(ngx_peer_connection_t *pc,
    void *data);
typedef void (*ngx_event_save_peer_session_pt)(ngx_peer_connection_t *pc,
    void *data);

// Annotate:
//  * active Connection
struct ngx_peer_connection_s {
    ngx_connection_t                *connection;

    // * peer socket info
    struct sockaddr                 *sockaddr;
    socklen_t                        socklen;
    ngx_str_t                       *name;

    // * retry times, if failed
    ngx_uint_t                       tries;
    ngx_msec_t                       start_time;

    // * function, get/free connection
    // * if use connection pool, these function must implement
    ngx_event_get_peer_pt            get;
    ngx_event_free_peer_pt           free;
    // * ?
    ngx_event_notify_peer_pt         notify;
    void                            *data;

#if (NGX_SSL || NGX_COMPAT)
    ngx_event_set_peer_session_pt    set_session;
    ngx_event_save_peer_session_pt   save_session;
#endif

    // * local addr
    ngx_addr_t                      *local;

    int                              type;
    // * recv buf
    int                              rcvbuf;

    ngx_log_t                       *log;

    // * cache enable ?
    unsigned                         cached:1;
    // * enable IP_TRANSPARENT
    // * allows the calling application to bind to a nonlocal IP address
    // * and operate both as a client
    // * and a server with the foreign address as the local endpoint.
    unsigned                         transparent:1;

                                     /* ngx_connection_log_error_e */
    unsigned                         log_error:2;

    NGX_COMPAT_BEGIN(2)
    NGX_COMPAT_END
};


ngx_int_t ngx_event_connect_peer(ngx_peer_connection_t *pc);
ngx_int_t ngx_event_get_peer(ngx_peer_connection_t *pc, void *data);


#endif /* _NGX_EVENT_CONNECT_H_INCLUDED_ */
