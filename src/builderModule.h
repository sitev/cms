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

	virtual void paintParams(int siteId, WebTemplate *tpl);
	virtual void paintDesign(int siteId, WebTemplate *tpl);
	virtual void paintPages(int siteId, WebTemplate *tpl);
	virtual void paintMenu(int siteId, WebTemplate *tpl);
	virtual void paintWidgets(int siteId, WebTemplate *tpl);

	virtual void paintSitesEdit(WebPage *page, HttpRequest &request);
	virtual void paintEditContent(WebPage *page, HttpRequest &request);

	virtual void ajax(WebPage *page, HttpRequest &request);
	virtual void ajaxCreateSite(WebPage *page, HttpRequest &request);
	virtual void ajaxGetUrlByIndex(WebPage *page, HttpRequest &request);
	virtual void ajaxGetSiteIdByIndex(WebPage *page, HttpRequest &request);
	virtual void ajaxDeleteSite(WebPage *page, HttpRequest &request);


	virtual int getPageIndex(MySQL *query, int siteId);
	virtual int setMainPage(MySQL *query, int siteId);
	virtual bool createDataText(MySQL *query, int pageId, int moduleId, int userId);

	virtual void ajaxAddPage(WebPage *page, HttpRequest &request);
	virtual void ajaxGetModuleUrl(WebPage *page, HttpRequest &request);

	virtual void ajaxAccept(WebPage *page, HttpRequest &request);
	virtual void ajaxAcceptDesign(WebPage *page, HttpRequest &request);
	virtual void ajaxEditPage(WebPage *page, HttpRequest &request);
	virtual void ajaxDeletePage(WebPage *page, HttpRequest &request);
	virtual void ajaxGetPageId(WebPage *page, HttpRequest &request);
	virtual void ajaxSaveContent(WebPage *page, HttpRequest &request);
	virtual void ajaxMoveTableRow(WebPage *page, HttpRequest &request);

	virtual void ajaxAddMenuItem(WebPage *page, HttpRequest &request);
};

}