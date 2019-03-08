# Notes


## Tools
- `nc`/`netcat` `domain` `port`: open a connection to the server
  - `-l` listen
  - write your own HTTP request: just enter `GET /`
- browser devtools `network` tab: view/edit/resend requests
- `python3 -m http.server PORT` a local server similar to what I'm trying to make
  - `-b ADDRESS` address to bind to
  - `-d DIRECTORY` directory to serve from

## Asked Questions

Q: I can write spaces in urls now in Chrome and Firefox. Are spaces accepted in HTTP?  
A: No, the browsers escape the urls before sending them.

Q: How do I know when an HTTP request/response on the socket is finished? As far as I can tell the `Content-Length` header or closing the connection are the only answers.  
A: https://tools.ietf.org/html/rfc7230#section-3.3.3 ([the 1.0 spec](https://www.w3.org/Protocols/HTTP/1.0/spec.html#BodyLength) also puts it a little more plainly):
  > When an Entity-Body is included with a message, the length of that body may be determined in one of two ways. If a Content-Length header field is present, its value in bytes represents the length of the Entity-Body. Otherwise, the body length is determined by the closing of the connection by the server.  
  > Closing the connection cannot be used to indicate the end of a request body, since it leaves no possibility for the server to send back a response. Therefore, HTTP/1.0 requests containing an entity body must include a valid Content-Length header field. If a request contains an entity body and Content-Length is not specified, and the server does not recognize or cannot calculate the length from other fields, then the server should send a 400 (bad request) response. 

## Writing your own http server

- [stackoverflow thread on the question](https://stackoverflow.com/questions/176409/build-a-simple-http-server-in-c)
  - relevant RFCs: [RFC 2616 (original HTTP/1.1), now outdated](http://tools.ietf.org/html/rfc2616), [RFC 7230 (Message Syntax and Routing)](http://tools.ietf.org/html/rfc7230), [RFC 7231 (Semantics and Content)](http://tools.ietf.org/html/rfc7231), [RFC 7232 (Conditional Requests)](http://tools.ietf.org/html/rfc7232), [RFC 7233 (Range Requests)](http://tools.ietf.org/html/rfc7233), [RFC 7234 (Caching)](http://tools.ietf.org/html/rfc7234), [RFC 7235 (Authentication)](http://tools.ietf.org/html/rfc7235)
  - [A Practical Guide to Writing Clients and Servers](https://www.jmarshall.com/easy/http/)
  - [BSD Sockets (Berkeley Sockets)](https://en.wikipedia.org/wiki/Berkeley_sockets#Client-server_example_using_UDP)
  - recommended examples:
    - [tinyhttpd](https://github.com/larryhe/tinyhttpd) ([original sourceforge project](https://sourceforge.net/projects/tinyhttpd/)) see [tinyhttpd](#tinyhttpd)
    - [nweb](https://github.com/ankushagarwal/nweb)
- [Concurrent Servers in C](https://eli.thegreenplace.net/2017/concurrent-servers-part-1-introduction/)
- [Writing a Web Server from Scratch (in C#)](https://www.codeproject.com/articles/859108/writing-a-web-server-from-scratch)
- [HTTP server in Java](https://javarevisited.blogspot.com/2015/06/how-to-create-http-server-in-java-serversocket-example.html)
- [libsocket](https://github.com/dermesser/libsocket) C socket library "implementing TCP, UDP, and Unix sockets"
- Earlier SoftSys projects:
  - [ServerClient](https://github.com/shrutiyer/SoftSysServerClient) chatroom (Project 2)
  - [HTTP Server in C](https://github.com/matthewruehle/softsysquestingquail)
    - [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [Wikipedia page on HTTP protocol](https://en.wikipedia.org/wiki/Hypertext_Transfer_Protocol)
- https://www.w3.org/Protocols/
- [MDN page on HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)

### tinyhttpd

- website: http://www.acme.com/software/thttpd/
- most of the protocol work seems to be in `libhttpd`
- from these [notes on non-blocking I/o](http://www.acme.com/software/thttpd/notes.html#nbio), a reasonable way to prioritize for this project seems to be:
  1. start w/ a single-threaded server
  2. implement forking for each response
  3. implement pre-forking or single-process non-blocking  
  We'll see where I get, right now I think (3) is out-of-scope for what I want to do with this project.
- syslog logging
- setting a directory with `chdir` function, pretty much just `cd`

### HTTP standard

- https://tools.ietf.org/html/rfc2616#section-2.2
  - [RFC on how to read common RFC notation](https://tools.ietf.org/html/rfc822#section-2)
  - line endings are CRLF
  - header fields can wrap lines w/ leading space or tab characters
  - > An application that sends a request or response message that includes
    > HTTP-Version of "HTTP/1.1" MUST be at least conditionally compliant
    > with this specification. Applications that are at least conditionally
    > compliant with this specification SHOULD use an HTTP-Version of
    > "HTTP/1.1" in their messages, and MUST do so for any message that is
    > not compatible with HTTP/1.0. For more details on when to send
    > specific HTTP-Version values, see RFC 2145 [36].
  - [http url spec](https://tools.ietf.org/html/rfc2616#section-3.2.2) is pretty close to a regex
    - [comparing URIs](https://tools.ietf.org/html/rfc2616#section-3.2.3)
  - [content type spec](https://tools.ietf.org/html/rfc2616#section-3.7)
  - [message spec](https://tools.ietf.org/html/rfc2616#section-4)
    - ```
      generic-message = start-line
                  *(message-header CRLF)
                  CRLF
                  [ message-body ]
      start-line      = Request-Line | Status-Line
      ```
  - persistent connections
- HTTP/2 sends data in frames/streams
- HTTP/2 allows you to send more data than requested

### General C stuff
- [argp and getopt for cmdline interfaces](https://stackoverflow.com/questions/9642732/parsing-command-line-arguments)
- [thoughts on `make install`](https://superuser.com/questions/360178/what-does-make-install-do)
  - [GNU Autotools](http://en.wikipedia.org/wiki/Autotools)

## Networking

- [OSI model](https://en.wikipedia.org/wiki/OSI_model)
- [TCP vs UDP](https://www.diffen.com/difference/TCP_vs_UDP)

## Server w/in a browser

### General wasm
- https://webassembly.org/
- [MDN page on WebAssembly](https://developer.mozilla.org/en-US/docs/WebAssembly)
- [reading on how C runs as wasm, and tips for writing compatible code](https://emscripten.org/docs/porting/emscripten-runtime-environment.html#emscripten-runtime-environment)

### Sockets in the browser

There doesn't seem to be much out there regarding sockets with wasm.
- [this website](https://floooh.github.io/2017/06/09/webassembly-demystified.html#can-i-access-sockets-files-native-api-x-from-webassembly) claims that emscripten wraps websockets as sockets
- [broken (but still high-ranking in Google) stackoverflow link to a related question](https://stackoverflow.com/questions/44637000/how-to-call-from-webassembly-websocket-api)
- [this 2017 reply to a question about UDP and wasm claims that wasm has no communication APIs](https://stackoverflow.com/questions/46873268/udp-socket-via-javascript-in-a-browser)
- [this person seems to have gotten it working](https://stackoverflow.com/questions/44819300/udp-socket-at-webassembly/44846128#44846128)
- the emscripten docs have a networking page that [shows up in search](https://emscripten.org/search.html?q=network&check_keywords=yes&area=default) but returns a 404


#### websockets

- [An Introduction to WebSockets](https://blog.teamtreehouse.com/an-introduction-to-websockets)
  - [echo websockets demo](https://codepen.io/matt-west/full/tHlBb)
- [debugging websockets with Chrome/Chromium](https://stackoverflow.com/questions/8952773/chrome-web-inspector-web-socket-debugging)
  - [similar but old and incompatible Firefox extension](https://github.com/firebug/websocket-monitor)

#### WebRTC

- https://webrtc.org/start/
- [sending text demo](https://webrtc.github.io/samples/src/content/datachannel/basic/)
- [generating/sending data](https://webrtc.github.io/samples/src/content/datachannel/datatransfer/)
- [debugging WebRTC](https://testrtc.com/webrtc-api-trace/)
- [Getting Started with WebRTC](https://www.html5rocks.com/en/tutorials/webrtc/basics/)
- [WebRTC in the real world: STUN, TURN and signaling](https://www.html5rocks.com/en/tutorials/webrtc/infrastructure/#what-is-signaling)
- [another WebRTC intro w/ Sockets.io](https://blog.jscrambler.com/mixed-signals-with-socket-io-and-webrtc/)
- [WebRTC without a signaling server](https://blog.printf.net/articles/2013/05/17/webrtc-without-a-signaling-server/)
  - [Serverless WebRTC using QR codes](https://franklinta.com/2014/10/19/serverless-webrtc-using-qr-codes/)
- Can I do WebRTC if I give it the IP of the other computer? [RTCPeerConnection on MDN](https://developer.mozilla.org/en-US/docs/Web/API/RTCPeerConnection/RTCPeerConnection) wants ICE stuff.
  - [webRTC on isolated LAN - Without ice/stun/turn server](https://stackoverflow.com/questions/30742431/webrtc-on-isolated-lan-without-ice-stun-turn-server)

### Client w/in a browser

#### Browser w/in a browser

General consensus seems to be to use an iframe for local content

- [stack overflow question](https://stackoverflow.com/questions/4199458/specifying-content-of-an-iframe-instead-of-the-src-to-a-page)
  - [iframes have a srcdoc property](https://caniuse.com/#feat=iframe-srcdoc)
- [loading html inside a div](https://stackoverflow.com/questions/3564356/loading-html-page-inside-div)
- [Someone who's done this already and used "blobs" w/ an iframe](https://dev.to/pulljosh/how-to-load-html-css-and-js-code-into-an-iframe-2blc#solution-blob-urls)
  - [Blob documentation](https://developer.mozilla.org/en-US/docs/Web/API/Blob)
- [going backwards/forwards within an iframe](https://stackoverflow.com/questions/3254985/back-and-forward-buttons-in-an-iframe#3255004)
- [Fullscreen API](https://developer.mozilla.org/en-US/docs/Web/API/Fullscreen_API#Browser_compatibility)

#### Receiving a request w/in a browser

- [Fetch API](https://developer.mozilla.org/en-US/docs/Web/API/Fetch_API/Using_Fetch)
- you can construct your own [`Response` objects](https://developer.mozilla.org/en-US/docs/Web/API/Response/Response) but it doesn't offer parsing of a raw response. I'll need to do that anyways for the C version with a request, so it _shouldn't_ be too hard to extend that to a response parser (they share a pretty similar format?) and use that in wasm

#### Files in the browser

How do I serve a directory of files from within the browser?

- emscripten provides [a filesystem API](https://emscripten.org/docs/api_reference/Filesystem-API.html) that C can interface with

##### Uploading files/folders

- ["It's now possible [to upload folders]"](https://stackoverflow.com/questions/3590058/does-html5-allow-drag-drop-upload-of-folders-or-a-folder-tree#11410455)
  - [2015 Firefox example](https://stackoverflow.com/questions/23423163/html5-drag-and-drop-folder-detection-in-firefox-is-it-even-possible/33431704#33431704)
- [the non-standard "File and Directories Entries API"](https://developer.mozilla.org/en-US/docs/Web/API/File_and_Directory_Entries_API)
- [HTML Drag and Drop API](https://developer.mozilla.org/en-US/docs/Web/API/HTML_Drag_and_Drop_API)

Seems like a good course of action would be to implement individual file uploads and then the funky folder uploads.

## Misc

- [does the name already exist?](https://www.google.com/search?safe=off&q=%22aychteeteepee%22)
  - someone used to have aychteeteepee.com
  - other people have used it to be funny with links
- [IP over Avian Carriers](https://en.wikipedia.org/wiki/IP_over_Avian_Carriers)
- Update: There are no original ideas, two people already did HTTP over WebRTC for a senior final project at Stanford in 2013, complete with link redirecting and an in-browser editor. The domain name is now owned by a VPN something-or-other, but the code is available: [PeerServer: A Server in the Browser with WebRTC](https://github.com/PeerServer/peer-server).
