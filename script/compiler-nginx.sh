#!/bin/bash
#
# Compile nginx
#

PREFIX="/usr/local/nginx"
DIRS="${PREFIX}/etc/nginx
${PREFIX}/usr/sbin/nginx
${PREFIX}/var/log/nginx
${PREFIX}/var/run/
${PREFIX}/var/cache/nginx"

NGX_OPS="--prefix=${PREFIX}/etc/nginx --sbin-path=${PREFIX}/usr/sbin/nginx --modules-path=${PREFIX}/usr/lib/nginx/modules --conf-path=${PREFIX}/etc/nginx/nginx.conf --error-log-path=${PREFIX}/var/log/nginx/error.log --http-log-path=${PREFIX}/var/log/nginx/access.log --pid-path=${PREFIX}/var/run/nginx.pid --lock-path=${PREFIX}/var/run/nginx.lock --http-client-body-temp-path=${PREFIX}/var/cache/nginx/client_temp --http-proxy-temp-path=${PREFIX}/var/cache/nginx/proxy_temp --http-fastcgi-temp-path=${PREFIX}/var/cache/nginx/fastcgi_temp --http-uwsgi-temp-path=${PREFIX}/var/cache/nginx/uwsgi_temp --http-scgi-temp-path=${PREFIX}/var/cache/nginx/scgi_temp --user=nginx --group=nginx --with-compat --with-file-aio --with-threads --with-http_addition_module --with-http_auth_request_module --with-http_dav_module --with-http_flv_module --with-http_gunzip_module --with-http_gzip_static_module --with-http_mp4_module --with-http_random_index_module --with-http_realip_module --with-http_secure_link_module --with-http_slice_module --with-http_ssl_module --with-http_stub_status_module --with-http_sub_module --with-http_v2_module --with-mail --with-mail_ssl_module --with-stream --with-stream_realip_module --with-stream_ssl_module --with-stream_ssl_preread_module"

for d in ${DIRS}
do
    mkdir -p ${d}
done

make clean
./configure ${NGX_OPS} --with-cc-opt='-g -O2 -fstack-protector-strong -Wformat -Werror=format-security -Wp,-D_FORTIFY_SOURCE=2 -fPIC' --with-ld-opt='-Wl,-Bsymbolic-functions -Wl,-z,relro -Wl,-z,now -Wl,--as-needed -pie' --with-debug \
    && make \
    && make install
