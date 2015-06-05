#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class SiteManagerHandler          --------------------------------------------
//--------------------------------------------------------------------------------------------------
SiteManagerHandler::SiteManagerHandler(SiteManager *manager) {
	this->manager = manager;
}

void SiteManagerHandler::step(HttpRequest &request, HttpResponse &response) {
	g_mutex.lock();
	manager->paintPage(request, response);
	g_mutex.unlock();
}

bool SiteManagerHandler::isPageExist(string host) {
	WebSite *ws = manager->sites[host];
	if (ws != NULL) {
		int count = ws->pages.size();
		if (count > 0) return true; else return false;
	}
	return false;
}


//--------------------------------------------------------------------------------------------------
//----------          class SiteManager          ---------------------------------------------------
//--------------------------------------------------------------------------------------------------

SiteManager::SiteManager(int port) : WebServer(port) {
	documentRoot = "/var/www";
	fillSites();
}

void SiteManager::threadFunction(Socket *socket)
{
	g_mutex.lock();
	SiteManagerHandler *handler = new SiteManagerHandler(this);
	g_mutex.unlock();

	handler->threadStep(socket);

	g_mutex.lock();
	delete handler;
	g_mutex.unlock();
}

void SiteManager::fillSites() {
	WebSite *wsSitev = new WebSite(this, "sitev.ru");
	sites.insert(std::pair<string, WebSite*>("sitev.ru", wsSitev));
	WebPage *wpSitevMain = new WebPage(wsSitev, "");
	wsSitev->pages.insert(std::pair<string, WebPage*>("", wpSitevMain));

	WebSite *wsVitartas = new WebSite(this, "vitartas.ru");
	sites.insert(std::pair<string, WebSite*>("vitartas.ru", wsVitartas));
	//WebPage *wpVitartasMain = new WebPage(wsVitartas, "");
	//wsVitartas->pages.insert(std::pair<string, WebPage*>("", wpVitartasMain));
}

void SiteManager::paintPage(HttpRequest &request, HttpResponse &response) {
	string host = request.header.getValue("Host").toString8();
	//host = "sitev.ru";
	string page = "";
	int count = request.header.params.getCount();
	if (count > 0) {
		page = request.header.params.getName(0).toString8();
	}

	WebSite *ws = sites[host];
	if (ws != NULL) {
		WebPage *wp = ws->pages[page];
		if (wp != NULL) {
			wp->paint(request, response);
		}
	}
}



}