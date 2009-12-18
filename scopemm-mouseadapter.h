#ifndef SCOPEMM_MOUSEADAPTER_H
#define SCOPEMM_MOUSEADAPTER_H

#include "scopemm-base.h"

class MouseAdapter {
public:
	MouseAdapter(PlotBase *_plot);

	virtual void mouse_motion();

	virtual void mouse_clicked(int button);

	bool mouseIn()  const { return mouse_in; }
	double mouseX() const { return mouse_x; }
	double mouseY() const { return mouse_y; }
	bool button1()  const { return button_state & 1; }
	bool button2()  const { return button_state & 2; }
	bool button3()  const { return button_state & 4; }

private:
	void updateState(gdouble evt_x, gdouble evt_y, guint evt_state);
	bool on_motion_notify_event(GdkEventMotion* event);
	bool on_enter_notify_event(GdkEventCrossing* event);
	bool on_leave_notify_event(GdkEventCrossing* event);
	bool on_button_press_event(GdkEventButton* event);

private:
	PlotBase *plot;
	bool mouse_in;
	double mouse_x, mouse_y;
	int button_state;
};

#endif // SCOPEMM_MOUSEADAPTER_H
