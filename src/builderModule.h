#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class SiteManager;

class BuilderModule : public WebModule {
public:
	BuilderModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);
};

}