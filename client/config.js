var HMUD_Config = {
/*
 * The host hMUD will try to connect, you can use window.location.hostname to
 * make hMUD automatically connect to the same host it's being accessed by the
 * user. Examples:
 *
 * host: "debomud.org",
 * host: window.location.hostname,
 * host: "127.0.0.1",
 */
    host: "127.0.0.1",
    port: 4001,
    path: "/sillymud_server/game_loop",

/* The maximum length of the command history list */
    maxHistorySize: 100,

/* The minimum length a command should have to be added to the history. */
    historyMinLength: 1,

/*
 * If you scroll up, the scroll will be locked at that position, so that you
 * won't be annoyed by new content being sent. But you might want to
 * automatically scroll down when you type a command. This options does
 * precisely that.
 */
  forceScrollOnCmd: true
};

