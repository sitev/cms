#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebSite          -------------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebSite::WebSite(SiteManager *manager, string host) {
	this->manager = manager;
	this->host = host;
	tplId = 0; //0 - ��������� ������
	mainPage = NULL;
}

}
