// Script to exact raw JPEG image streams from a PDF

function zeropad(num, digits) {
	num = num.toString()
	while (num.length < digits)
		num = "0" + num
	return num
}

var page = 0

function processPages(pages) {
	if ("Kids" in pages) {
		var kids = pages.Kids
		for (var i = 0; i < kids.length; ++i) {
			processPages(kids[i])
		}
	} else if ("Resources" in pages) {
		var resources = pages.Resources
		var image = 0
		if ("XObject" in resources) {
			resources.XObject.forEach(function(name, xobj) {
				if (xobj.Subtype == "Image" && xobj.Filter == "DCTDecode") {
					var filename = "page-" + zeropad(page, 5) + "-image-" + zeropad(image, 4) + ".jpg"
					var buf = xobj.readRawStream()
					buf.save(filename)
					print("saved " + filename)
					image = image + 1
				}
			})
		}
	}
}

if (scriptArgs.length < 1) {
	print("usage: mutool run pdf-extract-jpeg.js document.pdf")
	quit()
}

var doc = new Document(scriptArgs[0])
var pages = doc.getTrailer().Root.Pages
processPages(pages, 0)
