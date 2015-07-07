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
	modulePath = documentRoot + "/modules";
	initModules();
	initSites();
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

void SiteManager::initModules() {
	WebModule *wm = new StaticPageModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new NewsModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new BlogModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new GuestbookModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new BoardModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new ArticleModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new QuestionAnswerModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

//	wm = new ForumModule(this);
//	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));
}

void SiteManager::initSites() {
	MySQL *query = new MySQL();
	if (!query->init()) return;
	if (!query->connect("127.0.0.1", "root", "123qwe", "sitev")) return;

	MySQL *queryPages = new MySQL();
	if (!queryPages->init()) return;
	if (!queryPages->connect("127.0.0.1", "root", "123qwe", "sitev")) return;

	String sql = "select * from sites order by id";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			for (int i = 0; i < count; i++) {
				int siteId = query->getFieldValue(i, "id").toInt();
				string url = query->getFieldValue(i, "url").toString8();
				WebSite *ws = new WebSite(this, url);
				sites.insert(std::pair<string, WebSite*>(url, ws));

				sql = "select p.url, p.id, p.moduleId, m.name from pages p, modules m where p.moduleId = m.id and p.siteId='" + (String)siteId + "'";
				if (queryPages->exec(sql)) {
					if (queryPages->storeResult()) {
						int count = queryPages->getRowCount();
						for (int i = 0; i < count; i++) {
							string url = queryPages->getFieldValue(i, "url").toString8();
							int pageId = queryPages->getFieldValue(i, "id").toInt();
							int moduleId = queryPages->getFieldValue(i, "moduleId").toInt();
							WebModule *wm = modules[moduleId];
							WebPage *wp = new WebPage(ws, url, pageId, wm);
							ws->pages.insert(std::pair<string, WebPage*>(url, wp));
						}
					}
				}
			}
		}
	}
}

void SiteManager::paintPage(HttpRequest &request, HttpResponse &response) {
	string host = request.header.getValue("Host").toString8();
	if (host == "127.0.0.1:8080") host = "sitev.ru";
	string page = "";
	int count = request.header.params.getCount();
	if (count > 0) {
		page = request.header.params.getValue_s("p1");
//		page = request.header.params.getName(0).toString8();
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
