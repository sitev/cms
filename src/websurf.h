#pragma once

namespace cms {

class WebSurf : public WebModule {
public:
	WebSurf(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);
	virtual void ajax(WebPage *page, HttpRequest &request);
	virtual void ajaxGetNextSite(WebPage *page, HttpRequest &request);
};

}