#pragma once

namespace cms {

class SiteManager;
class WebPage;

class WebSite {
public:
	SiteManager *manager;
	String host;
	int siteId;
	int tplId;
	WebPage *mainPage;

	map <string, WebPage*> pages;
	WebSite(SiteManager *manager, string host, int siteId);
	virtual WebPage* getPageById(int pageId);
};

}