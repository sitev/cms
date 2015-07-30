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

void WebModule::setOptionsFromDB(int moduleId) {
	this->moduleId = moduleId;
	MySQL *query = manager->newQuery();

	String sql = "select * from modules where id='" + (String)moduleId + "'";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				name = query->getFieldValue(0, "name");
				about = query->getFieldValue(0, "about");
				url = query->getFieldValue(0, "url");
				return;
			}
		}
	}
}

String WebModule::getModuleUrl() {
	MySQL *query = manager->newQuery();

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

void WebModule::paint404(WebPage *page, HttpRequest &request) {

}



//---------------------------------------------------------------------------------------------------
//----------      class StaticPageModule     --------------------------------------------------------
//---------------------------------------------------------------------------------------------------

StaticPageModule::StaticPageModule(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(1);
}
void StaticPageModule::paint(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();


	String sql = "select txt.value from data d, dataText txt where d.dataId=txt.id and d.pageId='" + (String)page->pageId + "'";
	printf("sql = %s\n", sql.toString8().c_str());
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0)  {
				String content = query->getFieldValue(0, "value");
				page->out("content", content);
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------
//----------      class PostModule     --------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
PostModule::PostModule(SiteManager *manager) : WebModule(manager) {
}

//---------------------------------------------------------------------------------------------------
//----------      class NewsModule     --------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
NewsModule::NewsModule(SiteManager *manager) : PostModule(manager) {
	setOptionsFromDB(2);
}
void NewsModule::paint(WebPage *page, HttpRequest &request) {
	printf("news 1\n");
	String newsId = request.header.GET.getValue("p2");

	printf("news 2\n");

	if (newsId == "") paintNews(page);
	return paintNewsItemView(page, newsId);

	printf("news 3\n");

}

void NewsModule::paintNews(WebPage *page) {
	WebTemplate *tpl = new WebTemplate();
	if (tpl->open(manager->modulePath + "/" + url + "/index_tpl.html")) {
		String sql = "select * from dataNews n, data d where d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" + (String)moduleId + "' order by dt desc";
		//page->out("out", sql);
		MySQL *query = manager->newQuery();

		if (query->exec(sql)) {
			if (query->storeResult()) {
				int count = query->getRowCount();
				for (int i = 0; i < count; i++) {
					String id = query->getFieldValue(i, "id");
					String dt = query->getFieldValue(i, "dt");
					dt = dtRus(dt, 0);
					String name = query->getFieldValue(i, "name");
					String about = query->getFieldValue(i, "about");
					String text = query->getFieldValue(i, "text");
					int num = query->getFieldValue(i, "num").toInt();

					WebTemplate *tpl1 = new WebTemplate();
					String fn;
					if (i + 1 != count) fn = manager->modulePath + "/" + url + "/item_tpl.html";
					else fn = manager->modulePath + "/" + url + "/itemLast_tpl.html";
					if (tpl1->open(fn)) {
						tpl1->out("page", page->page);
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
		page->out("content", tpl->html);
	}
}

void NewsModule::paintNewsItemView(WebPage *page, String newsId) {
	String sql = "select dt, name, about, text from dataNews n, data d where d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" +
		(String)moduleId + "' and n.num='" + newsId + "' order by n.num desc";
	MySQL *query = manager->newQuery();

	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				WebTemplate * tpl = new WebTemplate();
				if (tpl->open(manager->modulePath + "/" + url + "/view_tpl.html")) {
					String dt = query->getFieldValue(0, "dt");
					dt = dtRus(dt, 0);
					String name = query->getFieldValue(0, "name");
					String about = query->getFieldValue(0, "about");
					String text = query->getFieldValue(0, "text");

					tpl->out("dt", dt);
					tpl->out("name", name);
					tpl->out("text", text);
					tpl->exec();
					page->out("content", tpl->html);
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------
//----------      class BlogModule     --------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
BlogModule::BlogModule(SiteManager *manager) : NewsModule(manager) {
	setOptionsFromDB(3);
}


//---------------------------------------------------------------------------------------------------
//----------      class GuestbookModule     ---------------------------------------------------------
//---------------------------------------------------------------------------------------------------
GuestbookModule::GuestbookModule(SiteManager *manager) : NewsModule(manager) {
	setOptionsFromDB(4);
}

//---------------------------------------------------------------------------------------------------
//----------      class BoardModule     -------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
BoardModule::BoardModule(SiteManager *manager) : NewsModule(manager) {
	setOptionsFromDB(5);
}

//---------------------------------------------------------------------------------------------------
//----------      class ArticleModule     -----------------------------------------------------------
//---------------------------------------------------------------------------------------------------
ArticleModule::ArticleModule(SiteManager *manager) : NewsModule(manager) {
	setOptionsFromDB(6);
}

//---------------------------------------------------------------------------------------------------
//----------      class QuestionAnswerModule     ----------------------------------------------------
//---------------------------------------------------------------------------------------------------
QuestionAnswerModule::QuestionAnswerModule(SiteManager *manager) : NewsModule(manager) {
	setOptionsFromDB(7);
}

//---------------------------------------------------------------------------------------------------
//----------      class ForumModule     -------------------------------------------------------------
//---------------------------------------------------------------------------------------------------
ForumModule::ForumModule(SiteManager *manager) : PostModule(manager) {
	setOptionsFromDB(8);
}

}
