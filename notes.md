# Notes

## Writing your own http server

- [stackoverflow thread on the question](https://stackoverflow.com/questions/176409/build-a-simple-http-server-in-c)
  - relevant RFCs: [RFC 2616 (original HTTP/1.1)](http://tools.ietf.org/html/rfc2616), [RFC 7230 (Message Syntax and Routing)](http://tools.ietf.org/html/rfc7230), [RFC 7231 (Semantics and Content)](http://tools.ietf.org/html/rfc7231), [RFC 7232 (Conditional Requests)](http://tools.ietf.org/html/rfc7232), [RFC 7233 (Range Requests)](http://tools.ietf.org/html/rfc7233), [RFC 7234 (Caching)](http://tools.ietf.org/html/rfc7234), [RFC 7235 (Authentication)](http://tools.ietf.org/html/rfc7235)
  - [A Practical Guide to Writing Clients and Servers](https://www.jmarshall.com/easy/http/)
  - [BSD Sockets (Berkeley Sockets)](https://en.wikipedia.org/wiki/Berkeley_sockets#Client-server_example_using_UDP)
  - recommended examples:
    - [tinyhttpd](https://github.com/larryhe/tinyhttpd) ([original sourceforge project](https://sourceforge.net/projects/tinyhttpd/))
    - [nweb](https://github.com/ankushagarwal/nweb)
- [Writing a Web Server from Scratch (in C#)](https://www.codeproject.com/articles/859108/writing-a-web-server-from-scratch)
- [libsocket](https://github.com/dermesser/libsocket) C socket library "implementing TCP, UDP, and Unix sockets"

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