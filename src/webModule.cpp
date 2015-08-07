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
	String cmd = request.header.GET.getValue("cmd");
	if (cmd == "ajax")
		return ajax(page, request);
	String num = request.header.GET.getValue("p2");
	if (num == "") paintNews(page);
	else paintNewsItemView(page, request, num);
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

						paintTags(page, num, tpl1);

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

void NewsModule::paintNewsItemView(WebPage *page, HttpRequest &request, String num) {
	MySQL *query = manager->newQuery();
	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	String sql = "select dt, name, about, text, n.num, n.id newsId from dataNews n, data d where d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" +
		(String)moduleId + "' and n.num='" + num + "' order by n.num desc";
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
					//String num = query->getFieldValue(0, "num");
					int newsId = query->getFieldValue(0, "newsId").toInt();

					tpl->out("dt", dt);
					tpl->out("name", name);
					tpl->out("text", text);
					tpl->out("itemId", num);

					paintTags(page, num, tpl);

					sql = "select c.dt, c.comment, u.login from comments c, users u where u.id=c.userId and newsId='2' order by c.id";
					if (query->exec(sql)) {
						if (query->storeResult()) {
							int count = query->getRowCount();
							for (int i = 0; i < count; i++) {
								String dt = query->getFieldValue(i, "dt");
								String comment = query->getFieldValue(i, "comment");
								String login = query->getFieldValue(i, "login");
								WebTemplate * tplCommentItem = new WebTemplate();
								if (tplCommentItem->open(manager->modulePath + "/" + url + "/commentItem_tpl.html")) {
									tplCommentItem->out("login", login);
									tplCommentItem->out("dt", dt);
									tplCommentItem->out("comment", comment);
									tplCommentItem->exec();
									tpl->out("comments", tplCommentItem->html);
								}

							}
						}
					}
					WebTemplate * tplSendComment = new WebTemplate();
					if (userId != 0) {
						if (tplSendComment->open(manager->modulePath + "/" + url + "/sendComment_tpl.html")) {
							tplSendComment->out("newsId", newsId);
						}
					}
					else {
						if (tplSendComment->open(manager->modulePath + "/" + url + "/sendCommentNotEnter_tpl.html")) {
							tplSendComment->out("newsId", newsId);
						}
					}
					tplSendComment->exec();
					tpl->out("sendComment", tplSendComment->html);
					tpl->exec();

					page->out("title", name);
					page->out("keywords", name);
					page->out("description", name);
					page->out("content", tpl->html);
				}
			}
		}
	}
}

void NewsModule::paintTags(WebPage *page, String num, WebTemplate *tpl) {
	MySQL *query = manager->newQuery();
	String sql = "select tag1, tag2, tag3, tag4, tag5 from dataNews n, data d where d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" +
		(String)moduleId + "' and n.num='" + num + "' order by n.num desc";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				for (int i = 1; i <= 5; i++) {
					String tag = query->getFieldValue(0, "tag" + (String)i);

					if (tag != "") {
						WebTemplate * tplTag = new WebTemplate();
						if (tplTag->open(manager->modulePath + "/" + url + "/tag_tpl.html")) {
							tplTag->out("name", tag);
							tplTag->exec();
							tpl->out("tags", tplTag->html);
						}
					}
				}
			}
		}
	}
}

void NewsModule::ajax(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String obj = request.header.GET.getValue("p1");
	String func = request.header.GET.getValue("p2");
	String uuid = request.header.COOKIE.getValue("uuid");

	if (obj == "post") {
		if (func == "sendComment") {
			String comment = request.header.POST.getValue("comment");
			int newsId = request.header.POST.getValue("newsId").toInt();
			int userId = manager->getUserId(uuid);
			String result = "";

			String sql = "insert into comments (userId, newsId, comment) values ('" + (String)userId + "', '" + (String)newsId + "', '" + comment + "')";
			if (query->exec(sql)) {
				result = "1";
			}

			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>" + result + "</result>\n");
			page->tplIndex->out("out", "</note>\n");
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
