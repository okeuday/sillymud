#[SillyMUD 1.1b](http://www.mudbytes.net/file-319) With [CloudI](http://cloudi.org) Integration

## LICENSE

All SillyMUD source code was released under the DikuMud License
(see `silly/doc/license.doc`)

[CloudI](http://cloudi.org) is released under a BSD license

The websocket client interface is based on
[hMUD](https://github.com/blueoctopus/hMUD)
and hMUD is released under a MIT license
(see `client/LICENSE`)

## INSTALLATION

1. Install [CloudI](http://cloudi.org)
   (you can install locally by using the `--prefix` argument on configure)
2. Copy the `cloudi.conf` file
3. Set the `SILLYMUD` environment variable with the repository root directory
   (e.g., ``export SILLYMUD=`pwd``` if you are currently there)

## BUGS

The SillyMUD source code does probably have a few bugs hiding.
Make an issue here if you find any, or use the bug command within SillyMUD

## HISTORY

All Massively Multiplayer Online Role Playing Games (MMORPGs) like
World of Warcraft and EverQuest have developed features whose early origins
can be traced back to older text-based multiplayer online games known as
Multi-User Dungeons [MUD](http://en.wikipedia.org/wiki/MUD)s.  The first MUD
was Roy Trubshaw's Essex MUD (or MUD1) which was originally developed in 1978
using MACRO-10 assembly on a DEC PDP-10, but was converted to BCPL, a
predecessor of the C programming language (and was running until 1987).
MUDs gradually gained popularity during the late 1980s and early 1990s with
various MUD codebases that were written in the C programming language.
Around this time period, MUDs were known as the “Multi-Undergraduate Destroyer”.
The DikuMUD codebase is known as the root of one of the largest trees of
[derived MUD source code](http://en.wikipedia.org/wiki/MUD_trees), being
released in 1990 at DIKU (Datalogisk Institut Københavns Universitet, i.e.,
the University of Copenhagen in Copenhagen, Denmark) and leading to at least
51 unique variants all based on the same DikuMUD source code.

One of the major MUD codebases developed from the DikuMUD source code soon
after its release was the SillyMUD codebase.  I chose the SillyMUD source code
(from 1993) as a codebase to scale with [CloudI](http://cloudi.org) because
I knew the SillyMUD derived MUDs were fun to play in the past.  However,
one of the main problems with all the historical MUDs is that all the C source
code is single-threaded and only able to support about 50 concurrent players.
The original SillyMUD source code limits the number of concurrent connections
to 64 but it has also disabled the “donate” command due to its creation of
long linked-lists with donated items.  The original SillyMUD source code also
required you to rent at the inn to save all the objects you had acquired during
gameplay (a feature which was gradually abandoned in MUDs and derived MMORPGs).

With CloudI integration of SillyMUD it was possible to keep the source code
single-threaded while still providing socket connection scalability.
The SillyMUD codebase was originally written to be single-threaded with usage
of global variables throughout the source code and the goal was to preserve
the legacy source code functionality, while adapting it for modern usage.
The buffering of SillyMUD console (a terminal display, often connected with
Telnet) output was already in-place, but some direct file descriptor usage
did require buffering, so that the console output could become the response
to a CloudI service request.  The SillyMUD source code is easily ran within
CloudI as an external CloudI service after being integrated with the
C CloudI API (both a C++ and C CloudI API exist, but using the C CloudI API
helped to be consistent with the SillyMUD source code).

The CloudI integration of the SillyMUD source code subscribes to 3 main
service name patterns which allow it to handle a connect, a disconnect, and
game play commands.  The 3 main service name patterns come from the
C CloudI API calling [subscribe](http://cloudi.org/api.html#1_subscribe) in
the integration source code, so that either WebSocket connections or
Telnet connections have service name destinations for sending service requests
when connections are established.  The WebSocket support in CloudI is provided
by the internal CloudI service `cloudi_service_http_cowboy` and the Telnet
support is provided by the internal CloudI service `cloudi_service_tcp`.
Internal CloudI services are written in the Erlang programming language and
are able to utilize the extreme scalability possible within the Erlang
programming language while using the CloudI service abstraction that provides
the [CloudI API functions](http://cloudi.org/api.html#Service).

Taking the low-level socket handling out of the SillyMUD game event loop
helps to take the main scalability problem out of the original source code
(older MUD source code is generally unable to do domain name lookups due to
the latency involved, despite the fact it is desirable for banning malicious
users).  By avoiding the socket handling in the SillyMUD source code, less
processing occurs on socket errors or situations like link-death, where users
are disconnected, possibly temporarily.  The main scalability problem remaining
is the filesystem usage for various data files as a database for both static
and dynamic gameplay elements in the MUD (so players and their progress, along
with the world zones, objects and monsters (often referred to as a mob or
mobile)).  To stay consistent with the historical SillyMUD codebase, the
filesystem usage within the SillyMUD source code was not changed.  However,
changing the filesystem usage to instead rely on a CloudI service for a
database would be the next step to make the SillyMUD more fault-tolerant.
If a database was used instead of the filesystem, multiple SillyMUD external
CloudI service processes could be used concurrently as separate gaming shards
which would keep users isolated from runtime errors within the source code
(actively developed games generally have runtime errors that cause downtime
and annoy the users).

With the current SillyMUD CloudI service integration, the isolation of socket
errors and latency from the SillyMUD source code does provide a degree of
fault-tolerance.  With the connection scalability provided by the internal
CloudI services `cloudi_service_http_cowboy` and `cloudi_service_tcp` the
limitation on SillyMUD concurrent users can easily go from 64 users
historically to 16384 users when ran within CloudI (without latency problems
that would impact gameplay on a modern machine).  So, with simple CloudI
integration of legacy source code, the number of connections can scale
by 3 orders of magnitude while providing fault-tolerance for the connections
and increasing the efficiency of the game-play.
