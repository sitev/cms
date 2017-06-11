#pragma once

namespace cms {

class SiteManager;

class WebStudioModule : public WebModule {
public:
	WebStudioModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);
	virtual void paintIndex(WebPage *page, HttpRequest &request);
	virtual void paintSite(WebPage *page, HttpRequest &request);
	virtual void paintMobile(WebPage *page, HttpRequest &request);
	virtual void paintDesktop(WebPage *page, HttpRequest &request);
	virtual void paintSEO(WebPage *page, HttpRequest &request);
	virtual void paintArticles(WebPage *page, HttpRequest &request);
	virtual void paintOutsourcing(WebPage *page, HttpRequest &request);
};

}