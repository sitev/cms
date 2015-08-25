#pragma once

#include "cj.h"
#pragma once

#include "cjCms.h"

namespace cj {

class SiteManager;

class WebModule {
public:
	SiteManager *manager;
	int moduleId;
	String name;
	String about;
	String url;
	WebModule(SiteManager *manager);
	virtual void setOptions(int moduleId, string name, string about, string url);
	virtual void setOptionsFromDB(int moduleId);
	virtual void paint(WebPage *page, HttpRequest &request) = 0;
	virtual String getModuleUrl();
	virtual void paint404(WebPage *page, HttpRequest &request);
};

class StaticPageModule : public WebModule {
public:
	StaticPageModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);
};

class PostModule : public WebModule {
public:
	PostModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request) = 0;
};

class NewsModule : public PostModule {
protected:
	String caption;
public:
	NewsModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);
	virtual void paintNews(WebPage *page, HttpRequest &request);
	virtual void paintNewsItemView(WebPage *page, HttpRequest &request, String num);
	virtual void paintTags(WebPage *page, String num, WebTemplate *tpl);
	virtual void paintAddNews(WebPage *page, HttpRequest &request);
	virtual void ajax(WebPage *page, HttpRequest &request);
};

class BlogModule : public NewsModule {
public:
	BlogModule(SiteManager *manager);
};


class GuestbookModule : public NewsModule {
public:
	GuestbookModule(SiteManager *manager);
};

class BoardModule : public NewsModule {
public:
	BoardModule(SiteManager *manager);
};

class ArticleModule : public NewsModule {
public:
	ArticleModule(SiteManager *manager);
};

class QuestionAnswerModule : public NewsModule {
public:
	QuestionAnswerModule(SiteManager *manager);
};

class ForumModule : public PostModule {
public:
	ForumModule(SiteManager *manager);
};

}