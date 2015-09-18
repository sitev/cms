#include "cjCms.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class UserModule          ----------------------------------------------------
//--------------------------------------------------------------------------------------------------

UserModule::UserModule(SiteManager *manager) : WebModule(manager) {
	setOptionsFromDB(9);
}

void UserModule::paint(WebPage *page, HttpRequest &request) {
	printf("UserModule::paint\n");
	String cmd = request.header.GET.getValue("cmd");
	if (cmd == "ajax") 
		return ajax(page, request);

	String p2 = request.header.GET.getValue("p2");

	if (p2 == "") paintAbout(page, request);
	else if (p2 == "recovery") paintRecovery(page, request);
	else if (p2 == "signup") paintSignup(page, request);
	else if (p2 == "sendAccount") sendAccount(page, request);
	else if (p2 == "activate") activate(page, request);
	else if (p2 == "changePassword") changePassword(page, request);
	else paint404(page, request);
}

void UserModule::paintAbout(WebPage *page, HttpRequest &request) {
	WebTemplate *tpl = new WebTemplate();
	String userTpl = "user_tpl.html";

	String uuid = request.header.COOKIE.getValue("uuid");
	String login = page->site->manager->getLogin(uuid);
	if (login == "") userTpl = "userNoEnter_tpl.html";

	String tplPath = manager->modulePath + "/user/" + userTpl;
	if (tpl->open(tplPath)) {
		tpl->out("email", login);
		tpl->out("money", "0");
		tpl->exec();
		page->out("content", tpl->html);
	}
}
void UserModule::paintRecovery(WebPage *page, HttpRequest &request) {
	WebTemplate * tpl = new WebTemplate();
	String userTpl = "recovery_tpl.html";
	if (tpl->open(manager->modulePath + "/user/" + userTpl)) {
		tpl->exec();
		page->out("content", tpl->html);
	}
}
void UserModule::paintSignup(WebPage *page, HttpRequest &request) {
	WebTemplate * tpl = new WebTemplate();
	String userTpl = "signup_tpl.html";
	if (tpl->open(manager->modulePath + "/user/" + userTpl)) {
		tpl->exec();
		page->out("content", tpl->html);
	}
}
void UserModule::sendAccount(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String guid = generateUUID();
	String email = request.header.POST.getValue("email");
	String login = request.header.POST.getValue("login");
	if (email != "") {
		String password = manager->generateUserPassword();
		String sql = "select * from users where email='" + email + "'";
		if (query->exec(sql)) {
			if (query->storeResult()) {
				int count = query->getRowCount();
				if (count > 0) {
					guid = query->getFieldValue(0, "uuid");
					sql = "update users set newPassword='" + password + "', uuid='" + guid + "' where email='" + email + "'";
					if (query->exec(sql)) {}
				}
				else {
					sql = "insert into users (email, login, newPassword, uuid) values('" + email + "', '" + login + "', '" + password + "', '" + guid + "')";
					if (query->exec(sql)) {}
				}
			}
		}
		WebTemplate * tplEmail = new WebTemplate();
		String userTpl = "email_tpl.html";
		if (tplEmail->open(manager->modulePath + "/user/" + userTpl)) {
			tplEmail->out("host", page->site->host);
			tplEmail->out("email", email);
			tplEmail->out("password", password);
			tplEmail->out("guid", guid);
			tplEmail->exec();
			sendMail(email, "no-reply@" + page->site->host, page->site->host + ": подтверждение аккаунта", tplEmail->html);
		}

		WebTemplate * tpl = new WebTemplate();
		if (tpl->open(manager->modulePath + "/user/loginSendAccount_tpl.html")) {
			tpl->out("out", email);
			tpl->exec();
			page->out("content", tpl->html);
		}
	}
	manager->deleteQuery(query);
}

void UserModule::activate(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String p3 = request.header.GET.getValue("p3");
	String sql = "update users set active = '1', password=newPassword where uuid = '" + p3 + "'";
	WebTemplate * tpl = new WebTemplate();
	String activateTpl = "";
	if (query->exec(sql)) {
		activateTpl = "activateSuccess_tpl.html";
	}
	else {
		activateTpl = "activateFail_tpl.html";
	}
	if (tpl->open(manager->modulePath + "/user/" + activateTpl)) {
		tpl->exec();
		page->out("content", tpl->html);
	}
	manager->deleteQuery(query);
}
void UserModule::changePassword(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String uuid = request.header.COOKIE.getValue("uuid");
	int userId = manager->getUserId(uuid);

	WebTemplate * tpl = new WebTemplate();
	String p3 = request.header.GET.getValue("p3");
	if (p3 == "") {
		if (userId != 0) {
			if (tpl->open(manager->modulePath + "/2/changePassword_tpl.html")) {
				tpl->exec();
				page->out("out", tpl->html);
			}
		}
		else {
			if (tpl->open(manager->documentRoot + "/tpl/message_tpl.html")) {
				tpl->out("caption", "����� ������");
				tpl->out("error", "��� ����� ������ ������� �� ���� ��� ����� ������� � ������ �������");
				tpl->exec();
				page->out("out", tpl->html);
			}
		}
	}
	else if (p3 == "done") {
		if (tpl->open(manager->documentRoot + "/tpl/message_tpl.html")) {
			tpl->out("caption", "����� ������");

			String message, error;

			if (userId != 0) {
				String oldPassword = request.header.POST.getValue("oldPassword");
				String sql = "select * from users where id='" + (String)userId + "' and password='" + oldPassword + "'";
				if (query->exec(sql)) {
					if (query->storeResult()) {
						int count = query->getRowCount();
						if (count > 0) {
							String newPassword = request.header.POST.getValue("newPassword");
							String repeatPassword = request.header.POST.getValue("repeatPassword");
							error = manager->isPasswordCorrect(newPassword);
							if (error == "") {
								if (newPassword == repeatPassword) {
									String sql = "update users set password='" + newPassword + "' where id='" + userId + "'";
									if (query->exec(sql)) {
										message = "������ ��� ������� ������!";
									}
									else {
										error = "������ ���� ������";
									}
								}
								else error = "������ �� ���������";
							}
						}
						else error = "������ ������ ����� �� ���������";
					}
				}
			}
			else {
				error = "��� ����� ������ ������� �� ���� ��� ����� ������� � ������ �������";
			}
			tpl->out("message", message);
			tpl->out("error", error);
			tpl->exec();
			page->out("out", tpl->html);
		}
	}
}
void UserModule::ajax(WebPage *page, HttpRequest &request) {
	MySQL *query = manager->newQuery();
	String obj = request.header.GET.getValue("p1");
	String func = request.header.GET.getValue("p2");
	String uuid = request.header.COOKIE.getValue("uuid");

	if (obj == "user") {
		if (func == "login") {
			String login = request.header.POST.getValue("login");
			String password = request.header.POST.getValue("password");
			String chkSave = request.header.POST.getValue("chkSave");

			page->tplIndex->out("out", "<note>\n");
			page->tplIndex->out("out", "<login>" + login + "</login>\n");

			String sql = (String)"select * from users where (email='" + login + "' or login='" + login + "') and password='" + password + "'";
			if (query->exec(sql)) {
				if (query->storeResult()) {
					int count = query->getRowCount();
					if (count > 0) {
						String userId = query->getFieldValue(0, "id");
						//						if (userId == 1) {
						if (true) {
							String sql = (String)"insert into uuid (uuid, userId, sec) values ('" + uuid + "', '" + userId + "', '10000')";
							if (chkSave == "")
								sql = (String)"insert into uuid (uuid, userId, sec) values ('" + uuid + "', '" + userId + "', '0')";
							if (query->exec(sql)) {
								page->tplIndex->out("out", "<result>" + ((String)1) + "</result>\n");
							}

							bool flag = false;
							sql = "select serviceId, u.id from uuidPartner up, users u where isnull(up.deleted) and up.userId=u.id and cookie='" + uuid + "' order by u.id";
							if (query->exec(sql)) {
								if (query->storeResult()) {
									int count = query->getRowCount();
									if (count > 0) {
										flag = true;
										String ref = query->getFieldValue(0, "id");
										String serviceId = query->getFieldValue(0, "serviceId");

										sql = "select * from users where id='" + userId + "' and isnull(ref" + serviceId + ")";
										if (query->exec(sql)) {
											if (query->storeResult()) {
												int count = query->getRowCount();
												if (count > 0) {
													//String ref1_1 = query->getFieldValue(0, "ref1_1");
													sql = "update users set ref" + serviceId + "='" + ref + "' where id='" + userId + "'";
													query->exec(sql);

													for (int i = 1; i <= 4; i++) {
														String si = i;
														sql = "update users set ref" + serviceId + "_" + si + "=ref" + serviceId + "_" + si + "+1 where id='" + ref + "'";
														query->exec(sql);

														sql = "select * from users where id='" + ref + "' and ref1<>id";
														if (query->exec(sql)) {
															if (query->storeResult()) {
																int count = query->getRowCount();
																if (count > 0) {
																	ref = query->getFieldValue(0, "ref" + serviceId);
																}
																else break;
															}
														}
													}
												}
											}
										}
										sql = "update uuidPartner set deleted=1 where cookie='" + uuid + "'";
										query->exec(sql);
									}
								}
							}

							if (flag == false) {
								sql = "update users set ref1=0 where id='" + userId + "'";
								query->exec(sql);
							}

						}
						else {
							page->tplIndex->out("out", "<error>���� �������� ������������ - ���� � ������ ����������...</error>");
						}
					}
				}
			}
		}
		else if (func == "logout") {
			/*
			String login = request.header.POST.getValue("login");
			if (login != "") {
				int userId = getUserId(login);
				String sql = (String)"delete from uuid where userId='" + (String)userId + "'";
				page->tplIndex->out("out", "<login>" + login + "</login>\n");
				if (query->exec(sql)) {
					page->tplIndex->out("out", "<result>1</result>");
				}
			}
			*/
		}
		page->tplIndex->out("out", "</note>\n");
	}
	manager->deleteQuery(query);
}

}
