#include "cjCms.h"

namespace cj {

TaskModule::TaskModule(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(12);
}

void TaskModule::paint(WebPage *page, HttpRequest &request) {
	String cmd = request.header.GET.getValue("cmd");
	string cmd_8 = cmd.to_string();
	if (cmd == "ajax")
		return ajax(page, request);

	p1 = request.header.GET.getValue("p1");
	p2 = request.header.GET.getValue("p2");
	p3 = request.header.GET.getValue("p3");

	uuid = request.header.COOKIE.getValue("uuid");
	userId = manager->getUserId(uuid);

	MySQL *query = manager->newQuery();

	String sql = "select value from user_params where userId='" + (String)userId + "' and paramId=1";
	int count = query->active(sql);
	whoseTasks = 1;
	if (count > 0) whoseTasks = query->getFieldValue(0, "value").toInt();
	
	manager->deleteQuery(query);

	if (p2 == "" || p2 == "projects" || p2 == "priority") paintTable(page, request);
	else if (p2 == "edit") paintEdit(page, request);
	else paintDetail(page, request, p2);
}

void TaskModule::paintTable(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	String tplPath = manager->modulePath + "/task/table_tpl.html";
	if (tpl->open(tplPath)) {
		String sql;
		String uuid = request.header.COOKIE.getValue("uuid");
		int userId = manager->getUserId(uuid);
		paintWhoseTasks(tpl, userId);
		if (p2 == "projects") {
			tpl->out("checked2", "checked");
			sql = "select t.id, t.owner, t.name, t.about, t.procent, p.icon, is_project, s.id statusId, s.name sname, s.glyph from task t, task_status s, task_priority p where t.status=s.id and t.priority = p.id and isnull(owner) and isnull(deleted) and is_project=1";
			if (whoseTasks == 0) sql += " and (t.customer='" + (String)userId + "' or t.executor='" + (String)userId + "')";
			else sql += " and t.visibility=3 and t.customer='" + (String)whoseTasks + "'";
			sql += " order by t.id";
			paintRow(sql, tpl, 0, true);
		}
		else if (p2 == "priority") {
			tpl->out("checked3", "checked");
			sql = "select t.id, t.owner, t.name, t.about, t.procent, p.icon, s.id statusId, s.name sname, s.glyph from task t, task_status s, task_priority p where t.status = s.id and t.priority = p.id and isnull(deleted) and is_project=0";
			if (whoseTasks == 0) sql += " and (t.customer='" + (String)userId + "' or t.executor='" + (String)userId + "')";
			else sql += " and t.visibility=3 and t.customer='" + (String)whoseTasks + "'";
			sql += " order by t.status!=7 desc, t.priority, t.id";
			string sql8 = sql.to_string();
			paintPriorityRow(sql, tpl, 0, false);
		}
		else {
			tpl->out("checked1", "checked");

			sql = "select t.id, t.owner, t.name, t.about, t.procent, p.icon, is_project, s.id statusId, s.name sname, s.glyph from task t, task_status s, task_priority p where t.status=s.id and t.priority = p.id and isnull(owner) and isnull(deleted)";
			if (whoseTasks == 0) sql += " and (t.customer='" + (String)userId + "' or t.executor='" + (String)userId + "')";
			else sql += " and t.visibility=3 and t.customer='" + (String)whoseTasks + "'";
			sql += " order by t.id";
			paintRow(sql, tpl, 0, false);
		}

		tpl->exec();
		page->out("content", tpl->html);

		String email = manager->getEmail(uuid);

		WebTemplate tplContent;
		if (tplContent.open(manager->modulePath + "/task/createTask_tpl.html")) {
			tplContent.out("customer", email);
			tplContent.out("executor", email);
			tplContent.out("customerId", userId);
			tplContent.out("executorId", userId);
			tplContent.exec();
		}
		WebTemplate tplButtons;
		if (tplButtons.open(manager->modulePath + "/task/createTaskButtons_tpl.html")) {
			tplButtons.exec();
		}

		Modal modal("addTask", page->tplIndex, "content");
		modal.setCaption("Создать задачу");
		modal.setContent(tplContent.html);
		modal.setButtons(tplButtons.html);
		modal.paint();


		WebTemplate tplContentDelete;
		if (tplContentDelete.open(manager->modulePath + "/task/deleteTask_tpl.html")) {
			tplContentDelete.out("customer", email);
			tplContentDelete.out("executor", email);
			tplContentDelete.out("hidCustomerId", userId);
			tplContentDelete.out("hidExecutorId", userId);
			tplContentDelete.exec();
		}
		WebTemplate tplButtonsDelete;
		if (tplButtonsDelete.open(manager->modulePath + "/task/deleteTaskButtons_tpl.html")) {
			tplButtonsDelete.exec();
		}

		Modal modalDelete("deleteTask", page->tplIndex, "content");
		modalDelete.setCaption("Удалить задачу");
		modalDelete.setContent("Вы хотите удалить выбранную задачу?");
		modalDelete.setButtons(tplButtonsDelete.html);
		modalDelete.paint();

		page->out("css", "<link href=\"/modules/task/task.css\" rel=\"stylesheet\">");
		page->out("title", "Task manager");
		page->out("javascript", "<script src='/modules/task/task.js'></script>");

	}

}

void TaskModule::paintWhoseTasks(WebTemplate *tpl, int userId) {
	MySQL *query = manager->newQuery();

	String sql = "select value from user_params where userId='" + (String)userId + "' and paramId=1";
	int count = query->active(sql);
	int value = -1;
	if (count > 0) value = query->getFieldValue(0, "value").toInt();

	String selected = "";
	if (userId != 0) {
		if (value == -1) value = 0;
		if (value == 0) selected = "selected";
		tpl->out("userTask", "<option value='0' " + selected + ">Мои задачи</option>");
		selected = "";
	}
	else selected = "selected";
	if (value == 1) selected = "selected";
	tpl->out("userTask", "<option value='1' " + selected + ">Задачи Sitev.ru</option>");
	selected = "";

	manager->deleteQuery(query);
}
/*
void TaskModule::paintIndex(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	String userTpl = "index_tpl.html";

	String tplPath = manager->modulePath + "/task/" + userTpl;
	if (tpl->open(tplPath)) {
		String sql = "select t.id, t.owner, t.name, t.about, t.procent, is_project, s.id statusId, s.name sname, s.glyph from task t, task_status s where t.status=s.id and isnull(owner) and isnull(deleted) order by t.id";
		paintRow(sql, tpl, 0, false);
		tpl->exec();
		page->out("content", tpl->html);
		page->out("title", "Task manager");
	}
}

void TaskModule::paintProjects(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	String userTpl = "index_tpl.html";

	String tplPath = manager->modulePath + "/task/" + userTpl;
	if (tpl->open(tplPath)) {
		String sql = "select t.id, t.owner, t.name, t.about, t.procent, is_project, s.id statusId, s.name sname, s.glyph from task t, task_status s where t.status=s.id and isnull(owner) and isnull(deleted) and is_project=1 order by t.id";
		string sql8 = sql.to_string();
		paintRow(sql, tpl, 0, true);
		tpl->exec();
		page->out("content", tpl->html);
		page->out("title", "Task manager");
	}
}

void TaskModule::paintPriority(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	String userTpl = "priority_tpl.html";

	String tplPath = manager->modulePath + "/task/" + userTpl;
	if (tpl->open(tplPath)) {
		String sql = "select t.id, t.owner, t.name, t.about, t.procent, p.icon, s.id statusId, s.name sname, s.glyph from task t, task_status s, task_priority p where t.status = s.id and t.priority = p.id and isnull(deleted) and is_project=0 order by t.status!=7 desc, t.priority, t.id";
		paintPriorityRow(sql, tpl, 0, true);
		
		tpl->exec();
		page->out("content", tpl->html);

		String uuid = request.header.COOKIE.getValue("uuid");
		String email = manager->getEmail(uuid);
		int userId = manager->getUserId(uuid);

		WebTemplate tplContent;
		if (tplContent.open(manager->modulePath + "/task/createTask_tpl.html")) {
			tplContent.out("customer", email);
			tplContent.out("executor", email);
			tplContent.out("customerId", userId);
			tplContent.out("executorId", userId);
			tplContent.exec();
		}
		WebTemplate tplButtons;
		if (tplButtons.open(manager->modulePath + "/task/createTaskButtons_tpl.html")) {
			tplButtons.exec();
		}

		Modal modal("addTask", page->tplIndex, "content");
		modal.setCaption("Создать задачу");
		modal.setContent(tplContent.html);
		modal.setButtons(tplButtons.html);
		modal.paint();


		WebTemplate tplContentDelete;
		if (tplContentDelete.open(manager->modulePath + "/task/deleteTask_tpl.html")) {
			tplContentDelete.out("customer", email);
			tplContentDelete.out("executor", email);
			tplContentDelete.out("hidCustomerId", userId);
			tplContentDelete.out("hidExecutorId", userId);
			tplContentDelete.exec();
		}
		WebTemplate tplButtonsDelete;
		if (tplButtonsDelete.open(manager->modulePath + "/task/deleteTaskButtons_tpl.html")) {
			tplButtonsDelete.exec();
		}

		Modal modalDelete("deleteTask", page->tplIndex, "content");
		modalDelete.setCaption("Удалить задачу");
		modalDelete.setContent(tplContentDelete.html);
		modalDelete.setButtons(tplButtonsDelete.html);
		modalDelete.paint();

		page->out("title", "Task manager");
		page->out("javascript", "<script src=\"/modules/task/task.js\"></script>");
		page->out("css", "<link href=\"/modules/task/task.css\" rel=\"stylesheet\">");

	}

}
*/
void TaskModule::paintEdit(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();

	String sql = "select t.id, t.owner, t.name, t.about, u1.email customer, t.customer customerId, u2.email executor, t.executor executorId, t.visibility, t.procent, is_project, t.status, s.glyph, t.time, t.time_unit, t.complexity, t.priority, t.creation, deadline " +
		(String)"from task t, task_status s, users u1, users u2, task_visibility v, task_complexity c, task_priority p " +
		"where t.status=s.id and t.customer=u1.id and t.executor=u2.id and t.visibility=v.id and t.complexity=c.id and t.priority=p.id and isnull(deleted) and t.id='" + p3 + "'";
	sql += " and (t.customer='" + (String)userId + "')";

	bool flag = false;

	MySQL *query = manager->newQuery();
	int count = query->active(sql);
	if (count > 0) {
		String tplPath = manager->modulePath + "/task/edit_tpl.html";
		if (tpl->open(tplPath)) {
			flag = true;
			int taskId = query->getFieldValue(0, "id").toInt();
			int owner = query->getFieldValue(0, "owner").toInt();
			String name = query->getFieldValue(0, "name");
			String about = query->getFieldValue(0, "about");
			String customer = query->getFieldValue(0, "customer");
			int customerId = query->getFieldValue(0, "customerId").toInt();
			String executor = query->getFieldValue(0, "executor");
			int executorId = query->getFieldValue(0, "executorId").toInt();
			String visibility = query->getFieldValue(0, "visibility");
			String status = query->getFieldValue(0, "status");
			int procent = query->getFieldValue(0, "procent").toInt();
			bool is_project = query->getFieldValue(0, "is_project").toInt();
			int time = query->getFieldValue(0, "time").toInt();
			String timeUnit = query->getFieldValue(0, "time_unit");
			String complexity = query->getFieldValue(0, "complexity");
			String priority = query->getFieldValue(0, "priority");
			String creation = query->getFieldValue(0, "creation");
			String deadline = query->getFieldValue(0, "deadline");

			paintSubTask(tpl, taskId, 0, false);

			tpl->out("taskId", taskId);
			tpl->out("name", name);
			if (is_project) tpl->out("is-project", "checked");
			tpl->out("about", about);
			tpl->out("procent", procent);
			tpl->out("customer", customer);
			tpl->out("customerId", customerId);
			tpl->out("executor", executor);
			tpl->out("executorId", executorId);
			tpl->out("visibility" + visibility, "selected");
			tpl->out("time", time);
			tpl->out("time" + timeUnit, "selected");
			tpl->out("status" + status, "selected");
			tpl->out("complexity" + complexity, "selected");
			tpl->out("priority" + priority, "selected");
			tpl->out("creation", creation);
			tpl->out("deadline", deadline);

			sql = "select id from task where owner='" + (String)taskId + "'";
			if (query->exec(sql)) {
				if (query->storeResult()) {
					int count = query->getRowCount();
					if (count != 0)	tpl->out("visible", "none");
				}
			}
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
	page->out("javascript", "<script src=\"/modules/task/edit.js\"></script>");
}

void TaskModule::paintDetail(WebPage *page, HttpRequest &request, String p2) {
	WebTemplate *tpl = new WebTemplate();

	String sql = "select t.id, t.owner, t.name, t.about, u1.email customer, u2.email executor, v.name visibility, t.procent, is_project, s.id statusId, s.name sname, s.glyph, t.time, c.name complexity, p.name priority, t.creation, deadline " +
		(String)"from task t, task_status s, users u1, users u2, task_visibility v, task_complexity c, task_priority p " +
		"where t.status=s.id and t.customer=u1.id and t.executor=u2.id and t.visibility=v.id and t.complexity=c.id and t.priority=p.id and isnull(deleted) and t.id='" + p2 + "'";
	if (whoseTasks == 0) sql += " and (t.customer='" + (String)userId + "' or t.executor='" + (String)userId + "')";
	else sql += " and t.visibility=3 and t.customer='" + (String)whoseTasks + "'";
	MySQL *query = manager->newQuery();
	bool flag = false;
	int count = query->active(sql);
	if (count > 0) {
		String tplPath = manager->modulePath + "/task/detail_tpl.html";
		if (tpl->open(tplPath)) {
			flag = true;
			int taskId = query->getFieldValue(0, "id").toInt();
			int owner = query->getFieldValue(0, "owner").toInt();
			String name = query->getFieldValue(0, "name");
			String about = query->getFieldValue(0, "about");
			String customer = query->getFieldValue(0, "customer");
			String executor = query->getFieldValue(0, "executor");
			String visibility = query->getFieldValue(0, "visibility");
			int statusId = query->getFieldValue(0, "statusId").toInt();
			String sname = query->getFieldValue(0, "sname");
			String status = query->getFieldValue(0, "glyph");
			int procent = query->getFieldValue(0, "procent").toInt();
			bool isProject = query->getFieldValue(0, "is_project").toInt();
			int time = query->getFieldValue(0, "time").toInt();
			String complexity = query->getFieldValue(0, "complexity");
			String priority = query->getFieldValue(0, "priority");
			String creation = query->getFieldValue(0, "creation");
			String deadline = query->getFieldValue(0, "deadline");

			paintSubTask(tpl, taskId, 0, false);

			tpl->out("name", name);
			tpl->out("about", about);
			tpl->out("status", sname);
			tpl->out("procent", procent);
			tpl->out("customer", customer);
			tpl->out("executor", executor);
			tpl->out("visibility", visibility);
			tpl->out("time", time);
			tpl->out("complexity", complexity);
			tpl->out("priority", priority);
			tpl->out("creation", creation);
			tpl->out("deadline", deadline);

			sql = "select id from task where owner='" + (String)taskId + "'";
			if (query->exec(sql)) {
				if (query->storeResult()) {
					int count = query->getRowCount();
					if (count != 0)	tpl->out("visible", "none");
				}
			}
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
		page->out("title", "Task manager");
	}
}

void TaskModule::paintSubTask(WebTemplate *tpl, int taskId, int level, bool isShowOnlyProject) {
	String sql = "select t.id, t.owner, t.name, t.about, t.procent, p.icon, is_project, s.id statusId, s.name sname, s.glyph from task t, task_status s, task_priority p where t.`status`=s.id and t.priority = p.id and owner = '" + (String)taskId +
		"' and isnull(deleted)";
	if (isShowOnlyProject)	sql += " and is_project=1";
	if (whoseTasks == 0) sql += " and (t.customer='" + (String)userId + "' or t.executor='" + (String)userId + "')";
	else sql += " and t.visibility=3 and t.customer='" + (String)whoseTasks + "'";
	sql += " order by t.id";
	paintRow(sql, tpl, level, isShowOnlyProject);
}

void TaskModule::paintRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject) {
	//tpl->out("content", "paintRow ");
	MySQL *query = manager->newQuery();
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			count = count;
			for (int i = 0; i < count; i++) {
				int taskId = query->getFieldValue(i, "id").toInt();
				int owner = query->getFieldValue(i, "owner").toInt();
				String name = query->getFieldValue(i, "name");
				String about = query->getFieldValue(i, "about");
				int statusId = query->getFieldValue(i, "statusId").toInt();
				String sname = query->getFieldValue(i, "sname");
				String status = query->getFieldValue(i, "glyph");
				int procent = query->getFieldValue(i, "procent").toInt();
				String icon = query->getFieldValue(i, "icon");
				bool isProject = query->getFieldValue(i, "is_project").toInt();
				if (statusId == 4 || statusId == 5 || statusId == 6) status = "<span title='" + sname + "'>" + (String)procent + "%</span>";
				else status = "<span class='" + status + "' aria-hidden='true' title='" + sname + "'></span>";

				String s = "<tr class='treegrid-" + (String)taskId; 
				if (owner != 0 && (i != 0 || level != 0)) s += " treegrid-parent-" + (String)owner;
				
				s += "' style='cursor:pointer' data-id='" + (String)taskId + "'>\n<td><span class='treegrid-expander'></span><div style='display: inline-block;'>";
				s += "<a href='/" + p1 + "/" + (String)taskId + "'>";
				if (name != "") {
					if (isProject) s += "<b>" + name + "</b><br>";
					else s += name + "<br>";
				}
				if (about != "") s += "<small>" + about + "</small>";
				s += "</a></div></td><td align='center'><img src=\"/images/task/" + icon + "\"></td><td align='center'>" + status + "</td>\n</tr>\n";
				tpl->out("content", s);
				paintSubTask(tpl, taskId, level + 1, isShowOnlyProject);
			}
		}
	}
	manager->deleteQuery(query);
}

void TaskModule::paintPriorityRow(String sql, WebTemplate *tpl, int level, bool isShowOnlyProject) {
	MySQL *query = manager->newQuery();
	if (query->exec(sql)) {
		if (query->storeResult()) {
			int count = query->getRowCount();
			for (int i = 0; i < count; i++) {
				int taskId = query->getFieldValue(i, "id").toInt();
				int owner = query->getFieldValue(i, "owner").toInt();
				String name = query->getFieldValue(i, "name");
				String about = query->getFieldValue(i, "about");
				int statusId = query->getFieldValue(i, "statusId").toInt();
				String sname = query->getFieldValue(i, "sname");
				String status = query->getFieldValue(i, "glyph");
				int procent = query->getFieldValue(i, "procent").toInt();
				String icon = query->getFieldValue(i, "icon");
				bool isProject = query->getFieldValue(i, "is_project").toInt();
				if (statusId == 4 || statusId == 5 || statusId == 6) status = "<span title='" + sname + "'>" + (String)procent + "%</span>";
				else status = "<span class='" + status + "' aria-hidden='true' title='" + sname + "'></span>";

				String s = "<tr class='treegrid-" + (String)taskId;
				if (owner != 0 && (i != 0 || level != 0)) s += " treegrid-parent-" + (String)owner;

				s += "' style='cursor:pointer' data-id='" + (String)taskId;// + "' data-name1='" + name;// + /*"' data-about='" + about + */
					//"' data-isproject='" + (String)isProject;
				s += "'>\n<td><span class='treegrid-expander'></span><div style='display: inline-block;'>";
				s += "<a href='/" + p1 + "/" + (String)taskId + "'>";
				if (name != "") {
					if (isProject) s += "<b>" + name + "</b><br>";
					else s += name + "<br>";
				}
				if (about != "") s += "<small>" + about + "</small>";
				s += "</a></div></td><td align='center'><img src=\"/images/task/" + icon + "\"></td><td align='center'>" + status + "</td>\n</tr>\n";
				tpl->out("content", s);
				//paintSubTask(tpl, taskId, level + 1, isShowOnlyProject);
			}
		}
	}
	manager->deleteQuery(query);

}

void TaskModule::ajax(WebPage *page, HttpRequest &request) {
	String p2 = request.header.GET.getValue("p2");
	string p2_8 = p2.to_string();

	if (p2 == "accept") ajaxAccept(page, request);
	else if (p2 == "add") ajaxAdd(page, request);
	else if (p2 == "delete") ajaxDelete(page, request);
	else if (p2 == "change") ajaxChange(page, request);
}

void TaskModule::ajaxAccept(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int taskId = request.header.POST.getValue("taskId").toInt();
	String name = request.header.POST.getValue("name");
	String about = request.header.POST.getValue("about");

	bool isProject = request.header.POST.getValue("isProject").toInt();
	String customer = request.header.POST.getValue("customer");
	String executor = request.header.POST.getValue("executor");
	int visibility = request.header.POST.getValue("visibility").toInt();
	int timeUnit = request.header.POST.getValue("timeUnit").toInt();
	int time = request.header.POST.getValue("time").toInt();
	int status = request.header.POST.getValue("status").toInt();
	int procent = request.header.POST.getValue("procent").toInt();
	int complexity = request.header.POST.getValue("complexity").toInt();
	int priority = request.header.POST.getValue("priority").toInt();
	String creation = request.header.POST.getValue("creation");
	String deadline = request.header.POST.getValue("deadline");

	DateTime dt;
	dt.now();
	if (creation == "") creation = dt.toString();
	if (deadline == "") deadline = dt.toString();

	String sql = "update task set name='" + name + "', about='" + about + 
		"', is_project='" + (String)isProject +
		"', customer='" + customer +
		"', executor='" + executor +
		"', visibility='" + (String)visibility +
		"', time_unit='" + (String)timeUnit +
		"', time='" + (String)time +
		"', status='" + (String)status +
		"', procent='" + (String)procent +
		"', complexity='" + (String)complexity +
		"', priority='" + (String)priority +
		"', creation='" + creation +
		"', deadline='" + deadline +
		"' where id='" + (String)taskId + "'";
	string sql8 = sql.to_string();
	if (query->exec(sql)) {
		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "</note>\n");
	}

	manager->deleteQuery(query);
}

void TaskModule::ajaxAdd(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int owner = request.header.POST.getValue("owner").toInt();
	String sowner = owner;
	if (owner == 0) sowner = "null";
	String name = request.header.POST.getValue("name");
	String about = request.header.POST.getValue("about");
	bool isProject = request.header.POST.getValue("isProject").toInt();
	int customer = request.header.POST.getValue("customer").toInt();
	int executor = request.header.POST.getValue("executor").toInt();
	int visibility = request.header.POST.getValue("visibility").toInt();
	int timeUnit = request.header.POST.getValue("timeUnit").toInt();
	int time = request.header.POST.getValue("time").toInt();
	int status = request.header.POST.getValue("status").toInt();
	int procent = request.header.POST.getValue("procent").toInt();
	int complexity = request.header.POST.getValue("complexity").toInt();
	int priority = request.header.POST.getValue("priority").toInt();
	String creation = request.header.POST.getValue("creation");
	String deadline = request.header.POST.getValue("deadline");

	DateTime dt;
	dt.now();
	if (creation == "") creation = dt.toString();
	if (deadline == "") deadline = dt.toString();

	String sql = "insert into task (owner, name, about, is_project, customer, executor, visibility, time_unit, time, status, procent, complexity, priority, creation, deadline) values (" +
		sowner + ", '" + name + "', '" + about + "', '" + isProject + "', '" + customer + "', '" + executor + "', '" + visibility + "', '" + timeUnit + "', '" + time + "', '" + status + "', '" +
		procent + "', '" + complexity + "', '" + priority + "', '" + creation + "', '" + deadline + "')";
	string sql8 = sql.to_string();
	if (query->exec(sql)) {
		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "</note>\n");
	}

	manager->deleteQuery(query);
}

void TaskModule::ajaxDelete(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();

	int taskId = request.header.POST.getValue("taskId").toInt();
	String sql = "update task set deleted=1 where id='" + (String)taskId + "'";
	string sql8 = sql.to_string();
	if (query->exec(sql)) {
		page->tplIndex->out("out", "<note>\n");
		page->tplIndex->out("out", "<result>1</result>\n");
		page->tplIndex->out("out", "</note>\n");
	}

	manager->deleteQuery(query);
}

void TaskModule::ajaxChange(WebPage *page, HttpRequest &request) {
	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);
	int value = request.header.POST.getValue("userId").toInt();

	MySQL *query = manager->newQuery();

	String sql = "select id from user_params where userId='" + (String)userId + "' and paramId=1";
	int count = query->active(sql);
	if (count > 0) sql = "update user_params set value='" + (String)value + "' where userId='" + (String)userId + "' and paramId=1";
	else sql = "insert into user_params (userId, paramId, value) values ('" + (String)userId + "', 1, '" + (String)value + "')";
	if (query->exec(sql)) {
		page->tplIndex->out("out", "<note><result>1</result></note>");
	}

	manager->deleteQuery(query);
}

}