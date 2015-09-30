#include <sys/stat.h>
#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebPage          -------------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebPage::WebPage(WebSite *site, string page, int pageId, WebModule *module) {
	this->site = site;
	this->page = page;
	this->pageId = pageId;
	this->module = module;
	moduleId = 0; //0 - никакой модуль не привязан
	tplIndex = new WebTemplate();
}

void WebPage::paint(HttpRequest &request, HttpResponse &response) {
	this->response = &response;
	if (site == NULL) return;
	if (site->manager == NULL) return;
	if (site->host == "") return;

	String cmd = request.header.GET.getValue("cmd");
	String fn = "";
	if (cmd == "ajax") {
		fn = site->manager->documentRoot + "/common/ajax_tpl.html";
		tplIndex->clearTag("out");
		
		int count = tplIndex->lstTag.getCount();
		this->module->paint(this, request);
		count = tplIndex->lstTag.getCount();

		String uuid = request.header.COOKIE.getValue("uuid");
		if (uuid.getLength() < 10)
			uuid = generateUUID();

		//String t = "<note><result>1</result></note>";
		
		File *f = new File(fn, "rb");
		String s, t;
		f->readAll(s);
		tplIndex->exec(s, t);
		
		string t8 = t.to_string();
		int len = t8.length();

		this->out("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n");
		this->out("Content-Length: " + (String)len + "\r\n\r\n");
		this->out(t);
	}
	else {
		printf("WebPage::paint\n");
		fn = site->manager->documentRoot + "/" + site->host + "/index_tpl.html";
		printf("fn = %s\n", fn.to_string().c_str());
		int ret;
		struct stat buf;
		if ((ret = stat(fn.to_string().c_str(), &buf)) != 0) {
			fn = site->manager->documentRoot + "/common/index_tpl.html";
		}

		string fn8 = fn.to_string();

		tplIndex->clearTag("title");
		tplIndex->clearTag("keywords");
		tplIndex->clearTag("description");
		tplIndex->clearTag("content");
		tplIndex->clearTag("sidebar");
		tplIndex->clearTag("sidebar2");
		tplIndex->clearTag("caption");
		tplIndex->clearTag("menu");
		tplIndex->clearTag("theme");

		MySQL *query = site->manager->newQuery();
		String sql = "select theme, layout, caption from sites where id='" + (String)site->siteId + "'";
		if (query->active(sql)) {
			int theme = query->getFieldValue(0, "theme").toInt();
			int layout = query->getFieldValue(0, "layout").toInt();
			if (layout > 0) fn = site->manager->documentRoot + "/design/1/layout" + (String)layout + "_tpl.html";
			String caption = query->getFieldValue(0, "caption");

			String sTheme = "/css/";
			if (theme > 0) {
				sql = "select theme from themes where id='" + (String)theme + "'";
				if (query->active(sql) > 0) {
					sTheme = query->getFieldValue(0, "theme");
					sTheme = "https://bootswatch.com/" + sTheme + "/";
				}
			}
			tplIndex->out("theme", sTheme);
			tplIndex->out("caption", caption);
		}

		//paintMenu();
		site->manager->paintMainMenu(site->siteId, tplIndex);
		this->module->paint(this, request);
		site->manager->widgetManager.paintPageWidgets(this);

		String uuid = request.header.COOKIE.getValue("uuid");
		string uuid8 = uuid.to_string();
		if (uuid.getLength() < 10) 
			uuid = generateUUID();


		String login = site->manager->getLogin(uuid);

		WebTemplate *tpl = new WebTemplate();
		if (login == "") {
			tpl->open(site->manager->documentRoot + "/modules/user/loginField_tpl.html");
		}
		else {
			tpl->open(site->manager->documentRoot + "/modules/user/logoutField_tpl.html");
			tpl->out("user", login);
		}
		tpl->exec();

		tplIndex->clearTag("user");
		tplIndex->out("user", tpl->html);

		int count = tplIndex->lstTag.getCount();
		File *f = new File(fn, "rb");
		String s, t;
		f->readAll(s);

		tplIndex->exec(s, t);


		string t8 = t.to_string();
		int len = t8.length();
		this->out("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n");
		this->out("Set-Cookie: uuid=" + uuid + "\r\nContent-Length: " + (String)len + "\r\n\r\n");
		this->out(t);

		LOGGER_OUT("DEBUG", t);
		delete f;
	}
}

void WebPage::out(String s) {
	int len = s.to_string().length();
	response->memory.write((void*)(s.to_string().c_str()), len);
}

void WebPage::out(String tag, String s) {
	tplIndex->out(tag, s);
}

}
