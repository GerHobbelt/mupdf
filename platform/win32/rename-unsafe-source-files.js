//
// rename all unsafe filenames in all (sub)directories. When DIR is not specified (argv[1]), then '.' is assumed.
// 
// usage: run as
//
//     node ./rename-unsafe-source-files.js path-to-sources/
//

// UTF8 hint for editors: aAÀÁÂÃÄÅàáâãäåɑΑαаᎪbBßʙΒβВЬᏴᛒcCϲϹСсᏟⅭⅽ𐐠dDĎďĐđԁժᎠḍⅮⅾeEÈÉÊËéêëĒēĔĕĖėĘĚěΕЕеᎬfFϜgGɡɢԌնᏀhHʜΗНһᎻiIlΙІіjJϳЈјյᎫkKΚКᏦᛕKLʟᏞⅬmMΜϺМᎷᛖⅯⅿnNɴΝ0OoΟοОоՕpPΡρРрᏢqQႭႳrRʀᏒᚱsSЅѕՏႽᏚtTΤτТᎢuUμυԱՍ⋃vVνѴѵᏙⅤⅴwWѡᎳxXΧχХхⅩⅹyYʏΥγуҮzZΖᏃ!

let fs = require('fs');
let path = require('path');
let glob = require('@gerhobbelt/glob');

let DEBUG = 0;
const globDefaultOptions = {
  debug: (DEBUG > 4),
  matchBase: true, // true: pattern starting with / matches the basedir, while non-/-prefixed patterns will match in any subdirectory --> act like **/<pattern>
  silent: false,   // report errors to console.error UNLESS those are already emitted (`strict` option)
  strict: true,    // emit errors
  realpath: true,
  realpathCache: {},
  follow: false,
  dot: false,
  mark: true,    // postfix '/' for DIR entries
  nodir: true,
  sync: false,
  nounique: false,
  nonull: false,
  nosort: true,
  nocase: true,     //<-- uncomment this one for total failure to find any files >:-((
  stat: false,
  noprocess: false,
  absolute: false,
  maxLength: Infinity,
  cache: {},
  statCache: {},
  symlinks: {},
  cwd: null,    // changed to, during the scan
  root: null,
  nomount: false
};


function unixify(path) {
  return path.replace(/\\/g, '/');
}

function sanitize(path) {
	path = path
	.replace(/["'<>&;?*]/g, function (m) {
		switch (m) {
			default:
				break;

			case "<":
			case ">":
			case ";":
			case "?":
			case "*":
				return " - ";
		}
		return m;
	})
	.replace(/\.\.+/g, '.')
	.replace(/  /g, ' - ')
	.replace(/\s+/g, ' ')
	.replace(/ -\s+- /g, ' - ')
	.replace(/- \./g, '-.')
	.trim();

	return path;
}

let ignores = [];
let rawIgnoresPath = process.argv[3] || '';
if (rawIgnoresPath.trim() !== '') {
  let ignoresPath = unixify(path.resolve(rawIgnoresPath));
  ignores = fs.readFileSync(ignoresPath, 'utf8').split('\n')
  .map((line) => line.trim())
  // filter out commented lines in the ignore spec
  .filter((line) => line.length > 0 && !/^[#;]/.test(line));

  console.log('Will ignore any paths which include: ', ignores);
}

let rawSourcesPath = process.argv[2];
if (!rawSourcesPath) {
  console.error("Missing sources directory argument");
	console.error("call:\n  rename-unsafe-source-files.js directory-of-sourcefiles");
  process.exit(1);
}
let sourcesPath = unixify(path.resolve(rawSourcesPath));
if (!fs.existsSync(sourcesPath)) {
    console.error("must specify valid sources directory argument");
	console.error("call:\n  rename-unsafe-source-files.js directory-of-sourcefiles");
    process.exit(1);
}
const globConfig = Object.assign({}, globDefaultOptions, {
  nodir: false,
  cwd: sourcesPath
});


let ignoreCount = 0;

let pathWithWildCards = '*.*';
glob(pathWithWildCards, globConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

  let filterDirs = new Set();

	let a = files.map((f) => {
		return f; // .replace(sourcesPath + '/', '');
	})
	.filter((f) => {
		if (ignores.length > 0) {
			for (const spec of ignores) {
				if (f.includes(spec)) {
					//console.log('IGNORE: testing:', f, {spec});
					ignoreCount++;
					return false;
				}
			}
			//console.log('PASS: testing:', f);
		}
		return true;
	})
	.map((f) => {
		f = f.replace(/\//g, '\\');
		let basename = path.basename(f);
		let base = path.dirname(f);
		if (base === '.') {
			base = '';
		}
		let sanename = sanitize(basename);

		if (sanename === basename)
			return null;

		console.log({ f, base, basename, sanename })

		fs.renameSync(f, base + '/' + sanename);

		return f;
	})
	.filter((f) => {
		return !!f;
	});


  if (ignores.length > 0) {
    console.log("Processed", a.length, "entries. (", ignoreCount, " files IGNORED due to IgnoreSpec.)");
  } else {
    console.log("Processed", a.length, "entries.");
  }
});




