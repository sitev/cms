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
	else if (p2 == "sites") {
		String p3 = request.header.GET.getValue("p3");
		if (p3 == "add") paintSitesAdd(page, request);
		else if (p3 == "edit") paintSitesEdit(page, request);
		else if (p3 == "delete") paintSitesDelete(page, request);
	}
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
				String url = query->getFieldValue(i, "url");
				String name = query->getFieldValue(i, "name");
				String about = query->getFieldValue(i, "about");

				WebTemplate *tplRow = new WebTemplate();
				if (tplRow->open(manager->modulePath + "/builder/row_tpl.html")) {
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

void BuilderModule::ajax(WebPage *page, HttpRequest &request) {
	String p2 = request.header.GET.getValue("p2");
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

}

void BuilderModule::paintSitesDelete(WebPage *page, HttpRequest &request) {

}



}