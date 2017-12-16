#include <download.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <hacksaw/tools/debug.h>
#include <curl/curl.h>

struct curlprog {
  double lastruntime;
  CURL *curl;
};

int download_progress (void *clientp, double dltotal, double dlnow, double ultotal, double ulnow) {
    struct ttysize ts;
    ioctl(0, TIOCGSIZE, &ts);
    
    int first_part_n = 32
    
    char first_part[64];
    sprintf (first_part, "\r [ %d / %d ] ", dlnow, dltotal);
    
    strncpy (first_part + strlen (first_part), 32 - strlen (first_part), )
    
    return 0;
}

response_t download (char* url, char* dest, down_progress p) {
    CURL* curl = curl_easy_init ();
    FILE* out = fopen (dest, "wb+");
    
    linfo ("Downloading from %s...\n", url);
    
    struct curlprog prog;
    if (!curl) {
        lerror ("Could not initialize CURL request!");
        fclose (out);
        return INTERNAL_ERROR;
    }
    
    if (p == SHOW_PROGRESS) {
        curl_easy_setopt (curl, CURLOPT_PROGRESSDATA, &prog);
    }
    
    curl_easy_setopt (curl, CURLOPT_URL, url);
    curl_easy_setopt (curl, CURLOPT_WRITEDATA, out);
    CURLcode res = curl_easy_perform (curl);
    
    curl_easy_cleanup (curl);
    
    if (res != CURLE_OK) {
        lerror ("curl_easy_perform() failed: %s\n", curl_easy_strerror (res));
        fclose (out);
        return INTERNAL_ERROR;
    }
    
    fclose (out);
    return OK;
}