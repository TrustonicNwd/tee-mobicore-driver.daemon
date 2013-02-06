/*
Copyright  © Trustonic Limited 2013

All rights reserved.

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright notice, this 
     list of conditions and the following disclaimer.

  2. Redistributions in binary form must reproduce the above copyright notice, 
     this list of conditions and the following disclaimer in the documentation 
     and/or other materials provided with the distribution.

  3. Neither the name of the Trustonic Limited nor the names of its contributors 
     may be used to endorse or promote products derived from this software 
     without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include <curl/curl.h>

#include "logging.h"
#include "rootpaErrors.h"
#include "seclient.h"

#define HTTP_CODE_MOVED                 301
#define HTTP_CODE_BAD_REQUEST           400
#define HTTP_CODE_INTERNAL_ERROR        405
#define HTTP_CODE_NOT_ACCEPTABLE        406
#define HTTP_CODE_REQUEST_TIMEOUT       408
#define HTTP_CODE_CONFLICT              409

rootpaerror_t httpCommunicate(const char* const inputP, const char** linkP, const char** relP, const char** commandP, httpMethod_t method);

rootpaerror_t httpPostAndReceiveCommand(const char* const inputP, const char** linkP, const char** relP, const char** commandP)
{
    LOGD("httpPostAndReceiveCommand %ld", (long int) inputP);

    return httpCommunicate(inputP, linkP, relP, commandP, httpMethod_POST);
}

rootpaerror_t httpPutAndReceiveCommand(const char* const inputP, const char** linkP, const char** relP, const char** commandP)
{
    LOGD("httpPutAndReceiveCommand %ld", (long int) inputP);
    if(NULL==inputP)
    {
        return ROOTPA_ERROR_ILLEGAL_ARGUMENT;
    }
    LOGD("%s", inputP);
    return httpCommunicate(inputP, linkP, relP, commandP, httpMethod_PUT);
}


rootpaerror_t httpGetAndReceiveCommand(const char** linkP, const char** relP, const char** commandP)
{
    LOGD("httpGetAndReceiveCommand");
    return httpCommunicate(NULL, linkP, relP, commandP, false);
}

rootpaerror_t httpDeleteAndReceiveCommand(const char** linkP, const char** relP, const char** commandP)
{
    LOGD("httpDeleteAndReceiveCommand");
    return httpCommunicate(NULL, linkP, relP, commandP, httpMethod_DELETE);
}


typedef struct 
{
    char*  memoryP;
    size_t    size;
} MemoryStruct;
 

typedef struct 
{
    char*  linkP;
    size_t    linkSize;
    char*  relP;
    size_t    relSize;
} HeaderStruct;

typedef struct 
{
    const char*      responseP;
    size_t           size;
    uint32_t         offset;
} ResponseStruct;


static size_t readResponseCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    size_t totalSize=nmemb*size;
    size_t readSize;
    ResponseStruct* rspP=(ResponseStruct*) userp;
    LOGD(">>readResponseCallback %d %d %d\n", (int) totalSize, (int) rspP->size, rspP->offset);

    if(rspP->offset>=rspP->size) return 0;
    
    if(totalSize<((rspP->size)))
    {
        readSize=totalSize;
    }
    else
    {
        readSize=rspP->size;
    }

    memcpy(ptr, (rspP->responseP+rspP->offset), readSize);

    rspP->offset+=readSize;

    LOGD("<<readResponseCallback %d %d %d\n", (int) readSize, (int) rspP->size, rspP->offset);
    return readSize;
}
 
static size_t writeMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    MemoryStruct* mem = (MemoryStruct *)userp;
 
    mem->memoryP = realloc(mem->memoryP, mem->size + realsize + 1);
    if (mem->memoryP == NULL) {
        /* out of memory! */ 
        LOGE("not enough memory (realloc returned NULL)\n");
        return 0; // returning anything different from what was passed to this function indicates an error
    }
 
    memcpy(&(mem->memoryP[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memoryP[mem->size] = 0;
 
    return realsize;
}
 
bool copyHeader(void *contents, size_t length, char** headerP)
{
    *headerP = malloc(length + 1);
    if (*headerP == NULL) {
        /* out of memory! */ 
        LOGE("not enough memory (malloc returned NULL)\n");
        return false;
    }
 
    memcpy(*headerP , contents, length);
    (*headerP)[length] = 0; 
    return true;
}

//
// The header format is as follow 
// Link <https://se.cgbe.trustonic.com:8443/activity/00000000-4455-6677-8899-aabbccddeeff>;rel="http://10.0.2.2/relation/system_info"
// parse out uri's specified in Link and rel
//
bool updateLinkAndRel(HeaderStruct* memP, void* ptr)
{
    char* startP=NULL;
    char* endP=NULL;

    // first update link

    startP=strcasestr((char*) ptr, "Link");
    if(NULL==startP) return false;
        
    startP=strstr(startP,"<");
    if(NULL==startP) return false;
    startP++;
                
    endP=strstr(startP,">");
    if(NULL==endP) return false;

    memP->linkSize=endP-startP;
    if(copyHeader(startP, memP->linkSize, &(memP->linkP))==false)
    {
        return false;
    }

    // then update rel, we will be successful even if it is not found

    startP=strcasestr(endP, "rel=");
    if(NULL==startP)
    {
        return true;
    } 
    startP+=5; // sizeof "rel="
                        
    endP=strstr(startP,"\"");
    if(NULL==endP)
    {
        return true;        
    } 
    memP->relSize=endP-startP;
    if(copyHeader(startP, memP->relSize, &(memP->relP))==false)
    {
        LOGE("could not copy rel, but since we are this far, continuing anyway");
    }

    return true;
}

static size_t writeHeaderCallback( void *ptr, size_t size, size_t nmemb, void *userp)
{
    size_t realSize = size * nmemb;
    HeaderStruct* memP = (HeaderStruct *)userp;

    if(realSize>=sizeof("Link:") && memcmp(ptr, "Link:", sizeof("Link:")-1)==0)
    {
        if(updateLinkAndRel(memP, ptr)==false)
        {
            LOGE("Problems in updating Link and rel");
        }
    }

    return realSize;
}

bool setBasicOpt(CURL* curl_handle, MemoryStruct* chunkP, HeaderStruct* headerChunkP, const char* linkP)
{
    if(curl_easy_setopt(curl_handle, CURLOPT_URL, linkP)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_URL failed");
        return false;
    }
    
    /* reading response to memory instead of file */
    if(curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, writeMemoryCallback)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_WRITEFUNCTION failed");
        return false;
    }
    
    if(curl_easy_setopt(curl_handle, CURLOPT_HEADERFUNCTION, writeHeaderCallback)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_HEADERFUNCTION failed");
        return false;
    }

    if(curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) chunkP)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_WRITEDATA failed");
        return false;
    }

    if(curl_easy_setopt(curl_handle, CURLOPT_WRITEHEADER, (void *) headerChunkP)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_WRITEHEADER failed");
        return false;
    }

     
    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */ 
    if(curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "rpa/1.0")!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_USERAGENT failed");
        return false;
    }

    if(curl_easy_setopt(curl_handle, CURLOPT_NOPROGRESS, 1L)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_USERAGENT failed");
        return false;
    }

    if(curl_easy_setopt(curl_handle, CURLOPT_TIMEOUT, 120L)!=0)  // timeout after 120 seconds
    {
        LOGE("curl_easy_setopt CURLOPT_TIMEOUT failed");
        return false;
    }


// TODO-RELEASE, hopefully these are not needed but libcurl can use the environment variable settings.
// if they are needed, the settings need to be obtained from the database
// also disablingCURLOPT_SSL_VERIFYPEER is for testing purposes only
//
//    curl_easy_setopt(curl_handle,CURLOPT_PROXY, "http://web-proxy.intern:3128");
//    curl_easy_setopt(curl_handle,CURLOPT_PROXYUSERNAME, "soukkote");
//    curl_easy_setopt(curl_handle,CURLOPT_PROXYPASSWORD, "");
//    curl_easy_setopt(curl_handle, CURLOPT_SSL_VERIFYPEER, 0L);

    return true;
}

bool setPutOpt(CURL* curl_handle, ResponseStruct* responseChunk)
{
    LOGD(">>setPutOpt");
    if (curl_easy_setopt(curl_handle, CURLOPT_READFUNCTION, readResponseCallback)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_READFUNCTION failed");
        return false;
    }

    if (curl_easy_setopt(curl_handle, CURLOPT_UPLOAD, 1L)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_UPLOAD failed");
        return false;
    }

    if (curl_easy_setopt(curl_handle, CURLOPT_PUT, 1L)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_PUT failed");
        return false;
    }

    if (curl_easy_setopt(curl_handle, CURLOPT_READDATA, responseChunk)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_READDATA failed");
        return false;
    }

    long s=responseChunk->size;
    if (curl_easy_setopt(curl_handle, CURLOPT_INFILESIZE, s)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_INFILESIZE_LARGE failed");
        return false;
    }
    
    LOGD("<<setPutOpt");
    return true;
}

bool setPostOpt(CURL* curl_handle, const char* inputP)
{
    inputP?(LOGD(">>setPostOpt %d %s", (int) strlen(inputP), inputP)):(LOGD(">>setPostOpt "));
    if (curl_easy_setopt(curl_handle, CURLOPT_POST, 1L)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_POST failed");
        return false;
    }

    if(NULL==inputP) 
    {
        if (curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDSIZE, 0L)!=0)
        {
            LOGE("curl_easy_setopt CURLOPT_POSTFIELDSIZE failed");
            return false;
        }
    }
        
    if (curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, (void*) inputP)!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_POSTFIELDS failed");
        return false;
    }
    
    LOGD("<<setPostOpt");
    return true;
}

bool setDeleteOpt(CURL* curl_handle, const char* inputP)
{
    LOGD(">>setDeleteOpt %s", inputP);
    if (curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE")!=0)
    {
        LOGE("curl_easy_setopt CURLOPT_CUSTOMREQUEST failed");
        return false;
    }

    LOGD("<<setDeleteOpt");
    return true;
}


CURL* curl_handle_=NULL;

rootpaerror_t openSeClientAndInit()
{
    if(curl_global_init(CURL_GLOBAL_ALL)!=0)
    {
        LOGE("curl_gloabal_init failed");
        return ROOTPA_ERROR_NETWORK;
    }
    curl_handle_=curl_easy_init();
    if(NULL==curl_handle_)
    {
        LOGE("initialize failed");
        return ROOTPA_ERROR_NETWORK;
    }

    return ROOTPA_OK;
}

void closeSeClientAndCleanup()
{
    if(curl_handle_)
    {
        curl_easy_cleanup(curl_handle_);
        curl_handle_=NULL;
    }
    curl_global_cleanup();
}

rootpaerror_t httpCommunicate(const char * const inputP, const char** linkP, const char** relP, const char** commandP, httpMethod_t method)
{
    LOGD(">>httpCommunicate");
    if(NULL==linkP || NULL==relP || NULL==commandP || NULL==*linkP)
    {
        return ROOTPA_ERROR_ILLEGAL_ARGUMENT;
    }
    LOGD("url %s", *linkP);
    *commandP=NULL;
    *relP=NULL;

    ResponseStruct responseChunk;

    HeaderStruct headerChunk;     
    headerChunk.linkSize = 0;
    headerChunk.relSize = 0;
    headerChunk.linkP = NULL;
    headerChunk.relP = NULL;    

    MemoryStruct chunk;
    chunk.size = 0;    /* no data at this point */ 
    chunk.memoryP = malloc(1);  /* will be grown as needed by the realloc above */ 
    if(NULL==chunk.memoryP)
    {
        return ROOTPA_ERROR_OUT_OF_MEMORY;
    }
    chunk.memoryP[0]=0;

    LOGD("HTTP method %d", method);
   
    //Process HTTP methods
	if(method == httpMethod_PUT)
	{
		responseChunk.responseP=inputP;
		responseChunk.size=strlen(responseChunk.responseP);
		responseChunk.offset=0;
		if(setPutOpt(curl_handle_, &responseChunk)==false)
		{
			LOGE("setPutOpt failed");
			free(chunk.memoryP);
			return ROOTPA_ERROR_NETWORK;
		}
	}
	else if(method == httpMethod_POST)
	{
		if (setPostOpt(curl_handle_, inputP)==false)
		{
			LOGE("setPostOpt failed");
			free(chunk.memoryP);
			return ROOTPA_ERROR_NETWORK;
		}
	}
	else if(method == httpMethod_DELETE)
	{
	    LOGD("DELETE method. Calling setDeleteOpt..");
		if (setDeleteOpt(curl_handle_, inputP)==false)
		{
			LOGE("setDeleteOpt failed");
			free(chunk.memoryP);
			return ROOTPA_ERROR_NETWORK;
		}
	}
	else
	{
		if(method != httpMethod_GET)
		{
		LOGE("Unsupported HTTP method");
		free(chunk.memoryP);
		return ROOTPA_ERROR_NETWORK;
		}
	}


    if(setBasicOpt(curl_handle_, &chunk, &headerChunk, *linkP)==false)
    {
        LOGE("setBasicOpt failed");
        free(chunk.memoryP);
        return ROOTPA_ERROR_NETWORK;    
    }

    rootpaerror_t ret=ROOTPA_OK;
    ret=curl_easy_perform(curl_handle_);
    long int http_code = 0;
    curl_easy_getinfo (curl_handle_, CURLINFO_RESPONSE_CODE, &http_code);
    if(ret!=0)    
    {
        LOGE("curl_easy_perform failed %d", ret);
        free(chunk.memoryP);
        free(headerChunk.linkP);
        free(headerChunk.relP);
        return ROOTPA_ERROR_NETWORK;
    }
    
    LOGD("http return code from SE %ld", (long int) http_code);    
    if ((200 <= http_code &&  http_code < 300) ||  HTTP_CODE_MOVED == http_code) 
    {
        ret=ROOTPA_OK; 
    }
    else if (HTTP_CODE_BAD_REQUEST == http_code || 
             HTTP_CODE_INTERNAL_ERROR == http_code || 
             HTTP_CODE_NOT_ACCEPTABLE == http_code || 
             HTTP_CODE_CONFLICT == http_code )
    {
        ret=ROOTPA_ERROR_INTERNAL;
    }
    else if(HTTP_CODE_REQUEST_TIMEOUT == http_code  || (411 <= http_code && http_code <= 505))
    {
        ret=ROOTPA_ERROR_NETWORK;
    }
    else
    {
        LOGE("unexpected http return code from SE %ld", (long int)http_code);
        ret=ROOTPA_ERROR_NETWORK;    
    }
 
    /* cleanup curl stuff */ 
 
    *commandP=chunk.memoryP;  // this needs to be freed by client
    *linkP=headerChunk.linkP; // this needs to be freed by client
    *relP=headerChunk.relP;   // this needs to be freed by client
 
    curl_easy_reset(curl_handle_);
    LOGD("%lu bytes retrieved\n", (long)chunk.size);
     
    LOGD("<<httpCommunicate %ld %ld", (long int) ret, (long int) http_code); 
    return ret;
}

