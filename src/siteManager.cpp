#include "cms.h"

namespace cms {

//--------------------------------------------------------------------------------------------------
//----------          class SiteManagerHandler          --------------------------------------------
//--------------------------------------------------------------------------------------------------
SiteManagerHandler::SiteManagerHandler(SiteManager *manager) : WebServerHandler(manager) {
	this->manager = manager;
}

void SiteManagerHandler::step(HttpRequest &request, HttpResponse &response) {
	g_mutex.lock();
	manager->paintPage(request, response);
	g_mutex.unlock();
}

bool SiteManagerHandler::isSiteExist(string host) {
	WebSite *ws = manager->sites[host];
	if (ws != NULL) return true;
	return false;
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
	widgetPath = documentRoot + "/widgets";
	uiPath = documentRoot + "/ui";
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

	wm = new BuilderModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new WebStudioModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new TaskModule(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new WebSurf(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

	wm = new Shop(this);
	modules.insert(std::pair<int, WebModule*>(wm->moduleId, wm));

}

MySQL* SiteManager::newQuery() {
	return pool.newConnection();
	/*
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
	*/
}

void SiteManager::deleteQuery(MySQL *query) {
	pool.deleteConnection(query);
	//delete query;
}

void SiteManager::initSites() {
	printf("initSites();\n");
	MySQL *query = newQuery();
	MySQL *queryPages = newQuery(); 

	String sql = "select * from sites where deleted=0 order by id";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			printf("count = %d\n", count);
			for (int i = 0; i < count; i++) {
				int siteId = query->getFieldValue(i, "id").toInt();
				string url = query->getFieldValue(i, "url").to_string();
				WebSite *ws = addSite(siteId, url);

				sql = "select p.url, p.isMainPage, p.id, p.moduleId, p.layout, m.name from pages p, modules m where p.deleted=0 and p.moduleId = m.id and p.siteId='" + (String)siteId + "'";
				if (queryPages->exec(sql)) {
					if (queryPages->storeResult()) {
						int count = queryPages->getRowCount();
						for (int i = 0; i < count; i++) {
							string url = queryPages->getFieldValue(i, "url").to_string();
							bool isMainPage = queryPages->getFieldValue(i, "isMainPage").toInt();
							int pageId = queryPages->getFieldValue(i, "id").toInt();
							int moduleId = queryPages->getFieldValue(i, "moduleId").toInt();
							String design = queryPages->getFieldValue(i, "layout");

							addPage(ws, pageId, url, moduleId, design);
							if (isMainPage)	setMainPage(ws, pageId);
						}
					}
				}
			}
		}
	}
	this->deleteQuery(queryPages);
	this->deleteQuery(query);
}

WebSite* SiteManager::addSite(int siteId, string url) {
	WebSite *ws = new WebSite(this, url, siteId);
	sites.insert(std::pair<string, WebSite*>(url, ws));

	return ws;
}

void SiteManager::addPage(WebSite *site, int pageId, string url, int moduleId, String design) {
	WebModule *wm = modules[moduleId];
	WebPage *wp = new WebPage(site, url, pageId, wm, design);
	site->pages.insert(std::pair<string, WebPage*>(url, wp));
}

void SiteManager::addPage(int siteId, int pageId, string url, int moduleId, String design) {
	for (auto it = sites.begin(); it != sites.end(); it++)
	{
		WebSite *site = it->second;
		if (site->siteId == siteId) {
			addPage(site, pageId, url, moduleId, design);
			break;
		}
	}
}

void SiteManager::setMainPage(WebSite *site, int pageId) {
	WebPage *page = site->getPageById(pageId);
	site->mainPage = page;
}

void SiteManager::setMainPage(int siteId, int pageId) {
	for (auto it = sites.begin(); it != sites.end(); it++)
	{
		WebSite *site = it->second;
		if (site->siteId == siteId) {
			WebPage *page = site->getPageById(pageId);
			site->mainPage = page;
			break;
		}
	}
}

void SiteManager::setDesignPage(WebSite *site, int pageId, String design) {
	WebPage *page = site->getPageById(pageId);
	page->design = design;
}

void SiteManager::setDesignPage(int siteId, int pageId, String design) {
	for (auto it = sites.begin(); it != sites.end(); it++)
	{
		WebSite *site = it->second;
		if (site->siteId == siteId) {
			WebPage *page = site->getPageById(pageId);
			page->design = design;
			break;
		}
	}
}

void SiteManager::paintItem(int siteId, int itemId, WebTemplate *tpl) {
	MySQL *query = newQuery();

	//String sql = "select * from menu where isnull(deleted) and parent='" + (String)itemId + "' and siteId='" + (String)siteId + "' order by sorting, id";
	String sql = "select * from menu where isnull(deleted) and parent='" + (String)itemId + "' and siteId='" + (String)siteId + "' order by sorting, id";
	//String sql = "select parent.id, parent.name, parent.url, not isnull(child.name) as havechild from menu as parent left join menu as child on child.parent = parent.id where parent.parent='" + (String)itemId + "' order by parent.sorting, parent.id";
	string sql8 = sql.to_string();
	int count= query->active(sql);

	for (int i = 0; i < count; i++) {
		int itemId = query->getFieldValue(i, "id").toInt();
		String name = query->getFieldValue(i, "name");
		String url = query->getFieldValue(i, "url");
		bool haveChild = query->getFieldValue(i, "havechild").toInt();

		if (haveChild) {
			tpl->out("menu", "<li class='dropdown-submenu'><a href='#' class='dropdown-toggle' data-toggle='dropdown'>" + name + "</a>\r\n");
			tpl->out("menu", "<ul class='dropdown-menu'>\r\n");

			paintItem(siteId, itemId, tpl);

			tpl->out("menu", "</ul>\r\n");
			tpl->out("menu", "</li>\r\n");
		}
		else {
			tpl->out("menu", "<li><a href='/" + url + "'>" + name + "</a></li>\r\n");
		}
	}
	deleteQuery(query);
}

void SiteManager::paintMainMenu(int siteId, WebTemplate *tpl) {
	MySQL *query = newQuery();

	//String sql = "select parent.id, parent.name, parent.url, not isnull(child.name) as havechild from menu as parent left join menu as child on child.parent = parent.id where isnull(parent.level) order by parent.sorting, parent.id";

	String sql = "select * from menu where isnull(deleted) and isnull(parent) and siteId='" + (String)siteId + "' order by sorting, id";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int itemId = query->getFieldValue(i, "id").toInt();
		String name = query->getFieldValue(i, "name");
		String url = query->getFieldValue(i, "url");
		bool haveChild = query->getFieldValue(i, "havechild").toInt();

		if (haveChild) {
			tpl->out("menu", "<li class='dropdown'><a href='#' class='dropdown-toggle' data-toggle='dropdown'>" + name + "</a>\r\n");
			tpl->out("menu", "<ul class='dropdown-menu'>\r\n");

			paintItem(siteId, itemId, tpl);

			tpl->out("menu", "</ul>\r\n");
			tpl->out("menu", "</li>\r\n");
		}
		else {
			tpl->out("menu", "<li><a href='/" + url + "'>" + name + "</a></li>\r\n");
		}
	}
	deleteQuery(query);
}

void SiteManager::paintLoginModal(WebTemplate *tpl) {
	WebTemplate tplLogin;
	if (tplLogin.open(modulePath + "/user/loginModal_tpl.html")) {
		tplLogin.exec();
		tpl->out("body", tplLogin.html);
	}
}

void SiteManager::paintPage(HttpRequest &request, HttpResponse &response) {
	string host = request.header.getValue("Host").to_string();
	printf("host = %s\n", host.c_str());
	if (host == "127.0.0.1:8080") host = LOCALHOST;

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = getUserId(uuid);

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

			int sz = ws->pages.size();
			LOGGER_OUT("SIZE", sz);
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
		int rnd = (int)(::random() * (symbols.getLength() - 1));
		password += symbols.getCharString(rnd);
	}
	return password;
}

String SiteManager::getLogin(String uuid) {
	MySQL *query = newQuery();
	String sql = (String)"SELECT * FROM uuid u1, users u2  WHERE u1.uuid='" + uuid + "' and u1.userId=u2.id";
	String login = "";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count != 0) {
				login = query->getFieldValue(0, "login");
				if (login == "") login = query->getFieldValue(0, "email");
			}
		}
	}
	deleteQuery(query);
	return login;
}

String SiteManager::getEmail(String uuid) {
	MySQL *query = newQuery();
	String sql = (String)"SELECT * FROM uuid u1, users u2  WHERE u1.uuid='" + uuid + "' and u1.userId=u2.id";
	String email = "";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count != 0) {
				email = query->getFieldValue(0, "email");
			}
		}
	}
	deleteQuery(query);
	return email;
}

int SiteManager::getUserId(String uuid) {
	MySQL *query = newQuery();
	String sql = "select * from uuid u1, users u2 where u1.uuid='" + uuid + "' and u1.userId=u2.id";
	int userId = 0;
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count != 0) {
				userId = query->getFieldValue(0, "userId").toInt();
			}
		}
	}
	deleteQuery(query);
	return userId;
}

String SiteManager::isPasswordCorrect(String password) {
	int count = password.getLength();
	if (count < 6) return "< 6 chars";
	return "";
}

}
