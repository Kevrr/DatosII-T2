#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define PORT 4433

void handle_client(SSL* ssl) {
    char buffer[4096] = {0};
    int bytes = SSL_read(ssl, buffer, sizeof(buffer));
    if (bytes <= 0) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
        return;
    }

    std::string request(buffer);
    std::string message;

    // Extract message body after the HTTP headers
    size_t pos = request.find("\r\n\r\n");
    if (pos != std::string::npos) {
        message = request.substr(pos + 4);
    }

    std::cout << "📩 Received: " << message << std::endl;

    // Echo the message back in the HTTP response
    std::string response = "HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\nEcho: " + message;

    SSL_write(ssl, response.c_str(), response.size());

    SSL_shutdown(ssl);
    SSL_free(ssl);
}

int main() {
    SSL_load_error_strings();
    OpenSSL_add_ssl_algorithms();
    SSL_CTX* ctx = SSL_CTX_new(TLS_server_method());

    SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM);
    SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM);

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;
    bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
    listen(sockfd, 1);

    std::cout << "🔒 HTTPS Echo Server running on port " << PORT << "...\n";

    while (true) {
        int client = accept(sockfd, nullptr, nullptr);
        SSL* ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);
        if (SSL_accept(ssl) > 0) {
            handle_client(ssl);
        } else {
            SSL_free(ssl);
        }
        close(client);
    }

    close(sockfd);
    SSL_CTX_free(ctx);
    EVP_cleanup();
    return 0;
}
