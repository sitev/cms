#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebSite          -------------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebSite::WebSite(SiteManager *manager, string host, int siteId) {
	this->manager = manager;
	this->host = host;
	this->siteId = siteId;
	tplId = 0; //0 - кастомный дизайн
	mainPage = NULL;
}

}
