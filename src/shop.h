#pragma once

#include "cms.h"

namespace cms {
	class Shop : public WebModule {
		String uuid;
		int userId;
	public:
		String p1, p2, p3, p4, p5;

		Shop(SiteManager *manager);
		virtual void paint(WebPage *page, HttpRequest &request);
		virtual void paintIndex(WebPage *page, HttpRequest &request);

		virtual void paintCategory(WebPage *page, HttpRequest &request);
		virtual void paintCategoryIndex(WebPage *page, HttpRequest &request);
		virtual void paintCategoryEdit(WebPage *page, HttpRequest &request);
		virtual void paintCategoryRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject);
		virtual void paintCategorySub(WebTemplate *tpl, int categoryId, int level, bool isShowOnlyProject);

		virtual void paintProduct(WebPage *page, HttpRequest &request);
		virtual void paintProductIndex(WebPage *page, HttpRequest &request);
		virtual void paintProductEdit(WebPage *page, HttpRequest &request);
		virtual void paintProductRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject);
		virtual void paintProductSelectCategoryRow(String sql, WebTemplate *tpl, int level);


		virtual void ajax(WebPage *page, HttpRequest &request);
		virtual void ajaxAcceptCategory(WebPage *page, HttpRequest &request);
		virtual void ajaxAddCategory(WebPage *page, HttpRequest &request);
		virtual void ajaxDeleteCategory(WebPage *page, HttpRequest &request);
		virtual void ajaxAcceptProduct(WebPage *page, HttpRequest &request);
		virtual void ajaxAddProduct(WebPage *page, HttpRequest &request);
		virtual void ajaxDeleteProduct(WebPage *page, HttpRequest &request);

		virtual void api(WebPage *page, HttpRequest &request);

	};
}