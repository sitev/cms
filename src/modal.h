#pragma once

namespace cms {

class Modal {
	String modalId;
	WebTemplate *tplIndex;
	String tag;
	String caption; 
	String content;
	String buttons;
public:
	Modal(String modalId, WebTemplate *tpl, String tag);
	virtual void setCaption(String caption);
	virtual void setContent(String content);
	virtual void setButtons(String buttons);
	virtual void paint();
};

}