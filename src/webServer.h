#pragma once

#include "cj.h"

namespace cj {

class RequestHeader : public ParamList {
public:
	ParamList params;
	bool isFileFlag;
	string fileExt;
	RequestHeader();
	virtual bool parse(String request);
	virtual bool parse(Memory request);
private:
	virtual bool parseParams(String sss);
	virtual bool isFile(string s, string &fileExt);
	virtual string decodeCp1251(string s);
	virtual int find(Memory &request, char a);
	virtual int find(Memory &request, string s);
	virtual string substr(Memory &request, int pos, int count);
	virtual char getChar(Memory &request, int pos);
};

class HttpRequest {
public:
	Memory memory;
	RequestHeader header;
	HttpRequest() {}
	virtual ~HttpRequest() {}

	virtual void parse();
};

class HttpResponse {
public:
	Memory memory;
};

class WebServerHandler : public Object {
protected:
	HttpRequest request;
	HttpResponse response;
public:
	WebServerHandler() {}
	virtual void threadStep(Socket *socket);
	virtual void internalStep(HttpRequest &request, HttpResponse &response);
	virtual void step(HttpRequest &request, HttpResponse &response);
};

class WebServer : public Application {
protected:
	WebServerHandler *handler;
public:

	int socketPort;
	ServerSocket *ss;

	WebServer(int port = 80);
	virtual ~WebServer() {}
	virtual void run();
	virtual void setHandler(WebServerHandler *handler);
	virtual void threadStep(Socket *socket);
	virtual void threadFunction(Socket *socket);
};

}
