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
	WebModule *module;
	WebTemplate *tplIndex;
	WebPage(WebSite *site, string page, int pageId, WebModule *module = NULL);
	virtual void paint(HttpRequest &request, HttpResponse &response);
	virtual void out(String s);
	virtual void out(String tag, String s);
	virtual String htmlEntities(String s);
};

}
