#include <stdio.h>
#include <stdlib.h>
#include <curl/curl.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

pthread_mutex_t lock;

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fwrite(ptr, size, nmemb, stream);
}

void *downloadFile(void *webAddy) {
    pthread_mutex_lock(&lock);

    // critical section
    char *addy = (char *) webAddy;

    CURL *curl;
    FILE *fp;
    CURLcode res;
    char *outfilename = strrchr(addy, '/');

    if (outfilename != NULL) {
        outfilename += 1;
    } else {
        perror("url does not match format");
    }

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(outfilename, "wb");
        curl_easy_setopt(curl, CURLOPT_URL, addy);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res == CURLE_OK) {
            printf("Download succeeded!\n");
        } else {
            fprintf(stderr, "Download failed: %s\n", curl_easy_strerror(res));
        }
    }

    pthread_mutex_unlock(&lock);
    return NULL;
}

int main(int argc, char **argv) {
    
    if (argc < 2) {
        perror("Usage ./download url1 url2 ...");
    }

    pthread_t threads[argc-1];
    pthread_mutex_init(&lock, NULL);

    for (int i = 0; i < argc - 1; i++) {
        pthread_create(&threads[i], NULL, downloadFile, argv[i]);
    }

    for (int i = 0; i < argc - 1; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_mutex_destroy(&lock);

    return 0;
}