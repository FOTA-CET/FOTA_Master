#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <curl/curl.h>

// Hàm gửi yêu cầu POST
bool send_post_request(const std::string &url, const std::string &data) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "Failed to send POST request: " << curl_easy_strerror(res) << std::endl;
        curl_easy_cleanup(curl);
    }
    return true;
}

// Hàm gửi yêu cầu POST với tệp bin
std::string send_firmware_update_request(const std::string &url, const std::string &file_path) {
    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    struct curl_httppost *formpost = NULL;
    struct curl_httppost *lastptr = NULL;

    curl_global_init(CURL_GLOBAL_ALL);

    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "firmware", CURLFORM_FILE, file_path.c_str(), CURLFORM_END);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);

        res = curl_easy_perform(curl);
        if (res != CURLE_OK)
            std::cerr << "Failed to send POST request: " << curl_easy_strerror(res) << std::endl;

        curl_easy_cleanup(curl);
        curl_formfree(formpost);
    }

    curl_global_cleanup();

    return readBuffer;
}

int main() {
    // Thay đổi địa chỉ IP của ESP32 thành địa chỉ thực tế của nó
    std::string esp32_ip = "192.168.193.9";
    std::string url1 = "http://" + esp32_ip + "/update";
    std::string url2 = "http://" + esp32_ip + "/fileSize";

    // Đường dẫn đến tệp cần cập nhật
    std::string file_path = "/home/nuu9hc/Documents/app.ino.bin";
    //std::string file_path = "/home/nuu9hc/Documents/offical.ino.bin";

    // Mở tệp để đọc dữ liệu nhị phân
    std::ifstream bin_file(file_path, std::ios::binary);
    if (!bin_file) {
        std::cerr << "Failed to open file: " << file_path << std::endl;
        return 1;
    }


    // Đọc nội dung của tệp vào một stringstream
    std::ostringstream bin_content_stream;
    bin_content_stream << bin_file.rdbuf();
    std::string bin_content = bin_content_stream.str();

    // Gửi yêu cầu POST với kích thước của tệp
    send_post_request(url2, "fileSize=" + std::to_string(bin_content.size()));
    send_firmware_update_request(url1, file_path);
    return 0;
}  