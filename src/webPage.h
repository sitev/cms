#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class WebSite;

class WebPage {
	HttpResponse *response;
public:
	WebSite *site;
	string page;
	WebTemplate *tplIndex;
	int moduleId;
	WebPage(WebSite *site, string page);
	virtual void paint(HttpRequest &request, HttpResponse &response);
	virtual void out(String s);
	virtual void out(String tag, String s);
};

}
