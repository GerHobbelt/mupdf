// A JavaScript variant of mutool extract, but only for images.

function zeropad(num, digits) {
	num = num.toString()
	while (num.length < digits)
		num = "0" + num
	return num
}

function ExtractImageDevice(format, pageNumber) {
	var imageCounter = 0
	return {
		fillImage: function (image, ctm, alpha) {
			var pix = image.toPixmap()
			var n = pix.getNumberOfComponents()
			var a = pix.getAlpha()
			var filename = "page-" + zeropad(pageNumber, 5) + "-image-" + zeropad(imageCounter, 4)

			if (format == "auto") {
				if (n == 1 || n == 3) {
					filename = filename + ".png"
					pix.saveAsPNG(filename)
				} else {
					filename = filename + ".pam"
					pix.saveAsPAM(filename)
				}
			} else if (format == "png") {
				if (n != 1 && n != 3)
					pix = pix.convertToColorSpace(DeviceRGB, null, null, null, true)
				filename = filename + ".png"
				pix.saveAsPNG(filename)
			} else if (format == "pam") {
				if (n != 1)
					pix = pix.convertToColorSpace(DeviceGray, null, null, null, true)
				filename = filename + ".pam"
				pix.saveAsPAM(filename)
			} else if (format == "pnm") {
				if ((n != 1 && n != 3) || a != 0)
					pix = pix.convertToColorSpace(DeviceRGB)
				filename = filename + ".pnm"
				pix.saveAsPNM(filename)
			} else if (format == "pkm") {
				if ((n != 1 && n != 4) || a != 0)
					pix = pix.convertToColorSpace(DeviceGray)
				filename = filename + ".pkm"
				pix.saveAsPKM(filename)
			} else if (format == "pbm") {
				if ((n != 1 && n != 4) || a != 0)
					pix = pix.convertToColorSpace(DeviceGray)
				filename = filename + ".pbm"
				pix.saveAsPBM(filename)
			} else if (format == "jpeg") {
				if ((n != 1 && n != 3 && n != 4) || a != 0)
					pix = pix.convertToColorSpace(DeviceRGB)
				filename = filename + ".jpg"
				pix.saveAsJPEG(filename)
			}

			print("extracting " + filename)
			imageCounter = imageCounter + 1
		},
	}
}

var FORMATS = [
	"jpeg",
	"png",
	"pam",
	"pnm",
	"pbm",
	"pkm"
]

var i, n = scriptArgs.length
if (n < 1) {
	print("usage: mutool run extract-images.js [jpeg|png|pam|pnm|pbm|pkm] <document.pdf|document.epub|...>")
	quit()
}

var doc, format
if (FORMATS.indexOf(scriptArgs[0]) > -1) {
	format = scriptArgs[0]
	doc = new Document(scriptArgs[1])
} else {
	format = "auto"
	doc = new Document(scriptArgs[0])
}

var n = doc.countPages()
for (var i = 0; i < n; ++i) {
	var page = doc.loadPage(i)
	page.run(new ExtractImageDevice(format, i), Identity)
}
