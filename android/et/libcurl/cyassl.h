#ifndef HEADER_CURL_CYASSL_H
#define HEADER_CURL_CYASSL_H

#include "curl_setup.h"

#ifdef USE_CYASSL

CURLcode Curl_cyassl_connect(struct connectdata *conn, int sockindex);
bool Curl_cyassl_data_pending(const struct connectdata* conn,int connindex);
int Curl_cyassl_shutdown(struct connectdata* conn, int sockindex);

/* tell CyaSSL to close down all open information regarding connections (and
   thus session ID caching etc) */
void Curl_cyassl_close_all(struct SessionHandle *data);

 /* close a SSL connection */
void Curl_cyassl_close(struct connectdata *conn, int sockindex);

void Curl_cyassl_session_free(void *ptr);
size_t Curl_cyassl_version(char *buffer, size_t size);
int Curl_cyassl_shutdown(struct connectdata *conn, int sockindex);
int Curl_cyassl_init(void);
CURLcode Curl_cyassl_connect_nonblocking(struct connectdata *conn,
                                         int sockindex,
                                         bool *done);

/* API setup for CyaSSL */
#define curlssl_init Curl_cyassl_init
#define curlssl_cleanup() Curl_nop_stmt
#define curlssl_connect Curl_cyassl_connect
#define curlssl_connect_nonblocking Curl_cyassl_connect_nonblocking
#define curlssl_session_free(x)  Curl_cyassl_session_free(x)
#define curlssl_close_all Curl_cyassl_close_all
#define curlssl_close Curl_cyassl_close
#define curlssl_shutdown(x,y) Curl_cyassl_shutdown(x,y)
#define curlssl_set_engine(x,y) (x=x, y=y, CURLE_NOT_BUILT_IN)
#define curlssl_set_engine_default(x) (x=x, CURLE_NOT_BUILT_IN)
#define curlssl_engines_list(x) (x=x, (struct curl_slist *)NULL)
#define curlssl_version Curl_cyassl_version
#define curlssl_check_cxn(x) (x=x, -1)
#define curlssl_data_pending(x,y) Curl_cyassl_data_pending(x,y)

#endif /* USE_CYASSL */
#endif /* HEADER_CURL_CYASSL_H */
