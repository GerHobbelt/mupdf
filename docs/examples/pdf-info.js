// A JavaScript variant of mutool info.

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
	var begin, end, tmp;
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
		tmp = begin;
		begin = end;
		end = tmp;
	}

	return [begin, end, arg];
}

function isBoxSame(a, b) {
	if (a == null || b == null)
		return false;
	return (
		a[0].compare(b[0]) == 0 &&
		a[1].compare(b[1]) == 0 &&
		a[2].compare(b[2]) == 0 &&
		a[3].compare(b[3]) == 0
	);
}

function insertBox(boxes, pageNo, newBox) {
	for (var i = 0; i < boxes.length; ++i)
		if (isBoxSame(boxes[i], newBox))
			return;

	boxes[pageNo] = newBox;
}

function gatherDimensions(page, pageNo, mediaBoxes) {
	var box = page.MediaBox;

	if (page.userUnit) {
		box[0] = box[0] * page.userUnit;
		box[1] = box[1] * page.userUnit;
		box[2] = box[2] * page.userUnit;
		box[3] = box[3] * page.userUnit;
	}

	insertBox(mediaBoxes, pageNo, box);
}

function isFontSame(a, b) {
	if (a == null && b != null)
		return false;
	if (a != null && b == null)
		return false;
	return a.compare(b) == 0;
}

function insertFont(fonts, pageNo, newFont) {
	for (var i = 0; i < fonts.length; ++i) {
		if (fonts[i] == null)
			continue;
		for (var k = 0; k < fonts[i].length; ++k) {
			if (isFontSame(fonts[i][k], newFont))
				return;
		}
	}

	if (fonts[pageNo] == null)
		fonts[pageNo] = [];
	fonts[pageNo].push(newFont);
}

function gatherFonts(page, pageNo, fonts) {
	if (page.Resources == null || page.Resources.Font == null)
		return;
	page.Resources.Font.forEach(function (name, fontdict) {
		insertFont(fonts, pageNo, fontdict);
	});
}

function isImageSame(a, b) {
	if (a == null && b != null)
		return false;
	if (a != null && b == null)
		return false;
	return a.compare(b) == 0;
}

function insertImage(images, pageNo, newImage) {
	for ( i = 0; i < images.length; ++i) {
		if (images[i] == null)
			continue;
		for (var k = 0; k < images[i].length; ++k) {
			if (isImageSame(images[i][k], newImage))
				return;
		}
	}

	if (images[pageNo] == null)
		images[pageNo] = [];
	images[pageNo].push(newImage);
}

function gatherImages(page, pageNo, images) {
	if (page.Resources == null || page.Resources.XObject == null)
		return;
	page.Resources.XObject.forEach(function (name, imagedict) {
		if (imagedict.Subtype == "Image")
			insertImage(images, pageNo, imagedict);
	});
}

function isShadingSame(a, b) {
	if (a == null && b != null)
		return false;
	if (a != null && b == null)
		return false;
	return a.compare(b) == 0;
}

function insertShading(shadings, pageNo, newShading) {
	for ( i = 0; i < shadings.length; ++i) {
		if (shadings[i] == null)
			continue;
		for (var k = 0; k < shadings[i].length; ++k) {
			if (isShadingSame(shadings[i][k], newShading))
				return;
		}
	}

	if (shadings[pageNo] == null)
		shadings[pageNo] = [];
	shadings[pageNo].push(newShading);
}

function gatherShadings(page, pageNo, shadings) {
	if (page.Resources == null || page.Resources.Shading == null)
		return;
	page.Resources.Shading.forEach(function (name, shadingdict) {
		if (shadingdict.ShadingType >= 1 && shadingdict.ShadingType <= 7)
			insertShading(shadings, pageNo, shadingdict);
	});
}

function isPatternSame(a, b) {
	if (a == null && b != null)
		return false;
	if (a != null && b == null)
		return false;
	return a.compare(b) == 0;
}

function insertPattern(patterns, pageNo, newPattern) {
	for ( i = 0; i < patterns.length; ++i) {
		if (patterns[i] == null)
			continue;
		for (var k = 0; k < patterns[i].length; ++k) {
			if (isPatternSame(patterns[i][k], newPattern))
				return;
		}
	}

	if (patterns[pageNo] == null)
		patterns[pageNo] = [];
	patterns[pageNo].push(newPattern);
}

function gatherPatterns(page, pageNo, patterns) {
	if (page.Resources == null || page.Resources.Pattern == null)
		return;
	page.Resources.Pattern.forEach(function (name, patterndict) {
		if (patterndict.PatternType >= 1 && patterndict.PatternType <= 7)
			insertPattern(patterns, pageNo, patterndict);
	});
}

function isFormSame(a, b) {
	if (a == null && b != null)
		return false;
	if (a != null && b == null)
		return false;
	return a.compare(b) == 0;
}

function insertForm(forms, pageNo, newForm) {
	for ( i = 0; i < forms.length; ++i) {
		if (forms[i] == null)
			continue;
		for (var k = 0; k < forms[i].length; ++k) {
			if (isFormSame(forms[i][k], newForm))
				return;
		}
	}

	if (forms[pageNo] == null)
		forms[pageNo] = [];
	forms[pageNo].push(newForm);
}

function gatherForms(page, pageNo, forms) {
	if (page.Resources == null || page.Resources.XObject == null)
		return;
	page.Resources.XObject.forEach(function (name, formdict) {
		if (formdict.Subtype == "Form" && formdict.Subtype2 == "PS")
			insertForm(forms, pageNo, formdict);
	});
}

function isPSSame(a, b) {
	if (a == null && b != null)
		return false;
	if (a != null && b == null)
		return false;
	return a.compare(b) == 0;
}

function insertPS(pss, pageNo, newPS) {
	for ( i = 0; i < pss.length; ++i) {
		if (pss[i] == null)
			continue;
		for (var k = 0; k < pss[i].length; ++k) {
			if (isFormSame(pss[i][k], newPS))
				return;
		}
	}

	if (pss[pageNo] == null)
		pss[pageNo] = [];
	pss[pageNo].push(newPS);
}

function gatherPSs(page, pageNo, pss) {
	if (page.Resources == null || page.Resources.XObject == null)
		return;
	page.Resources.XObject.forEach(function (name, psdict) {
		if (psdict.Subtype == "Form" && psdict.Subtype2 == "PS")
			insertPS(pss, pageNo, psdict);
	});
}

function printMediaBoxes(doc, mediaBoxes) {
	if (mediaBoxes == null || mediaBoxes.length == 0)
		return;

	print("Mediaboxes (" + (mediaBoxes.length - 1) + "):");
	for (var i = 1; i < mediaBoxes.length; ++i) {
		if (mediaBoxes[i] == null)
			continue;
		var page = doc.findPage(i - 1);
		print("\t" + i + "\t(" + page.toString() + "):\t" + mediaBoxes[i]);
	}
}

function printFonts(doc, fonts) {
	if (fonts == null || fonts.length == 0)
		return;

	var total = 0;
	for (var i = 1; i < fonts.length; ++i) {
		if (fonts[i] == null)
			continue;
		total = total + fonts[i].length;
	}
	print("Fonts (" + total + "):");
	for (var i = 1; i < fonts.length; ++i) {
		if (fonts[i] == null)
			continue;

		for (var k = 0; k < fonts[i].length; ++k) {
			var page = doc.findPage(i - 1);
			var subtype = fonts[i][k].Subtype;
			var name = fonts[i][k].BaseFont
			if (name == null)
				name = fonts[i][k].Name;
			var encoding = fonts[i][k].Encoding;
			if (encoding != null && encoding.isDictionary())
				encoding = encoding.BaseEncoding;

			if (encoding == null)
				encoding = "";
			else
				encoding = encoding.asName() + " ";
			print("\t" + i + "\t(" + page.toString() + "):\t" + subtype + " '" + name + "' " + encoding + "(" + fonts[i][k].asIndirect() + " 0 R)");
		}
	}
}

function printImages(doc, images) {
	if (images == null || images.length == 0)
		return;

	var total = 0;
	for (var i = 1; i < images.length; ++i) {
		if (images[i] == null)
			continue;
		total = total + images[i].length;
	}
	print("Images (" + total + "):");
	for (var i = 1; i < images.length; ++i) {
		if (images[i] == null)
			continue;

		for (var k = 0; k < images[i].length; ++k) {
			var page = doc.findPage(i - 1);

			var s = "\t" + i + "\t(" + page.toString() + "):\t[ ";
			var filter = images[i][k].Filter;
			if (filter != null && filter.isArray()) {
				var filters = filter;
				for (var m = 0; m < filters.length; ++m) {
					var name = filters[m].asName();
					if (name.indexOf("Decode"));
						name = name.substring(0, name.indexOf("Decode"));
					s = s + name + " ";
				}
			} else if (filter != null) {
				var name = filter.asName();
				if (name.indexOf("Decode"));
					name = name.substring(0, name.indexOf("Decode"));
				s = s + name + " ";
			} else {
				s = s + "Raw ";
			}
			s = s + "] ";

			var width = images[i][k].Width;
			var height = images[i][k].Height;
			var bpc = images[i][k].BitsPerComponent;
			s = s + width + "x" + height + " " + bpc + "bpc ";

			var cs = images[i][k].ColorSpace;
			var altcs = null;
			if (cs != null && cs.isArray()) {
				var cses = cs;
				cs = cses[0];
				if (cs != null && (cs == "DeviceN" || cs == "Separation")) {
					altcs = cses[2];
					if (altcs != null && altcs.isArray())
						altcs = altcs[0];
				}
			}
			if (cs != null) {
				var name = cs.asName();
				if (name.substring(0, 6) == "Device")
					s = s + name.slice(6);
				else if (name == "ICC")
					s = s + "ICC";
				else if (name == "Indexed")
					s = s + "Idx";
				else if (name == "Pattern")
					s = s + "Pat";
				else if (name == "Separation")
					s = s + "Sep";
			}
			if (altcs != null) {
				var name = altcs.asName();
				if (name.substring(0, 6) == "Device")
					s = s + " " + name.slice(6);
				else if (name == "ICC")
					s = s + " ICC";
				else if (name == "Indexed")
					s = s + " Idx";
				else if (name == "Pattern")
					s = s + " Pat";
				else if (name == "Separation")
					s = s + " Sep";
			}
			s = s + " (" + images[i][k].asIndirect() + " 0 R)";
			print(s);
		}
	}
}

function printShadings(doc, shadings) {
	if (shadings == null || shadings.length == 0)
		return;

	var total = 0;
	for (var i = 1; i < shadings.length; ++i) {
		if (shadings[i] == null)
			continue;
		total = total + shadings[i].length;
	}
	print("Shadings (" + total + "):");
	for (var i = 1; i < shadings.length; ++i) {
		if (shadings[i] == null)
			continue;

		for (var k = 0; k < shadings[i].length; ++k) {
			var page = doc.findPage(i - 1);

			var s = "\t" + i + "\t(" + page.toString() + "):\t ";
			switch (shadings[i][k].ShadingType.asNumber()) {
			default: s = s + "unknown"; break;
			case 1: s = s + "Function"; break;
			case 2: s = s + "Axial"; break;
			case 3: s = s + "Radial"; break;
			case 4: s = s + "Triangle mesh"; break;
			case 5: s = s + "Lattice"; break;
			case 6: s = s + "Coons patch"; break;
			case 7: s = s + "Tensor patch"; break;
			}
			s = s + " (" + shadings[i][k].asIndirect() + " 0 R)";
			print(s);
		}
	}
}

function printPatterns(doc, patterns) {
	if (patterns == null || patterns.length == 0)
		return;

	var total = 0;
	for (var i = 1; i < patterns.length; ++i) {
		if (patterns[i] == null)
			continue;
		total = total + patterns[i].length;
	}
	print("Patterns (" + total + "):");
	for (var i = 1; i < patterns.length; ++i) {
		if (patterns[i] == null)
			continue;

		for (var k = 0; k < patterns[i].length; ++k) {
			var page = doc.findPage(i - 1);
			var s = "\t" + i + "\t(" + page.toString() + "):\t ";
			switch (patterns[i][k].PatternType.asNumber()) {
			default:
			case 1:
				s = s + "Tiling";
				switch(patterns[i][k].PaintType.asNumber()) {
				default:
				case 1: s = s + " Colored"; break;
				case 2: s = s + " Uncolored"; break;
				}
				switch(patterns[i][k].TilingType.asNumber()) {
				default:
				case 1: s = s + " Constant"; break;
				case 2: s = s + " No distortion"; break;
				case 3: s = s + " Constant/fast tiling"; break;
				}
			break;
			case 2: s = s + "Shading " + patterns[i][k].Shading.asIndirect() + " 0 R"; break;
			}
			s = s + " (" + patterns[i][k].asIndirect() + " 0 R)";
			print(s);
		}
	}
}

function printForms(doc, forms) {
	if (forms == null || forms.length == 0)
		return;

	var total = 0;
	for (var i = 1; i < forms.length; ++i) {
		if (forms[i] == null)
			continue;
		total = total + forms[i].length;
	}
	print("Form xobjects (" + total + "):");
	for (var i = 1; i < forms.length; ++i) {
		if (forms[i] == null)
			continue;

		for (var k = 0; k < forms[i].length; ++k) {
			var page = doc.findPage(i - 1);
			var s = "\t" + i + "\t(" + page.toString() + "):\t Form";
			if (forms[i][k].Group != null && forms[i][k].Group.S != null) {
				s = s + " " + forms[i][k].Group.S.asName() + " Group";
			}
			if (forms[i][k].Ref != null) {
				s = s + " Reference";
			}
			s = s + " (" + forms[i][k].asIndirect() + " 0 R)";
			print(s);
		}
	}
}

function printPSs(doc, pss) {
	if (pss == null || pss.length == 0)
		return;

	var total = 0;
	for (var i = 1; i < pss.length; ++i) {
		if (pss[i] == null)
			continue;
		total = total + pss[i].length;
	}
	print("Postscript xobjects (" + total + "):");
	for (var i = 1; i < pss.length; ++i) {
		if (pss[i] == null)
			continue;

		for (var k = 0; k < pss[i].length; ++k) {
			var page = doc.findPage(i - 1);
			var s = "\t" + i + "\t(" + page.toString() + "):\t";
			s = s + " (" + pss[i][k].asIndirect() + " 0 R)";
			print(s);
		}
	}
}

function printInfo(doc, mediaBoxes, fonts, images, shadings, patterns, forms, pss)  {
	if (doDimensions)
		printMediaBoxes(doc, mediaBoxes);
	if (doFonts)
		printFonts(doc, fonts);
	if (doImages)
		printImages(doc, images);
	if (doShadings)
		printShadings(doc, shadings);
	if (doPatterns)
		printPatterns(doc, patterns);
	if (doXObjects) {
		printForms(doc, forms);
		printPSs(doc, pss);
	}
}

function process(doc, begin, end, pages) {
	if (doc == null)
		return;

	var singlePage = begin == end;

	var mediaBoxes = [];
	var fonts = [];
	var images = [];
	var shadings = [];
	var patterns = [];
	var forms = [];
	var pss = [];
	for (var pageNo = begin; pageNo <= end; ++pageNo) {

		var page = doc.findPage(pageNo - 1);
		gatherDimensions(page, pageNo, mediaBoxes);
		gatherFonts(page, pageNo, fonts);
		gatherImages(page, pageNo, images);
		gatherShadings(page, pageNo, shadings);
		gatherPatterns(page, pageNo, patterns);
		gatherForms(page, pageNo, forms);
		gatherPSs(page, pageNo, pss);

		if (singlePage) {
			print("Page " + pageNo + ":");
			printInfo(doc, mediaBoxes, fonts, images, shadings, patterns, forms, pss);
			mediaBoxes = [];
			fonts = [];
			images = [];
			shadings = [];
			patterns = [];
			forms = [];
			pss = [];
		}
	}

	if (!singlePage)
		printInfo(doc, mediaBoxes, fonts, images, shadings, patterns, forms, pss);
}

var arg = 0;
var password = null;
var doc = null;
var range;
var pages = 0;
var somethingShown = false;
var pageRanges = [];

var doDimensions = true;
var doFonts = true;
var doImages = true;
var doShadings = true;
var doPatterns = true;
var doXObjects = true;
var clearedOutput = false;

function clearOutput() {
	if (clearedOutput)
		return;
	clearedOutput = true;
	doDimensions = doFonts = doImage = doShadings = doPatterns = doXObjects = false;
}

if (scriptArgs.length == 0) {
	usage();
	failed = true;
} else {
	while (arg < scriptArgs.length) {
		if (scriptArgs[arg] == "-F") {
			clearOutput();
			doForms = true;
		} else if (scriptArgs[arg] == "-I") {
			clearOutput();
			doImages = true;
		} else if (scriptArgs[arg] == "-M") {
			clearOutput();
			doDimensions = true;
		} else if (scriptArgs[arg] == "-P") {
			clearOutput();
			doPatterns = true;
		} else if (scriptArgs[arg] == "-S") {
			clearOutput();
			doShadings = true;
		} else if (scriptArgs[arg] == "-X") {
			clearOutput();
			doXObjects = true;
		} else if (isPageRange(scriptArgs[arg])) {
			pageRanges.push(parsePageRange(scriptArgs[arg], pages));
			arg = arg + 1;
		} else {
			if (doc != null && pageRanges.length > 0) {
				for (var i = 0; i < pageRanges.length; ++i) {
					process(doc, pageRanges[i][0], pageRanges[i][1], pages);
				}
			}

			pageRanges = [];
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

			var version = doc.getVersion();
			print("PDF-" + version.major + "." + version.minor);
			print("");

			var info = doc.getTrailer().Info;
			if (info != null) {
				print("Info object (" + info.asIndirect() + " 0 R):");
				print(info.resolve().toString(false, true));
				print("");
			}

			var encrypt = doc.getTrailer().Encrypt;
			if (encrypt != null) {
				print("Encrypt object (" + encrypt.asIndirect() + " 0 R):");
				print(encrypt.resolve().toString(false, true));
				print("");
			}

			print("Pages: " + doc.countPages());
			print("");
		}
	}

	if (doc != null && pageRanges.length == 0) {
		process(doc, 1, pages, pages);
	} else {
		for (var i = 0; i < pageRanges.length; ++i) {
			process(doc, pageRanges[i][0], pageRanges[i][1], pages);
		}
	}
}
quit(failed);
