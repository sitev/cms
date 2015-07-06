#pragma once

#include "cj.h"
#include "cjCms.h"

namespace cj {

class SiteManager;

class WebModule {
public:
	SiteManager *manager;
	int moduleId;
	string name;
	string about;
	String url;
	WebModule(SiteManager *manager);
	virtual void setOptions(int moduleId, string name, string about, string url);
	virtual String generateContent(WebPage *page, HttpRequest &request) = 0;
	virtual String getModuleUrl();
};

class StaticPageModule : public WebModule {
public:
	StaticPageModule(SiteManager *manager);
	virtual String generateContent(WebPage *page, HttpRequest &request);
};

class NewsModule : public WebModule {
protected:
	String caption;
public:
	NewsModule(SiteManager *manager);
	virtual String generateContent(WebPage *page, HttpRequest &request);
	virtual String generateNews(WebPage *page);
	/*
	virtual void paintNewsItemView(String newsId);
	virtual void paintNewsItemInput();
	virtual void paintNewsItemPost();
	*/
};

class BoardModule : public NewsModule {
public:
	BoardModule(SiteManager *manager);
};

class GuestbookModule : public NewsModule {
public:
	GuestbookModule(SiteManager *manager);
};

class BlogModule : public NewsModule {
public:
	BlogModule(SiteManager *manager);
};




}