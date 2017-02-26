package com.artifex.mupdf.fitz.android;

import android.graphics.Bitmap;

import com.artifex.mupdf.fitz.NativeDevice;
import com.artifex.mupdf.fitz.RectI;
import com.artifex.mupdf.fitz.Rect;

import com.artifex.mupdf.fitz.Page;
import com.artifex.mupdf.fitz.Matrix;

public final class AndroidDrawDevice extends NativeDevice
{
	private native long newNative(Bitmap bitmap, int pageX0, int pageY0, int pageX1, int pageY1, int patchX0, int patchY0, int patchX1, int patchY1);

	public AndroidDrawDevice(Bitmap bitmap,
			int pageX0, int pageY0, int pageX1, int pageY1,
			int patchX0, int patchY0, int patchX1, int patchY1) {
		super(0);
		pointer = newNative(bitmap, pageX0, pageY0, pageX1, pageY1, patchX0, patchY0, patchX1, patchY1);
	}

	public AndroidDrawDevice(Bitmap bitmap, RectI page, RectI patch) {
		super(0);
		pointer = newNative(bitmap, page.x0, page.y0, page.x1, page.y1, patch.x0, patch.y0, patch.x1, patch.y1);
	}

	public static Bitmap drawPage(Page page, Matrix ctm) {
		Rect fbox = page.getBounds().transform(ctm);
		RectI ibox = new RectI((int)fbox.x0, (int)fbox.y0, (int)fbox.x1, (int)fbox.y1);
		int w = ibox.x1 - ibox.x0;
		int h = ibox.y1 - ibox.y0;
		Bitmap bmp = Bitmap.createBitmap(w, h, Bitmap.Config.ARGB_8888);
		AndroidDrawDevice dev = new AndroidDrawDevice(bmp, ibox, ibox);
		page.run(dev, ctm, null);
		dev.close();
		dev.destroy();
		return bmp;
	}

	public static Bitmap drawPage(Page page, float dpi, int rotate) {
		return drawPage(page, new Matrix(dpi / 72).rotate(rotate));
	}

	public static Bitmap drawPage(Page page, float dpi) {
		return drawPage(page, new Matrix(dpi / 72));
	}

	public static Bitmap drawPageFit(Page page, int fitW, int fitH) {
		Rect bbox = page.getBounds();
		float pageW = bbox.x1 - bbox.x0;
		float pageH = bbox.y1 - bbox.y0;
		float scaleH = (float)fitW / pageW;
		float scaleV = (float)fitH / pageH;
		float scale = scaleH < scaleV ? scaleH : scaleV;
		scaleH = (float)Math.floor(pageW * scale) / pageW;
		scaleV = (float)Math.floor(pageH * scale) / pageH;
		return drawPage(page, new Matrix(scaleH, scaleV));
	}

	public static Bitmap drawPageFitWidth(Page page, int fitW) {
		Rect bbox = page.getBounds();
		float pageW = bbox.x1 - bbox.x0;
		float scale = (float)fitW / pageW;
		scale = (float)Math.floor(pageW * scale) / pageW;
		return drawPage(page, new Matrix(scale));
	}
}
