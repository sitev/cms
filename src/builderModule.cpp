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
			title + "</td><td>" + description + "</td><td>" + keywords + "</td><td><a href='#' data-toggle='modal' data-target='#modRemovePage' data-pageid='" + 
			(String)pageId + "' data-index='" + (String)i + "' data-url='" + url + "' data-title='" + title + "'><i class='glyphicon glyphicon-remove'></i></a></td></tr>");
	}

	delete query;
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
		int siteId = request.header.GET.getValue("p3").toInt();
		paintPages(siteId, tpl);
		paintWidgets(siteId, tpl);

		String sql = "select * from sites where id='" + (String)siteId + "'";
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
		}
		tpl->exec();
		page->out("content", tpl->html);
	}

	delete query;
}

void BuilderModule::paintSitesDelete(WebPage *page, HttpRequest &request) {

}

void BuilderModule::ajax(WebPage *page, HttpRequest &request) {
	String p2 = request.header.GET.getValue("p2");

	if (p2 == "addPage") ajaxAddPage(page, request);
	else if (p2 == "deletePage") ajaxDeletePage(page, request);
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

	page->tplIndex->out("out", "<note>\n");
	if (siteId > 0) {
		String sql = "insert into pages (siteId, url, isMainPage, moduleId, title, description, keywords) values('"
			+ (String)siteId + "', '" + url + "', '" + (String)isMainPage + "', '" + (String)moduleId + "', '" + title + "', '" + description + "', '" + keywords + "')";
		string sql8 = sql.toString8();
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<result>1</result>\n");

			String sql = "select count(*) cnt from pages where siteId='" + (String)siteId + "' and deleted=0 and moduleId>0";
			string sql8 = sql.toString8();
			int count = query->active(sql);
			if (count > 0) {
				int cnt = query->getFieldValue(0, "cnt").toInt();
				int index = cnt;
				page->tplIndex->out("out", "<index>" + (String)index + "</index>\n");
			}

			sql = "select name from modules where id='" + (String)moduleId + "'";
			sql8 = sql.toString8();
			count = query->active(sql);
			if (count > 0) {
				String name = query->getFieldValue(0, "name");
				page->tplIndex->out("out", "<moduleName>" + name + "</moduleName>\n");
			}

		}
	}
	page->tplIndex->out("out", "</note>\n");
}

void BuilderModule::ajaxDeletePage(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int pageId = request.header.POST.getValue("pageId").toInt();
	int index = request.header.POST.getValue("index").toInt();
	if (pageId > 0) {
		String sql = "update pages set deleted=1, dtDeleted=now() where id='" + (String)pageId + "'";
		string sql8 = sql.toString8();
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}
	}

}

}