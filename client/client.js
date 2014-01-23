var c = HMUD_Config;
// remove VT100 character sequences, telnet echo sequences and \r characters
var removeTextRegex = /\r|\x1B\[([0-9]{1,2};[0-9]{1,2}[rHm]|[0-9]{1,2}[CDJm]|[Kr])|\x1B[MD78]/g;
// escape characters for the HTML display
var escapeTextRegex = /[&<>«»]/g;
var escapeText = {
    "&": "&amp;",
    "<": "&lt;",
    ">": "&gt;",
    "«": "<",
    "»": ">",
};
var HMUD_Client = {

    bridge: undefined,
    refresh: undefined,
    refreshDisable: false,
    refreshDelay: 4000, // milliseconds
    lastLine: undefined,

    connect: function() {
        if (! ("WebSocket" in window)) {
            alert("This browser does not support WebSockets");
            return;
        }
        var destination = "ws://".concat(c.host, ":", c.port, c.path);
        HMUD_UI.handleMessage("connecting", null);
        this.bridge = new WebSocket(destination);
        this.bridge.binaryType = "blob";
        console.log("Connecting to " + destination);
        this.bridge.onopen = function() {
            console.log("Connected");
            HMUD_UI.handleMessage("connected", null);
        };
        this.bridge.onmessage = function (evt) {
            reader = new FileReader();
            reader.onload = function (reader_evt) {
                HMUD_Client.handleMessage(reader_evt.target.result.
                    replace(removeTextRegex, '').
                    replace(escapeTextRegex,
                            function (c) {return escapeText[c];}));
            };
            reader.readAsText(evt.data, "text/plain");
        };
        this.bridge.onclose = function() {
            if (HMUD_Client.refresh != undefined) {
                window.clearTimeout(HMUD_Client.refresh);
                HMUD_Client.refresh = undefined;
            }
            console.log("Disconnected");
            HMUD_UI.handleMessage("disconnected", null);
        };
    },

    disconnect: function() {
        if (this.bridge == undefined) {
            alert("Not connected!");
            return;
        }
        this.bridge.close();
    },

    tryToConnect: function () {
        if (this.bridge != undefined) {
            this.bridge.close();
            this.bridge = undefined;
        }
        this.connect();
    },

    // send a command to the MUD
    command: function (cmd) {
        if (this.refresh != undefined) {
            window.clearTimeout(this.refresh);
            this.refresh = undefined;
        }
        if (this.refreshDisabled) {
            this.refreshDisabled = (cmd.charAt(0) != '@');
        }
        this.bridge.send(new Blob([cmd, "\r\n"]));
    },

    // handle incoming data from the MUD
    handleMessage: function (data) {
        if (this.refresh != undefined) {
            window.clearTimeout(this.refresh);
            this.refresh = undefined;
        }
        var refreshPossible = false;
        var newData = true;
        var i = data.lastIndexOf('\n');
        if (i == -1) {
            if (data == this.lastLine) {
                newData = false;
                refreshPossible = true;
            }
            else {
                var promptCharacter = data.substr(data.length - 6, 4);
                if (promptCharacter == "&gt;") {
                    refreshPossible = true;
                    this.lastLine = data;
                }
                else if (promptCharacter.match(/[:?\]]/) == null) {
                    refreshPossible = true;
                }
                else {
                    this.lastLine = data;
                }
            }
        }
        else {
            var currentLastLine = data.substr(i + 1);
            var promptCharacter = currentLastLine.substr(
                currentLastLine.length - 6, 4);
            if (promptCharacter == "&gt;") {
                refreshPossible = true;
            }
            this.lastLine = currentLastLine;
        }
        if (newData) {
            if (data.charAt(0) != "\n") {
                data = "<br>" + data
            }
            HMUD_UI.handleMessage("receive", data)
        }
        if (! this.refreshDisabled) {
            this.refreshDisabled = (data.match(/Terminate with a @\./) != null);
        }
        if (refreshPossible && ! this.refreshDisabled) {
            this.refresh = window.setTimeout(function() {
                HMUD_Client.bridge.send(new Blob(["\r\n"]));
                HMUD_Client.refresh = undefined;
            }, this.refreshDelay);
        }
    }

};

