#ifndef REST_SERVICE_HPP
#define REST_SERVICE_HPP

/*****************************************************************************
* Facilitates handling REST via libevhtp
* ****************************************************************************/

#include <stdexcept>
#include <string>

#include <evhtp.h>


/*****************************************************************************
* Base class to derive from to create
* REST service that handles requests to a part of hierarchy
*  e.g. localhost:80/site/service1/for/bar or
*  localhost:80/site/another_service/another_foo
* with all HTTP methods known for libevhtp library
*  Child should provide a virtual function per HTTP method (GET, POST, etc)
* ****************************************************************************/
template<typename ARG>
class RestService
{
public:
  RestService(const std::string& path_regex, ARG arg);

  // register the service in libevent (via libevhtp)
  void
  reg(evhtp_t*);  

protected:
  // raises std::invalid_argument exception
  //   if the service does not support specific HTTP method
  // should be overloaded if the behavior is not convenient
  virtual
  void
  not_supported(htp_method);

public:
  // a virtual function to specify in a child per HTTP method  
  virtual
  void
  GET(evhtp_request_t* request);

  virtual
  void 
  HEAD(evhtp_request_t* request);

  virtual
  void
  POST(evhtp_request_t* request);

  virtual
  void
  PUT(evhtp_request_t* request);
  
  virtual
  void
  DELETE(evhtp_request_t* request);
  
  virtual
  void
  MKCOL(evhtp_request_t* request);
  
  virtual
  void
  COPY(evhtp_request_t* request);
  
  virtual
  void
  MOVE(evhtp_request_t* request);
  
  virtual
  void
  OPTIONS(evhtp_request_t* request);
  
  virtual
  void
  PROPFIND(evhtp_request_t* request);
  
  void
  PROPPATCH(evhtp_request_t* request);
  
  virtual
  void
  LOCK(evhtp_request_t* request);
  
  virtual
  void
  UNLOCK(evhtp_request_t* request);
  
  virtual
  void
  TRACE(evhtp_request_t* request);
  
  virtual
  void
  CONNECT(evhtp_request_t* request);
  
  virtual
  void
  PATCH(evhtp_request_t* request);

  virtual
  void
  UNKNOWN(evhtp_request_t* request);

private:
  // internal callback
  void
  call(evhtp_request_t* request);

protected:
  const std::string path_regex_;
  ARG arg_;
};


template<typename ARG>
RestService<ARG>::RestService(const std::string& path_regex, ARG arg)
  : path_regex_(path_regex), arg_(arg)
{
}

template<typename ARG>
void
RestService<ARG>::reg(evhtp_t* evhtp)
{
  evhtp_set_regex_cb(evhtp, path_regex_.c_str(),
    [](evhtp_request_t* request, void* args)
    {
      RestService<ARG>* me = static_cast<RestService<ARG>*>(args);
      me->call(request);
    }
    , this);
}

template<typename ARG>
void
RestService<ARG>::not_supported(htp_method method)
{
  std::__throw_invalid_argument(("HTTP method " +
      std::to_string(method) + " not supported by service"  + path_regex_).c_str());
}

#define CASEMETHOD(method)  { case htp_method_##method: \
      method(request); break; }
  
template<typename ARG>
void
RestService<ARG>::call(evhtp_request_t* request)
{
  htp_method method = evhtp_request_get_method(request);
  switch(method)
  {
    CASEMETHOD(GET);
    CASEMETHOD(HEAD);
    CASEMETHOD(POST);
    CASEMETHOD(PUT);
    CASEMETHOD(DELETE);
    CASEMETHOD(MKCOL);
    CASEMETHOD(COPY);
    CASEMETHOD(MOVE);
    CASEMETHOD(OPTIONS);
    CASEMETHOD(PROPFIND);
    CASEMETHOD(PROPPATCH);
    CASEMETHOD(LOCK);
    CASEMETHOD(UNLOCK);
    CASEMETHOD(TRACE);
    CASEMETHOD(CONNECT);
    CASEMETHOD(PATCH);
    CASEMETHOD(UNKNOWN);
  }
}

#define IMPLMETHOD(method) \
template<typename ARG> \
void \
RestService<ARG>::method(evhtp_request_t*) \
{ \
  not_supported(htp_method_##method); \
}

IMPLMETHOD(GET)
IMPLMETHOD(HEAD)
IMPLMETHOD(POST)
IMPLMETHOD(PUT)
IMPLMETHOD(DELETE)
IMPLMETHOD(MKCOL)
IMPLMETHOD(COPY)
IMPLMETHOD(MOVE)
IMPLMETHOD(OPTIONS)
IMPLMETHOD(PROPFIND)
IMPLMETHOD(PROPPATCH)
IMPLMETHOD(LOCK)
IMPLMETHOD(UNLOCK)
IMPLMETHOD(TRACE)
IMPLMETHOD(CONNECT)
IMPLMETHOD(PATCH)
IMPLMETHOD(UNKNOWN)

#endif
