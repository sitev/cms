#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebStudioModule          -----------------------------------------------
//--------------------------------------------------------------------------------------------------

WebStudioModule::WebStudioModule(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(10);
}

void WebStudioModule::paint(WebPage *page, HttpRequest &request) {
	String p2 = request.header.GET.getValue("p2");
	if (p2 == "") paintIndex(page, request);
	else if (p2 == "site") paintSite(page, request);
	else if (p2 == "mobile") paintMobile(page, request);
	else if (p2 == "desktop") paintDesktop(page, request);
	else if (p2 == "seo") paintSEO(page, request);
	else if (p2 == "articles") paintArticles(page, request);
	else if (p2 == "outsourcing") paintOutsourcing(page, request);
}

void WebStudioModule::paintIndex(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(manager->modulePath + "/webstudio/tpl.html")) return;
	tpl->exec();
	page->out("content", tpl->html);
}

void WebStudioModule::paintSite(WebPage *page, HttpRequest &request) {

}

void WebStudioModule::paintMobile(WebPage *page, HttpRequest &request) {

}

void WebStudioModule::paintDesktop(WebPage *page, HttpRequest &request) {

}

void WebStudioModule::paintSEO(WebPage *page, HttpRequest &request) {

}

void WebStudioModule::paintArticles(WebPage *page, HttpRequest &request) {

}

void WebStudioModule::paintOutsourcing(WebPage *page, HttpRequest &request) {

}

}