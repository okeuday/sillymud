#-*-Mode:make;coding:utf-8;tab-width:4;c-basic-offset:4-*-
# ex: set ft=make fenc=utf-8 sts=4 ts=4 sw=4 noet:
CC=gcc
CLOUDI_VERSION=1.5.1
CLOUDI_PATH=/usr/local/lib/cloudi-$(CLOUDI_VERSION)
CLOUDI_CFLAGS=-I$(CLOUDI_PATH)/api/c/ -fexceptions
CLOUDI_LDFLAGS=-L$(CLOUDI_PATH)/api/c/ -Wl,-rpath,$(CLOUDI_PATH)/api/c/ -lcloudi
CFLAGS=-g -O0 -I. -D_XOPEN_SOURCE
MUD_CFLAGS=-DIMPL_SECURITY -DNO_RENT -DLEVEL_LOSS -DNEWEXP -DGROUP_NAMES \
           -Wno-deprecated-declarations \
           -Wno-pointer-to-int-cast \
           -Wno-int-to-pointer-cast
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
##  To provide offers for rent of 0 and only charge for time, not items
##  -DNEW_RENT
##
##  Completely disable rent and allow objects to be saved automatically
##  -DNO_RENT

all: dmserver

OBJS = act.comm.o \
       act.info.o \
       act.move.o \
       act.obj1.o \
       act.obj2.o \
       act.off.o \
       act.other.o \
       act.social.o \
       act.wizard.o \
       board.o \
       bsd.o \
       comm.o \
       constants.o \
       create.o \
       db.o \
       fight.o \
       handler.o \
       hash.o \
       Heap.o \
       interpreter.o \
       intrinsics.o \
       limits.o \
       magic.o \
       magic2.o \
       magic3.o \
       magicutils.o \
       mobact.o \
       modify.o \
       multiclass.o \
       Opinion.o \
       parser.o \
       reception.o \
       security.o \
       shop.o \
       signals.o \
       skills.o \
       Sound.o \
       spec_assign.o \
       spec_procs.o \
       spec_procs2.o \
       spec_procs3.o \
       spell_parser.o \
       spells1.o \
       spells2.o \
       Trap.o \
       utility.o \
       weather.o

protos.h: structs.h \
          area.h \
          comm.h \
          db.h \
          handler.h \
          hash.h \
          heap.h \
          interpreter.h \
          limits.h \
          poly.h \
          race.h \
          script.h \
          spells.h \
          trap.h \
          utils.h \
          vt100c.h \
          wizlist.h \
          parser.h

act.comm.c: protos.h
act.info.c: protos.h
act.move.c: protos.h
act.obj1.c: protos.h
act.obj2.c: protos.h
act.off.c: protos.h
act.other.c: protos.h
act.social.c: protos.h
act.wizard.c: protos.h
bart.c: protos.h
board.c: protos.h
comm.c: protos.h
constants.c: structs.h \
             limits.h \
             trap.h \
             race.h
convert.c: protos.h
create.c: protos.h
db.c: protos.h
fight.c: protos.h
fight.old.c: protos.h
handler.c: protos.h
hash.c: protos.h
Heap.c: protos.h
hoard.c: protos.h
interpreter.c: protos.h
intrinsics.c: protos.h
limits.c: protos.h
magic.c: protos.h
magic2.c: protos.h
magic3.c: protos.h
magicutils.c: protos.h
mobact.c: protos.h
modify.c: protos.h
multiclass.c: protos.h
Opinion.c: protos.h
parser.c: protos.h
pcedit.c: protos.h
reception.c: protos.h
rentedit.c: structs.h
security.c: protos.h
shop.c: protos.h
signals.c: protos.h
skills.c: protos.h
Sound.c: protos.h
spec_assign.c: protos.h
spec_procs.c: protos.h
spec_procs2.c: protos.h
spec_procs3.c: protos.h
spell_parser.c: protos.h
spells1.c: protos.h
spells2.c: protos.h
temp.c: protos.h
Trap.c: protos.h
utility.c: protos.h
weather.c: protos.h

comm_cloudi.c: structs.h \
               utils.h \
               vt100c.h \
               comm.h

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) $(MUD_CFLAGS)

clean:
	rm -f dmserver $(OBJS) comm_cloudi.o

dmserver: $(OBJS) comm_cloudi.c
	$(CC) -c -o comm_cloudi.o comm_cloudi.c $(CFLAGS) $(CLOUDI_CFLAGS)
	$(CC) -o dmserver $(OBJS) comm_cloudi.o -lcrypt $(CLOUDI_LDFLAGS)

