#include "cms.h"

namespace cms {
	Shop::Shop(SiteManager *manager) : WebModule(manager) {
		setOptionsFromDB(14);
	}

	void Shop::paint(WebPage *page, HttpRequest &request) {
		uuid = request.header.COOKIE.getValue("uuid");
		userId = manager->getUserId(uuid);

		String cmd = request.header.GET.getValue("cmd");
		if (cmd == "ajax")
			return ajax(page, request);
		p2 = request.header.GET.getValue("p2");
		if (p2 == "api") 
			return api(page, request);
		
		if (p2 == "category") paintCategory(page, request);
		else if (p2 == "product") paintProduct(page, request);
		else paintIndex(page, request);
	}

	void Shop::paintIndex(WebPage *page, HttpRequest &request) {
		String tplPath = manager->modulePath + "/shop/shop_tpl.html";
		WebTemplate *tpl = new WebTemplate();
		if (tpl->open(tplPath)) {
			int parent = p2.toInt();

			MySQL *query = manager->newQuery();
			String sql;
			if (parent == 0) sql = "select * from shop_category where isnull(deleted) and isnull(parent) order by id";
			else sql = "select * from shop_category where isnull(deleted) and parent='" + to_string(parent) + "' order by id";
			
			int count = query->active(sql);
			for (int i = 0; i < count; i++) {
				String id = query->getFieldValue(i, "id");
				String name = query->getFieldValue(i, "name");
				tpl->out("category", "<div class=\"col-sm-2\"><a href=\"/shop/" + id + "\">" + name + "</a></div>");
			}
			manager->deleteQuery(query);

			sql = "select p.name, p.about, c.name category, p.price from shop_products p, shop_category c where ";
			sql += "p.category_id='" + to_string(parent) + "' and p.category_id=c.id and isnull(p.deleted) order by p.id";
			paintProductRow(sql, tpl, 0, false);
			tpl->exec();
		}
		page->out("content", tpl->html);
		delete tpl;
	}

	void Shop::paintCategory(WebPage *page, HttpRequest &request) {
		p3 = request.header.GET.getValue("p3");
		p4 = request.header.GET.getValue("p4");
		if (p3 == "") paintCategoryIndex(page, request);
		else if (p3 == "edit") paintCategoryEdit(page, request);
	}

	void Shop::paintCategoryIndex(WebPage *page, HttpRequest &request) {
		String tplPath = manager->modulePath + "/shop/category_tpl.html";
		WebTemplate *tpl = new WebTemplate();
		if (tpl->open(tplPath)) {
			MySQL *query = manager->newQuery();

			String sql = "select * from shop_category where isnull(deleted) and isnull(parent) order by id";
			paintCategoryRow(sql, tpl, 0, false);

			tpl->exec();
		}
		page->out("content", tpl->html);

		String email = manager->getEmail(uuid);


		WebTemplate tplContent; 
		if (tplContent.open(manager->modulePath + "/shop/createCategory_tpl.html")) {
			tplContent.exec();
		}
		WebTemplate tplButtons;
		if (tplButtons.open(manager->modulePath + "/shop/createCategoryButtons_tpl.html")) {
			tplButtons.exec();
		}

		Modal modal("addTask", page->tplIndex, "content");
		modal.setCaption("Create task");
		modal.setContent(tplContent.html);
		modal.setButtons(tplButtons.html);
		modal.paint();


		WebTemplate tplContentDelete;
		if (tplContentDelete.open(manager->modulePath + "/shop/deleteCategory_tpl.html")) {
			tplContentDelete.exec();
		}
		WebTemplate tplButtonsDelete;
		if (tplButtonsDelete.open(manager->modulePath + "/shop/deleteCategoryButtons_tpl.html")) {
			tplButtonsDelete.exec();
		}

		Modal modalDelete("deleteTask", page->tplIndex, "content");
		modalDelete.setCaption("РЈРґР°Р»РёС‚СЊ РєР°С‚РµРіРѕСЂРёСЋ");
		modalDelete.setContent("Р’С‹ С…РѕС‚РёС‚Рµ СѓРґР°Р»РёС‚СЊ РІС‹Р±СЂР°РЅРЅСѓСЋ РєР°С‚РµРіРѕСЂРёСЋ?");
		modalDelete.setButtons(tplButtonsDelete.html);
		modalDelete.paint();

		page->out("css", "<link href=\"/modules/shop/shop.css\" rel=\"stylesheet\">");
		page->out("title", "Shop");
		page->out("javascript", "<script src='/modules/shop/shop.js'></script>");
		delete tpl;
	}

	void Shop::paintCategoryEdit(WebPage *page, HttpRequest &request) {
		WebTemplate *tpl = new WebTemplate();

		String sql = "select id, parent, name, about " +
			(String)"from shop_category " +
			"where isnull(deleted) and id='" + p4 + "'";

		bool flag = false;

		MySQL *query = manager->newQuery();
		int count = query->active(sql);
		if (count > 0) {
			String tplPath = manager->modulePath + "/shop/editCategory_tpl.html";
			if (tpl->open(tplPath)) {
				flag = true;
				int taskId = query->getFieldValue(0, "id").toInt();
				int parent = query->getFieldValue(0, "parent").toInt();
				String name = query->getFieldValue(0, "name");
				String about = query->getFieldValue(0, "about");

				paintCategorySub(tpl, taskId, 0, false);

				tpl->out("taskId", taskId);
				tpl->out("name", name);
				tpl->out("about", about);

			}
			manager->deleteQuery(query);
		}
		else {
			String tplPath = manager->modulePath + "/task/notAvailable_tpl.html";
			if (tpl->open(tplPath)) flag = true;
		}

		if (flag) {
			tpl->exec();
			page->out("content", tpl->html);
		}
		page->out("title", "Task manager");
		page->out("javascript", "<script src=\"/modules/shop/edit.js\"></script>");
	}

	void Shop::paintCategoryRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject) {
		//tpl->out("content", "paintRow ");
		MySQL *query = manager->newQuery();
		if (query->exec(sql)) {
			if (query->storeResult()) {
				int count = query->getRowCount();
				count = count;
				for (int i = 0; i < count; i++) {
					int categoryId = query->getFieldValue(i, "id").toInt();
					int parent = query->getFieldValue(i, "parent").toInt();
					String name = query->getFieldValue(i, "name");
					String about = query->getFieldValue(i, "about");

					String s = "<tr class='treegrid-" + (String)categoryId;
					if (parent != 0 && (i != 0 || level != 0)) s += " treegrid-parent-" + (String)parent;

					s += "' style='cursor:pointer' data-id='" + (String)categoryId + "'>\n<td><span class='treegrid-expander'></span><div style='display: inline-block;'>";
					s += "<a href='/" + p1 + "/" + (String)categoryId + "'>";
					if (name != "") s += name + "<br>";
					if (about != "") s += "<small>" + about + "</small>";
					s += "</a></div></td></tr>\n";
					tpl->out("content", s);
					paintCategorySub(tpl, categoryId, level + 1, isShowOnlyProject);
				}
			}
		}
		manager->deleteQuery(query);
	}

	void Shop::paintCategorySub(WebTemplate *tpl, int categoryId, int level, bool isShowOnlyProject) {
		String sql = "select * from shop_category where parent = '" + (String)categoryId +
			"' and isnull(deleted) order by id";
		paintCategoryRow(sql, tpl, level, isShowOnlyProject);
	}

	void Shop::paintProduct(WebPage *page, HttpRequest &request) {
		p3 = request.header.GET.getValue("p3");
		p4 = request.header.GET.getValue("p4");
		if (p3 == "") paintProductIndex(page, request);
		else if (p3 == "edit") paintProductEdit(page, request);
	}

	void Shop::paintProductIndex(WebPage *page, HttpRequest &request) {
		String tplPath = manager->modulePath + "/shop/product_tpl.html";
		WebTemplate *tpl = new WebTemplate();
		if (tpl->open(tplPath)) {
			MySQL *query = manager->newQuery();

			String sql = "select p.name, p.about, c.name category, p.price from shop_products p, shop_category c where p.category_id=c.id and isnull(p.deleted) order by p.id";
			paintProductRow(sql, tpl, 0, false);

			tpl->exec();
		}
		page->out("content", tpl->html);

		String email = manager->getEmail(uuid);


		WebTemplate tplContent;
		if (tplContent.open(manager->modulePath + "/shop/createProduct_tpl.html")) {
			String sql = "select * from shop_category where isnull(parent) and isnull(deleted) order by id";
			paintProductSelectCategoryRow(sql, &tplContent, 0);

			//tplContent.out("category", "<option>option 1</option>");
			tplContent.exec();
		}
		WebTemplate tplButtons;
		if (tplButtons.open(manager->modulePath + "/shop/createProductButtons_tpl.html")) {
			tplButtons.exec();
		}

		Modal modal("addTask", page->tplIndex, "content");
		modal.setCaption("Create task");
		modal.setContent(tplContent.html);
		modal.setButtons(tplButtons.html);
		modal.paint();


		WebTemplate tplContentDelete;
		if (tplContentDelete.open(manager->modulePath + "/shop/deleteProduct_tpl.html")) {
			tplContentDelete.exec();
		}
		WebTemplate tplButtonsDelete;
		if (tplButtonsDelete.open(manager->modulePath + "/shop/deleteProductButtons_tpl.html")) {
			tplButtonsDelete.exec();
		}

		Modal modalDelete("deleteTask", page->tplIndex, "content");
		modalDelete.setCaption("РЈРґР°Р»РёС‚СЊ РєР°С‚РµРіРѕСЂРёСЋ");
		modalDelete.setContent("Р’С‹ С…РѕС‚РёС‚Рµ СѓРґР°Р»РёС‚СЊ РІС‹Р±СЂР°РЅРЅСѓСЋ РєР°С‚РµРіРѕСЂРёСЋ?");
		modalDelete.setButtons(tplButtonsDelete.html);
		modalDelete.paint();

		page->out("css", "<link href=\"/modules/shop/shop.css\" rel=\"stylesheet\">");
		page->out("title", "Shop");
		page->out("javascript", "<script src='/modules/shop/product.js'></script>");
		delete tpl;
	}

	void Shop::paintProductEdit(WebPage *page, HttpRequest &request) {
		WebTemplate *tpl = new WebTemplate();

		String sql = "select id, parent, name, about " +
			(String)"from shop_products " +
			"where isnull(deleted) and id='" + p4 + "'";

		bool flag = false;

		MySQL *query = manager->newQuery();
		int count = query->active(sql);
		if (count > 0) {
			String tplPath = manager->modulePath + "/shop/editProduct_tpl.html";
			if (tpl->open(tplPath)) {
				flag = true;
				int taskId = query->getFieldValue(0, "id").toInt();
				int parent = query->getFieldValue(0, "parent").toInt();
				String name = query->getFieldValue(0, "name");
				String about = query->getFieldValue(0, "about");

				//paintProductSub(tpl, taskId, 0, false);

				tpl->out("taskId", taskId);
				tpl->out("name", name);
				tpl->out("about", about);
			}
			manager->deleteQuery(query);
		}
		else {
			String tplPath = manager->modulePath + "/task/notAvailable_tpl.html";
			if (tpl->open(tplPath)) flag = true;
		}

		if (flag) {
			tpl->exec();
			page->out("content", tpl->html);
		}
		page->out("title", "Task manager");
		page->out("javascript", "<script src=\"/modules/shop/edit.js\"></script>");
	}

	void Shop::paintProductRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject) {
		MySQL *query = manager->newQuery();
		if (query->exec(sql)) {
			if (query->storeResult()) {
				int count = query->getRowCount();
				count = count;
				for (int i = 0; i < count; i++) {
					int categoryId = query->getFieldValue(i, "id").toInt();
					int parent = query->getFieldValue(i, "parent").toInt();
					String name = query->getFieldValue(i, "name");
					String about = query->getFieldValue(i, "about");
					String category = query->getFieldValue(i, "category");
					int price = query->getFieldValue(i, "price").toInt();

					String s = "<tr class='treegrid-" + (String)categoryId;
					if (parent != 0 && (i != 0 || level != 0)) s += " treegrid-parent-" + (String)parent;

					s += "' style='cursor:pointer' data-id='" + (String)categoryId + "'>\n<td><span class='treegrid-expander'></span><div style='display: inline-block;'>";
					s += "<a href='/" + p1 + "/" + (String)categoryId + "'>";
					if (name != "") s += name + "<br>";
					if (about != "") s += "<small>" + about + "</small>";
					s += "</a></div></td><td>" + category + "</td><td>" + to_string(price) + "</td></tr>\n";
					tpl->out("content", s);
					//paintProductSub(tpl, categoryId, level + 1, isShowOnlyProject);
				}
			}
		}
		manager->deleteQuery(query);
	}

	void Shop::paintProductSelectCategoryRow(String sql, WebTemplate *tpl, int level) {
		MySQL *query = manager->newQuery();
		if (query->active(sql)) {
			int count = query->getRowCount();
			count = count;
			for (int i = 0; i < count; i++) {
				int categoryId = query->getFieldValue(i, "id").toInt();
				int parent = query->getFieldValue(i, "parent").toInt();
				String name = query->getFieldValue(i, "name");
				String about = query->getFieldValue(i, "about");

				String space = "";
				for (int i = 0; i < level; i++) space += "- ";

				String s = (String)"<option value='" + (String)categoryId + "'>" + space + name + "</option>";

				tpl->out("category", s);

				String sql = (String)"select * from shop_category where parent='" + (String)categoryId + "' and isnull(deleted) order by id";
				paintProductSelectCategoryRow(sql, tpl, level + 1);
				//paintCategorySub(tpl, categoryId, level + 1, isShowOnlyProject);
			}
		}
		manager->deleteQuery(query);
	}

	void Shop::ajax(WebPage *page, HttpRequest &request) {
		String p2 = request.header.GET.getValue("p2");
		String p3 = request.header.GET.getValue("p3");

		if (p2 == "category") {
			if (p3 == "accept") ajaxAcceptCategory(page, request);
			else if (p3 == "add") ajaxAddCategory(page, request);
			else if (p3 == "delete") ajaxDeleteCategory(page, request);
		}
		else if (p2 == "product") {
			if (p3 == "accept") ajaxAcceptProduct(page, request);
			else if (p3 == "add") ajaxAddProduct(page, request);
			else if (p3 == "delete") ajaxDeleteProduct(page, request);
		}
	}

	void Shop::ajaxAcceptCategory(WebPage *page, HttpRequest &request) {
		MySQL *query = manager->newQuery();

		int categoryId = request.header.POST.getValue("categoryId").toInt();
		String name = request.header.POST.getValue("name");
		String about = request.header.POST.getValue("about");

		String sql = "update shop_category set name='" + name + "', about='" + about +
			"' where id='" + (String)categoryId + "'";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}

		manager->deleteQuery(query);
	}

	void Shop::ajaxAddCategory(WebPage *page, HttpRequest &request) {
		MySQL *query = manager->newQuery();

		int parent = request.header.POST.getValue("parent").toInt();
		String sparent = parent;
		if (parent == 0) sparent = "null";
		String name = request.header.POST.getValue("name");
		String about = request.header.POST.getValue("about");

		String sql = "insert into shop_category (parent, name, about) values (" +
			sparent + ", '" + name + "', '" + about + "')";
		string sql8 = sql.to_string();
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}

		manager->deleteQuery(query);
	}

	void Shop::ajaxDeleteCategory(WebPage *page, HttpRequest &request) {
		MySQL *query = manager->newQuery();

		int categoryId = request.header.POST.getValue("categoryId").toInt();
		String sql = "update shop_category set deleted=1 where id='" + (String)categoryId + "'";
		string sql8 = sql.to_string();
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}

		manager->deleteQuery(query);
	}

	void Shop::ajaxAcceptProduct(WebPage *page, HttpRequest &request) {
		MySQL *query = manager->newQuery();

		int categoryId = request.header.POST.getValue("categoryId").toInt();
		String name = request.header.POST.getValue("name");
		String about = request.header.POST.getValue("about");

		String sql = "update shop_products set name='" + name + "', about='" + about +
			"' where id='" + (String)categoryId + "'";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}

		manager->deleteQuery(query);
	}

	void Shop::ajaxAddProduct(WebPage *page, HttpRequest &request) {
		MySQL *query = manager->newQuery();

		String name = request.header.POST.getValue("name");
		String about = request.header.POST.getValue("about");
		int price = request.header.POST.getValue("price").toInt();
		int category = request.header.POST.getValue("category").toInt();

		String sql = "insert into shop_products (name, about, price, category_id) values ('" + name + "', '" + about + "', '" + price + "', '" + category + "')";
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}

		manager->deleteQuery(query);
	}

	void Shop::ajaxDeleteProduct(WebPage *page, HttpRequest &request) {
		MySQL *query = manager->newQuery();

		int categoryId = request.header.POST.getValue("categoryId").toInt();
		String sql = "update shop_products set deleted=1 where id='" + (String)categoryId + "'";
		string sql8 = sql.to_string();
		if (query->exec(sql)) {
			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<result>1</result>\n");
			page->tplIndex->out("out", "</note>\n");
		}

		manager->deleteQuery(query);
	}

	void Shop::api(WebPage *page, HttpRequest &request) { }

}