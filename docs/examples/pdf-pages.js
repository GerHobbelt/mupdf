// Javascript port of mutool pages.

var failed = false;

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

function isPageRange(arg) {
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

function parsePageRange(arg, pages) {
	var begin, end;
	var ret;

	if (arg.length == 0) {
		return false
	}

	if (arg[0] == ",") {
		arg = arg.slice(1);
	}

	if (arg[0] == "N") {
		begin = pages;
		arg = arg.slice(1);
	} else {
		ret = parseNumber(arg);
		begin = ret[0];
		arg = ret[1];
	}

	if (arg[0] == "-") {
		arg = arg.slice(1);
		if (arg[0] == "N") {
			end = pages;
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
		begin = pages + 1 + begin;
	}
	if (end < 0) {
		end = pages + 1 + end;
	}

	begin = clamp(begin, 1, pages);
	end = clamp(end, 1, pages);

	if (begin > end) {
		var tmp = begin;
		begin = end;
		end = tmp;
	}

	return [begin, end, arg];
}

function showBBox(page, key) {
	try {
		var bbox = page.get(key);
		if (bbox != null && bbox.isArray()) {
			var s = "<" + key + " ";
			s = s + "l=\"" + bbox[0] + "\" ";
			s = s + "b=\"" + bbox[1] + "\" ";
			s = s + "r=\"" + bbox[2] + "\" ";
			s = s + "t=\"" + bbox[3] + "\" ";
			print(s);
		}
	} catch (error) {
		failed = true;
	}
}

function showNumber(page, key) {
	try {
		var number = page.get(key);
		if (number != null && number.isNumber()) {
			var s = "<" + key + " ";
			s = s + "v=\"" + number + "\" ";
			s = s + "/>";
			print(s);
		}
	} catch (error) {
		failed = true;
	}
}

function processPage(doc, pageNumber) {
	var page;
	try  {
		page = doc.findPage(pageNumber-1);
	} catch (error) {
		print("Failed to gather information for page", pageNumber);
		failed = true;
		return;
	}

	print("<page pagenum=\"" + pageNumber + "\">");
	if (!failed) showBBox(page, "MediaBox");
	if (!failed) showBBox(page, "CropBox");
	if (!failed) showBBox(page, "ArtBox");
	if (!failed) showBBox(page, "BleedBox");
	if (!failed) showBBox(page, "TrimBox");
	if (!failed) showNumber(page, "Rotate");
	if (!failed) showNumber(page, "UserUnit");
	print("</page>");
}

function processPages(doc, begin, end) {
	if (doc == null)
		return;

	for (var page = begin; page <= end; ++page) {
		processPage(doc, page);
	}
}

var arg = 0;
var password = null;
var doc = null;
var pages = 0;
var range = null;

if (scriptArgs.length == 0) {
	print("usage: mutool run pdf-pages.js [options] file.pdf [pages]");
	print("\t-p\tpassword for decryption");
	print("\tpages\tcomma separated list of page numbers and ranges");
} else {
	while (arg < scriptArgs.length) {
		if (scriptArgs[arg] == "-p") {
			password = scriptArgs[arg + 1];
			arg = arg + 2;
		} else if (isPageRange(scriptArgs[arg])) {
			range = parsePageRange(scriptArgs[arg], pages);
			arg = arg + 1;
			processPages(doc, range[0], range[1]);
		} else {
			doc = null;
			pages = 0;
			try {
				doc = new Document(scriptArgs[arg])
				if (doc.needsPassword()) {
					if (!doc.authenticatePassword(password)) {
						print("cannot authenticate password: " + password);
					}
				}
				pages = doc.countPages();
			} catch (error) {
				print("cannot open document: " + scriptArgs[arg]);
			}
			arg = arg + 1;
		}
	}
}

quit(failed);
