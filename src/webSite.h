#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class SiteManager;

class WebSite {
public:
	SiteManager *manager;
	string host;
	int designId;
	map <string, WebPage*> pages;
	WebSite(SiteManager *manager, string host);
};

}