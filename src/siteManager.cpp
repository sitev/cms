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
	printf("\nInit all...\n");
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

	wm = new UserModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

}

MySQL* SiteManager::newQuery() {
	printf("newQuery();\n");
	MySQL *query = new MySQL();
	if (!query->init()) {
		printf("initSites: !query->init()\n");
		return NULL;
	}
	if (!query->connect("127.0.0.1", "root", "123qwe", "sitev")) {
		printf("initSites: !query->connect()\n");
		return NULL;
	}
	query->exec("SET NAMES utf8");
	return query;
}

void SiteManager::initSites() {
	printf("initSites();\n");
	MySQL *query = newQuery();
	MySQL *queryPages = newQuery(); 

	String sql = "select * from sites order by id";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			printf("count = %d\n", count);
			for (int i = 0; i < count; i++) {
				int siteId = query->getFieldValue(i, "id").toInt();
				string url = query->getFieldValue(i, "url").toString8();
				WebSite *ws = new WebSite(this, url);
				sites.insert(std::pair<string, WebSite*>(url, ws));

				sql = "select p.url, p.isMainPage, p.id, p.moduleId, m.name from pages p, modules m where p.moduleId = m.id and p.siteId='" + (String)siteId + "'";
				if (queryPages->exec(sql)) {
					if (queryPages->storeResult()) {
						int count = queryPages->getRowCount();
						for (int i = 0; i < count; i++) {
							string url = queryPages->getFieldValue(i, "url").toString8();
							int isMainPage = queryPages->getFieldValue(i, "isMainPage").toInt();
							int pageId = queryPages->getFieldValue(i, "id").toInt();
							int moduleId = queryPages->getFieldValue(i, "moduleId").toInt();
							WebModule *wm = modules[moduleId];
							WebPage *wp = new WebPage(ws, url, pageId, wm);
							if (isMainPage == 1) 
								ws->mainPage = wp;
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
	printf("host = %s\n", host.c_str());
	if (host == "127.0.0.1:8080") host = "sitev.ru";

	string page = "";
	int count = request.header.GET.getCount();
	if (count > 0) {
		page = request.header.GET.getValue_s("p1");
	}
	printf("page = %s\n", page.c_str());

	WebSite *ws = sites[host];
	if (ws != NULL) {
		if (ws->mainPage != NULL) {
			if (page == ws->mainPage->page && count == 1) {
				return;
			}
			if (page == "") {
				page = ws->mainPage->page;
			}

			WebPage *wp = ws->pages[page];
			if (wp != NULL) {
				wp->paint(request, response);
			}
		}
	}
}

String SiteManager::generateUserPassword() {
	int count = 8;
	String symbols = "123456789ABCDEFGHIJKLMNPQRSTUVWXYZ";
	String password = "";
	for (int i = 0; i < count; i++) {
		int rnd = (int)(random() * (symbols.getLength() - 1));
		password += symbols.getCharString(rnd);
	}
	return password;
}

String SiteManager::getLogin(String uuid) {
	MySQL *query = newQuery();
	String sql = (String)"SELECT * FROM uuid u1, users u2  WHERE u1.uuid='" + uuid + "' and u1.userId=u2.id";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count == 0) return "";
			String login = query->getFieldValue(0, "email");
			return login;
		}
	}
	return "";
}

int SiteManager::getUserId(String uuid) {
	MySQL *query = newQuery();
	String sql = "select * from uuid u1, users u2 where u1.uuid='" + uuid + "' and u1.userId=u2.id";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count == 0) return 0;
			int userId = query->getFieldValue(0, "userId").toInt();
			return userId;
		}
	}
	return 0;
}

String SiteManager::isPasswordCorrect(String password) {
	int count = password.getLength();
	if (count < 6) return "����� ������ �� ����� ���� ������ 5-�� ��������";
	return "";
}

}
