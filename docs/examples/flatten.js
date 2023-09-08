// usage: mutool run flatten.js input.pdf output.pdf
//
// This tool will rewrite all Widgets and Annotations as plain XObjects
// drawn on the page and remove them as interactive objects.
//
// Links will be preserved. All other Annotations will be removed.
// The AcroForm object will be deleted if present.
//

"use strict"

function flatten_document(doc) {
	var i, n, page
	n = doc.countPages()
	for (i = 0; i < n; ++i) {
		page = doc.loadPage(i)
		flatten_page(doc, page.getObject())
	}
	delete doc.getTrailer().Root.AcroForm
}

function flatten_page(doc, page) {
	var i, n, list, links, buf
	list = page.Annots
	if (list) {
		if (!page.Resources)
			page.Resources = {}
		if (!page.Resources.XObject)
			page.Resources.XObject = {}
		if (!page.Contents.isArray())
			page.Contents = [ page.Contents ]

		links = []

		buf = ""
		for (i = 0; i < list.length; ++i) {
			if (list[i].type == "Link")
				links.push(list[i])
			buf += flatten_annot(doc, page, list[i])
		}

		if (links.length > 0)
			page.Annots = links
		else
			delete page.Annots

		page.Contents.push(doc.addStream(buf))
	}
}

function calc_annot_transform(rect, bbox, transform) {
	var w, h, x, y
	if (!transform)
		transform = [ 1, 0, 0, 1, 0, 0 ]
	bbox = mupdf.Rect.transform(bbox, transform)
	w = (rect[2] - rect[0]) / (bbox[2] - bbox[0])
	h = (rect[3] - rect[1]) / (bbox[3] - bbox[1])
	x = rect[0] - bbox[0] * w
	y = rect[1] - bbox[1] * w
	return [ w, 0, 0, h, x, y ]
}

function flatten_annot(doc, page, annot) {
	var name = "Annot" + annot.asIndirect()
	var rect = annot.Rect
	var ap = annot.AP
	var transform
	if (ap) {
		var ap_n = ap.N
		if (ap_n) {
			ap_n.Type = "XObject"
			ap_n.Subtype = "Form"
			page.Resources.XObject[name] = ap_n
			transform = calc_annot_transform(rect, ap_n.BBox, ap_n.Matrix)
			return "q\n" + transform.join(" ") + " cm\n/" + name + " Do\nQ\n"
		}
	}
	return ""
}

var doc = mupdf.Document.openDocument(scriptArgs[0])
flatten_document(doc)
doc.save(scriptArgs[1], "garbage,compress")
