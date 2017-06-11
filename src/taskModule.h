#pragma once

namespace cms {

class SiteManager;

class TaskModule : public WebModule {
	String uuid;
	int userId;
	int whoseTasks;
public:
	String p1, p2, p3;

	TaskModule(SiteManager *manager);
	virtual void paint(WebPage *page, HttpRequest &request);

	virtual void paintTable(WebPage *page, HttpRequest &request);
	virtual void paintWhoseTasks(WebTemplate *tpl, int userId);

	//virtual void paintIndex(WebPage *page, HttpRequest &request);
	//virtual void paintProjects(WebPage *page, HttpRequest &request);
	//virtual void paintPriority(WebPage *page, HttpRequest &request);

	virtual void paintEdit(WebPage *page, HttpRequest &request);
	virtual void paintDetail(WebPage *page, HttpRequest &request, String p2);


	virtual void paintSubTask(WebTemplate *tpl, int taskId, int level, bool isShowOnlyProject);
	virtual void paintRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject);
	virtual void paintPriorityRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject);

	virtual void ajax(WebPage *page, HttpRequest &request);
	virtual void ajaxAccept(WebPage *page, HttpRequest &request);
	virtual void ajaxAdd(WebPage *page, HttpRequest &request);
	virtual void ajaxDelete(WebPage *page, HttpRequest &request);
	virtual void ajaxChange(WebPage *page, HttpRequest &request);
	//	virtual void ajaxGetUrlByIndex(WebPage *page, HttpRequest &request);

};

}