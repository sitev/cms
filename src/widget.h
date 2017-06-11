#pragma once

namespace cms {

class WebPage;

class Widget {
public:
	Widget();
	virtual void paint(WebPage *page, String tag) = 0;
};

class StaticTextWidget {
public:
	StaticTextWidget();
	virtual void paint(WebPage *page, int widgetId, int widgetType, String location, String name, String about);
};

class WebStudioWidget {
public:
	WebStudioWidget();
	virtual void paint(WebPage *page, String tag);
};

class FreelancersWidget {
public:
	FreelancersWidget();
	virtual void paint(WebPage *page, String tag);
};

class SitevConceptWidget {
public:
	SitevConceptWidget();
	virtual void paint(WebPage *page, String tag);
};

class SitevContactWidget {
public:
	SitevContactWidget();
	virtual void paint(WebPage *page, String tag);
};

class PostForMoneyWidget {
public:
	PostForMoneyWidget();
	virtual void paint(WebPage *page, String tag);
};

class FreeReclameWidget {
public:
	FreeReclameWidget();
	virtual void paint(WebPage *page, String tag);
};

class WidgetManager {
	StaticTextWidget *staticTextWidget;

	WebStudioWidget *webStudioWidget;
	FreelancersWidget *freelancersWidget;
	SitevConceptWidget *sitevConceptWidget;
	SitevContactWidget *sitevContactWidget;
	PostForMoneyWidget *postForMoneyWidget;
	FreeReclameWidget *freeReclameWidget;
public:
	WidgetManager();
	virtual void paintWidget(WebPage *page, int widgetId, int widgetType, String location, String name, String about);
	virtual void paintPageWidgets(WebPage *page);
};



}