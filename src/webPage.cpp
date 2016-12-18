#include "cjCMS.h"

namespace cj {

//--------------------------------------------------------------------------------------------------
//----------          class WebPage          -------------------------------------------------------
//--------------------------------------------------------------------------------------------------

WebPage::WebPage(WebSite *site, string page, int pageId, WebModule *module, String design) {
    this->site = site;
    this->page = page;
    this->pageId = pageId;
    this->module = module;
    this->design = design;
    moduleId = 0; //0 - никакой модуль не привязан
    tplIndex = new WebTemplate();
    tplLayout = new WebTemplate();
}

void WebPage::out(String s) {
    int len = s.to_string().length();
    response->memory.write((void*)(s.to_string().c_str()), len);
}

void WebPage::out(String tag, String s) {
    if (isLayout && (tag == "content" || tag == "sidebar" || tag == "sidebar2"))
        tplLayout->out(tag, s);
    else
        tplIndex->out(tag, s);
}

void WebPage::paint(HttpRequest &request, HttpResponse &response) {
    this->response = &response;
    if (site == NULL) return;
    if (site->manager == NULL) return;
    if (site->host == "") return;

    String cmd = request.header.GET.getValue("cmd");
    if (cmd == "ajax") {
        paintAjax(request);
        return;
    }

    clearAllTags();
    paintTemplates();

    site->manager->paintMainMenu(site->siteId, tplIndex);
    this->module->paint(this, request);
    site->manager->widgetManager.paintPageWidgets(this);

    String uuid = request.header.COOKIE.getValue("uuid");
    if (uuid.getLength() < 10) uuid = generateUUID();
    paintUser(uuid);

    tplLayout->exec();
    tplIndex->out("content", tplLayout->html);
    paintStdTags();
    tplIndex->exec();

    string t8 = tplIndex->html.to_string();
    int len = t8.length();
    this->out("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n");
    this->out("Set-Cookie: uuid=" + uuid + "\r\nContent-Length: " + (String)len + "\r\n\r\n");
    this->out(tplIndex->html);
}

void WebPage::paintTemplates() {
    String fn;

    isLayout = false;
    MySQL *query = site->manager->newQuery();
    String sql = "select s.design, s.theme, s.layout, s.caption, p.layout pageLayout from sites s, pages p where s.id=p.siteId and s.id='" +
        (String)site->siteId + "' and p.id='" + (String)pageId + "'";
    bool flag = false;
    if (query->active(sql)) {
        String caption = query->getFieldValue(0, "caption");
        int design = query->getFieldValue(0, "design").toInt(); // 0 - пользовательский
        int theme = query->getFieldValue(0, "theme").toInt();
        int layout = query->getFieldValue(0, "layout").toInt();
        int pageLayout = query->getFieldValue(0, "pageLayout").toInt(); // 0 - как у сайта
        if (pageLayout > 0) layout = pageLayout;

        if (design > 0) {
            fn = site->manager->documentRoot + "/design/" + (String)design + "/index_tpl.html";
            if (tplIndex->open(fn)) {
                if (theme >= 0) {
                    sql = "select theme from themes where designId='" + (String)design + "' and themeId='" + (String)theme + "'";
                    if (query->active(sql) > 0) {
                        String sTheme = query->getFieldValue(0, "theme");
                        tplIndex->out("theme", sTheme);
                    }
                }
                else tplIndex->out("theme", "/css/");
                if (layout > 0) {
                    String fl = site->manager->documentRoot + "/design/" + (String)design + "/layout" + (String)layout + "_tpl.html";
                    tplLayout->clearAllTags();
                    if (tplLayout->open(fl)) {
                        isLayout = true;
                        flag = true;
                    }
                }
            }
        }

        /*
        String sTheme = "/css/";
        if (theme > 0) {
            sql = "select theme from themes where id='" + (String)theme + "'";
            if (query->active(sql) > 0) {
                sTheme = query->getFieldValue(0, "theme");
                sTheme = "https://bootswatch.com/" + sTheme + "/";
            }
        }
        */

        /*
        if (design != "") {
            fn = site->manager->documentRoot + "/design/" + (String)site->siteId + "/" + design + "_tpl.html";
            tplIndex->out("theme", "/css/");
        }
        else tplIndex->out("theme", sTheme);
        */

        tplIndex->out("caption", caption);
    }
    if (!flag) {
        fn = site->manager->documentRoot + "/" + site->host + "/index_tpl.html";
        struct stat buf;
        if ((stat(fn.to_string().c_str(), &buf)) != 0) {
            fn = site->manager->documentRoot + "/common/index_tpl.html";
        }
        string fn8 = fn.to_string();

        tplIndex->open(fn);
    }
    site->manager->deleteQuery(query);
}

void WebPage::paintStdTags() {
    tplIndex->out("css", "<link href='/css/tabs.css' rel='stylesheet'>");
    tplIndex->out("css", "<link href='/css/style.css' rel='stylesheet'>");
    tplIndex->out("css", "<link href=\"/plugin/summernote/summernote.css\" rel=\"stylesheet\">");
    tplIndex->out("css", "<link href=\"/plugin/jquery-treegrid/css/jquery.treegrid.css\" rel=\"stylesheet\">");

    tplIndex->out("javascript", "<script src='/js/user.js'></script>\n");
    tplIndex->out("javascript", "<script src='/js/others.js'></script>\n");

    tplIndex->out("javascript", "<script src=\"/plugin/summernote/summernote.min.js\"></script>");
    tplIndex->out("javascript", "<script src=\"/plugin/jquery-treegrid/js/jquery.treegrid.js\"></script>");
    tplIndex->out("javascript", "<script src=\"/plugin/jquery-treegrid/js/jquery.treegrid.bootstrap3.js\"></script>");

    tplIndex->out("javascript", "<script src='/js/common.js'></script>\n");
}

void WebPage::clearAllTags() {
    tplIndex->clearTag("title");
    tplIndex->clearTag("keywords");
    tplIndex->clearTag("description");
    tplIndex->clearTag("user");
    tplIndex->clearTag("content");
    tplIndex->clearTag("sidebar");
    tplIndex->clearTag("sidebar2");
    tplIndex->clearTag("caption");
    tplIndex->clearTag("menu");
    tplIndex->clearTag("theme");
    tplIndex->clearTag("javascript");
}

void WebPage::paintUser(String uuid) {
    String login = site->manager->getLogin(uuid);

    WebTemplate *tpl = new WebTemplate();
    if (login == "") {
        tpl->open(site->manager->documentRoot + "/modules/user/loginField_tpl.html");
    }
    else {
        tpl->open(site->manager->documentRoot + "/modules/user/logoutField_tpl.html");
        tpl->out("user", login);
    }
    tpl->exec();

    tplIndex->out("user", tpl->html);
}


void WebPage::paintAjax(HttpRequest &request) {
    String fn = site->manager->documentRoot + "/common/ajax_tpl.html";
    tplIndex->clearTag("out");

    //int count = tplIndex->lstTag.getCount();
    this->module->paint(this, request);
    //count = tplIndex->lstTag.getCount();

    String uuid = request.header.COOKIE.getValue("uuid");
    if (uuid.getLength() < 10)
        uuid = generateUUID();

    //String t = "<note><result>1</result></note>";

    File *f = new File(fn, "rb");
    String s, t;
    f->readAll(s);
    tplIndex->exec(s, t);

    string t8 = t.to_string();
    int len = t8.length();

    this->out("HTTP/1.1 200 OK\r\nContent-type: text/html; charset=UTF-8\r\n");
    this->out("Content-Length: " + (String)len + "\r\n\r\n");
    this->out(t);
}


}
