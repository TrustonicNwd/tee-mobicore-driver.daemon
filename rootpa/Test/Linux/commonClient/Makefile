#
# Copyright  © Trustonic Limited 2013
#
# All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without modification, 
#  are permitted provided that the following conditions are met:
#
#  1. Redistributions of source code must retain the above copyright notice, this 
#     list of conditions and the following disclaimer.
#
#  2. Redistributions in binary form must reproduce the above copyright notice, 
#     this list of conditions and the following disclaimer in the documentation 
#     and/or other materials provided with the distribution.
#
#  3. Neither the name of the Trustonic Limited nor the names of its contributors 
#     may be used to endorse or promote products derived from this software 
#     without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
# INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
# OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
# OF THE POSSIBILITY OF SUCH DAMAGE.
#


# sudo apt-get install libxml2-devel
# sudo apt-get install libcurl4-openssl-d
TLCM=/home/tersou01/mc/Common/TlCm/trunk/00028991
TLSDK=/home/tersou01/mc/Common/TlSdk/trunk/00028826
MOBICOREDRIVERLIB=/home/tersou01/mc/Common/MobiCoreDriverLib/trunk/00028995

LIBXML2 = /usr/lib/x86_64-linux-gnu/libxml2.a
LIBCURL = /usr/lib/x86_64-linux-gnu/libcurl.a
LIBSSL = /usr/lib/x86_64-linux-gnu/libssl.a
LIBCRYPTO = /usr/lib/x86_64-linux-gnu/libcrypto.a

LIBS= $(LIBXML2) $(LIBCURL) $(LIBSSL) $(LIBCRYPTO)

LDPATH = -L/usr/lib/x86_64-linux-gnu
LDLIBS = -lz -lm -ldl -lldap -lrtmp -lgssapi_krb5 -lrt -llzma -lidn

CC=gcc
GCPP=g++
AR=ar
LINT=splint 
VALGRIND=valgrind
VALGRINDPARAMS=-v  --leak-check=full  --show-reachable=yes --track-origins=yes

CFLAGS= -Wall -D_GNU_SOURCE # -D_GNU_SOURCE is here for removing warnings and making sure strcasestr works properly
CFLAGS+= -Wall -D__DEBUG 

LDFLAGS=rcs
INCDIR=-I../../../Code/Common/include -I/usr/include/libxml2 -I/usr/include/curl -I$(TLSDK)/Out/Public/MobiCore/inc -I$(TLCM)/Out/Public -I$(MOBICOREDRIVERLIB)/Out/Public
_SOURCEDIR=../../../Code/Common
SOURCES=$(_SOURCEDIR)/*.c
OBJECTS=*.o
ROOTPALIB=rootpacommon.a

TESTSOURCES=*.cpp 
MCSTUBSRC = ../../Common/mcStub/mcStub.c
STUB=mcStub.o
TESTAPP=rootpaclient_linux

all: $(TESTAPP)

lint: $(SOURCES)
	$(LINT) $(INCDIR) $(SOURCES) 
    
test: $(TESTAPP)
	$(VALGRIND) $(VALGRINDPARAMS) ./$(TESTAPP)

$(TESTAPP): $(STUB) $(ROOTPALIB) 
	$(GCPP) -o $(TESTAPP) $(INCDIR) $(CFLAGS) $(LDPATH) $(TESTSOURCES) $(ROOTPALIB) $(LIBS)  $(LDLIBS) 
    
$(ROOTPALIB): $(OBJECTS) $(STUB)
	$(AR) $(LDFLAGS) $(ROOTPALIB) $(OBJECTS)

$(STUB): $(MCSTUBSRC) 
	$(CC) -c $(INCDIR) $(CFLAGS) $(MCSTUBSRC) 
    
$(OBJECTS): $(SOURCES)
	$(CC) -c $(INCDIR) $(CFLAGS) $(SOURCES) 

clean:
	rm -rf *.o
	rm -rf *.xsd
	rm -rf $(ROOTPALIB)
	rm -rf $(TESTAPP)     
