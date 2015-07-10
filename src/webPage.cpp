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

	String fn = site->manager->documentRoot + "/" + site->host + "/index_tpl.html";
	printf("fn = %s\n", fn.toString8().c_str());
	int ret;
	struct stat buf;
	if ((ret = stat(fn.toString8().c_str(), &buf)) != 0) {
		fn = site->manager->documentRoot + "/common/index_tpl.html";
	}

	printf("1\n");

	tplIndex->clearTag("content");
	printf("1_1\n");
	String content = this->module->generateContent(this, request);
	printf("content = %s\n", content.toString8().c_str());
	printf("1_2\n");
	tplIndex->out("content", content);

	printf("2\n");


	int count = tplIndex->lstTag.getCount();
	File *f = new File(fn, "rb");
	String s, t;
	f->readAll(s);
	tplIndex->exec(s, t);
	

	printf("3\n");


	string t8 = t.toString8();
	int len = t8.length();
	this->out("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n");
	this->out("Content-Length: " + (String)len + "\r\n\r\n");
	this->out(t);


	printf("4\n");

	LOGGER_OUT("DEBUG", t);
	delete f;


	printf("5\n");

}

void WebPage::out(String s) {
	int len = s.toString8().length();
	response->memory.write((void*)(s.toString8().c_str()), len);
}

void WebPage::out(String tag, String s) {
	tplIndex->out(tag, s);
}

}
