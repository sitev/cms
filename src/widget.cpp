#include "cjCms.h"

namespace cj {

Widget::Widget() {

}

StaticTextWidget::StaticTextWidget() {

}

void StaticTextWidget::paint(WebPage *page, int widgetId, int widgetType, String location, String name, String about) {
	WebTemplate *tpl = new WebTemplate();
	if (!tpl->open(page->site->manager->widgetPath + "/staticTextWidget/tpl.html")) return;

	MySQL *query = page->site->manager->newQuery();
	String sql = "select text from widget_text where widgetId='" + (String)widgetId + "'";
	string sql8 = sql.to_string();
	if (query->active(sql) > 0) {
		String text = query->getFieldValue(0, "text");
		tpl->out("content", text);
	}
	tpl->out("caption", name);
	tpl->exec();

	page->out(location, tpl->html);
	page->site->manager->deleteQuery(query);
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
	staticTextWidget = new StaticTextWidget();
	webStudioWidget = new WebStudioWidget();
	freelancersWidget = new FreelancersWidget();
	sitevConceptWidget = new SitevConceptWidget();
	sitevContactWidget = new SitevContactWidget();
	postForMoneyWidget = new PostForMoneyWidget();
	freeReclameWidget = new FreeReclameWidget();
}

void WidgetManager::paintWidget(WebPage *page, int widgetId, int widgetType, String location, String name, String about) {
	if (widgetType == 1) 
		staticTextWidget->paint(page, widgetId, widgetType, location, name, about);
	/*
	if (widgetType == 1) webStudioWidget->paint(page, tag);
	else if (widgetType == 2) freelancersWidget->paint(page, tag);
	else if (widgetType == 3) sitevConceptWidget->paint(page, tag);
	else if (widgetType == 4) sitevContactWidget->paint(page, tag);
	else if (widgetType == 5) postForMoneyWidget->paint(page, tag);
	else if (widgetType == 6) freeReclameWidget->paint(page, tag);
	*/
}

void WidgetManager::paintPageWidgets(WebPage *page) {
	if (page == NULL) return;

	MySQL *query = page->site->manager->newQuery();
	String sql = "select id, widgetType, tag, name, about from widget_site where isnull(deleted) and siteId='" + (String)page->site->siteId + "' order by sorting, id";
	if (query->active(sql)) {
		int count = query->getRowCount();
		for (int i = 0; i < count; i++) {
			int widgetId = query->getFieldValue(i, "id").toInt();
			int widgetType = query->getFieldValue(i, "widgetType").toInt();
			String location = query->getFieldValue(i, "tag");
			String name = query->getFieldValue(i, "name");
			String about = query->getFieldValue(i, "about");
			paintWidget(page, widgetId, widgetType, location, name, about);
		}
	}
	page->site->manager->deleteQuery(query);
}

}