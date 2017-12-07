#pragma once

namespace cms {

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
	WebTemplate *tplIndex, *tplLayout;
	bool isLayout;

	bool buttons_in_line = false;
	bool through_cursor = false;

	WebPage(WebSite *site, string page, int pageId, WebModule *module = NULL, String design = "");

	virtual void out(String s);
	virtual void out(String tag, String s);

	virtual void paint(HttpRequest &request, HttpResponse &response);
	virtual void paintTemplates();
	virtual void paintStdTags();
	virtual void clearAllTags();
	virtual void paintUser(String uuid);
	virtual void loadOptions();

	virtual void paintAjax(HttpRequest &request);
	virtual void paintApi(HttpRequest &request);
};

}
