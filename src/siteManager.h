#pragma once

#include "webServer.h"

namespace cj {

class SiteManager : public WebServer {
public:
	ParamList sites;
	SiteManager(int port = 80);
};

}