# Use Case

### health check for frontend proxy
1. Architecture
    * Client -> Frontend Proxy -> Backend Proxy -> Backend Server
2. How to work
    1. Normal, maintaining.html should not exist
    2. Maintaining, maintaining.html exist
    3. Frontend proxy verify backend health status depend on status_code and body of response
3. Advantage:
    * Backend mask down, frontend proxy won't send new request
    * Established Connection can be graceful close, not terminal suddenly
    * A waiting timeout needed, prevent long-live connection
4. Example
```
location /health_check/ {
    try_files maintaining.html @health_check_backend;
}

location @health_check_backend {
    proxy_pass http://backend;
}
```

### Local Proxy for large established connection
1. How to work
    * usable local ip address number (127.0.0.1/8): 2 ** 24 - 2
    * usable local port range: ```sysctl -n net.ipv4.ip_local_port_range```
    * max established connection (4-tuple network connection): (2**24 -2) * (net.ipv4.ip_local_port_range - 1) - 1
2. Other Solution
    * http protocol
        * http1 keepalive
        * http2 stream
    * tcp protocol
        * reuse time_wait
        * more ip address
    * kernel
        * net.ipv4.tcp_tw_recycle
        * net.ipv4.tcp_tw_reuse
        * net.ipv4.tcp_timestamps
        * net.netfilter.nf_conntrack_tcp_timeout_time_wait
        * net.netfilter.nf_conntrack_max
        * 4.18-rc support only enable net.ipv4.tcp_tw_reuse=2 for loopback traffic
    * unix domain socket
        * extra mutex added while many thread/process communicating across unix domain socket
3. References
    * https://github.com/torvalds/linux/blob/master/Documentation/networking/ip-sysctl.txt
    * https://lists.freebsd.org/pipermail/freebsd-performance/2005-February/001143.html
    * https://tools.ietf.org/html/rfc3330
4. Example
```
map $remote_port $bind_addr {
    default 127.0.0.1;
    "~^2" 127.0.0.2;
    "~^3" 127.0.0.3;
    "~^4" 127.0.0.4;
    "~^5" 127.0.0.5;
    "~^6" 127.0.0.6;
    "~^7" 127.0.0.7;
    "~^8" 127.0.0.8;
    "~^9" 127.0.0.9;
}

upstream localbackend {
    # per-worker, upstream keepalive
    keepalive 64;
    # consider min_conns as net.ipv4.ip_local_port_range that may cause no avaliable port
    # max_conns avoid anyone server consume all avaliable port
    # increse server to increase max avaliable established connection
    server 127.0.0.1:8080 max_conns=30000;
    server 127.0.0.2:8080 max_conns=30000;
    server 127.0.0.3:8080 max_conns=30000;
}

server {
    listen 80;
    server_name _;
    location / {
        # frontend keepalive
        keepalive_requests 256;
        keepalive_timeout 60s;
        proxy_http_version 1.1;

        # can't get $upstream_addr before proxy_bind
        #proxy_bind $upstream_addr;
        proxy_bind $bind_addr;
        proxy_set_header Host $http_host;
        proxy_next_upstream error timeout;
        proxy_pass http://localbackend;
        proxy_redirect off;
    }
}

server {
    listen 127.0.0.1:8080;
    server_name _;
    return 200 "$remote_addr:$remote_port -->> $server_addr:$server_port\r\n";
}

server {
    listen 127.0.0.2:8080;
    server_name _;
    return 200 "$remote_addr:$remote_port -->> $server_addr:$server_port\r\n";
}

server {
    listen 127.0.0.3:8080;
    server_name _;
    return 200 "$remote_addr:$remote_port -->> $server_addr:$server_port\r\n";
}
```
