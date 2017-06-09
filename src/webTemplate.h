#pragma once

namespace cms {

class ParamItem : public Object {
public:
	String name;
	String value;
	ParamItem();
	virtual void set(String name, String value);
};

class WebTemplate {
public:
	List lstTag;
	String src, html;

	WebTemplate();

	bool open(String url);

	virtual void out(String tag, String s);
	virtual String getTagValue(String tag);
	virtual void clearTag(String tag);
	virtual void clearAllTags();

	virtual void exec(String source, String &target);
	virtual void exec();
};

}