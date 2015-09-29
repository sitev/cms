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

void BuilderModule::paintParams(int siteId, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String sql = "select * from sites where id='" + (String)siteId + "' and deleted=0 order by id";
	int count = query->active(sql);
	if (count > 0) {

		String tplPath = manager->modulePath + "/builder/editParams_tpl.html";
		WebTemplate *tplParams = new WebTemplate();
		if (tplParams->open(tplPath)) {
			String url = query->getFieldValue(0, "url");
			String name = query->getFieldValue(0, "name");
			String about = query->getFieldValue(0, "about");
			String keywords = query->getFieldValue(0, "keywords");
			String caption = query->getFieldValue(0, "caption");
			tplParams->out("url", url);
			tplParams->out("name", name);
			tplParams->out("about", about);
			tplParams->out("keywords", keywords);
			tplParams->out("caption", caption);
			tplParams->exec();

			tpl->out("out", tplParams->html);
		}
	}

	tpl->out("siteId", siteId);
	tpl->out("activeParams", "class='active'");
	manager->deleteQuery(query);
}

void BuilderModule::paintDesign(int siteId, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String tplPath = manager->modulePath + "/builder/editDesign_tpl.html";
	WebTemplate *tplSub = new WebTemplate();
	if (tplSub->open(tplPath)) {

		String sql = "select theme, layout from sites where id='" + (String)siteId + "' and deleted=0 order by id";
		int count = query->active(sql);
		if (count > 0) {
			int theme = query->getFieldValue(0, "theme").toInt();
			int layout = query->getFieldValue(0, "layout").toInt();
			tplSub->out("themeChecked" + (String)theme, "checked");
			tplSub->out("layoutChecked" + (String)layout, "checked");
		}

		tplSub->exec();
		tpl->out("out", tplSub->html);
	}

	tpl->out("siteId", siteId);
	tpl->out("activeDesign", "class='active'");
	manager->deleteQuery(query);
}

void BuilderModule::paintPages(int siteId, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String tplPath = manager->modulePath + "/builder/editPages_tpl.html";
	WebTemplate *tplSub = new WebTemplate();
	if (tplSub->open(tplPath)) {
		paintModules(tplSub);
		String sql = "select p.id, p.url, p.isMainPage, m.name, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" +
			(String)siteId + "' and deleted=0 order by p.sorting, p.id";
		int count = query->active(sql);
		for (int i = 0; i < count; i++) {
			int pageId = query->getFieldValue(i, "id").toInt();
			String url = query->getFieldValue(i, "url");
			int iMainPage = query->getFieldValue(i, "isMainPage").toInt();
			bool isMainPage = iMainPage;
			String moduleName = query->getFieldValue(i, "name");
			String title = query->getFieldValue(i, "title");
			String description = query->getFieldValue(i, "description");
			String keywords = query->getFieldValue(i, "keywords");

			//if (isMainPage) url = "!" + url;

			tplSub->out("pages", "<tr id='" + (String)pageId + "'><td>" + (String)(i + 1) + "</td><td>" + url + "</td><td>" + moduleName + "</td><td>" +
				title + "</td><td>" + description + "</td><td>" + keywords + "</td></tr>");
		}

		tplSub->exec();
		tpl->out("out", tplSub->html);
	}


	tpl->out("siteId", siteId);
	tpl->out("activePages", "class='active'");
	manager->deleteQuery(query);
}

void BuilderModule::paintMenu(int siteId, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String tplPath = manager->modulePath + "/builder/editMenu_tpl.html";
	WebTemplate *tplSub = new WebTemplate();
	if (tplSub->open(tplPath)) {
		paintModules(tplSub);
		String sql = "select * from menu where siteId='" + (String)siteId + "' order by sorting, id";
		int count = query->active(sql);
		for (int i = 0; i < count; i++) {
			String name = query->getFieldValue(i, "name");
			String url = query->getFieldValue(i, "url");

			tplSub->out("items", "<tr><td>" + name + "</td><td>" + url + "</td></tr>");
		}

		tplSub->exec();
		tpl->out("out", tplSub->html);
	}

	tpl->out("siteId", siteId);
	tpl->out("activeMenu", "class='active'");
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

	tpl->out("siteId", siteId);
	tpl->out("activeWidgets", "class='active'");
	manager->deleteQuery(query);
}

void BuilderModule::paintSitesEdit(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	WebTemplate *tpl = new WebTemplate();
	String tplName = "edit_tpl.html";
	if (tpl->open(manager->modulePath + "/builder/" + tplName)) {
		int siteId = request.header.GET.getValue("p3").toInt();
		String tabs = request.header.GET.getValue("p4");

		String sql = "select url from sites where id='" + (String)siteId + "'";
		if (query->active(sql) > 0) {
			String siteUrl = query->getFieldValue(0, "url");
			tpl->out("url", siteUrl);
		}

		if (tabs == "") paintParams(siteId, tpl);
		else if (tabs == "design") paintDesign(siteId, tpl);
		else if (tabs == "pages") paintPages(siteId, tpl);
		else if (tabs == "menu") paintMenu(siteId, tpl);
		else if (tabs == "widgets") paintWidgets(siteId, tpl);

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
	else if (p2 == "getModuleUrl") ajaxGetModuleUrl(page, request);
	else if (p2 == "accept") ajaxAccept(page, request);
	else if (p2 == "acceptDesign") ajaxAcceptDesign(page, request);
	else if (p2 == "editPage") ajaxEditPage(page, request);
	else if (p2 == "deletePage") ajaxDeletePage(page, request);
	else if (p2 == "getPageId") ajaxGetPageId(page, request);
	else if (p2 == "saveContent") ajaxSaveContent(page, request);
	else if (p2 == "moveTableRow") ajaxMoveTableRow(page, request);
	else if (p2 == "addMenuItem") ajaxAddMenuItem(page, request);
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

int BuilderModule::setMainPage(MySQL *query, int siteId) {
	String sql = "select p.id, p.url, m.name, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" +
		(String)siteId + "' and deleted=0 order by p.sorting, p.id";
	int count = query->active(sql);
	if (count > 0) {
		int pageId = query->getFieldValue(0, "id").toInt();

		sql = "update pages set isMainPage=0 where siteId='" + (String)siteId + "'";
		query->exec(sql);

		sql = "update pages set isMainPage=1 where siteId='" + (String)siteId + "' and id='" + (String)pageId + "'";
		query->exec(sql);

		return pageId;
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
	String title = request.header.POST.getValue("title");
	String description = request.header.POST.getValue("description");
	String keywords = request.header.POST.getValue("keywords");

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	page->tplIndex->out("out", "<note>\n");
	if (siteId > 0) {
		String sql = "select max(p.sorting) ms from pages p, modules m where p.moduleId=m.id and siteId='" +
			(String)siteId + "' and deleted=0 order by p.sorting, p.id";
		if (query->active(sql) > 0) {
			int sorting = query->getFieldValue(0, "ms").toInt() + 1;

			sql = "insert into pages (siteId, sorting, url, moduleId, title, description, keywords) values('"
				+ (String)siteId + "', '" + (String)sorting + "', '" + url + "', '" + (String)moduleId + "', '" + title + "', '" + description + "', '" + keywords + "')";
			string sql8 = sql.to_string();
			if (query->exec(sql)) {
				int pageId = query->getLastId();
				manager->addPage(siteId, pageId, url.to_string(), moduleId);

				int mainPageId = setMainPage(query, siteId);
				manager->setMainPage(siteId, mainPageId);

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
	}
	page->tplIndex->out("out", "</note>\n");
	manager->deleteQuery(query);
}

void BuilderModule::ajaxGetModuleUrl(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int moduleId = request.header.POST.getValue("moduleId").toInt();
	String sql = "select url from modules where id='" + (String)moduleId + "'";

	int count = query->active(sql);
	if (count > 0) {
		String url = query->getFieldValue(0, "url");
		string url8 = url.to_string();
		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "<moduleUrl>" + url + "</moduleUrl>\n");
		page->tplIndex->out("out", "</note>\n");
	}

	manager->deleteQuery(query);
}

void BuilderModule::ajaxAccept(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	String name = request.header.POST.getValue("name");
	String about = request.header.POST.getValue("about");
	String caption = request.header.POST.getValue("caption");

	String sql = "update sites set name='" + name + "', about='" + about + "', caption='" + caption + "' where id='" + (String)siteId + "'";

	if (query->exec(sql)) {
		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "</note>\n");
	}
	manager->deleteQuery(query);
}

void BuilderModule::ajaxAcceptDesign(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int siteId = request.header.POST.getValue("siteId").toInt();
	int design = request.header.POST.getValue("desing").toInt();
	int theme = request.header.POST.getValue("theme").toInt();
	int layout = request.header.POST.getValue("layout").toInt();

	String sql = "update sites set design='" + (String)design + "', theme='" + (String)theme + "', layout='" + (String)layout + "' where id='" + (String)siteId + "'";

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

			int mainPageId = setMainPage(query, siteId);
			manager->setMainPage(siteId, mainPageId);
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

void BuilderModule::ajaxMoveTableRow(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	int siteId = request.header.POST.getValue("siteId").toInt();
	int index1 = request.header.POST.getValue("index1").toInt();
	int index2 = request.header.POST.getValue("index2").toInt();

	String sql = "select p.id, p.sorting, p.url, m.name, p.isMainPage, p.title, p.description, p.keywords from pages p, modules m where p.moduleId=m.id and siteId='" +
		(String)siteId + "' and deleted=0 order by p.sorting, p.id";
	if (query->active(sql)) {
		int count = query->getRowCount();
		int rowId1 = -1, rowId2 = -1;
		int sorting1 = 1, sorting2 = 1;
		for (int i = 0; i < count; i++) {
			if (index1 == i) {
				rowId1 = query->getFieldValue(i, "id").toInt();
				sorting1 = query->getFieldValue(i, "sorting").toInt();
			}
			if (index2 == i) {
				rowId2 = query->getFieldValue(i, "id").toInt();
				sorting2 = query->getFieldValue(i, "sorting").toInt();
			}
		}

		sql = "update pages set sorting='" + (String)sorting2 + "' where id='" + (String)rowId1 + "'";
		query->exec(sql);

		sql = "update pages set sorting='" + (String)sorting1 + "' where id='" + (String)rowId2 + "'";
		query->exec(sql);

		int mainPageId = setMainPage(query, siteId);
		manager->setMainPage(siteId, mainPageId);
	}

	manager->deleteQuery(query);
}

void BuilderModule::ajaxAddMenuItem(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	int siteId = request.header.POST.getValue("siteId").toInt();
	String name = request.header.POST.getValue("name");
	String url = request.header.POST.getValue("url");

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	page->tplIndex->out("out", "<note>\n");
	if (siteId > 0) {
		String sql = "select max(sorting) ms from menu where siteId='" + (String)siteId + "' and deleted=0 order by sorting, id";
		if (query->active(sql) > 0) {
			int sorting = query->getFieldValue(0, "ms").toInt() + 1;

			sql = "insert into menu (siteId, sorting, name, url) values('" + (String)siteId + "', '" + (String)sorting + "', '" + name + "', '" + url + "')";
			string sql8 = sql.to_string();
			if (query->exec(sql)) {
				page->tplIndex->out("out", "<result>1</result>\n");
			}
		}
	}
	page->tplIndex->out("out", "</note>\n");
	manager->deleteQuery(query);
}

}