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
		Edge(int position, float x, float y, float nx, float ny, float curv, float i0, float i1) {
			this->position = position;
			this->x = x;
			this->y = y;
			this->nx = nx;
			this->ny = ny;
			this->curv = curv;
			this->i0 = i0;
			this->i1 = i1;
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