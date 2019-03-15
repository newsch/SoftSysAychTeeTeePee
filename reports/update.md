# AychTeeTeePee

> _"It's very pronounceable"_

_TL;DR: It looks like a duck and sounds like a duck but on closer inspection there is only a small part of a duck implemented._

_TL;TL;DR:DR: It's pretty similar to HTTP, but not quite the same and definitely worse._

Team Member: Evan Lloyd New-Schmidt

## Project Goals

I want to write an HTTP server in C. The lower bound I'm looking to reach is a server that:
- listens on a port for http requests _(achieved)_
- implements GET requests by serving files from a directory
- returns correct error (and non-error!) codes
- I can use with a standard web browser _(achieved)_

If I have the time, I'd still like to extend it by:
- compiling/hooking into WebAssembly to make a web server inside of a web browser, which includes other complications and a mix of Javascript and C interface questions:
  - uploading files to be served rather than pointing to a directory
  - using websockets and/or WebRTC to create connections between browsers? (My current understanding is that I could use websockets as a stand-in for TCP, but it doesn't allow for client-client/peer-peer connections. As far as I can tell, WebRTC has potential for peer-peer, but is a more complicated protocol that isn't quite as analogous to TCP.)
  - on the client side, loading a webpage within a webpage (iframes?)

## Learning Goals

What I hope to achieve by working on this project:
- learn more about networking protocols _(achieved)_
- learn how sockets work and how to interact with them _(achieved)_
- write C code with multiple(!) original and 3rd-party files/libraries
- learn how to structure a C-ish project reasonably (folder hierarchy, where 3rd-party libs live, Makefiles)
- get experience reading, understanding, and using RFCs _(achieved)_
- learn about ways of using C in other places (WebAssembly, Python FFI/ctypes)

## What's Needed

I'm feeling pretty comfortable with what we've done in C so far. I've done a small amount of work with sockets in Python, but still don't really understand how they relate to the network stack and many other aspects of working with them. I've found some examples of small http servers and recommendations on the web already.

I'm cautiously optimistic that the WebAssembly extension can be implemented. I've been reading up on wasm for a little while now and I'd like some experience working with it. I still need to do a lot more research on the other aspects of that.

## What I've Done

I've spent about 10 hours working on this project so far. I've been working for longer chunks intermittently, which has been nice from a productivity perspective but hasn't left me with a consistent sense of progress.

- I've read up on TCP and sockets. Initially I misunderstood how TCP and UDP were implemented related to sockets (I thought they were something implemented on top of them, rather than supported protocols). So far I've been using the TCP equivalent of sockets, and I don't think I'm going to write my own TCP protocol on top of raw sockets.
- I've read up on HTTP and it's various versions. Supporting aspects of HTTP 1.1 (at least supporting `keep-alive`) seems like a reasonable goal for the project. HTTP 2.0 and up, with UDP-based messaging, has been pretty interesting to read about but I don't think I'll get to implementing that. One takeaway that I got from that
- Tried a couple of examples. `tinyhttpd` was interesting to read through, but is built to be a production server and has some pretty complex configuration options that were beyond me. Reading Python's `http.server` module was useful from a higher-level perspective of the general behavior.
- Written a basic http server that listens on a port, and on request returns a 404 response that is accepted by a browser. [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/) was very helpful for figuring out sockets.
- Read up more on wasm and webrtc. I found [an earlier implementation](https://github.com/PeerServer/peer-server) of HTTP over WebRTC, and I think I have a good idea of how to approach each aspect of the system, if I choose to continue in that direction.

## What's Next

I foresee about another 8 hours of work ahead (excluding the wasm extension), which I'm planning on continuing over break.

The next steps are:
- try different responses (including message-body content) beyond 404
- parse HTTP message startlines - I think this is the minimum necessary to serve files from a directory.
- improve the control flow for accepting, responding, and listening for more requests by factoring out socket business into separate functions
- hook up to file reader to serve files from a directory - this will be a good point to evaluate how effective my API is **(at this point I'll have met my project goals)**
- parsing headers (usable for requests and responses) my plan right now is to implement something similar to the argparse API, where it calls a function repeatedly on each header to change a struct of state
- explore wasm feasibility and get a time estimate
