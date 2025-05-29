#include <iostream>
#include <string>
#include <curl/curl.h>

size_t write_cb(char* ptr, size_t size, size_t nmemb, void* userdata) {
    std::string* response = static_cast<std::string*>(userdata);
    response->append(ptr, size * nmemb);
    return size * nmemb;
}

int main() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    CURL* curl = curl_easy_init();

    if (!curl) {
        std::cerr << "Failed to initialize CURL.\n";
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, "https://localhost:4433");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L); // Disable cert verification
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    std::string input, response;
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

    while (true) {
        std::cout << "You: ";
        std::getline(std::cin, input);
        if (input.empty()) continue;

        response.clear();
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, input.c_str());

        CURLcode res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            std::cerr << "Request failed: " << curl_easy_strerror(res) << "\n";
        } else {
            std::cout << response << std::endl;
        }
    }

    curl_easy_cleanup(curl);
    curl_global_cleanup();
    return 0;
}
