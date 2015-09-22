#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class BuilderModule          -------------------------------------------------
//--------------------------------------------------------------------------------------------------

BuilderModule::BuilderModule(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(10);
}

void BuilderModule::paint(WebPage *page, HttpRequest &request) {
	String cmd = request.header.GET.getValue("cmd");
	if (cmd == "ajax")
		return ajax(page, request);

	String p2 = request.header.GET.getValue("p2");
	if (p2 == "") paintMain(page, request);
	else if (p2 == "edit") paintSitesEdit(page, request);
	else if (p2 == "editContent") paintEditContent(page, request);
}

void BuilderModule::paintMain(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	WebTemplate *tpl = new WebTemplate();
	String mainTpl = "tpl.html";

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);
	if (userId == 0) mainTpl = "notAuth_tpl.html";

	String tplPath = manager->modulePath + "/builder/" + mainTpl;
	if (tpl->open(tplPath)) {
		if (userId > 0) {
			String sql = "select * from sites where userId='" + (String)userId + "' and deleted=0";
			int count = query->active(sql);
			for (int i = 0; i < count; i++) {
				int siteId = query->getFieldValue(i, "id").toInt();
				String url = query->getFieldValue(i, "url");
				String name = query->getFieldValue(i, "name");
				String about = query->getFieldValue(i, "about");

				WebTemplate *tplRow = new WebTemplate();
				if (tplRow->open(manager->modulePath + "/builder/row_tpl.html")) {
					tplRow->out("index", i + 1);
					tplRow->out("siteId", siteId);
					tplRow->out("url", url);
					tplRow->out("name", name);
					tplRow->out("about", about);
					tplRow->exec();
					tpl->out("rows", tplRow->html);
				}
			}
		}
		tpl->exec();
		page->out("content", tpl->html);
	}
	manager->deleteQuery(query);
}

void BuilderModule::paintModules(WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String sql = "select * from modules where isWork = 1 order by id";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int moduleId = query->getFieldValue(i, "id").toInt();
		String name = query->getFieldValue(i, "name");

		tpl->out("modules", "<option value='" + (String)moduleId + "'>" + name + "</option>");
	}

	manager->deleteQuery(query);
}

void BuilderModule::paintPages(int siteId, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String sql = "select p.id, p.url, m.name, p.isMainPage, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" + 
		(String)siteId + "' and deleted=0 order by p.id";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int pageId = query->getFieldValue(i, "id").toInt();
		String url = query->getFieldValue(i, "url");
		String moduleName = query->getFieldValue(i, "name");
		int isMainPage = query->getFieldValue(i, "isMainPage").toInt();
		String title = query->getFieldValue(i, "title");
		String description = query->getFieldValue(i, "description");
		String keywords = query->getFieldValue(i, "keywords");

		tpl->out("pages", "<tr id='" + (String)pageId + "'><td>" + (String)(i + 1) + "</td><td>" + url + "</td><td>" + moduleName + "</td><td>" + (String)isMainPage + "</td><td>" +
			title + "</td><td>" + description + "</td><td>" + keywords + "</td></tr>");
	}

	manager->deleteQuery(query);
}

void BuilderModule::paintWidgets(int siteId, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String sql = "select w.id, ws.tag, w.name from widget_site ws, widgets w where ws.widgetId=w.id and siteId='" + (String)siteId + "' order by ws.id";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int widgetId = query->getFieldValue(i, "id").toInt();
		String tag = query->getFieldValue(i, "tag");
		String name = query->getFieldValue(i, "name");

		tpl->out("widgets", "<tr><td>" + (String)(i + 1) + "</td><td>" + tag + "</td><td>" + name + "</td></tr>");
	}

	manager->deleteQuery(query);
}

void BuilderModule::paintSitesEdit(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	WebTemplate *tpl = new WebTemplate();
	String tplName = "edit_tpl.html";
	if (tpl->open(manager->modulePath + "/builder/" + tplName)) {
		paintModules(tpl);
		int siteId = request.header.GET.getValue("p3").toInt();
		paintPages(siteId, tpl);
		paintWidgets(siteId, tpl);

		String sql = "select * from sites where id='" + (String)siteId + "' and deleted=0 order by id";
		int count = query->active(sql);
		if (count > 0) {
			String url = query->getFieldValue(0, "url");
			String urlView = query->getFieldValue(0, "urlView");
			String name = query->getFieldValue(0, "name");
			String about = query->getFieldValue(0, "about");
			String keywords = query->getFieldValue(0, "keywords");
			tpl->out("siteId", siteId);
			tpl->out("url", url);
			tpl->out("urlView", urlView);
			tpl->out("name", name);
			tpl->out("about", about);
			tpl->out("keywords", keywords);

			String p4 = request.header.GET.getValue("p4");
			if (p4 == "") tpl->out("in0", "in");
			else if (p4 == "pages") tpl->out("in1", "in");
			else if (p4 == "widgets") tpl->out("in2", "in");
		}
		tpl->exec();
		page->out("content", tpl->html);
	}

	manager->deleteQuery(query);
}

void BuilderModule::paintEditContent(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	int siteId = request.header.GET.getValue("p3").toInt();
	int pageId = request.header.GET.getValue("p4").toInt();
	WebTemplate *tpl = new WebTemplate();
	String tplName = "editContent_tpl.html";
	if (tpl->open(manager->modulePath + "/builder/" + tplName)) {
		tpl->out("siteId", siteId);
		tpl->out("pageId", pageId);

		String sql = "select t.value from pages p, data d, dataText t where d.pageid=p.id and d.dataId=t.id and p.id='" + (String)pageId + "'";
		int count = query->active(sql);
		if (count > 0) {
			String text = query->getFieldValue(0, "value");
			string text8 = text.to_string();
			tpl->out("text", text);
		}
		tpl->exec();
		page->out("content", tpl->html);
	}
}

void BuilderModule::ajax(WebPage *page, HttpRequest &request) {
	String p2 = request.header.GET.getValue("p2");
	string p2_8 = p2.to_string();

	if (p2 == "createSite") ajaxCreateSite(page, request);
	else if (p2 == "getUrlByIndex") ajaxGetUrlByIndex(page, request);
	else if (p2 == "getSiteIdByIndex") ajaxGetSiteIdByIndex(page, request);
	else if (p2 == "deleteSite") ajaxDeleteSite(page, request);
	else if (p2 == "addPage") ajaxAddPage(page, request);
	else if (p2 == "accept") ajaxAccept(page, request);
	else if (p2 == "editPage") ajaxEditPage(page, request);
	else if (p2 == "deletePage") ajaxDeletePage(page, request);
	else if (p2 == "getPageId") ajaxGetPageId(page, request);
	else if (p2 == "saveContent") ajaxSaveContent(page, request);
}

void BuilderModule::ajaxCreateSite(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	String url = request.header.POST.getValue("url");
	String name = request.header.POST.getValue("name");
	String about = request.header.POST.getValue("about");

	page->tplIndex->out("out", "<note>\n");
	if (userId != 0 && url != "") {
		String sql = "insert into sites (userId, url, name, about) values ('" + (String)userId + "', '" + url + "', '" + name + "', '" + about + "')";
		string s8 = sql.to_string();
		if (query->exec(sql)) {

			int siteId = query->getLastId();
			manager->addSite(siteId, url.to_string());
			
			page->tplIndex->out("out", "<result>1</result>\n");

			String sql = "select count(*) cnt from sites where userId='" + (String)userId + "' and deleted=0";
			int count = query->active(sql);
			if (count > 0) {
				int cnt = query->getFieldValue(0, "cnt").toInt();
				int index = cnt;
				page->tplIndex->out("out", "<index>" + (String)index + "</index>\n");
			}
		}
	}
	page->tplIndex->out("out", "</note>\n");
	manager->deleteQuery(query);
}

void BuilderModule::ajaxGetUrlByIndex(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	int index = request.header.POST.getValue("index").toInt();

	page->tplIndex->out("out", "<note>\n");

	String sql = "select * from sites where userId='" + (String)userId + "' and deleted=0";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int siteId = query->getFieldValue(i, "id").toInt();
		String url = "/builder/edit/" + (String)siteId;
		if (i == index) {
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "<url>" + url + "</url>\n");
			break;
		}
	}

	page->tplIndex->out("out", "</note>\n");

	manager->deleteQuery(query);
}

void BuilderModule::ajaxGetSiteIdByIndex(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	int index = request.header.POST.getValue("index").toInt();

	page->tplIndex->out("out", "<note>\n");

	String sql = "select * from sites where userId='" + (String)userId + "' and deleted=0";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int siteId = query->getFieldValue(i, "id").toInt();
		String url = "/builder/edit/" + (String)siteId;
		if (i == index) {
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "<siteId>" + (String)siteId + "</siteId>\n");
			break;
		}
	}

	page->tplIndex->out("out", "</note>\n");
	manager->deleteQuery(query);
}

void BuilderModule::ajaxDeleteSite(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	int siteId = request.header.POST.getValue("siteId").toInt();

	page->tplIndex->out("out", "<note>\n");
	if (userId > 0) {
		String sql = "update sites set deleted=1 where id='" + (String)siteId + "' and userId='" + (String)userId + "'";
		string sql8 = sql.to_string();
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<result>1</result>\n");
		}
	}
	page->tplIndex->out("out", "</note>\n");
	manager->deleteQuery(query);
}

int BuilderModule::getPageIndex(MySQL *query, int siteId) {
	String sql = "select count(*) cnt from pages where siteId='" + (String)siteId + "' and deleted=0 and moduleId>0";
	string sql8 = sql.to_string();
	int count = query->active(sql);
	if (count > 0) {
		int cnt = query->getFieldValue(0, "cnt").toInt();
		return cnt;
	}
	return 0;
}

bool BuilderModule::createDataText(MySQL *query, int pageId, int moduleId, int userId) {
	String sql = "insert into dataText (value) values ('')";
	if (query->exec(sql)) {
		int dataId = query->getLastId();

		sql = "insert into data (pageId, dataId, moduleId, userId) values('" + (String)pageId + "', '" + (String)dataId + "', '" + 
			(String)moduleId + "', '" + (String)userId + "')";
		if (query->exec(sql)) {
			return true;
		}
	}
	return false;
}


void BuilderModule::ajaxAddPage(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	int siteId = request.header.POST.getValue("siteId").toInt();
	String url = request.header.POST.getValue("url");
	int moduleId = request.header.POST.getValue("moduleId").toInt();
	int isMainPage = request.header.POST.getValue("isMainPage").toInt();
	String title = request.header.POST.getValue("title");
	String description = request.header.POST.getValue("description");
	String keywords = request.header.POST.getValue("keywords");

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	page->tplIndex->out("out", "<note>\n");
	if (siteId > 0) {
		String sql = "insert into pages (siteId, url, isMainPage, moduleId, title, description, keywords) values('"
			+ (String)siteId + "', '" + url + "', '" + (String)isMainPage + "', '" + (String)moduleId + "', '" + title + "', '" + description + "', '" + keywords + "')";
		string sql8 = sql.to_string();
		if (query->exec(sql)) {
			int pageId = query->getLastId();
			manager->addPage(siteId, pageId, url.to_string(), isMainPage, moduleId);

			int index = getPageIndex(query, siteId);
			page->tplIndex->out("out", "<index>" + (String)index + "</index>\n");

			bool flag = true;
			if (moduleId == 1) {
				flag = createDataText(query, pageId, moduleId, userId);
			}
			if (flag) page->tplIndex->out("out", "<result>1</result>\n");

			String sql = "select name from modules where id='" + (String)moduleId + "'";
			string sql8 = sql.to_string();
			int count = query->active(sql);
			if (count > 0) {
				String name = query->getFieldValue(0, "name");
				page->tplIndex->out("out", "<moduleName>" + name + "</moduleName>\n");
			}

		}
	}
	page->tplIndex->out("out", "</note>\n");
	manager->deleteQuery(query);
}
void BuilderModule::ajaxAccept(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	String url = request.header.POST.getValue("url");
	String name = request.header.POST.getValue("name");
	String about = request.header.POST.getValue("about");
	String caption = request.header.POST.getValue("caption");
	int design = request.header.POST.getValue("desing").toInt();
	int theme = request.header.POST.getValue("theme").toInt();
	int layout = request.header.POST.getValue("maket").toInt();

	String sql = "update sites set url='" + url + "', name='" + name + "', about='" + about + "', caption='" + caption + "', design='" + design + 
		"', theme='" + theme + "', layout='" + layout + "' where id='" + (String)siteId + "'";

	if (query->exec(sql)) {
		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "</note>\n");
	}
	manager->deleteQuery(query);
}

void BuilderModule::ajaxEditPage(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	int pageIndex = request.header.POST.getValue("pageIndex").toInt();
	int pageId = 0;

	String url = request.header.POST.getValue("url");
	int moduleId = request.header.POST.getValue("moduleId").toInt();
	int isMainPage = request.header.POST.getValue("isMainPage").toInt();
	String title = request.header.POST.getValue("title");
	String description = request.header.POST.getValue("description");
	String keywords = request.header.POST.getValue("keywords");

	String sql = "select p.id, p.url, m.name, p.isMainPage, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" +
		(String)siteId + "' and deleted=0 order by p.id limit " + (String)pageIndex + ", 1";
	int count = query->active(sql);
	if (count > 0) {
		pageId = query->getFieldValue(0, "id").toInt();
	}

	if (pageId > 0) {
		sql = "update pages set url='" + url + "', moduleId='" + (String)moduleId + "', isMainPage='" + (String)isMainPage +
			"', title='" + title + "', description='" + description + "', keywords='" + keywords + "' where siteId='" + (String)siteId + "' and id='" + (String)pageId + "'";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}
	}
	manager->deleteQuery(query);
}

void BuilderModule::ajaxDeletePage(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	int pageIndex = request.header.POST.getValue("pageIndex").toInt();
	int pageId = 0;

	String sql = "select p.id, p.url, m.name, p.isMainPage, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" +
		(String)siteId + "' and deleted=0 order by p.id limit " + (String)pageIndex + ", 1";
	int count = query->active(sql);
	if (count > 0) {
		pageId = query->getFieldValue(0, "id").toInt();
	}

	if (pageId > 0) {
		sql = "update pages set deleted=1, dtDeleted=now() where siteId='" + (String)siteId + "' and id='" + (String)pageId + "'";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}
	}
	manager->deleteQuery(query);
}

void BuilderModule::ajaxGetPageId(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	int pageIndex = request.header.POST.getValue("pageIndex").toInt();

	String sql = "select p.id, p.url, m.name, p.isMainPage, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" +
		(String)siteId + "' and deleted=0 order by p.id limit " + (String)pageIndex + ", 1";
	int count = query->active(sql);
	if (count > 0) {
		int pageId = query->getFieldValue(0, "id").toInt();

		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "<pageId>" + (String)pageId + "</pageId>\n");
		page->tplIndex->out("out", "</note>\n");
	}
	manager->deleteQuery(query);
}

void BuilderModule::ajaxSaveContent(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	int pageId = request.header.POST.getValue("pageId").toInt();
	String text = request.header.POST.getValue("text");
	text = request.header.htmlEntitiesDecode(text.to_string());

	String sql = "select dataId from data where pageId='" + (String)pageId + "'";
	string sql8 = sql.to_string();
	int count = query->active(sql);
	if (count > 0) {
		int dataId = query->getFieldValue(0, "dataId").toInt();

		sql = "update dataText set value='" + text + "' where id='" + (String)dataId + "'";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}
	}

	manager->deleteQuery(query);
}

}