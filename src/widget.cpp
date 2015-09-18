#include "cjCms.h"

namespace cj {

Widget::Widget() {

}


WebStudioWidget::WebStudioWidget() {

}

void WebStudioWidget::paint(WebPage *page, String tag) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/webStudioWidget/tpl.html")) return;
	tpl->exec();

	page->out(tag, tpl->html);
}

FreelancersWidget::FreelancersWidget() {

}

void FreelancersWidget::paint(WebPage *page, String tag) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/freelancersWidget/tpl.html")) return;
	tpl->exec();

	page->out(tag, tpl->html);
}

SitevConceptWidget::SitevConceptWidget() {

}

void SitevConceptWidget::paint(WebPage *page, String tag) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/sitevConceptWidget/tpl.html")) return;
	tpl->exec();

	page->out(tag, tpl->html);
}

SitevContactWidget::SitevContactWidget() {
}

void SitevContactWidget::paint(WebPage *page, String tag) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/sitevContactWidget/tpl.html")) return;
	tpl->exec();

	page->out(tag, tpl->html);
}

PostForMoneyWidget::PostForMoneyWidget() {

}

void PostForMoneyWidget::paint(WebPage *page, String tag) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/postForMoneyWidget/tpl.html")) return;
	tpl->exec();

	page->out(tag, tpl->html);
}

FreeReclameWidget::FreeReclameWidget() {

}

void FreeReclameWidget::paint(WebPage *page, String tag) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/freeReclameWidget/tpl.html")) return;
	tpl->exec();

	page->out(tag, tpl->html);
}


WidgetManager::WidgetManager() {
	webStudioWidget = new WebStudioWidget();
	freelancersWidget = new FreelancersWidget();
	sitevConceptWidget = new SitevConceptWidget();
	sitevContactWidget = new SitevContactWidget();
	postForMoneyWidget = new PostForMoneyWidget();
	freeReclameWidget = new FreeReclameWidget();
}

void WidgetManager::paintWidget(WebPage *page, String tag, int widgetId) {
	if (widgetId == 1) webStudioWidget->paint(page, tag);
	else if (widgetId == 2) freelancersWidget->paint(page, tag);
	else if (widgetId == 3) sitevConceptWidget->paint(page, tag);
	else if (widgetId == 4) sitevContactWidget->paint(page, tag);
	else if (widgetId == 5) postForMoneyWidget->paint(page, tag);
	else if (widgetId == 6) freeReclameWidget->paint(page, tag);
}

void WidgetManager::paintPageWidgets(WebPage *page) {
	if (page == NULL) return;

	MySQL *query = page->site->manager->newQuery();
	String sql = "select widgetId, tag from widget_site where siteId='" + (String)page->site->siteId + "'";
	if (query->active(sql)) {
		int count = query->getRowCount();
		for (int i = 0; i < count; i++) {
			int widgetId = query->getFieldValue(i, "widgetId").toInt();
			String tag = query->getFieldValue(i, "tag");
			paintWidget(page, tag, widgetId);
		}
	}
	page->site->manager->deleteQuery(query);
}

}