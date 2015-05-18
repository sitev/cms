#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebPage          -------------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebPage::WebPage(WebSite *site, string page) {
	this->site = site;
	this->page = page;
	moduleId = 0; //0 - никакой модуль не привязан
	tplIndex = new WebTemplate();
}

void WebPage::paint() {
	if (site == NULL) return;
	if (site->manager == NULL) return;
	if (site->host == "") return;
	string fn = site->manager->documentRoot + "/" + site->host + "/index_tpl.html";
	File *f = new File(fn, "r");
	String s, t;
	f->readAll(s);
	tplIndex->exec(s, t);
	this->out("HTTP/1.1 200 OK\r\n");
	this->out("Content-type: text/html; charset=UTF-8\r\n");
	this->out("Content-Length: " + (String)(t.getLength()) + "\r\n\r\n" + t + "\r\n");
	this->out(t);
	application->logger_out(t);
	delete f;
}

void WebPage::out(String s) {
}

void WebPage::out(String tag, String s) {
	tplIndex->out(tag, s);
}

}
