#ifndef SCOPEMM_GRID_H
#define SCOPEMM_GRID_H

#include "scopemm-plotcanvas.h"

namespace scopemm {

class GridLayerImpl : public PlotLayerImplBase {
	friend class GridLayer;
public:
	virtual void draw(PlotCanvas *parent, Cairo::RefPtr<Cairo::Context>);

	void drawStripes(
		const PlotCanvas *parent,
		const Cairo::RefPtr<Cairo::Context> &cr,
		double from, double to, double step,
		bool horiz
	) const;

	GridLayerImpl();

	bool draw_x_grid, draw_y_grid;
};

class GridLayer : public PlotLayerBase {
public:
	GridLayer();

	~GridLayer() { }

	void setDrawXGrid(bool state);
	void setDrawYGrid(bool state);

private:
	GridLayerImpl *impl;
};

} // namespace scopemm

#endif // SCOPEMM_GRID_H
