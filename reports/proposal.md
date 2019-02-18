# AychTeeTeePee

> _"It's very pronounceable"_

_TL;DR: It looks like a duck and sounds like a duck but on closer inspection there is only a small part of a duck implemented._

_TL;TL;DR:DR: It's pretty similar to HTTP, but not quite the same and definitely worse._

Team Member: Evan Lloyd New-Schmidt

## Project Goal
I want to write an HTTP server in C. The lower bound I'm looking to reach is a server that:
- listens on a port for http requests
- implements GET requests by serving files from a directory
- returns correct error (and non-error!) codes
- is built with a library that implements TCP sockets
- I can use with a standard web browser

My expectation is that this won't require the entire project time to reach, and some possible extensions that I have in mind are:
- writing my own TCP layer in C
- writing a basic Markdown parser to serve markdown files as html
- compiling/hooking into WebAssembly to make a web server inside of a web browser, which includes other complications and a mix of Javascript and C interface questions:
  - uploading files to be served rather than pointing to a directory
  - using websockets and/or WebRTC to create connections between browsers? (My current understanding is that I could use websockets as a stand-in for TCP, but it doesn't allow for client-client/peer-peer connections. As far as I can tell, WebRTC has potential for peer-peer, but is a more complicated protocol that isn't quite as analogous to TCP.)
  - on the client side, loading a webpage within a webpage (iframes?)

## Learning Goals

What I hope to achieve by working on this project:
- learn more about networking protocols
- learn how sockets work and how to interact with them
- write C code with multiple(!) original and 3rd-party files/libraries
- learn how to structure a C-ish project reasonably (folder hierarchy, where 3rd-party libs live, Makefiles)
- learn about ways of using C in other places (WebAssembly, Python FFI/ctypes)
- get experience reading, understanding, and using RFCs

## What's Needed

I'm feeling pretty comfortable with what we've done in C so far. I've done a small amount of work with sockets in Python, but still don't really understand how they relate to the network stack and many other aspects of working with them. I've found some examples of small http servers and recommendations on the web already.

I'm cautiously optimistic that the WebAssembly extension can be implemented. I've been reading up on wasm for a little while now and I'd like some experience working with it. I still need to do a lot more research on the other aspects of that.

## (Immediate) Tasks

_Since it's just me, I'll be responsible for all of them._

- Read up on tcp and http, focusing on what is necessary to the specs vs added features (like caching in http, for instance). Find/run small http server examples in C.  
  Deliverable(s): larger game plan for creating the server and a working install of an example server.
- Refresh on the network layers and which ones I'll be implementing/working with.  
  Deliverable(s): a quick writeup w/ diagrams
- Make diagrams of how the protocols work.  
  Deliverable(s): the diagrams
- Write an even simpler http server using a tcp library that only returns a single response and test it with a browser.  
  Deliverable(s): a working version of the thing
- Continue researching wasm/websockets/webrtc.  
  Deliverable(s): write an overview of the pieces involved in the problem