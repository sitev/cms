#include "cms.h"

namespace cms {

WebSurf::WebSurf(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(13);
}

void WebSurf::paint(WebPage *page, HttpRequest &request) {
	String cmd = request.header.GET.getValue("cmd");
	string cmd_8 = cmd.to_string();
	if (cmd == "ajax")
		return ajax(page, request);
}

void WebSurf::ajax(WebPage *page, HttpRequest &request) {
	String p2 = request.header.GET.getValue("p2");
	string p2_8 = p2.to_string();
	String p3 = request.header.GET.getValue("p3");

	if (p2 == "getNextSite") ajaxGetNextSite(page, request);

}

void WebSurf::ajaxGetNextSite(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int taskId = request.header.POST.getValue("taskId").toInt();
	String sql = "select websurf_target where isnull(done)";
	string sql8 = sql.to_string();
	if (query->active(sql) > 0) {
		int site_id = query->getFieldValue(0, "site_url").toInt();
		String site_url = query->getFieldValue(0, "site_url");
		sql = "update websurf_target set done=1 where id='" + (String)site_id + "'";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "<site_url>" + site_url + "</site_url>\n");
			page->tplIndex->out("out", "</note>\n");
		}
	}

	manager->deleteQuery(query);

}

}