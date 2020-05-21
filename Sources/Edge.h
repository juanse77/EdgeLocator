#pragma once

#if !defined(_EDGE_H_)
#define _EDGE_H_

namespace EdgeLocator {
	
	class Edge {

	private:
		int position;  // 1D index inside image 
		float x, y;    // subpixel position
		float nx, ny;  // normal vector (normalized)
		float curv;    // curvature
		float i0, i1;  // intensities at both sides

	public:
		Edge(int c_position, float c_x, float c_y, float c_nx, float c_ny, float c_curv, float c_i0, float c_i1) {
			position = c_position;
			x = c_x;
			y = c_y;
			nx = c_nx;
			ny = c_ny;
			curv = c_curv;
			i0 = c_i0;
			i1 = c_i1;
		}

		int getPosition() {
			return position;
		}

		float getX() {
			return x;
		}

		float getY() {
			return y;
		}

		float getNx() {
			return nx;
		}

		float getNy() {
			return ny;
		}

		float getCurv() {
			return curv;
		}

		float getI0() {
			return i0;
		}

		float getI1() {
			return i1;
		}

	};
}

#endif // _EDGE_H_