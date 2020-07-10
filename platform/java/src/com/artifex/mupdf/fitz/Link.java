package com.artifex.mupdf.fitz;

public class Link
{
	public Rect bounds;
	public Quad[] quads;
	public String uri;

	public Link(Rect bounds, Quad[] quads, String uri) {
		this.bounds = bounds;
		this.quads = quads;
		this.uri = uri;
	}

	public boolean isExternal() {
		char c = uri.charAt(0);
		if (!(c >= 'a' && c <= 'z') && !(c >= 'A' && c <= 'Z'))
			return false;

		for (int i = 1; i < uri.length(); i++)
		{
			c = uri.charAt(i);
			if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == '+' || c == '-' || c == '.')
				continue;
			else
				return c == ':';
		}
		return false;
	}

	public String toString() {
		return "Link(bounds="+bounds+",quads="+quads+",uri="+uri+")";
	}
}
