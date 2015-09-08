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
	else if (p2 == "add") paintSitesAdd(page, request);
	else if (p2 == "edit") paintSitesEdit(page, request);
	else if (p2 == "delete") paintSitesDelete(page, request);

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
			String sql = "select * from sites where userId='" + (String)userId + "'";
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

	delete query;
}

void BuilderModule::paintPages(WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String sql = "select * from pages where deleted=0 order by id";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int pageId = query->getFieldValue(i, "id").toInt();
		String url = query->getFieldValue(i, "url");
		int isMainPage = query->getFieldValue(i, "isMainPage").toInt();
		String title = query->getFieldValue(i, "title");
		String description = query->getFieldValue(i, "description");
		String keywords = query->getFieldValue(i, "keywords");

		tpl->out("pages", "<tr><td>" + (String)(i + 1) + "</td><td>" + url + "</td><td>" + (String)isMainPage + "</td><td>" + 
			title + "</td><td>" + description + "</td><td>" + keywords + "</td></tr>");
	}

	delete query;
}

void BuilderModule::paintWidgets(WebTemplate *tpl) {
	MySQL *query = manager->newQuery();

	String sql = "select w.id, ws.tag, w.name from widget_site ws, widgets w where ws.widgetId=w.id and siteId=1 order by ws.id";
	int count = query->active(sql);
	for (int i = 0; i < count; i++) {
		int widgetId = query->getFieldValue(i, "id").toInt();
		String tag = query->getFieldValue(i, "tag");
		String name = query->getFieldValue(i, "name");

		tpl->out("widgets", "<tr><td>" + (String)(i + 1) + "</td><td>" + tag + "</td><td>" + name + "</td></tr>");
	}

	delete query;
}

void BuilderModule::paintSitesAdd(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	WebTemplate *tpl = new WebTemplate();
	String tplName = "sitesAdd_tpl.html";

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);
	if (userId == 0) tplName = "notAuth_tpl.html";

	String tplPath = manager->modulePath + "/builder/" + tplName;
	if (tpl->open(tplPath)) {
		tpl->exec();
		page->out("content", tpl->html);
	}
}

void BuilderModule::paintSitesEdit(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	WebTemplate *tpl = new WebTemplate();
	String tplName = "edit_tpl.html";
	if (tpl->open(manager->modulePath + "/builder/" + tplName)) {
		paintModules(tpl);
		paintPages(tpl);
		paintWidgets(tpl);

		String sql = "select * from sites where id='" + (String)page->site->siteId + "'";
		int count = query->active(sql);
		if (count > 0) {
			String url = query->getFieldValue(0, "url");
			String urlView = query->getFieldValue(0, "urlView");
			String name = query->getFieldValue(0, "name");
			String about = query->getFieldValue(0, "about");
			String keywords = query->getFieldValue(0, "keywords");
			tpl->out("url", url);
			tpl->out("urlView", urlView);
			tpl->out("name", name);
			tpl->out("about", about);
			tpl->out("keywords", keywords);
		}
		tpl->exec();
		page->out("content", tpl->html);
	}

	delete query;
}

void BuilderModule::paintSitesDelete(WebPage *page, HttpRequest &request) {

}

void BuilderModule::ajax(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String func = request.header.GET.getValue("p2");

	if (func == "addPage") {
		String url = request.header.POST.getValue("url");
		int moduleId = request.header.POST.getValue("moduleId").toInt();

		int siteId = page->site->siteId; // request.header.GET.getValue("p3").toInt();
		int isMainPage = 0;
		String title = "", description, keyword;


		page->tplIndex->out("out", "<note>\n");

		if (siteId > 0) {
			String sql = "insert into page (siteId, url, isMainPage, moduleId, title, description, keywords) values('"
				+ (String)siteId + "', '" + url + "', '" + (String)isMainPage + "', '" + (String)moduleId + "', '" + title +"', '" + description +"', '" + keyword +"')";
			string sql8 = sql.toString8();
			if (query->exec(sql)) {
				page->tplIndex->out("out", "<result>1</result>\n");
			}
		}
		page->tplIndex->out("out", "</note>\n");
	}
}

}