// Copyright (C) 2004-2021 Artifex Software, Inc.
//
// This file is part of MuPDF.
//
// MuPDF is free software: you can redistribute it and/or modify it under the
// terms of the GNU Affero General Public License as published by the Free
// Software Foundation, either version 3 of the License, or (at your option)
// any later version.
//
// MuPDF is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
// FOR A PARTICULAR PURPOSE. See the GNU Affero General Public License for more
// details.
//
// You should have received a copy of the GNU Affero General Public License
// along with MuPDF. If not, see <https://www.gnu.org/licenses/agpl-3.0.en.html>
//
// Alternative licensing terms are available from the licensor.
// For commercial licensing, see <https://www.artifex.com/> or contact
// Artifex Software, Inc., 39 Mesa Street, Suite 108A, San Francisco,
// CA 94129, USA, for further information.

package com.artifex.mupdf.fitz;

public class RectI
{
	public int x0;
	public int y0;
	public int x1;
	public int y1;

	// Minimum and Maximum values that can survive round trip
	// from int to float.
	protected static final int MIN_INF_RECT = Rect.MIN_INF_RECT;
	protected static final int MAX_INF_RECT = Rect.MAX_INF_RECT;

	public RectI()
	{
		// Invalid (hence zero area) rectangle. Unioning
		// this with any rectangle (or point) will 'cure' it
		x0 = y0 = MAX_INF_RECT;
		x1 = y1 = MIN_INF_RECT;
	}

	public RectI(int x0, int y0, int x1, int y1) {
		this.x0 = x0;
		this.y0 = y0;
		this.x1 = x1;
		this.y1 = y1;
	}

	public RectI(RectI r) {
		this(r.x0, r.y0, r.x1, r.y1);
	}

	public RectI(Rect r) {
		this.x0 = (int)Math.floor(r.x0);
		this.y0 = (int)Math.ceil(r.y0);
		this.x1 = (int)Math.floor(r.x1);
		this.y1 = (int)Math.ceil(r.y1);
	}

	public String toString() {
		return "[" + x0 + " " + y0 + " " + x1 + " " + y1 + "]";
	}

	public boolean isInfinite()
	{
		return this.x0 == MIN_INF_RECT &&
			this.y0 == MIN_INF_RECT &&
			this.x1 == MAX_INF_RECT &&
			this.y1 == MAX_INF_RECT;
	}

	public RectI transform(Matrix tm)
	{
		if (this.isInfinite())
			return this;
		if (!this.isValid())
			return this;

		float ax0 = x0 * tm.a;
		float ax1 = x1 * tm.a;

		if (ax0 > ax1) {
			float t = ax0;
			ax0 = ax1;
			ax1 = t;
		}

		float cy0 = y0 * tm.c;
		float cy1 = y1 * tm.c;

		if (cy0 > cy1) {
			float t = cy0;
			cy0 = cy1;
			cy1 = t;
		}
		ax0 += cy0 + tm.e;
		ax1 += cy1 + tm.e;

		float bx0 = x0 * tm.b;
		float bx1 = x1 * tm.b;

		if (bx0 > bx1) {
			float t = bx0;
			bx0 = bx1;
			bx1 = t;
		}

		float dy0 = y0 * tm.d;
		float dy1 = y1 * tm.d;

		if (dy0 > dy1) {
			float t = dy0;
			dy0 = dy1;
			dy1 = t;
		}
		bx0 += dy0 + tm.f;
		bx1 += dy1 + tm.f;

		x0 = (int)Math.floor(ax0);
		x1 = (int)Math.ceil(ax1);
		y0 = (int)Math.floor(bx0);
		y1 = (int)Math.ceil(bx1);

		return this;
	}

	public RectI transformed(Matrix tm)
	{
		if (this.isInfinite())
			return this;
		if (!this.isValid())
			return this;

		float ax0 = x0 * tm.a;
		float ax1 = x1 * tm.a;

		if (ax0 > ax1) {
			float t = ax0;
			ax0 = ax1;
			ax1 = t;
		}

		float cy0 = y0 * tm.c;
		float cy1 = y1 * tm.c;

		if (cy0 > cy1) {
			float t = cy0;
			cy0 = cy1;
			cy1 = t;
		}
		ax0 += cy0 + tm.e;
		ax1 += cy1 + tm.e;

		float bx0 = x0 * tm.b;
		float bx1 = x1 * tm.b;

		if (bx0 > bx1) {
			float t = bx0;
			bx0 = bx1;
			bx1 = t;
		}

		float dy0 = y0 * tm.d;
		float dy1 = y1 * tm.d;

		if (dy0 > dy1) {
			float t = dy0;
			dy0 = dy1;
			dy1 = t;
		}
		bx0 += dy0 + tm.f;
		bx1 += dy1 + tm.f;

		return new RectI(
			(int)Math.floor(ax0),
			(int)Math.floor(bx0),
			(int)Math.ceil(ax1),
			(int)Math.ceil(bx1)
		);
	}

	public boolean contains(int x, int y)
	{
		if (isEmpty())
			return false;

		return (x >= x0 && x < x1 && y >= y0 && y < y1);
	}

	public boolean contains(Rect r)
	{
		if (isEmpty() || r.isEmpty())
			return false;

		return (r.x0 >= x0 && r.x1 <= x1 && r.y0 >= y0 && r.y1 <= y1);
	}

	public boolean isEmpty()
	{
		return (x0 >= x1 || y0 >= y1);
	}

	public boolean isValid()
	{
		return (x0 <= x1 || y0 <= y1);
	}

	public void union(RectI r)
	{
		if (!r.isValid() || this.isInfinite())
			return;
		if (!this.isValid() || r.isInfinite())
		{
			x0 = r.x0;
			y0 = r.y0;
			x1 = r.x1;
			y1 = r.y1;
			return;
		}

		if (r.x0 < x0)
			x0 = r.x0;
		if (r.y0 < y0)
			y0 = r.y0;
		if (r.x1 > x1)
			x1 = r.x1;
		if (r.y1 > y1)
			y1 = r.y1;
	}

	public RectI unioned(RectI r)
	{
		if (!r.isValid() || this.isInfinite())
			return new RectI(this);
		if (!this.isValid() || r.isInfinite())
			return new RectI(this);

		return new RectI(
			r.x0 < x0 ? r.x0 : x0,
			r.y0 < y0 ? r.y0 : y0,
			r.x1 > x1 ? r.x1 : x1,
			r.y1 > y1 ? r.y1 : y1
		);
	}

	public void inset(int dx, int dy) {
		if (!this.isValid() || this.isInfinite() || this.isEmpty())
			return;
		x0 += dx;
		y0 += dy;
		x1 -= dx;
		y1 -= dy;
	}

	public RectI insetted(int dx, int dy) {
		if (!isValid() || isInfinite() || isEmpty())
			return new RectI(this);
		return new RectI(x0 + dx, y0 + dy, x1 - dx, y1 - dy);
	}

	public void inset(int left, int top, int right, int bottom) {
		if (!this.isValid() || this.isInfinite() || this.isEmpty())
			return;
		x0 += left;
		y0 += top;
		x1 -= right;
		y1 -= bottom;
	}

	public RectI insetted(int left, int top, int right, int bottom) {
		if (!isValid() || isInfinite() || isEmpty())
			return new RectI(this);
		return new RectI(x0 + left, y0 + top, x1 - right, y1 - bottom);
	}

	public void offset(int dx, int dy) {
		if (!this.isValid() || this.isInfinite() || this.isEmpty())
			return;
		x0 += dx;
		y0 += dy;
		x1 += dx;
		y1 += dy;
	}

	public RectI offsetted(int dx, int dy) {
		if (!isValid() || isInfinite() || isEmpty())
			return new RectI(this);
		return new RectI(x0 + dx, y0 + dy, x1 + dx, y1 + dy);
	}

	public void offsetTo(int left, int top) {
		if (!this.isValid() || this.isInfinite() || this.isEmpty())
			return;
		x1 += left - x0;
		y1 += top - y0;
		x0 = left;
		y0 = top;
	}

	public RectI offsettedTo(int left, int top) {
		if (!isValid() || isInfinite() || isEmpty())
			return new RectI(this);
		return new RectI(left, top, left + x1 - x0, top + y1 - y0);
	}

	public static RectI Infinite() {
		return new RectI(MIN_INF_RECT, MIN_INF_RECT, MAX_INF_RECT, MAX_INF_RECT);
	}

	public static RectI Empty() {
		return new RectI(MAX_INF_RECT, MAX_INF_RECT, MIN_INF_RECT, MIN_INF_RECT);
	}
}
