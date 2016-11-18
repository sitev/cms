#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class WebSite;
class WebModule;

class WebPage {
	HttpResponse *response;
public:
	WebSite *site;
	string page;
	int pageId;
	int moduleId;
	String design;
	WebModule *module;
	WebTemplate *tplIndex;
	WebPage(WebSite *site, string page, int pageId, WebModule *module = NULL, String design = "");

	virtual void out(String s);
	virtual void out(String tag, String s);

	virtual void paint(HttpRequest &request, HttpResponse &response);
	virtual void paintAjax(HttpRequest &request);
};

}
