#include "gtk-blitz.h"

GridCanvas::GridCanvas() : buf(0) { }

GridCanvas::~GridCanvas() { 
	if(buf) delete[] buf;
}

void GridCanvas::setData(blitz::Array<double, 2> data) {
	setData(data, data, data);
}

void GridCanvas::setData(
	blitz::Array<double, 2> data_r,
	blitz::Array<double, 2> data_g,
	blitz::Array<double, 2> data_b
) {
	blitz::Array<double, 2> data[3] = { data_r, data_g, data_b };
	const int h = data[0].shape()[0];
	const int w = data[0].shape()[1];
	data_h = w;
	data_w = w;

	if(buf) {
		delete[] buf;
		buf = 0;
	}

	if(h>0 && w>0) {
		buf = new guchar[h*w*3];

		for(int band=0; band<3; band++) {
			const double min = blitz::min(data[band]);
			const double max = blitz::max(data[band]);
			for(int y=0; y<data_h; y++) {
				for(int x=0; x<data_w; x++) {
					const int v = int(255.0 * (data[band](y,x)-min) / (max-min));
					buf[(y*data_w + x)*3 + band] = v;
				}
			}
		}
	}

	queue_draw();
}

bool GridCanvas::on_expose_event(GdkEventExpose* event) {
	Glib::RefPtr<Gdk::Window> window = get_window();
	if(window && buf) {
		Gtk::Allocation allocation = get_allocation();
		const int width = allocation.get_width();
		const int height = allocation.get_height();

		// FIXME - only needed if sizes don't match
		guchar *buf2 = new guchar[height*width*3];
		for(int y=0; y<height; y++) {
			const int j = y * (data_h-1) / (height-1);
			for(int x=0; x<width; x++) {
				const int i = x * (data_w-1) / (width-1);
				for(int band=0; band<3; band++) {
					buf2[(y*width + x)*3 + band] =
						buf[(j*data_w + i)*3 + band];
				}
			}
		}

		get_window()->draw_rgb_image(
			get_style()->get_fg_gc(Gtk::STATE_NORMAL),
			0, 0, width, height,
			Gdk::RGB_DITHER_NONE,
			buf2, width*3
		);

		delete[] buf2;
	}
	return true;
}

///////////////////////////////////////////////////////////

MouseCanvas::MouseCanvas() {
	set_events(get_events() | Gdk::POINTER_MOTION_MASK);
	set_events(get_events() | Gdk::POINTER_MOTION_HINT_MASK);
	set_events(get_events() | Gdk::ENTER_NOTIFY_MASK);
	set_events(get_events() | Gdk::LEAVE_NOTIFY_MASK);
	set_events(get_events() | Gdk::BUTTON_PRESS_MASK);
	set_events(get_events() | Gdk::BUTTON_RELEASE_MASK);
}

MouseCanvas::~MouseCanvas() { }

bool MouseCanvas::on_motion_notify_event(GdkEventMotion* event) {
	//std::cout << "Motion " << event->state << std::endl;
	mouse_x = event->x;
	mouse_y = event->y;
	button1 = (event->state & Gdk::BUTTON1_MASK);
	button2 = (event->state & Gdk::BUTTON2_MASK);
	button3 = (event->state & Gdk::BUTTON3_MASK);
	mouse_motion();
	return true;
}

bool MouseCanvas::on_enter_notify_event(GdkEventCrossing* event) {
	//std::cout << "enter_notify" << event->state << std::endl;
	mouse_in = true;
	mouse_x = event->x;
	mouse_y = event->y;
	button1 = (event->state & Gdk::BUTTON1_MASK);
	button2 = (event->state & Gdk::BUTTON2_MASK);
	button3 = (event->state & Gdk::BUTTON3_MASK);
	mouse_motion();
	return true;
}

bool MouseCanvas::on_leave_notify_event(GdkEventCrossing* event) {
	//std::cout << "leave_notify" << event->state << std::endl;
	mouse_in = false;
	button1 = (event->state & Gdk::BUTTON1_MASK);
	button2 = (event->state & Gdk::BUTTON2_MASK);
	button3 = (event->state & Gdk::BUTTON3_MASK);
	mouse_motion();
	return true;
}

bool MouseCanvas::on_button_press_event(GdkEventButton* event) {
	mouse_x = event->x;
	mouse_y = event->y;
	//std::cout << "state " << event->state << "," << event->button << std::endl;
	mouse_clicked(event->button);
	return true;
}

void MouseCanvas::mouse_motion() {
	std::cout
		<< "mouse: " << mouse_in << "," << mouse_x << "," << mouse_y
		<< " / " << button1 << "," << button2 << "," << button3
		<< std::endl;
}

void MouseCanvas::mouse_clicked(int button) {
	std::cout << "click: " << button << std::endl;
}
