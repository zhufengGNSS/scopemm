#ifndef GTK_BLITZ_LINEPLOT_H
#define GTK_BLITZ_LINEPLOT_H

#include <gtkmm/main.h>
#include <gtkmm/box.h>
#include <gtkmm/drawingarea.h>
#include <boost/noncopyable.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <blitz/array.h>

class Plot1D;
class PlotTrace;

typedef boost::shared_ptr<PlotTrace> PlotTracePtr;

class Plot1D : public Gtk::DrawingArea, public boost::noncopyable {
	friend class PlotTrace;

public:
	Plot1D();
	virtual ~Plot1D() { }

	virtual PlotTracePtr addTrace();
	virtual void setXAutoRange();
	virtual void setYAutoRange();
	virtual void setXRange(double min, double max);
	virtual void setYRange(double min, double max);
	virtual void setSwapAxes(bool state=true);
	virtual void setDrawAxes(bool state=true);
	virtual void setDrawXAxis(bool state=true);
	virtual void setDrawYAxis(bool state=true);
	virtual void setDrawGrids(bool state=true);
	virtual void setDrawXGrid(bool state=true);
	virtual void setDrawYGrid(bool state=true);
	virtual bool on_expose_event(GdkEventExpose* event);
	template <int N>
	void setYData(blitz::Array<blitz::TinyVector<double, N>, 1> ydata);
	virtual PlotTracePtr trace(int idx) const;

private:
	virtual void drawStripes(
		const Cairo::RefPtr<Cairo::Context> &cr,
		double from, double to, double step,
		bool horiz
	) const;

	virtual void drawGrid(const Cairo::RefPtr<Cairo::Context> &cr) const;

	inline void coordToScreen(int w, int h, double xi, double yi, double &xo, double &yo) const {
		double x = (xi-xmin)/(xmax-xmin);
		double y = (yi-ymin)/(ymax-ymin);
		if(swap_axes) std::swap(x, y);
		y = 1.0-y;
		xo = x * (w-1);
		yo = y * (h-1);
	}

	virtual void dataChanged();
	virtual void configChanged();
	virtual void recalcAutoRange();

	std::vector<PlotTracePtr> traces;
	bool x_auto, y_auto;
	double xmin, xmax, ymin, ymax;
	bool swap_axes;
	bool draw_x_axis, draw_y_axis;
	bool draw_x_grid, draw_y_grid;
};

class PlotTrace : private boost::noncopyable {
	friend class Plot1D;

	PlotTrace(Plot1D *_parent);

public:
	~PlotTrace();

	PlotTrace& setColor(double r, double g, double b);

	template <class Iter>
	PlotTrace& setYData(Iter _yfirst, Iter _ylast, bool steps=false);
	template <class Iter>
	PlotTrace& setXYData(Iter _xfirst, Iter _xlast, Iter _yfirst, Iter _ylast);
	template <class T>
	PlotTrace& setYData(T _ypts, bool steps=false);
	template <class T>
	PlotTrace& setXYData(T _xpts, T _ypts);

	void draw(Cairo::RefPtr<Cairo::Context>);

private:
	Plot1D *parent;
	std::vector<double> xpts;
	std::vector<double> ypts;
	double rgb[3];
};

template <class Iter>
PlotTrace& PlotTrace::setYData(Iter yfirst, Iter ylast, bool steps) {
	size_t npts;
	ypts.clear();
	if(steps) {
		Iter p = yfirst;
		npts = 0;
		while(p != ylast) {
			ypts.push_back(*p);
			ypts.push_back(*p);
			++p;
			++npts;
		}
	} else {
		ypts.insert(ypts.begin(), yfirst, ylast);
		npts = ypts.size();
	}

	xpts.clear();
	if(steps) {
		for(size_t i=0; i<npts; ++i) {
			xpts.push_back(i);
			xpts.push_back(i+1);
		}
	} else {
		for(size_t i=0; i<npts; ++i) xpts.push_back(i);
	}

	parent->dataChanged();

	return *this;
}

template <class Iter>
PlotTrace& PlotTrace::setXYData(Iter _xfirst, Iter _xlast, Iter _yfirst, Iter _ylast) {
	ypts.clear();
	ypts.insert(ypts.begin(), _yfirst, _ylast);

	xpts.clear();
	xpts.insert(xpts.begin(), _xfirst, _xlast);

	assert(xpts.size() == ypts.size());

	parent->dataChanged();

	return *this;
}

template <class T>
PlotTrace& PlotTrace::setYData(T _ypts, bool steps) {
	return setYData(_ypts.begin(), _ypts.end(), steps);
}

template <class T>
PlotTrace& PlotTrace::setXYData(T _xpts, T _ypts) {
	return setXYData(_xpts.begin(), _xpts.end(), _ypts.begin(), _ypts.end());
}

template <int N>
void Plot1D::setYData(blitz::Array<blitz::TinyVector<double, N>, 1> ydata) {
	assert(N == traces.size());
	for(int i=0; i<N; i++) {
		traces[i]->setYData(ydata[i]);
	}
}

#endif // GTK_BLITZ_LINEPLOT_H
