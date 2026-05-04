// A JavaScript variant of mutool extract.

var failed = false;
var fontNumber = 0;
var imageNumber = 0;

function usage() {
	print("usage: mutool run pdf-extract.js [options] file.pdf [object numbers]")
	print("\t-p\tpassword for decryption");
	print("\t-r\tconvert images to rgb");
	print("\t-a\tembed SMasks as alpha channel");
	print("\t-N\tdo not use ICC color conversion");
}

function zeropad(num, digits) {
	num = num.toString()
	while (num.length < digits)
		num = "0" + num
	return num
}

function parseNumber(arg) {
	var number = "";

	if (arg.length > 0 && arg[0] == "-") {
		number = "-";
		arg = arg.slice(1);
	}

	while (arg.length > 0 && arg[0] >= "0" && arg[0] <= "9") {
		number = number + arg[0];
		arg = arg.slice(1);
	}

	number = parseInt(number, 10);
	return [number, arg];
}

function clamp(val, min, max) {
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

function isObjectRange(arg) {
	var idx = 0;
	while (idx < arg.length) {
		if (arg[idx] >= "0" && arg[idx] <= "9") {
		} else if (arg[idx] == "N") {
		} else if (arg[idx] == "-") {
		} else if (arg[idx] == ".") {
		} else {
			return false;
		}
		idx = idx + 1;
	}
	return true;
}

function parseObjectRange(arg, objects) {
	var begin, end;
	var ret;

	if (arg.length == 0) {
		return false
	}

	if (arg[0] == ",") {
		arg = arg.slice(1);
	}

	if (arg[0] == "N") {
		begin = objects;
		arg = arg.slice(1);
	} else {
		ret = parseNumber(arg);
		begin = ret[0];
		arg = ret[1];
	}

	if (arg[0] == "-") {
		arg = arg.slice(1);
		if (arg[0] == "N") {
			end = objects;
			arg = arg.slice(1);
		} else {
			ret = parseNumber(arg);
			end = ret[0];
			arg = ret[1];
		}
	} else  {
		end = begin;
	}

	if (begin < 0) {
		begin = objects + 1 + begin;
	}
	if (end < 0) {
		end = objects + 1 + end;
	}

	begin = clamp(begin, 1, objects);
	end = clamp(end, 1, objects);

	if (begin > end) {
		var tmp = begin;
		begin = end;
		end = tmp;
	}

	return [begin, end, arg];
}

function isImage(ref) {
	return ref != null && ref.Subtype == "Image";
}

function saveImage(doc, ref) {
	var image = doc.loadImage(ref);
	var imgdata = image.getImageData();

	var type = imgdata.type;
	if (image.getColorKey() != null)
		type = "unknown";
	if (image.getDecode() != null)
		type = "unknown";
	if (image.getMask())
		type = "unknown";

	if (doRGB) {
		var imgcs = image.getColorSpace();
		if (!imgcs.isRGB() && imgcs.isGray())
			type = "unknown";
	}

	var filename = "image-" + zeropad(imageNumber, 4);
	imageNumber = imageNumber + 1;
	if (type == "jpeg") {
		filename = filename + ".jpg";
		print("extracting " + filename);
		imgdata.buffer.save(filename);
	} else {
		var pix = image.toPixmap();

		if (image.getMask() != null && doAlpha) {
			var mask = image.getMask().toPixmap();
			if (mask.getWidth() == pixmap.getWidth() && mask.getHeight() == pixmap.getHeight()) {
				pix = new Pixmap(pix, mask);
			} else {
				printf("cannot combine image with smaks if different resolution");
			}
		}

		var cs = pix.getColorSpace();

		if (doRGB && cs != null && !cs.isRGB()) {
			pix = pix.convertToColorSpace(DeviceRGB, null, null, null, true);
			cs = pix.getColorSpace();
		}

		if (cs.isCMYK()) {
			filename = filename + ".pam";
			print("extracting " + filename);
			pix.saveAsPAM(filename);
		}
		else
		{
			if (cs != null && !cs.isGray() && !cs.isRGB())
				pix = pix.convertToColorSpace(DeviceRGB, null, null, null, true)
			filename = filename + ".png";
			print("extracting " + filename);
			pix.saveAsPNG(filename);
		}
	}
}

function isFontdesc(ref) {
	return ref != null && ref.Type == "FontDescriptor";
}

function saveFont(ref) {
	var stream, ext;

	if (ref.get("FontFile")) {
		stream = ref.FontFile;
		ext = "pfa";
	}
	if (ref.get("FontFile2")) {
		stream = ref.FontFile2;
		ext = "ttf";
	}
	if (ref.get("FontFile3")) {
		stream = ref.FontFile3;
		var subtype = ref.FontFile3.Subtype;
		var ext;
		if (subtype == "Type1C") {
			ext = "cff";
		} else if (subtype == "CIDFontType0C") {
			ext = "cid";
		} else if (subtype == "OpenType") {
			ext = "otf";
		} else {
			print("Unhandled font type: " + subtype);
			return;
		}
	}

	if (stream == null) {
		print("No font data");
		return;
	}

	var filename = "font-" + zeropad(fontNumber, 4) + "." + ext;
	var buf = stream.readStream();
	buf.save(filename);
	fontNumber = fontNumber + 1;
}

function processObject(doc, num) {
	try {
		var ref = doc.newIndirect(num, 0);

		if (isImage(ref)) {
			saveImage(doc, ref);
		} else if (isFontdesc(ref)) {
			saveFont(ref);
		}
	} catch (error) {
		print("Ignoring object " + num + " " + error);
	}
}

function processObjects(doc, begin, end) {
	if (doc == null)
		return;

	for (var idx = begin; idx < end; ++idx) {
		processObject(doc, idx);
	}
}

var arg = 0;
var password = null;
var doc = null;
var range;
var doRGB = false;
var doAlpha = false;
var doicc = true;
var objects = 0;
var objectRanges = [];

if (scriptArgs.length == 0) {
	usage();
	failed = true;
} else {
	while (arg < scriptArgs.length) {
		if (scriptArgs[arg] == "-p") {
			password = scriptArgs[arg + 1];
			arg = arg + 2;
		} else if (scriptArgs[arg] == "-r") {
			doRGB = true;
			arg = arg + 1;
		} else if (scriptArgs[arg] == "-a") {
			doalpha = true;
			arg = arg + 1;
		} else if (scriptArgs[arg] == "-N") {
			doicc = false;
			arg = arg + 1;
		} else if (isObjectRange(scriptArgs[arg])) {
			objectRanges.push(parseObjectRange(scriptArgs[arg], objects));
			arg = arg + 1;
		} else {
			if (doc != null && objectRanges.length > 0) {
				for (var i = 0; i < objectRanges.length; ++i) {
					processObjects(doc, objectRanges[i][0], objectRanges[i][1], objects);
				}
			}

			objectRanges = [];
			doc = null;
			objects = 0;
			try {
				doc = new Document(scriptArgs[arg])
				if (doc.needsPassword()) {
					if (!doc.authenticatePassword(password)) {
						print("cannot authenticate password: " + password);
					}
				}
				objects = doc.countObjects();
			} catch (error) {
				print("cannot open document: " + scriptArgs[arg]);
			}
			arg = arg + 1;
		}
	}

	if (doc != null && objectRanges.length == 0) {
		processObjects(doc, 1, objects, objects);
	} else {
		for (var i = 0; i < objectRanges.length; ++i) {
			processObjects(doc, objectRanges[i][0], objectRanges[i][1], objects);
		}
	}
}
quit(failed);
