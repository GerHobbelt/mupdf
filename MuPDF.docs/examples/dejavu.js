// Extract the image masks from DjVu-like PDF files and create a new monochrome
// PDF from them.
//
// This assumes that each page consists of three full page images:
//   * A full color background image.
//   * A full color foreground image.
//   * A black and white selection mask.
//
// The background image typically holds the white page color, the foreground
// image holds the ink color, and the mask selects whether the foreground ink
// or background paper shows for a given pixel.
//
// This allows the background and foreground images to be encoded with an
// algorithm where the compressor can ignore the foreground ink pixels when
// compressing the background image, and vice versa, accomplishing much higher
// compression ratios since all the high-frequency data is moved to the
// selection mask which is compressed using a black&white algorithm.
//
// Typically these files are created with JPEG2000 compression for the full
// color images, which is very slow to decompress. The selection mask is then
// compressed with JBIG2 which is also quite slow.
//
// If we create a new PDF file containing only the selection masks drawn as
// monochrome images, we can usually render these files much faster, and they
// look nicer since the muddy colors are removed and the text is nice and
// crisp.
//
// There is of course the danger of losing actual color images in the file!

if (scriptArgs.length < 2) {
	print("usage: mutool run dejavu.js input.pdf output.pdf");
	quit();
}

var doc = new PDFDocument(scriptArgs[0]);
var out = new DocumentWriter(scriptArgs[1], "pdf", "compress");
for (var i = 0; i < doc.countPages(); ++i) {
	var page = doc.findPage(i);
	var w = page.MediaBox[2] - page.MediaBox[0];
	var h = page.MediaBox[3] - page.MediaBox[1];
	page.Resources.XObject.forEach(function (name, xobj) {
		var mask = xobj.Mask;
		if (mask) {
			print("Drawing image mask", mask);
			mask.Decode = [1, 0]; // Invert image!
			var img = doc.loadImage(mask);
			var dev = out.beginPage([0, 0, w, h]);
			dev.fillImage(img, [w,0,0,h,0,0], 1, null);
			out.endPage();
		}
	});
}
print("Saving PDF file!");
out.close();
