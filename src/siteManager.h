#pragma once

#include "cjCms.h"

namespace cj {

class SiteManager;

class SiteManagerHandler : public WebServerHandler {
	mutex g_mutex;
public:
	SiteManager *manager;
	SiteManagerHandler(SiteManager *manager);
	virtual void step(HttpRequest &request, HttpResponse &response);
	virtual bool isPageExist(string host);
};

class SiteManager : public WebServer {
	mutex g_mutex;
public:
	map <string, WebSite*> sites;
	string documentRoot;
	SiteManager(int port = 80);
	virtual void threadFunction(Socket *socket);
	virtual void fillSites();
	virtual void paintPage(HttpRequest &request, HttpResponse &response);
};

}