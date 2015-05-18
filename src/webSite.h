#pragma once

#include "cj.h"
#include "webPage.h"

namespace cj {

class WebSite {
public:
	int designId;
	map <string, WebPage*> pages;
	WebSite();
};

}