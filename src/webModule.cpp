#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebModule          -----------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebModule::WebModule(SiteManager *manager) {
	this->manager = manager;
}

void WebModule::setOptions(int moduleId, string name, string about, string url) {
	this->moduleId = moduleId;
	this->name = name;
	this->about = about;
	this->url = url;
}

String WebModule::getModuleUrl() {
	MySQL *query = new MySQL();
	if (!query->init()) return "";
	if (!query->connect("127.0.0.1", "root", "123qwe", "sitev")) return "";

	String sql = "select * from modules where id='" + (String)moduleId + "'";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				String url = query->getFieldValue(0, "url");
				return url;
			}
		}
	}
	return "";
}


//---------------------------------------------------------------------------------------------------
//----------      class StaticPageModule     --------------------------------------------------------
//---------------------------------------------------------------------------------------------------

StaticPageModule::StaticPageModule(SiteManager *manager) : WebModule(manager) {
	setOptions(1, "Просто текст", "", "text");
}
String StaticPageModule::generateContent(WebPage *page, HttpRequest &request) {
	MySQL *query = new MySQL();
	if (!query->init()) return "";
	if (!query->connect("127.0.0.1", "root", "123qwe", "sitev")) return "";
	query->exec("SET NAMES utf8");

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

//---------------------------------------------------------------------------------------------------
//----------      class NewsModule     --------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
NewsModule::NewsModule(SiteManager *manager) : WebModule(manager) {
	moduleId = 2;
	url = getModuleUrl();
}
String NewsModule::generateContent(WebPage *page, HttpRequest &request) {
	String itemId = request.header.params.getValue("p2");

	//if (itemId == "") 
		return generateNews(page);
//	else if (itemId == "input") paintNewsItemInput();
//	else if (itemId == "post") paintNewsItemPost();
//	else paintNewsItemView(itemId);
}

String NewsModule::generateNews(WebPage *page) {
	WebTemplate *tpl = new WebTemplate();
	if (tpl->open(manager->modulePath + "/" + url + "/index_tpl.html")) {
		String sql = "select * from dataNews n, data d where d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" + (String)moduleId + "' order by dt desc";
		//page->out("out", sql);

		MySQL *query = new MySQL();
		if (!query->init()) return "";
		if (!query->connect("127.0.0.1", "root", "123qwe", "sitev")) return "";
		query->exec("SET NAMES utf8");

		if (query->exec(sql)) {
			if (query->storeResult()) {
				int count = query->getRowCount();
				for (int i = 0; i < count; i++) {
					String id = query->getFieldValue(i, "id");
					String dt = query->getFieldValue(i, "dt");
					dt = dtRus(dt, 1);
					String name = query->getFieldValue(i, "name");
					String about = query->getFieldValue(i, "about");
					String text = query->getFieldValue(i, "text");
					int num = query->getFieldValue(i, "num").toInt();

					WebTemplate *tpl1 = new WebTemplate();
					if (tpl1->open(manager->modulePath + "/" + url + "/item_tpl.html")) {
						tpl1->out("num", num);
						tpl1->out("dt", dt);
						tpl1->out("name", name);
						tpl1->out("about", about);
						tpl1->out("text", text);

						tpl1->exec();
						tpl->out("out", tpl1->html);
					}
				}
			}
		}

		tpl->out("caption", caption);
		tpl->exec();
		return tpl->html;
	}
}
/*
void NewsModule::paintNewsItemView(String newsId) {
	//	String sql = "select * from dataNews where id='" + newsId + "'";
	String sql = "select dt, name, about, text from dataNews n, data d where d.dataId=n.id and d.pageId='" + pageId + "' and d.moduleId='" +
		moduleId + "' and n.num='" + newsId + "' order by n.num desc";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				HTMLTemplate * tpl = new HTMLTemplate();
				if (tpl->open(platform->modulePath + "/" + url + "/view_tpl.html")) {
					String dt = query->getFieldValue(0, "dt");
					dt = dtRus(dt, 1);
					String name = query->getFieldValue(0, "name");
					String about = query->getFieldValue(0, "about");
					String text = query->getFieldValue(0, "text");

					tpl->out("dt", dt);
					tpl->out("name", name);
					tpl->out("text", text);
					tpl->changeAllTag();
					page->out("out", tpl->html);
					page->out("title", name);
					page->out("keywords", name);
					page->out("description", name);
				}
			}
		}
	}
}

void NewsModule::paintNewsItemInput() {
	HTMLTemplate * tpl = new HTMLTemplate();
	if (tpl->open(platform->modulePath + "/" + url + "/input_tpl.html")) {
		String p1 = page->getParamValue(ptGET, "p1");
		tpl->out("p1", p1);
		tpl->changeAllTag();
		page->out("out", tpl->html);
	}
}

void NewsModule::paintNewsItemPost() {
	String name = page->getParamValue(ptPOST, "name");
	String about = page->getParamValue(ptPOST, "about");
	String text = page->getParamValue(ptPOST, "text");

	String sql = "select n.id, n.num from dataNews n, data d where d.dataId=n.id and d.pageId='" + pageId + "' and d.moduleId='" +
		moduleId + "' order by n.num desc";
	//page->out("out", sql);

	int num = 0;
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				num = query->getFieldValue(0, "num").toInt();
			}
		}
	}
	num++;

	sql = "insert into dataNews (name, about, text, num) values ('" + name + "', '" + about + "', '" +
		text + "', '" + (String)num + "')";
	//page->out("out", sql);
	if (query->exec(sql)) {
		sql = "select LAST_INSERT_ID();";
		if (query->exec(sql)) {
			if (query->storeResult()) {
				int count = query->getRowCount();
				if (count > 0) {
					String lastId = query->getFieldValue(0, "LAST_INSERT_ID()");
					sql = "insert into data (pageId, dataId, moduleId, userId) values ('" + pageId + "', '" +
						lastId + "', '" + moduleId + "', '" + platform->userId + "')";
					if (query->exec(sql)) {
						page->out("out", "<h2>Данные были добавлены...</h2>");

					}
				}
			}
		}
	}
}
*/
//---------------------------------------------------------------------------------------------------
//----------      class BoardModule     -------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
BoardModule::BoardModule(SiteManager *manager) : NewsModule(manager) {
	moduleId = 7;
	url = getModuleUrl();
}

//---------------------------------------------------------------------------------------------------
//----------      class GuestbookModule     ---------------------------------------------------------
//---------------------------------------------------------------------------------------------------
GuestbookModule::GuestbookModule(SiteManager *manager) : NewsModule(manager) {
	moduleId = 4;
	url = getModuleUrl();
}

//---------------------------------------------------------------------------------------------------
//----------      class BlogModule     --------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
BlogModule::BlogModule(SiteManager *manager) : NewsModule(manager) {
	moduleId = 11;
	url = getModuleUrl();
}



}
