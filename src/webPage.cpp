#include <sys/stat.h>
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

void WebPage::paint(HttpRequest &request, HttpResponse &response) {
	this->response = &response;
	if (site == NULL) return;
	if (site->manager == NULL) return;
	if (site->host == "") return;
	string fn = site->manager->documentRoot + "/" + site->host + "/index_tpl.html";
	int ret;
	struct stat buf;
	if ((ret = stat(fn.c_str(), &buf)) != 0) {
		fn = site->manager->documentRoot + "/common/index_tpl.html";
	}

	int count = tplIndex->lstTag.getCount();
	File *f = new File(fn, "rb");
	String s, t;
	f->readAll(s);
	tplIndex->exec(s, t);
	
	//s = "HTTP/1.1 200 OK\r\nContent-Length: " + ((String)t.getLength()) + "\r\n\r\n" + t + "\r\n";
	//response.memory.write((void*)(s.toString8().c_str()), s.getLength());
	
	string t8 = t.toString8();
	int len = t8.length();
	this->out("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n");
	this->out("Content-Length: " + (String)len + "\r\n\r\n");
	this->out(t);
	this->out("\r\n");

	application->logger_out(t);
	delete f;
}

void WebPage::out(String s) {
	int len = s.toString8().length();
	response->memory.write((void*)(s.toString8().c_str()), len);
}

void WebPage::out(String tag, String s) {
	tplIndex->out(tag, s);
}

}
