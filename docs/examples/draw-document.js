// Draw all pages in a document and save them as PNG files.

if (scriptArgs.length < 2) {
	print("usage: mutool run draw-document.js input.pdf outputDirectory/");
	quit();
}

var doc = new Document(scriptArgs[0]);
var n = doc.countPages();
for (var i = 0; i < n; ++i) {
	var page = doc.loadPage(i);
	var pixmap = page.toPixmap(Identity, DeviceRGB);
	pixmap.saveAsPNG(scriptArgs[1] + "out" + (i+1) + ".png");
}
