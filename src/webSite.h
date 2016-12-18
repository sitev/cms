#pragma once

#include "cjCMS.h"

namespace cj {

class SiteManager;

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
