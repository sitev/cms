#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class BuilderModule          -------------------------------------------------
//--------------------------------------------------------------------------------------------------

BuilderModule::BuilderModule(SiteManager *manager) {
	setOptionsFromDB(10);
}

void BuilderModule::paint(WebPage *page, HttpRequest &request) {

}

}