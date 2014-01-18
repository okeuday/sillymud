##
CC=gcc
##
LD=$(CC)
##
##  these are compile time constructs, that affect the running of the game.
CPPFLAGS = -DIMPL_SECURITY -DNEW_RENT -DLEVEL_LOSS -DNEWEXP \
 -DGROUP_NAMES
##  these are architecture dependent flags
TITANFLAGS = -g -43 -vector_c -safe=ptrs -DTITAN
SUNFLAGS = -g
##
CFLAGS = $(SUNFLAGS) $(CPPFLAGS)
##
##
##  For lower exp loss at lower levels -DNEWEXP
##
##  For level loss for really low exp -DLEVEL_LOSS
##
##  To make some optimizations, you need to add things to the line above.
##  to use hash tables for rooms  (More cpu, less storage), add
##  -DHASH    
##
##  To get rid of tracking (lower cpu)
##  -DNOTRACK
##
##  To limit items above a specific rent cost
##  -DLIMITED_ITEMS
##
##  To force new players to be authorized
##  -DPLAYER_AUTH
##
##  To add some new debugging stuff (mostly limited to malloc_debug)
##  -DDEBUG
##  To add some commands for locking out specific hosts
##  -DSITELOCK
##
## To lock out oak.grove.iup.edu from boot time.
## -DLOCKGROVE
##
##  To save the world (objects and mobs) -DSAVE_WORLD
##
##  To modify some commands to make item duplication near impossible:
##  -DNODUPLICATES
##
##  To have each 58+ god restricted to one site, or set of sites
##  -DIMPL_SECURITY
##
##  To use BSD memory allocation/deallocation routines instead of the
##  native memory routines:
##  -DKLUDGE_MEM
##
##  If your system doesn't have built in functions of strdup() and
##  strstr(), then:
##  -DKLUDGE_STRING
##

HEADERFILES = structs.h utils.h comm.h interpreter.h db.h
##
OBJS = comm.o act.comm.o act.info.o act.move.o act.obj1.o act.obj2.o \
	act.off.o act.other.o act.social.o act.wizard.o handler.o \
	db.o interpreter.o utility.o spec_assign.o shop.o limits.o mobact.o \
	fight.o modify.o weather.o spells1.o spells2.o spell_parser.o \
	reception.o constants.o spec_procs.o signals.o board.o magic.o \
	magic2.o skills.o Opinion.o Trap.o magicutils.o multiclass.o hash.o \
	Sound.o Heap.o spec_procs2.o magic3.o security.o spec_procs3.o \
        create.o bsd.o parser.o intrinsics.o

SRCS = comm.c act.comm.c act.info.c act.move.c act.obj1.c act.obj2.c \
	act.off.c act.other.c act.social.c act.wizard.c handler.c \
	db.c interpreter.c utility.c spec_assign.c shop.c limits.c mobact.c \
	fight.c modify.c weather.c spells1.c spells2.c spell_parser.c \
	reception.c constants.c spec_procs.c signals.c board.c magic.c \
	magic2.c skills.c Opinion.c Trap.c magicutils.c multiclass.c hash.c \
	Sound.c Heap.c spec_procs2.c magic3.c security.c spec_procs3.c \
        create.c bsd.c parser.c intrinsics.c

all : dmserver

dmserver : ${OBJS}
## if you want to add debugging stuff, uncomment the following, and comment
##  out the line after that
##	$(LD) -o $@ -g ${OBJS} /local/lib/gnu/gcc-gnulib /usr/lib/debug/malloc.o
	$(LD) -o $@ -g ${OBJS} 
##	$(LD) -o $@  ${OBJS}
##
depend :
	makedepend ${SRCS}



# DO NOT DELETE THIS LINE -- make depend depends on it.

comm.o: protos.h
act.comm.o: protos.h
act.move.o: protos.h
act.off.o: protos.h
act.obj1.o: protos.h
act.obj2.o: protos.h
act.info.o: protos.h
act.other.o: protos.h
act.social.o: protos.h
act.wizard.o: protos.h
handler.o: protos.h
db.o: protos.h
interpreter.o: protos.h
utility.o: protos.h
spec_assign.o: protos.h
shop.o: protos.h
limits.o: protos.h
mobact.o: protos.h
fight.o: protos.h
modify.o: protos.h
weather.o: protos.h
spells1.o: protos.h
spells2.o: protos.h
spell_parser.o: protos.h
reception.o: protos.h
constants.o: protos.h
spec_procs.o: protos.h
signals.o: protos.h
board.o: protos.h
magic.o: protos.h
magic2.o: protos.h
magic3.o: protos.h
magicutils.o: protos.h
multiclass.o: protos.h
skills.o: protos.h
Opinion.o: protos.h
Trap.o: protos.h
hash.o: protos.h
Sound.o: protos.h
Heap.o: protos.h
spec_procs2.o: protos.h
spec_procs3.o: protos.h
security.o:
create.o: protos.h
bsd.o:
parser.o: protos.h

intrinsics.o: protos.h
