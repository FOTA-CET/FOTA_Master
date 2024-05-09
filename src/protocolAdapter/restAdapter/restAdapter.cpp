#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <curl/curl.h>
#include <thread>
#include <chrono>

#include "restAdapter.hh"

void restAdapter::configConnection(const std::string& ip, std::string& urlUpdate, std::string& urlFileSize) {

  //config url
  urlFileSize = "http://" + ip + "/fileSize";
  urlUpdate = "http://" + ip + "/update";

}

bool restAdapter::readBinFileData(const std::string& file_path, std::string& bin_content) {
  std::ifstream bin_file(file_path, std::ios::binary);
  if(!bin_file) {
    std::cerr << "Failed to open file: " << file_path << std::endl;
    return false;
  }

  std::ostringstream bin_content_stream;
  bin_content_stream << bin_file.rdbuf();
  bin_content = bin_content_stream.str();
  return true;
}


bool restAdapter::sendDataRequest(const std::string &url, const std::string &data) {
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
      std::cerr << "Failed to send POST request: " << curl_easy_strerror(res) << std::endl;
      return false;
    }
    curl_easy_cleanup(curl);
  }
  return true;
}

bool restAdapter::sendFileRequest(const std::string &url, const std::string &file_path) {
  CURL *curl;
  CURLcode res;

  struct curl_httppost *formpost = NULL;
  struct curl_httppost *lastptr = NULL;

  curl_global_init(CURL_GLOBAL_DEFAULT);
  curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "firmware", CURLFORM_FILE, file_path.c_str(), CURLFORM_END);

  curl = curl_easy_init();
  if (curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 30L);

    res = curl_easy_perform(curl);
    std::cout << "res: " << res << std::endl;

    if (res != CURLE_OK) {
      std::cerr << "Failed to send POST request: " << curl_easy_strerror(res) << std::endl;
      return false;
    } else {
      std::cout << "restAdapter::sendDataRequest: Successfully to send POST request" << std::endl;
    }

    curl_easy_cleanup(curl);
    curl_formfree(formpost);
  }
  curl_global_cleanup();
  return true;
}
