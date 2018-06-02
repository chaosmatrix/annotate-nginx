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

