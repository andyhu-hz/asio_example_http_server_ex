# asio_example_http_server_ex

1. 添加了keep-alive支持  
2. 改为使用picohttpparser解析请求  
3. handler改为回调函数  
4. 一个链接长时间未收到数据会强制断开  
