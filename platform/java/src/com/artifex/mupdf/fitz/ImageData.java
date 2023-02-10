// Copyright (C) 2004-2023 Artifex Software, Inc.
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
// Artifex Software, Inc., 1305 Grant Avenue - Suite 200, Novato,
// CA 94945, U.S.A., +1(415)492-9861, for further information.

package com.artifex.mupdf.fitz;

public class ImageData
{
	protected Params params;
	protected Buffer buffer;

	static {
		Context.init();
	}

	protected long pointer;

	protected native void finalize();

	public void destroy() {
		finalize();
	}

	private native long newNativeRecognizeFormat(Buffer buffer);
	private native long newNative(Buffer buffer, Params params);

	protected ImageData(long p) {
		pointer = p;
	}

	public ImageData(Buffer buffer, Params params) {
		pointer = newNative(buffer, params);
	}

	public ImageData(Buffer buffer) {
		pointer = newNativeRecognizeFormat(buffer);
	}

	public Buffer getBuffer() {
		return buffer.slice();
	}

	public Params getParams() {
		if (params instanceof FaxParams)
			return new FaxParams((FaxParams)params);
		if (params instanceof FlateParams)
			return new FlateParams((FlateParams)params);
		if (params instanceof LZWParams)
			return new LZWParams((LZWParams)params);
		if (params instanceof JBIG2Params)
			return new JBIG2Params((JBIG2Params)params);
		if (params instanceof JPEGParams)
			return new JPEGParams((JPEGParams)params);
		if (params instanceof JPXParams)
			return new JPXParams((JPXParams)params);
		return new Params(params);
	}

	public static class Params {
		public int type;

		public Params(int type) {
			this.type = type;
		}

		public Params(Params original) {
			type = original.type;
		}
	}

	public static class FaxParams extends Params {
		public int columns;
		public int rows;
		public int k;
		public boolean endOfLine;
		public boolean encodedByteAlign;
		public boolean endOfBlock;
		public boolean blackIs1;
		public int damagedRowsBeforeError;

		public FaxParams(int columns, int rows, int k, boolean endOfLine, boolean encodedByteAlign, boolean endOfBlock, boolean blackIs1, int damagedRowsBeforeError) {
			super(TYPE_FAX);
			this.columns = columns;
			this.rows = rows;
			this.k = k;
			this.endOfLine = endOfLine;
			this.encodedByteAlign = encodedByteAlign;
			this.endOfBlock = endOfBlock;
			this.blackIs1 = blackIs1;
			this.damagedRowsBeforeError = damagedRowsBeforeError;
		}

		public FaxParams(FaxParams original) {
			super(original);
			columns = original.columns;
			rows = original.rows;
			k = original.k;
			endOfLine = original.endOfLine;
			encodedByteAlign = original.encodedByteAlign;
			endOfBlock = original.endOfBlock;
			blackIs1 = original.blackIs1;
			damagedRowsBeforeError = original.damagedRowsBeforeError;
		}
	}

	public static class FlateParams extends Params {
		public int columns;
		public int colors;
		public int predictor;
		public int bitsPerComponent;

		public FlateParams(int columns, int colors, int predictor, int bitsPerComponent) {
			super(TYPE_FLATE);
			this.columns = columns;
			this.colors = colors;
			this.predictor = predictor;
			this.bitsPerComponent = bitsPerComponent;
		}

		public FlateParams(FlateParams original) {
			super(original);
			columns = original.columns;
			colors = original.colors;
			predictor = original.predictor;
			bitsPerComponent = original.bitsPerComponent;
		}
	}

	public static class LZWParams extends Params {
		public int columns;
		public int colors;
		public int predictor;
		public int bitsPerComponent;
		public int earlyChange;

		public LZWParams(int columns, int colors, int predictor, int bitsPerComponent, int earlyChange) {
			super(TYPE_LZW);
			this.columns = columns;
			this.colors = colors;
			this.predictor = predictor;
			this.bitsPerComponent = bitsPerComponent;
			this.earlyChange = earlyChange;
		}

		public LZWParams(LZWParams original) {
			super(original);
			columns = original.columns;
			colors = original.colors;
			predictor = original.predictor;
			bitsPerComponent = original.bitsPerComponent;
			earlyChange = original.earlyChange;
		}
	}

	public static class JBIG2Params extends Params {
		public int embedded;
		public Buffer globals;

		public JBIG2Params(int embedded, Buffer globals) {
			super(TYPE_JBIG2);
			this.embedded = embedded;
			this.globals = globals;
		}

		public JBIG2Params(JBIG2Params original) {
			super(original);
			embedded = original.embedded;
			globals = original.globals;
		}
	}

	public static class JPEGParams extends Params {
		public int colorTransform;

		public JPEGParams(int colorTransform) {
			super(TYPE_JPEG);
			this.colorTransform = colorTransform;
		}

		public JPEGParams(JPEGParams original) {
			super(original);
			colorTransform = original.colorTransform;
		}
	}

	public static class JPXParams extends Params {
		public int sMaskInData;

		public JPXParams(int sMaskInData) {
			super(TYPE_JPX);
			this.sMaskInData = sMaskInData;
		}

		public JPXParams(JPXParams original) {
			super(original);
			sMaskInData = original.sMaskInData;
		}
	}

	public static final int TYPE_UNKNOWN = 0;
	public static final int TYPE_RAW = 1;
	public static final int TYPE_FAX = 2;
	public static final int TYPE_FLATE = 3;
	public static final int TYPE_LZW = 4;
	public static final int TYPE_RLD = 5;
	public static final int TYPE_BMP = 6;
	public static final int TYPE_GIF = 7;
	public static final int TYPE_JBIG2 = 8;
	public static final int TYPE_JPEG = 9;
	public static final int TYPE_JPX = 10;
	public static final int TYPE_JXR = 11;
	public static final int TYPE_PNG = 12;
	public static final int TYPE_PNM = 13;
	public static final int TYPE_TIFF = 14;
}
