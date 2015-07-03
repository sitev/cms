#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebModule          -----------------------------------------------------
//--------------------------------------------------------------------------------------------------

	WebModule::WebModule(SiteManager *manager, int moduleId, string name, string about, string url) {
	this->moduleId = moduleId;
	this->name = name;
	this->about = about;
	this->url = url;
}


//---------------------------------------------------------------------------------------------------
//----------      class StaticPageModule     --------------------------------------------------------
//---------------------------------------------------------------------------------------------------

StaticPageModule::StaticPageModule(SiteManager *manager) : WebModule(manager, 1, "Просто текст", "", "text") {
}
String StaticPageModule::generateContent(WebPage *page) {
	MySQL *query = new MySQL();
	if (!query->init()) return "";
	if (!query->connect("127.0.0.1", "root", "123qwe", "sitev")) return "";

	String sql = "select txt.value from data d, dataText txt where d.dataId=txt.id and d.pageId='" + (String)page->pageId + "'";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0)  {
				String content = query->getFieldValue(0, "value");
				return content;
			}
		}
	}

	return "";
}

}
