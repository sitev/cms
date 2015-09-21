#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class SiteManager;

class SiteManagerHandler : public WebServerHandler {
	mutex g_mutex;
public:
	SiteManager *manager;
	SiteManagerHandler(SiteManager *manager);
	virtual void step(HttpRequest &request, HttpResponse &response);
	virtual bool isSiteExist(string host);
	virtual bool isPageExist(string host);
};

class SiteManager : public WebServer {
	mutex g_mutex;
public:
	map <int, WebModule*> modules;
	map <string, WebSite*> sites;
	WidgetManager widgetManager;
	String documentRoot, modulePath, widgetPath;

	SiteManager(int port = 80);
	virtual void threadFunction(Socket *socket);
	virtual MySQL* newQuery();
	virtual void deleteQuery(MySQL *query);
	virtual void initModules();
	virtual void initSites();
	virtual void paintPage(HttpRequest &request, HttpResponse &response);

	String generateUserPassword();
	String getLogin(String uuid);
	int getUserId(String uuid);
	String isPasswordCorrect(String password);

};

}
