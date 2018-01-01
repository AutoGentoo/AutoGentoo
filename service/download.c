#include <download.h>

int download_progress (void* clientp, curl_off_t dltotal,
                       curl_off_t dlnow,
                       curl_off_t ultotal,
                       curl_off_t ulnow) {
    
    printf ("%" CURL_FORMAT_CURL_OFF_T " / %" CURL_FORMAT_CURL_OFF_T " (%.2f%%)", dlnow, dltotal,
            (((double)dlnow / dltotal) * 100));
    putchar ('\r');
    return 0;
}

response_t download (char* url, char* dest, down_progress p) {
    CURL* curl = curl_easy_init ();
    
    linfo ("Downloading from %s...\n", url);
    
    struct curlprog prog;
    prog.dest = dest;
    if (!curl) {
        lerror ("Could not initialize CURL request!");
        return INTERNAL_ERROR;
    }
    FILE* out = fopen (dest, "wb+");
    
    if (p == SHOW_PROGRESS) {
        printf ("Showing progress\n");
        curl_easy_setopt (curl, CURLOPT_XFERINFODATA, &prog);
        curl_easy_setopt (curl, CURLOPT_NOPROGRESS, 0L);
        curl_easy_setopt (curl, CURLOPT_WRITEDATA, out);
        curl_easy_setopt (curl, CURLOPT_XFERINFOFUNCTION, download_progress);
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