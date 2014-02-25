#[SillyMUD 1.1b](http://www.mudbytes.net/file-319) With [CloudI](http://cloudi.org) Integration

[Here is the case study](http://www.toptal.com/erlang/modernizing-legacy-software-an-example-using-erlang-and-cloudi) for the source code in this repository.

## LICENSE

All SillyMUD source code was released under the DikuMud License
(see `silly/doc/license.doc`)

[CloudI](http://cloudi.org) is released under a BSD license

The websocket client interface is based on
[hMUD](https://github.com/blueoctopus/hMUD)
and hMUD is released under a MIT license
(see `client/LICENSE`)

## INSTALLATION

Here is a simple way to use the repository directory for running SillyMUD

1. Install [CloudI](http://cloudi.org)
   (you can install locally by using the `--prefix` argument on configure)
2. Copy the `cloudi.conf` file to your CloudI installation's etc directory
3. Set the `SILLYMUD` environment variable with the repository root directory
   (e.g., ``export SILLYMUD=`pwd``` if you are currently there)
4. Compile SillyMUD (cd silly/src; make)
5. Start CloudI
6. Connect to port 4000 for Telnet or 4001 for WebSockets
   (browse to http://localhost:4001/sillymud/client.html)

## BUGS

The SillyMUD source code does probably have a few bugs hiding.
Make an issue here if you find any, or use the bug command within SillyMUD
if you are using an instance of this repository that someone has hosted.

