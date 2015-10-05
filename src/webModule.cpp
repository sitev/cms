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
			}
		}
	}
	manager->deleteQuery(query);
}

String WebModule::getModuleUrl() {
	MySQL *query = manager->newQuery();

	String sql = "select * from modules where id='" + (String)moduleId + "'";
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0) {
				String url = query->getFieldValue(0, "url");
				manager->deleteQuery(query);
				return url;
			}
		}
	}
	manager->deleteQuery(query);
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

	String sql = "select txt.value from data d, dataText txt where d.dataId=txt.id and d.pageId='" + (String)page->pageId + "' order by txt.id desc";
	printf("sql = %s\n", sql.to_string().c_str());
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			if (count > 0)  {
				String content = query->getFieldValue(0, "value");
				page->out("content", content);
			}
		}
	}
	manager->deleteQuery(query);
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
	String p2 = request.header.GET.getValue("p2");
	if (p2 == "") paintNews(page, request);
	else if (p2 == "add") paintAddNews(page, request);
	else paintNewsItemView(page, request, p2);
}

void NewsModule::paintNews(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	string url8 = url.to_string();
	if (!tpl->open(manager->modulePath + "/" + url + "/index_tpl.html")) return;

	WebTemplate *tplItem = new WebTemplate();
	if (!tplItem->open(manager->modulePath + "/" + url + "/item_tpl.html")) return;

	WebTemplate *tplLast = new WebTemplate();
	if (!tplLast->open(manager->modulePath + "/" + url + "/itemLast_tpl.html")) return;

	WebTemplate *tplTag = new WebTemplate();
	if (!tplTag->open(manager->modulePath + "/" + url + "/tag_tpl.html")) return;

	MySQL *query = manager->newQuery();

	String sql = "select count(*) cnt from dataNews n, data d where not isnull(num) and d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" + (String)moduleId + "' order by dt desc";
	int newsCount = 0;
	if (query->active(sql) > 0) {
		newsCount = query->getFieldValue(0, "cnt").toInt();
	}

	int p = request.header.GET.getValue("p").toInt();
	sql = "select * from dataNews n, data d where not isnull(num) and d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" + (String)moduleId + 
		"' order by dt desc limit " + (String)(p * 10) + ", 10";
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

				String tag1 = query->getFieldValue(i, "tag1");
				String tag2 = query->getFieldValue(i, "tag2");
				String tag3 = query->getFieldValue(i, "tag3");
				String tag4 = query->getFieldValue(i, "tag4");
				String tag5 = query->getFieldValue(i, "tag5");

				WebTemplate *tpli = tplItem;
				if (i + 1 == count) tpli = tplLast;
				tpli->clearAllTags();

				tpli->out("page", page->page);
				tpli->out("num", num);
				tpli->out("itemId", id);
				tpli->out("dt", dt);
				tpli->out("name", name);
				tpli->out("about", about);
				tpli->out("text", text);
				tpli->out("host", page->site->host);

				tplTag->clearAllTags();
				tplTag->out("tag1", tag1);
				tplTag->out("tag2", tag2);
				tplTag->out("tag3", tag3);
				tplTag->out("tag4", tag4);
				tplTag->out("tag5", tag5);
				tplTag->exec();

				tpli->out("tags", tplTag->html);
				tpli->exec();

				tpl->out("out", tpli->html);
			}
		}
	}

	if (newsCount != 0) {
		WebTemplate *tplPag = new WebTemplate();
		if (!tplPag->open(manager->modulePath + "/" + url + "/pagination_tpl.html")) return;

		int pageCount = newsCount / 10;
		if (newsCount % 10 != 0) pageCount++;
		if (pageCount > 1) {
			for (int i = 0; i < pageCount; i++) {
				if (i == 0)	tplPag->out("out", "<li><a href=\"/\">" + (String)(i + 1) + "</a></li>");
				else tplPag->out("out", "<li><a href=\"/post?p=" + (String)i + "\">" + (String)(i + 1) + "</a></li>");

				if (i + 1 == pageCount) tplPag->out("next", "/post?p=" + (String)i);
			}
		}
		tplPag->exec();
		tpl->out("out", tplPag->html);
	}

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	bool isAddFlag = false;

	sql = "select collective from pages where id='" + (String)page->pageId + "'";
	if (query->active(sql)) {
		isAddFlag = query->getFieldValue(0, "collective").toInt();
	}

	if (userId != 0 && !isAddFlag) {
		int siteId = page->site->siteId;
		sql = "select * from sites where id='" + (String)siteId + "' and userId='" + (String)userId + "'";
		int count = query->active(sql);
		if (count > 0) isAddFlag = true;
	}

	if (isAddFlag) {
		WebTemplate *tplWrite = new WebTemplate();
		if (userId != 0) {
			if (!tplWrite->open(manager->modulePath + "/" + url + "/addPostButton_tpl.html")) return;
		}
		else {
			if (!tplWrite->open(manager->modulePath + "/" + url + "/addPostButtonNotEnter_tpl.html")) return;
		}

		tplWrite->out("url", url);
		tplWrite->exec();
		tpl->out("out", tplWrite->html);
	}

	tpl->out("caption", caption);
	tpl->exec();
	page->out("content", tpl->html);
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
					tpl->out("num", num);
					tpl->out("itemId", newsId);

					paintTags(page, num, tpl);

					sql = "select c.dt, c.comment, u.login from comments c, users u where u.id=c.userId and newsId='" + (String)newsId + "' order by c.id";
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
	manager->deleteQuery(query);
}

void NewsModule::paintTags(WebPage *page, String num, WebTemplate *tpl) {
	WebTemplate *tplTag = new WebTemplate();
	if (tplTag->open(manager->modulePath + "/" + url + "/tag_tpl.html")) {
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
							tplTag->out("name", tag);
							tplTag->exec();
							tpl->out("tags", tplTag->html);
						}
					}
				}
			}
		}
		manager->deleteQuery(query);
	}
}

void NewsModule::paintAddNews(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	bool isAddFlag = false;
	if (userId != 0) {
		int siteId = page->site->siteId;
		String sql = "select * from sites where id='" + (String)siteId + "' and userId='" + (String)userId + "'";
		int count = query->active(sql);
		if (count > 0) isAddFlag = true;
	}

	WebTemplate *tpl = new WebTemplate();
	if (isAddFlag) {
		if (tpl->open(manager->modulePath + "/" + url + "/addPost_tpl.html")) {
		}
	}
	else {
		String sql = "select collective from pages where id='" + (String)page->pageId + "'";
		if (query->active(sql)) {
			if (tpl->open(manager->modulePath + "/" + url + "/addPostCollective_tpl.html")) {
			}
		}
	}
	tpl->exec();
	page->out("content", tpl->html);

	manager->deleteQuery(query);
}

void NewsModule::ajax(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String obj = request.header.GET.getValue("p1");
	String func = request.header.GET.getValue("p2");
	String uuid = request.header.COOKIE.getValue("uuid");

	string url8 = url.to_string();
	if (obj == url) {
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
		if (func == "sendPost") {
			String name = request.header.POST.getValue("name");
			String text = request.header.POST.getValue("content");
			text = request.header.htmlEntitiesDecode(text.to_string());
			int userId = manager->getUserId(uuid);
			String result = "";

			String sql = "select count(*) cnt from dataNews n, data d where not isnull(num) and d.dataId=n.id and d.pageId='" + (String)page->pageId + "' and d.moduleId='" + (String)moduleId + "' order by dt desc";
			string sql8 = sql.to_string();
			int newsCount = 0;
			if (query->active(sql) > 0) {
				newsCount = query->getFieldValue(0, "cnt").toInt();
			}
			int num = newsCount + 1;
			sql = "insert into dataNews (name, text, num) values ('" + name + "', '" + text + "', '" + (String)num + "')";
			if (query->exec(sql)) {
				sql = "insert into data (pageId, dataId, moduleId, userId) values('" + (String)page->pageId + "', LAST_INSERT_ID(), '" + (String)moduleId + "', '" + (String)userId + "')";
				if (query->exec(sql)) {
					result = "1";
				}
				else result = "3";
			}
			else result = "2";
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>" + result + "</result>\n");
			page->tplIndex->out("out", "<sql>" + sql + "</sql>\n");
			page->tplIndex->out("out", "</note>\n");
		}
	}
	manager->deleteQuery(query);
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
