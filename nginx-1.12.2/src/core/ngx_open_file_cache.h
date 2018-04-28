
/*
 * Copyright (C) Igor Sysoev
 * Copyright (C) Nginx, Inc.
 */


#include <ngx_config.h>
#include <ngx_core.h>


#ifndef _NGX_OPEN_FILE_CACHE_H_INCLUDED_
#define _NGX_OPEN_FILE_CACHE_H_INCLUDED_


#define NGX_OPEN_FILE_DIRECTIO_OFF  NGX_MAX_OFF_T_VALUE


// Annotate:
//  * open_cache_file file info
typedef struct {
    ngx_fd_t                 fd;
    // * inode number
    ngx_file_uniq_t          uniq;
    time_t                   mtime;
    // * file size, st_size
    off_t                    size;
    // * file size, associate filesystem blocksize
    // * max(size, st_blocks * 512)
    off_t                    fs_size;
    off_t                    directio;
    size_t                   read_ahead;

    // * error while stat/fstat file
    ngx_err_t                err;
    char                    *failed;

    // * elemnt valid time
    time_t                   valid;

    // * open_file_cache_min_uses
    ngx_uint_t               min_uses;

#if (NGX_HAVE_OPENAT)
    size_t                   disable_symlinks_from;
    unsigned                 disable_symlinks:2;
#endif

    unsigned                 test_dir:1;
    unsigned                 test_only:1;
    unsigned                 log:1;
    // * open_file_cache_errors  ?
    unsigned                 errors:1;
    unsigned                 events:1;

    unsigned                 is_dir:1;
    unsigned                 is_file:1;
    unsigned                 is_link:1;
    unsigned                 is_exec:1;
    unsigned                 is_directio:1;
} ngx_open_file_info_t;


typedef struct ngx_cached_open_file_s  ngx_cached_open_file_t;

// Annotate:
//  * open_file already cached
struct ngx_cached_open_file_s {
    ngx_rbtree_node_t        node;
    ngx_queue_t              queue;

    u_char                  *name;
    // * cache entry created time
    time_t                   created;
    // * least accessed time by client
    time_t                   accessed;

    ngx_fd_t                 fd;
    ngx_file_uniq_t          uniq;
    time_t                   mtime;
    off_t                    size;
    ngx_err_t                err;

    uint32_t                 uses;

#if (NGX_HAVE_OPENAT)
    size_t                   disable_symlinks_from;
    unsigned                 disable_symlinks:2;
#endif

    unsigned                 count:24;
    unsigned                 close:1;
    unsigned                 use_event:1;

    unsigned                 is_dir:1;
    unsigned                 is_file:1;
    unsigned                 is_link:1;
    unsigned                 is_exec:1;
    unsigned                 is_directio:1;

    ngx_event_t             *event;
};


// Annotate:
//  *
typedef struct {
    // * store elemtns
    ngx_rbtree_t             rbtree;
    ngx_rbtree_node_t        sentinel;
    // * elements already expire
    ngx_queue_t              expire_queue;

    // * current elements
    ngx_uint_t               current;
    // * max elements
    ngx_uint_t               max;
    // * inactive time, then element will be cleanup
    time_t                   inactive;
} ngx_open_file_cache_t;


// Annotate:
//  *
typedef struct {
    ngx_open_file_cache_t   *cache;
    ngx_cached_open_file_t  *file;
    // * open_file_cache_min_use
    ngx_uint_t               min_uses;
    ngx_log_t               *log;
} ngx_open_file_cache_cleanup_t;


// Annotate:
//  * open_file_cache associate event
typedef struct {

    /* ngx_connection_t stub to allow use c->fd as event ident */
    void                    *data;
    ngx_event_t             *read;
    ngx_event_t             *write;
    ngx_fd_t                 fd;

    ngx_cached_open_file_t  *file;
    ngx_open_file_cache_t   *cache;
} ngx_open_file_cache_event_t;


ngx_open_file_cache_t *ngx_open_file_cache_init(ngx_pool_t *pool,
    ngx_uint_t max, time_t inactive);
ngx_int_t ngx_open_cached_file(ngx_open_file_cache_t *cache, ngx_str_t *name,
    ngx_open_file_info_t *of, ngx_pool_t *pool);


#endif /* _NGX_OPEN_FILE_CACHE_H_INCLUDED_ */
