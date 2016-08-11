#Background Subtraction
This repo contains REST API with a single (POST) endpoint ```js /cut``` which takes in the number of threads to spawn and processes all images in a directory on the server and writes the results (4-channel PNG images) back into another directory. The endpoint returns with a JSON (if it executed successfully) with the following attributes:

```js
{
	"average_processing_time_ms": 1543,
	"output_dir": "/home/ubuntu/Workspace/cut­api/out",
	"total_processed": 100,
	"total_processing_time_ms": 154300
}
```

##My Stack 
I've used NodeJS with ExpressJS for the REST API with a single POST endpoint. The background subtraction module itself is all written in C++ and OpenCV 3.0.0 and this module connects with the REST API through a native NodeJS module written with nan (native bindings for node). There's no data transfer b/w NodeJS API and the Node module except for the query parameter 'num_threads'.

###Performance & Optimal threads
I tested it on my machine with i7 5500U (low power version) which is a dual-core processor with upto 4 threads. I tested the API on a directory with 10 images of varying sizes and I got optimal performance when num_threads=4. 

One key thing to note here is that even though NodeJS is single-threaded async loop but I spawned multiple threads with the 'async' module and the CPU usage confirms that since processing a single image almost takes up all compute resources for a single thread and 'top' showed ~400% CPU usage (basically 100% across max 4 threads) when processing 4 images in parallel.

###Background subtraction
At first I considered using OpenCV's BackgroundSubtractor which I had previously used to extract any movement from a static camera with stable background, however, this approach builds the background model based on consecutive images and not a single static image. The second obvious approach was to somehow build a model of the background from a single image and then extract a mask for the foreground based on that. Graphcut is probably the most popular algorithm to achieve this. It can be initialized with manual input (marking regions of background and foreground) which is more accurate in building the background model, however, since the pipeline did not allow for manual marking, the other option of initializing with a rectangle (which in this case is basically the whole image minus 10 pixels on each side) and then iteratively building the background model.

Once the foreground mask was extracted, the boundaries/curves were smoothened by dilation. You will find more in the in-line comments in code.

###Files/Folders
1.    **routes:** This contains the REST API request handler
2.    **routes/cut-handler.js:** All requests to ```js /cut``` get routed to this handler which then extracts query params and calls the native node module's ```c++ subtract()``` function and returns with the final JSON result sent back in the response.
3.    **package.json:** Package definition alongwith node module dependencies list.
4.    **server.js:** This is where ExpressJS app (REST server) is initialized with port definition and endpoint (routes) declaration
5.    **node-bgsub:** Native node module which does the main work
6.    **node-bgsub/src: ** All the headers and source files
7.    **node-bgsub/src/init.cc:** Defines the entry point function ```c++ Subtract()```
8.    **node-bgsub/src/internal.h/cc:** Defines the error struct
9.    **node-bgsub/src/bgsub.h/cc:** Defines the Async worker class which invokes a new async worker (NodeJS terminology) for each call to ```js subtract()``` from within the API
10.    **node-bgsub/src/subtract.h/cc:** The actual code which does background subtraction. It takes in input image path and writes the 4-channel PNG output image into the output path specified.
11.    **node-bgsub/bgsub.pc:** Package configuration specifying OpenCV lib and include paths
12.    **node-bgsub/binding.gyp:** Defines build including source files to compile and pkg-config to use with it
13.    **node-bgsub/package.json:** Package declaration including NodeJS dependencies which in our case is just *nan*

##How to Build & Test
Test environment I used is Ubuntu 14.04 LTS with OpenCV 3.0.0 and NodeJS v0.12.7 installed. However, any OpenCV version 2.4.8+ would do.

###Installation
You will need to build and install the node module first and then install API dependencies and then start the server
```sh
cd node-bgsub 
npm install
cd ..
npm install
node server.js
```

###Test
Once the server has started, you can test the ```js /cut``` endpoint either with browser based API test tool e.g. Postman for Chrome or through good ol' ```sh curl```.

```sh
curl ­v ­XPOST ­H 'Content­Type: application/json' ­d '{ "num_threads": 8 }' http://localhost:4000/cut
```



