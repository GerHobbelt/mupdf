function RegressionError() {
  var err = new Error(Array.prototype.join.call(arguments, ' '));
	err.name = 'RegressionError';
	return err;
}

function works(f)
{
	var succeeded = false;
	var threw = false;
	var exception = null;
	try {
		f();
		succeeded = true;
	} catch (error) {
		threw = true;
		exception = error;
	}
	if (!succeeded || threw)
		throw new RegressionError("caused by unexpected exception:\n" +
			"\t" + exception.toString().replace(/\n/g, "\n\t") +
			"\nRegressionError stack trace:");
}

function fails(f)
{
	var succeeded = false;
	var threw = false;
	try {
		f();
		succeeded = true;
	} catch (error) {
		threw = true;
	}
	if (succeeded || !threw)
		throw new RegressionError("Unexpected success");
}

function loadPageFromDocumentWithoutPages()
{
	var pdf = new PDFDocument();
	fails(function () {
		pdf.loadPage(0);
	});
}

function interactWithObject(deleteDocument, deleteObject)
{
	print("deleteDocument", deleteDocument, "deleteObject", deleteObject);
	var tests = [
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isIndirect(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isBoolean(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isInteger(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isReal(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isNumber(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isString(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isName(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isArray(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isDictionary(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.isStream(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.asBoolean(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.asIndirect(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.asName(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.asString(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.asByteString(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.toString(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.resolve(); }); } },
		{ run: function (obj, resources) { ((obj == null || !obj.isStream()) ? fails : works)(function () { obj.readStream(); }); } },
		{ run: function (obj, resources) { ((obj == null || !obj.isStream()) ? fails : works)(function () { obj.readRawStream(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.writeObject(null); }); } },
		{ run: function (obj, resources) { ((obj == null || (!obj.isStream() && !obj.isDictionary())) ? fails : works)(function () { obj.writeStream(); }); } },
		{ run: function (obj, resources) { ((obj == null || (!obj.isStream() && !obj.isDictionary())) ? fails : works)(function () { obj.writeRawStream(); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.get("key"); }); } },
		{ run: function (obj, resources) { ((obj == null || !obj.isDictionary()) ? fails : works)(function () { obj.put("key", "value"); }); } },
		{ run: function (obj, resources) { ((obj == null || !obj.isDictionary()) ? fails : works)(function () { obj.delete("key"); }); } },
		{ run: function (obj, resources) { (obj == null ? fails : works)(function () { obj.length; }); } },
		{ run: function (obj, resources) { ((obj == null || !obj.isArray()) ? fails : works)(function () { obj.push("key"); }); } },
	];

	var objtypes = [
		"null",
		"boolean",
		"integer",
		"real",
		"string",
		"bytestring",
		"name",
		"indirect",
		"array",
		"dictionary",
	];

	tests.forEach(function (test, index) {
		for (var i = 0; i < objtypes.length; ++i) {
			var pdf = new PDFDocument();

			var obj = 42;
			if (objtypes[i] == "null") obj = pdf.newNull();
			else if (objtypes[i] == "boolean") obj = pdf.newBoolean(false);
			else if (objtypes[i] == "integer") obj = pdf.newInteger(42);
			else if (objtypes[i] == "real") obj = pdf.newReal(21);
			else if (objtypes[i] == "string") obj = pdf.newString("hellorld!");
			else if (objtypes[i] == "bytestring") obj = pdf.newByteString([42, 21, 0]);
			else if (objtypes[i] == "name") obj = pdf.newName("hello");
			else if (objtypes[i] == "indirect") obj = pdf.newIndirect(42, 0);
			else if (objtypes[i] == "array") obj = pdf.newArray();
			else if (objtypes[i] == "dictionary") obj = pdf.newDictionary();

			if (deleteObject)
				pdf.deleteObject(obj)
			if (deleteDocument)
				pdf = null;
			gc();

			print(obj, typeof(obj));

			test['run'](obj, { });
		}
	});
}

function interactWithExistingObjectInExistingDocument()
{
	interactWithObject(false, false);
}

function interactWithDeletedObjectInExistingDocument()
{
	interactWithObject(false, true);
}

function interactWithExistingObjectInDeletedDocument()
{
	interactWithObject(true, false);
}

function interactWithDeletedObjectInDeletedDocument()
{
	interactWithObject(true, true);
}

function createAnnotationOnDeletedPage()
{
	var pdf = new PDFDocument();
	var pageobj = pdf.addPage([0, 0, 421, 595], 0, null, "");
	pdf.insertPage(-1, pageobj);
	var page = pdf.loadPage(0);

	pdf.deletePage(0);

	works(function () {
		page.createAnnotation("Square");
	});
}

function interactWithAnnot(deletePage, deleteAnnot)
{
	print("deletePage", deletePage, "deleteAnnot", deleteAnnot);
	var tests = [
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { var p = new Pixmap(DeviceRGB, [0, 0, 100, 100], false); var d = new DrawDevice(Identity, p); annot.run(d, Identity); d.close(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.toPixmap(Identity, DeviceRGB, false); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.bound(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.toDisplayList(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getType(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getFlags(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setFlags(0); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getContents(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setContents(""); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasRect(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasRect()) ? fails : works)(function () { annot.getRect(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasRect()) ? fails : works)(function () { annot.setRect([0, 0, 100, 100]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasBorder(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.getBorderStyle(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.setBorderStyle("Solid"); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.getBorderWidth(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.setBorderWidth(0); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.getBorderDashCount(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.getBorderDashItem(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.clearBorderDash(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.addBorderDashItem(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorder()) ? fails : works)(function () { annot.setBorderDashPattern([1, 0]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasBorderEffect(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorderEffect()) ? fails : works)(function () { annot.getBorderEffect(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorderEffect()) ? fails : works)(function () { annot.setBorderEffect("None"); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorderEffect()) ? fails : works)(function () { annot.getBorderEffectIntensity(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasBorderEffect()) ? fails : works)(function () { annot.setBorderEffectIntensity(0); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getBorder(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setBorder(1.0); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getColor(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setColor([1, 0, 0]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasInteriorColor(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInteriorColor()) ? fails : works)(function () { annot.getInteriorColor(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInteriorColor()) ? fails : works)(function () { annot.setInteriorColor([1, 0, 0]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getOpacity(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setOpacity(0.5); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasAuthor(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getAuthor(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setAuthor("Dummy"); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getCreationDate(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setCreationDate(new Date()); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getModificationDate(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setModificationDate(new Date()); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasLineEndingStyles(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasLineEndingStyles()) ? fails : works)(function () { annot.getLineEndingStyles(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasLineEndingStyles()) ? fails : works)(function () { annot.setLineEndingStyles("None", "None"); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasQuadPoints(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasQuadPoints()) ? fails : works)(function () { annot.getQuadPoints(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasQuadPoints()) ? fails : works)(function () { annot.setQuadPoints([0, 0, 0, 0, 1, 1, 1, 1]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasQuadPoints()) ? fails : works)(function () { annot.clearQuadPoints(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasQuadPoints()) ? fails : works)(function () { annot.addQuadPoint([0, 0, 0, 0, 1, 1, 1, 1]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasVertices(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasVertices()) ? fails : works)(function () { annot.getVertices(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasVertices()) ? fails : works)(function () { annot.clearVertices(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasVertices()) ? fails : works)(function () { annot.addVertex(0, 0); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasInkList(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInkList()) ? fails : works)(function () { annot.getInkList(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInkList()) ? fails : works)(function () { annot.setInkList([[[0,0], [1,1]]]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInkList()) ? fails : works)(function () { annot.clearInkList(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInkList()) ? fails : works)(function () { annot.addInkList([[0, 0], [1, 1]]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInkList()) ? fails : works)(function () { annot.addInkListStroke(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasInkList()) ? fails : works)(function () { annot.addInkListStrokeVertex([0, 0]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasIcon(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasIcon()) ? fails : works)(function () { annot.getIcon(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasIcon()) ? fails : works)(function () { annot.setIcon(""); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasOpen(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.isOpen(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setIsOpen(false); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventEnter(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventExit(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventDown(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventUp(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventEnter(); annot.eventDown(); annot.eventUp();}); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventFocus(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.eventBlur(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.update(); }); } },
		{ run: function (annot, resources) { works(function () { annot.getObject(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getLanguage(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setLanguage(null); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasQuadding(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasQuadding()) ? fails : works)(function () { annot.getQuadding(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasQuadding()) ? fails : works)(function () { annot.setQuadding(0); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasLine(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasLine()) ? fails : works)(function () { annot.getLine(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasLine()) ? fails : works)(function () { annot.setLine([0, 0], [0, 0]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.getDefaultAppearance(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setDefaultAppearance("TiRo", 12, [1, 0, 0]); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setAppearance(null, "N", Identity, new DisplayList()); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasRect(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setAppearance(null, "N", Identity, [0, 0, 1, 1], null, ""); }); } },
		// this is not implemented yet, so not possible to test!
		//{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.setAppearance(Image); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot) ? fails : works)(function () { annot.hasFilespec(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasFilespec()) ? fails : works)(function () { annot.getFilespec(); }); } },
		{ run: function (annot, resources) { ((deletePage || deleteAnnot || !annot.hasFilespec()) ? fails : works)(function () { annot.setFilespec(resources.fs); }); } },
		{ run: function (annot, resources) { works(function () { annot.getHiddenForEditing(); }); } },
		{ run: function (annot, resources) { works(function () { annot.setHiddenForEditing(false); }); } },
	];

	var annottypes = [
		 "Text",
		 "FreeText",
		 "Line",
		 "Square",
		 "Circle",
		 "Polygon",
		 "PolyLine",
		 "Highlight",
		 "Underline",
		 "Squiggly",
		 "StrikeOut",
		 "Stamp",
		 "Caret",
		 "Ink",
		 "FileAttachment",
	];

	tests.forEach(function (test, index) {
		for (var i = 0; i < annottypes.length; ++i) {
			var pdf = new PDFDocument();
			var pageobj = pdf.addPage([0, 0, 421, 595], 0, null, "");
			pdf.insertPage(-1, pageobj);
			var page = pdf.loadPage(0);

			var annot = page.createAnnotation(annottypes[i]);

			var fs = pdf.addEmbeddedFile("", "", new Buffer(), -1, -1, false);

			if (deleteAnnot)
				page.deleteAnnotation(annot);
			if (deletePage)
			{
				pdf.deletePage(0);
				page = null;
			}
			gc();

			test['run'](annot, { fs: fs });
		}
	});
}

function interactWithAnnotOnPage()
{
	interactWithAnnot(false, false);
}

function interactWithAnnotOnDeletedPage()
{
	interactWithAnnot(true, false);
}

function interactWithDeletedAnnotOnPage()
{
	interactWithAnnot(false, true);
}

function interactWithDeletedAnnotOnDeletedPage()
{
	interactWithAnnot(true, true);
}

function createLinkOnDeletedPage()
{
	var pdf = new PDFDocument();
	var pageobj = pdf.addPage([0, 0, 421, 595], 0, null, "");
	pdf.insertPage(-1, pageobj);
	var page = pdf.loadPage(0);

	pdf.deletePage(0);

	works(function () {
		page.createLink([0, 0, 100, 100], "http://example.com");
	});
}

function interactWithLink(deletePage, deleteLink)
{
	print("deletePage", deletePage, "deleteLink", deleteLink);
	var tests = [
		{ run: function (link, resources) { works(function () { link.bounds }); } },
		{ run: function (link, resources) { ((deletePage || deleteLink) ? fails : works)(function () { link.bounds = [0, 0, 1, 1]; }); } },
		{ run: function (link, resources) { works(function () { link.uri }); } },
		{ run: function (link, resources) { ((deletePage || deleteLink) ? fails : works)(function () { link.uri = "http://subdomain.example.com"; }); } },
	];

	tests.forEach(function (test, index) {
		var pdf = new PDFDocument();
		var pageobj = pdf.addPage([0, 0, 421, 595], 0, null, "");
		pdf.insertPage(-1, pageobj);
		var page = pdf.loadPage(0);

		var link = page.createLink([0, 0, 100, 100], "http://example.com");

		if (deleteLink)
			page.deleteLink(link);
		if (deletePage)
		{
			pdf.deletePage(0);
			page = null;
		}
		gc();

		test['run'](link, { });
	});
}

function interactWithLinkOnPage()
{
	interactWithLink(false, false);
}

function interactWithLinkOnDeletedPage()
{
	interactWithLink(true, false);
}

function interactWithDeletedLinkOnPage()
{
	interactWithLink(false, true);
}

function interactWithDeletedLinkOnDeletedPage()
{
	interactWithLink(true, true);
}

function interactWithWidget(deletePage, deleteWidget)
{
	print("deletePage", deletePage, "deleteWidget", deleteWidget);
	var tests = [
		{ run: function (widget, resources) { works(function () { widget.getFieldType}); } },
		{ run: function (widget, resources) { works(function () { widget.getFieldFlags}); } },
		{ run: function (widget, resources) { works(function () { widget.getLabel}); } },
		{ run: function (widget, resources) { works(function () { widget.getFieldType(); }); } },
		{ run: function (widget, resources) { works(function () { widget.getFieldFlags(); }); } },
		{ run: function (widget, resources) { works(function () { widget.getMaxLen(); }); } },
		{ run: function (widget, resources) { works(function () { widget.getOptions(true); }); } },
		{ run: function (widget, resources) { works(function () { widget.isReadOnly(); }); } },
		{ run: function (widget, resources) { works(function () { widget.getEditingState(); }); } },
		{ run: function (widget, resources) { works(function () { widget.setEditingState(false); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.getLabel(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.getValue(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.setTextValue(""); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.setChoiceValue(""); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.toggle(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.layoutTextWidget(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.eventEnter(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.eventExit(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.eventDown(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.eventUp(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.eventFocus(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.eventBlur(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.isSigned(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget) ? fails : works)(function () { widget.validateSignature(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature") ? fails : works)(function () { widget.checkCertificate(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature") ? fails : works)(function () { widget.checkDigest(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature")  ? fails : works)(function () { widget.getSignatory(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature")  ? fails : works)(function () { widget.clearSignature(); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature")  ? fails : works)(function () { widget.sign(resources.signer, { 'showLabels':true, 'showDN':false, 'showDate':false, 'showTextName':false, 'showGraphicName':false, 'showLogo':false }, null, "whoops", "Acme Acres"); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature")  ? fails : works)(function () { widget.previewSignature(resources.signer, { 'showLabels':true, 'showDN':false, 'showDate':false, 'showTextName':false, 'showGraphicName':false, 'showLogo':false }, null, "whoops", "Acme Acres"); }); } },
		{ run: function (widget, resources) { ((deletePage || deleteWidget || widget.getFieldType() != "signature")  ? fails : works)(function () { widget.incrementalChangesSinceSigning(); }); } },
	];

	var filename = "x/Expenses_Signable.pdf";
	var count = new Document(filename).loadPage(0).getWidgets().length;

	tests.forEach(function (test, index) {
		for (var i = 0; i < count; ++i) {
			var pdf = new Document(filename);
			var page = pdf.loadPage(0);
			var widget = page.getWidgets()[i];
			var signer = PDFPKCS7Signer("docs/examples/acme.pfx", "password");

			if (deleteWidget)
			{
				page.deleteAnnotation(widget);
			}
			if (deletePage)
			{
				pdf.deletePage(0);
				page = null;
			}
			gc();

			test['run'](widget, { signer: signer });
		}
	});
}

function interactWithWidgetOnPage()
{
	interactWithWidget(false, false);
}

function interactWithWidgetOnDeletedPage()
{
	interactWithWidget(true, false);
}

function interactWithDeletedWidgetOnPage()
{
	interactWithWidget(false, true);
}

function interactWithDeletedWidgetOnDeletedPage()
{
	interactWithWidget(true, true);
}

interactWithExistingObjectInExistingDocument();
interactWithDeletedObjectInExistingDocument();
interactWithExistingObjectInDeletedDocument();
interactWithDeletedObjectInDeletedDocument();

loadPageFromDocumentWithoutPages();

createAnnotationOnDeletedPage();
interactWithAnnotOnPage();
interactWithAnnotOnDeletedPage();
interactWithDeletedAnnotOnPage();
interactWithDeletedAnnotOnDeletedPage();

createLinkOnDeletedPage();
interactWithLinkOnPage();
interactWithLinkOnDeletedPage();
interactWithDeletedLinkOnPage();
interactWithDeletedLinkOnDeletedPage();

interactWithWidgetOnPage();
interactWithWidgetOnDeletedPage();
interactWithDeletedWidgetOnPage();
interactWithDeletedWidgetOnDeletedPage();
