#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class SiteManager;

class BuilderModule : public WebModule {
public:
	BuilderModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);
	virtual void paintMain(WebPage *page, HttpRequest &request);
	virtual void paintModules(WebTemplate *tpl);
	virtual void paintPages(int siteId, WebTemplate *tpl);
	virtual void paintWidgets(int siteId, WebTemplate *tpl);

	virtual void paintSitesAdd(WebPage *page, HttpRequest &request);
	virtual void paintSitesEdit(WebPage *page, HttpRequest &request);
	virtual void paintSitesDelete(WebPage *page, HttpRequest &request);

	virtual void ajax(WebPage *page, HttpRequest &request);
	virtual void ajaxCreateSite(WebPage *page, HttpRequest &request);
	virtual void ajaxGetUrlByIndex(WebPage *page, HttpRequest &request);
	virtual void ajaxGetSiteIdByIndex(WebPage *page, HttpRequest &request);
	virtual void ajaxDeleteSite(WebPage *page, HttpRequest &request);


	virtual void ajaxAddPage(WebPage *page, HttpRequest &request);
	virtual void ajaxDeletePage(WebPage *page, HttpRequest &request);
};

}