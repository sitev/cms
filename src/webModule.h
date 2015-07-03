#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class SiteManager;

class WebModule {
public:
	SiteManager *manager;
	int moduleId;
	string name;
	string about;
	string url;
	WebModule(SiteManager *manager, int moduleId, string name, string about, string url);
	virtual String generateContent(WebPage *page) = 0;
};

class StaticPageModule : public WebModule {
public:
	StaticPageModule(SiteManager *manager);
	virtual String generateContent(WebPage *page);
};


}