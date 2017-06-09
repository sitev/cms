#pragma once

#include "cms.h"

namespace cms {

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
	String documentRoot, modulePath, widgetPath, uiPath;
	ConnectionPool pool;

	SiteManager(int port = 80);
	virtual void threadFunction(Socket *socket);
	virtual MySQL* newQuery();
	virtual void deleteQuery(MySQL *query);
	virtual void initModules();
	virtual void initSites();
	virtual WebSite* addSite(int siteId, string url);
	virtual void addPage(WebSite *site, int pageId, string url, int moduleId, String design);
	virtual void addPage(int siteId, int pageId, string url, int moduleId, String design);
	virtual void setMainPage(WebSite *site, int pageId);
	virtual void setMainPage(int siteId, int pageId);

	virtual void setDesignPage(WebSite *site, int pageId, String design);
	virtual void setDesignPage(int siteId, int pageId, String design);

	virtual void paintItem(int siteId, int itemId, WebTemplate *tpl);
	virtual void paintMainMenu(int siteId, WebTemplate *tpl);
	virtual void paintLoginModal(WebTemplate *tpl);
	virtual void paintPage(HttpRequest &request, HttpResponse &response);

	String generateUserPassword();
	String getLogin(String uuid);
	String getEmail(String uuid);
	int getUserId(String uuid);
	String isPasswordCorrect(String password);

};

}
