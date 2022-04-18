#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <curl/curl.h>

#define RHOST  "http://localhost:31337/"
#define BRUTE_FORCE_LENGTH 3

const char *CHAR_SET = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKMLMNOPQRSTUVWXYZ0123456789";
const char *CUSTOM_HEADER = "Mozilla/5.0 (Windows NT 6.3; WOW64)" 
"AppleWebKit/537.36 (KHTML, like Gecko) Chrome/45.0.2454.85 Safari/537.36";


struct Node {
        int data;
        struct Node *next;
};


static int  
power(int base, int exp) {
        if (exp == 0) return 1;
        else if (exp % 2) return base * power(base, exp - 1);
        else return (power(base, exp / 2)) * (power(base, exp / 2));
}


static int
get_max() 
{
        int max = 0;
        for(int i = 0; i < BRUTE_FORCE_LENGTH; i++) {
                max += (power(strlen(CHAR_SET), i) * (strlen(CHAR_SET)-1));
        }

        return max;
}


static void *
send_request(const char *url)
{
        CURL *curl;
        FILE *fp;

        fp = fopen("/dev/null", "w");
        curl = curl_easy_init();
        if (curl) {
                curl_easy_setopt(curl, CURLOPT_URL, url);
                curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
                curl_easy_setopt(curl, CURLOPT_USERAGENT, CUSTOM_HEADER);
                curl_easy_perform(curl);

                curl_easy_cleanup(curl); 
        }

        fclose(fp);
        return NULL;
}


static int  
increment(struct Node *n) 
{
        n->data++;
        if (n->data == strlen(CHAR_SET)) {
                if (n->next == NULL) exit(1);
                increment(n->next);                
                n->data = 0;
        }                
                
        return 1;
}


int 
main(int argc, char **argv) 
{
        long int requests_sent;
        struct Node *nodes[BRUTE_FORCE_LENGTH];
        int max = get_max();

        /* Allocate nodes on the heap */
        for (int i = 0; i < BRUTE_FORCE_LENGTH; i++) {
                nodes[i] = (struct Node*)malloc(sizeof(struct Node));
                if (nodes[i] == NULL) return 1;
        }

        /* Populate nodes */
        for (int i = 0; i < BRUTE_FORCE_LENGTH; i++) {
                nodes[i]->data = (i != 0) ? 0 : -1;
                nodes[i]->next = (i != BRUTE_FORCE_LENGTH - 1)  ? nodes[i + 1] : NULL;
        }

        for(;;) {

                char *url;
                if (increment(nodes[0]) == 0) break;
                url = malloc(sizeof(RHOST) + BRUTE_FORCE_LENGTH);
                strcpy(url, RHOST);
                char tmp;

                for(int i = BRUTE_FORCE_LENGTH - 1; i != -1; i--) {
                        char tmp = CHAR_SET[nodes[i]->data];
                        strncat(url, &tmp, 1);
                }

                send_request(url);
                printf("%s    %d/%d\r", url, requests_sent, max);
                free(url);
                requests_sent++;

                fflush(stdout);
        }


        for (int i = 0; i < BRUTE_FORCE_LENGTH; i++) {
                free(nodes[i]);
        }

        return 0;
}
