#include "bgsub.h"

#include <iostream>

using namespace std;
using namespace v8;
using namespace node;

//#define DEBUG

class SubtractWorker : public NanAsyncWorker {
 public:
  SubtractWorker(string path, string outputPath, NanCallback *callback)
    : NanAsyncWorker(callback), path(path), outputPath(outputPath), latency(0.0), status(NO_ERROR){}
  ~SubtractWorker() {}

  void Execute() {
    #ifdef DEBUG 
      std::cout<<"bgsub.cc: calling subtract()"<<std::endl; 
    #endif
    
    status = subtract(path, outputPath, latency);
    #ifdef DEBUG 
      std::cout<<"bgsub.cc: subtract() called, status: "<<status<<std::endl; 
    #endif
  }

  //callback: error, extension, image
  void HandleOKCallback () {
    NanScope();

    #ifdef DEBUG 
      std::cout<<"bgsub.cc: inside HandleOKCallback()"<<std::endl; 
    #endif

    if(status == SUCCESS) {
    	Local<Value> argv[] = {
          NanNull(),
          NanNew(latency)
      };
      #ifdef DEBUG 
        std::cout<<"bgsub.cc: status==SUCCESS, calling callback->Call()"<<std::endl; 
      #endif
      callback->Call(2, argv);
      #ifdef DEBUG 
        std::cout<<"bgsub.cc: callback->Call() successful"<<std::endl; 
      #endif
    } else {
      #ifdef DEBUG 
        std::cout<<"bgsub.cc: status != SUCCESS, sending error callback"<<std::endl; 
      #endif
      Local<Value> argv[] = {
          NanNew(string("ERROR")),
          NanNull()
      };

      callback->Call(2, argv);
      #ifdef DEBUG 
        std::cout<<"bgsub.cc: error callback sent"<<std::endl; 
      #endif
    }
  }

 private:
  string path;
  string outputPath;
  double latency;
  BGSubError status;
};

NAN_METHOD(Subtract) {
  NanScope();
  #ifdef DEBUG 
      std::cout<<"bgsub.cc: in Subtract(), parsing"<<std::endl; 
    #endif
  string path = string(*(NanAsciiString(args[0])));
  string outputPath = string(*(NanAsciiString(args[1])));

  NanCallback *callback = new NanCallback(args[2].As<Function>());

  NanAsyncQueueWorker(new SubtractWorker(path, outputPath, callback));
  #ifdef DEBUG 
    std::cout<<"bgsub.cc: launched new async worker"<<std::endl; 
  #endif
  NanReturnUndefined();
}



