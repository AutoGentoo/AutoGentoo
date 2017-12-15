#include <download.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <hacksaw/tools/debug.h>
#include <curl/curl.h>

struct curlprog {
  double lastruntime;
  CURL *curl;
};

response_t download (char* url, char* dest, down_progress p) {
    CURL* curl = curl_easy_init ();
    
    struct curlprog prog;
    if (!curl) {
        lerror ("Could not initialize CURL request!");
        return INTERNAL_ERROR;
    }
    
    if (p & SHOW_PROGRESS) {
        curl_easy_setopt (curl, CURLOPT_PROGRESSDATA, &prog);
    }
    
    curl_easy_setopt (curl, CURLOPT_URL, url);
    CURLcode res = curl_easy_perform (curl);
    
    curl_easy_cleanup (curl);
    
    if (res != CURLE_OK) {
        lerror ("curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
        return INTERNAL_ERROR;
    }
    
    return OK;
}