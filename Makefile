#
# File          : Makefile
# Description   : Build file for CS 165 project 1
#                 


# Environment Setup
#LIBDIRS=-L. -L/opt/local/lib
#INCLUDES=-I. -I/opt/local/include
#CC=gcc 
CC=clang
CFLAGS=-c $(INCLUDES) -g -Wall
LINK=clang -g
LDFLAGS=$(LIBDIRS)
AR=ar rc
RANLIB=ranlib

# Suffix rules
.c.o :
	${CC} ${CFLAGS} $< -o $@

#
# Setup builds

TARGETS=cs165-p1
#LIBS=-lcrypto -lm
LIBS=

#
# Project Protections

p1 : $(TARGETS)

cs165-p1 : cs165-user.o
	$(LINK) $(LDFLAGS) cs165-user.o $(LIBS) -o $@

clean:
	rm -f *.o *~ $(TARGETS)

BASENAME=p1
tar: 
	tar cvfz $(BASENAME).tgz -C ..\
	    $(BASENAME)/Makefile \
            $(BASENAME)/cs165-user.c \
	    $(BASENAME)/cs165-user.h \
	    $(BASENAME)/test1 \
	    $(BASENAME)/test2 \
	    $(BASENAME)/test3 \
	    $(BASENAME)/out1 \
	    $(BASENAME)/out2 \
	    $(BASENAME)/out3 \
	    $(BASENAME)/submit-out1 \
	    $(BASENAME)/submit-out2 \
	    $(BASENAME)/submit-out3 \


