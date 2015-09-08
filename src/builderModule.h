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
	virtual void paintPages(WebTemplate *tpl);
	virtual void paintWidgets(WebTemplate *tpl);

	virtual void paintSitesAdd(WebPage *page, HttpRequest &request);
	virtual void paintSitesEdit(WebPage *page, HttpRequest &request);
	virtual void paintSitesDelete(WebPage *page, HttpRequest &request);

	virtual void ajax(WebPage *page, HttpRequest &request);
};

}