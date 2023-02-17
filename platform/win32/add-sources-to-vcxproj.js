//
// add C/C++ source files to a vcxproj file & vcxproj.filters file
// 
// usage: run as
//
//     node ./add-sources-to-vcxproj.js your_project.vcxproj path-to-sources/ [ignoreSpecFile]
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
  //cache: {},
  //statCache: {},
  //symlinks: {},
  cwd: null,    // changed to, during the scan
  root: null,
  nomount: false,
  sync: true
};


function unixify(path) {
  return path.replace(/\\/g, '/');
}

function xmlEncode(path) {
	return path.replace(/["'<>&;?*]/g, function (m) {
		switch (m) {
			case '"':
				return "&quot;";

			case "'":
				return "&apos;";

			case "<":
				return "&lt;";

			case ">":
				return "&gt;";

			case "&":
				return "&amp;";

			case ";":
				console.warn("WARNING: Filename contains semicolon. Using Unicode homoglyph to appease MSVC, but you SHOULD rename the bugger to be safe!\n    File:", path);
				return ";";  // use a Unicode homoglyph as MSVC20XX b0rks on projects including files which include a semicolon in their name -- even when you HTML-entity-encode the bugger!

			case "?":
			case "*":
				console.warn(`WARNING: Filename contains wildcard '${m}'. Using underscore to appease MSVC, but you MUST rename the bugger to be safe!\n    File:`, path);
				return "_";
		}
		return m;
	});
}

let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
    console.error("must specify valid vcxproj file");
    console.error("call:\n  add-sources-to-vcxproj.js xyz.vcxproj directory-of-sourcefiles");
    process.exit(1);
}

let spec = {
	nasm_or_masm: 0,   // 0: auto; -1: masm; +1: nasm
	ignores: [
		'thirdparty/',
		'third_party/',
		'3rd_party/',
		'3rdparty/',
		'owemdjee/',
		'3rd/',
		'/b/',             // our in-company default cmake build work directory. NOT '/build/' as many projects already have that one as part of their source tree!
		'CMakeFiles/',
		'CMakeModules/',
		'cmake/modules/',
		'cmake/checks/',
		'fuzz.*',
		'node_modules/',
		'obj/Debug[^/]*',
		'obj/Release[^/]*',
	],
	sources: [],
	directories: [],
	special_inject: null,
};

let rawSourcesPath = process.argv[3] || '';
if (DEBUG > 2) console.error({argv: process.argv, rawSourcesPath})
if (rawSourcesPath.trim() !== '') {
	let pa = rawSourcesPath
	.split(';')
	.map((line) => line.trim());

	spec.directories.push(...pa);
}

let specPath = filepath.replace(/\.vcxproj/, '.spec');
if (fs.existsSync(specPath)) {
  if (DEBUG > 0) console.error("read .spec file:", {specPath, content: fs.readFileSync(specPath, 'utf8') })
  let rawSpec = fs.readFileSync(specPath, 'utf8').split('\n')
  .map((line) => line.trimEnd().replace(/\t/g, '    '))
  // filter out commented lines in the ignore spec
  .filter((line) => line.trim().length > 0 && !/^[#;]/.test(line.trim()))
  .join('\n');

  rawSpec = '\n' + rawSpec + '\n';
  
  if (DEBUG > 1) console.error("RAW PREPROC'D SPEC [START]:", {rawSpec, spec});
  
  if (/^NASM/im.test(rawSpec))
	  spec.nasm_or_masm = 1;
  else if (/^MASM/im.test(rawSpec))
	  spec.nasm_or_masm = -1;

  if (/^ignore:/m.test(rawSpec)) {
    if (DEBUG > 0) console.log("SPEC include [ignore] section...");
    spec.ignores = rawSpec.replace(/^.*\nignore:(.*?)\n(?:[^\s].*)?$/s, '$1')
    // .replace(/\\/g, '/')   -- some backslashes are quitee relevant in here as some lines will specify REGEXES, so we'll have to apply this path conversion later!
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line.trim().length > 0);
  }
  if (/^also-ignore:/m.test(rawSpec)) {
    if (DEBUG > 0) console.log("SPEC include [also-ignore] section...");
    let a = rawSpec.replace(/^.*\nalso-ignore:(.*?)\n(?:[^\s].*)?$/s, '$1')
    // .replace(/\\/g, '/')   -- some backslashes are quitee relevant in here as some lines will specify REGEXES, so we'll have to apply this path conversion later!
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line.trim().length > 0);
    
    spec.ignores = spec.ignores.concat(a);
  }

  if (/^special-inject:/m.test(rawSpec)) {
    if (DEBUG > 0) console.log("SPEC include [special-inject] section...");
    spec.special_inject = rawSpec.replace(/^.*\nspecial-inject:(.*?)\n(?:[^\s].*)?$/s, '$1');
  }

  if (/^sources:/m.test(rawSpec)) {
    if (DEBUG > 0) console.log("SPEC include [sources] section...");
    spec.sources = rawSpec.replace(/^.*\nsources:(.*?)\n(?:[^\s].*)?$/s, '$1')
    .replace(/\\/g, '/')
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line.trim().length > 0);
  }

  if (/^directories:/m.test(rawSpec)) {
    if (DEBUG > 0) console.log("SPEC include [directories] section...");
    let a = rawSpec.replace(/^.*\ndirectories:(.*?)\n(?:[^\s].*)?$/s, '$1')
    .replace(/\\/g, '/')
    .split('\n')
    .map((line) => line.trim())
    .filter((line) => line.trim().length > 0);
    spec.directories = spec.directories.concat(a);
  }

  if (DEBUG > 0) console.error("PROC'D SPEC [DONE]:", {rawSpec, spec});
}

let rawIgnoresPath = process.argv[4] || '';
if (rawIgnoresPath.trim() !== '') {
  let ignoresPath = unixify(path.resolve(rawIgnoresPath));
  if (!fs.existsSync(ignoresPath)) {
    console.error(`ERROR: user-specified ignores list file "${ignoresPath}" does not exist or is not a file.`);
    process.exit(1);
  }
  spec.ignores = fs.readFileSync(ignoresPath, 'utf8')
  // .replace(/\\/g, '/')   -- some backslashes are quitee relevant in here as some lines will specify REGEXES, so we'll have to apply this path conversion later!
  .split('\n')
  .map((line) => line.trim())
  // filter out commented lines in the ignore spec
  .filter((line) => line.length > 0 && !/^[#;]/.test(line.trim()));

  console.info('Will ignore any paths which include: ', spec.ignores);
}

if (DEBUG > 2) console.error({spec})


if (spec.sources.length + spec.directories.length === 0) {
  console.error("Missing sources directory argument or spec file.");
  console.error("call:\n  add-sources-to-vcxproj.js xyz.vcxproj directory-of-sourcefiles");
  console.error("\n\nCould not infer source/submodule directory for given MSVC vcxproj.");
  process.exit(1);
}


let src = fs.readFileSync(filepath, 'utf8');

let filterSrc = '';
let filterFilepath = filepath + '.filters';
if (fs.existsSync(filterFilepath)) {
    filterSrc = fs.readFileSync(filterFilepath, 'utf8');

    // nuke the filters file when it's functionally empty:
    if (!filterSrc.match(/<\/Project>/)) {
      filterSrc = '';
    }
}

if (!filterSrc.match(/<\?xml/)) {
    filterSrc = `<?xml version="1.0" encoding="utf-8"?>
    ` + filterSrc; 
}

if (!filterSrc.match(/<\/Project>/)) {
    filterSrc += `
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
</Project>
    `;
}


const specialFilenames = [
  "README[^/]*", 
  "NEWS", 
  "TODO", 
  "CHANGES", 
  "ChangeLog[^/]*", 
  "Contributors",
  "SConstruct",
  "SConscript"
];
let specialFilenameRes = [];

function isSpecialMiscFile(f) {
  let name = path.basename(f);
  if (specialFilenameRes.length === 0) {
    for (let i = 0, len = specialFilenames.length; i < len; i++) {
      let mre = new RegExp(`${ specialFilenames[i] }$`, 'i');
      specialFilenameRes[i] = mre;
    }
  }

  for (let re of specialFilenameRes) {
    if (re.test(name))
      return true;
  }
  return false;
}


let ignoreCount = 0;
let addedCount = 0;

let fsrc1_arr = [];
let fsrc2_arr = [];

if (DEBUG > 2) console.error({spec})

// turn all ignores[] into regexes:
spec.ignores = spec.ignores
.map(f => {
	//
	// we use the next heuristic to detect 'literal paths' instead of regexes: 
	//
	// 1. when the path does not contain any special regex char (except '.') 
	//    then it's a literal filename (or directory/path); 
	// 2. when it does not start with a '/' we assume it's an *entire* directory or file name, 
	//    i.e. 'ger.c' would then not be meant to match 'bugger.c'...
	// 3. anything else is a regex that can match ANY part of a filename/path.
	//
	// When these regexes are tested, the path-to-test has ALWAYS been prefixed with '/'
	// so the obvious and easy way to write a regex which matches only whole file and
	// directory names is to start it with a '/', e.g. regex '/[A-D]+' would match
	// '/x/y/AAAA/z' but not '/x/yAAAA/z'.
	//
	if (!/[()\[\]?:*+{}]/.test(f)) {
		// a literal path --> convert to regex format:
		f = f.replace(/[.]/g, '[.]')
		.replace(/\\/g, '/');

		// when ignore path starts with '/' it means to match from start of (reduced) file path!
		if (f.startsWith('/')) {
			f = '^' + f;
		}
		else {
			f = '/' + f;
		}
	}
	else {
		// a regex: no path \ -> / conversion applied as that mistake any regex escape character for a path separator!
	}

	if (DEBUG > 1) console.error("'ignore' line half-way through conversion to regex:", {f})
	
	let re = new RegExp(f);
	
	return re;
});

if (DEBUG > 1) console.error({spec})



function process_glob_list(files, sourcesPath, is_dir, rawSourcesPath) {
  if (DEBUG > 2) console.error("process_glob_list:", {files, sourcesPath, is_dir, spec})

  let filterDirs = new Set();

  let a = files.map((f) => {
    if (DEBUG > 2) console.error("files.map:", {f, dst: f.replace(sourcesPath + '/', ''), sourcesPath })
    return f.replace(sourcesPath + '/', '');
  })
  .filter((f) => {
    let f4f = '/' + f;	  
    if (spec.ignores.length > 0) {
      for (const sp of spec.ignores) {
        if (sp.test(f4f)) {
          if (DEBUG > 1) console.log('IGNORE:', {f, f4f, sp});
          ignoreCount++;
          return false;
        }
      }
      //console.log('PASS: testing:', f);
    }
    return true;
  })
  .filter((f) => {
    if (DEBUG > 2) console.error("files.filter:", {f})
    let base;
    switch (path.extname(f).toLowerCase()) {
    case '.c':
    case '.cc':
    case '.c++':
    case '.cxx':
    case '.cpp':
    case '.asm':
        filterDirs.add('Source Files');
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Source Files/' + base;
            filterDirs.add(base);
        }
        return true;

    case '.ac':
    case '.am':
    case '.bkl':
    case '.cmd':
    case '.cppcode':
    case '.csv':
    case '.cmake':
    case '.config':
    case '.conf':
    case '.cfg':
    case '.bazel':
    case '.gcc':
    case '.gperf':
    case '.htcpp':
    case '.in':
    case '.js':
    case '.jscode':
    case '.json':
    case '.l':
    case '.lua':
    case '.luacode':
    case '.md':
    case '.msc':
    case '.phpcode':
    case '.p1':
    case '.ps1':
    case '.py':
    case '.pythoncode':
    case '.rst':
    case '.rtf':
    case '.s':
    case '.sh':
    case '.bat':
    case '.cmd':
    case '.ts':
    case '.txt':
    case '.unx':
    case '.vc':
    case '.xml':
    case '.y':
    case '.tcl':
    case '.nsi':
        filterDirs.add('Misc Files');
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Misc Files/' + base;
            filterDirs.add(base);
        }
        return true;

    case '.h':
    case '.hh':
    case '.h++':
    case '.hxx':
    case '.hpp':
    case '.icc':
    case '.inc':
        filterDirs.add('Header Files');
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Header Files/' + base;
            filterDirs.add(base);
        }
        return true;

    case '.rc':
    case '.ico':
    case '.bmp':
    case '.png':
    case '.xpm':
    case '.jpg':
    case '.gif':
    case '.xrc':
    case '.ttf':
    case '.otf':
    case '.ttc':
    case '.mpg':
    case '.pov':
    case '.fbp':
        filterDirs.add('Resource Files');
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Resource Files/' + base;
            filterDirs.add(base);
        }
        return true;

    default:
        if (!isSpecialMiscFile(f))
          return false;

        filterDirs.add('Misc Files');
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Misc Files/' + base;
            filterDirs.add(base);
        }
        return true;
    }
  });

  // construct the filters to add:
  let extraFilters = [];
  let m = filterSrc;            // scratchpad: add new slots as we create them to prevent duplicate new entries
  let fcnt = 0;
  if (DEBUG > 3) console.error("filterDirs:", {filterDirs})
  for (let item of Array.from(filterDirs.keys()).sort().values()) {
    item = item.replace(/\//g, '\\');
    do {
      if (DEBUG > 3) console.error("item -- dir build-up:", {item})
      if (!m.includes(`<Filter Include="${xmlEncode(item)}">`)) {
        fcnt++;
        let cntstr = '' + fcnt;
        cntstr = cntstr.padStart(4, '0');
        let slot = `
    <Filter Include="${xmlEncode(item)}">
      <UniqueIdentifier>{d2a97047-4937-4f7a-ab2f-4485e03f${cntstr}}</UniqueIdentifier>
    </Filter>
        `;
        extraFilters.push(slot);
        m += slot;
      }

      // make sure the entire parent path is present in the filters too, or MSVC will barf!
      item = path.dirname(item);
      if (item === '.') {
          item = '';
      }
    } while (item.length > 0);
  }

  // sort the filter list to ensure parents come before childs:
  extraFilters.sort();

  // construct the files to add
  let filesToAdd = [];
  let filesToAddToProj = [];
  for (let item of a) {
    let base;
    let slot;
    let f;
    switch (path.extname(item).toLowerCase()) {
    default:
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Misc Files/' + base;
        }
        else {
            base = 'Misc Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <Text Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </Text>
        `;
        filesToAdd.push(slot);

        slot = `
    <Text Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.c':
    case '.cc':
    case '.c++':
    case '.cxx':
    case '.cpp':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Source Files/' + base;
        }
        else {
            base = 'Source Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <ClCompile Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </ClCompile>
        `;
        filesToAdd.push(slot);

        slot = `
    <ClCompile Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.h':
    case '.hh':
    case '.h++':
    case '.hxx':
    case '.hpp':
    case '.icc':
    case '.inc':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Header Files/' + base;
        }
        else {
            base = 'Header Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <ClInclude Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </ClInclude>
        `;
        filesToAdd.push(slot);

        slot = `
    <ClInclude Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.rc':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Resource Files/' + base;
        }
        else {
            base = 'Resource Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <ResourceCompile Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </ResourceCompile>
        `;
        filesToAdd.push(slot);

        slot = `
    <ResourceCompile Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.ico':
    case '.bmp':
    case '.png':
    case '.jpg':
    case '.gif':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Resource Files/' + base;
        }
        else {
            base = 'Resource Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <Image Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </Image>
        `;
        filesToAdd.push(slot);

        slot = `
    <Image Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.ttf':
    case '.otf':
    case '.ttc':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Resource Files/' + base;
        }
        else {
            base = 'Resource Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <Font Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </Font>
        `;
        filesToAdd.push(slot);

        slot = `
    <Font Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.xpm':
    case '.xrc':
    case '.fbp':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Resource Files/' + base;
        }
        else {
            base = 'Resource Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <Text Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </Text>
        `;
        filesToAdd.push(slot);

        slot = `
    <Text Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.asm':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Source Files/' + base;
        }
        else {
            base = 'Source Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
	let do_nasm = spec.nasm_or_masm;
	if (!do_nasm) {
		do_nasm = /masm/.test(f) ? -1 : +1;
	}
	let asmexe =  do_nasm < 0 ? "MASM" : "NASM";
        slot = `
    <${ asmexe } Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </${ asmexe }>
        `;
        filesToAdd.push(slot);

        slot = `
    <${ asmexe } Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.mpg':
    case '.pov':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Resource Files/' + base;
        }
        else {
            base = 'Resource Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <None Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </None>
        `;
        filesToAdd.push(slot);

        slot = `
    <None Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.xml':
        base = path.dirname(item);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Misc Files/' + base;
        }
        else {
            base = 'Misc Files';
        }
        base = base.replace(/\//g, '\\');
        if (DEBUG > 3) console.error("item -- re-construct the file:", {item, rawSourcesPath, is_dir})
        f = unixify(is_dir ? `${rawSourcesPath}/${item}` : `${rawSourcesPath}`).replace(/\/\//g, '/');
        slot = `
    <Xml Include="${xmlEncode(f)}">
      <Filter>${xmlEncode(base)}</Filter>
    </Xml>
        `;
        filesToAdd.push(slot);

        slot = `
    <Xml Include="${xmlEncode(f)}" />
        `;
        filesToAddToProj.push(slot);
        break;
    }
  }

  filesToAdd.sort();
  filesToAddToProj.sort();

  // merge it all into the target file(s):
  let fsrc2 = `
  <ItemGroup>
    ${ extraFilters.join('\n') }
  </ItemGroup>
  <ItemGroup>
    ${ filesToAdd.join('\n') }
  </ItemGroup>
    `;
  
  let fsrc1 = `
  <ItemGroup>
    ${ filesToAddToProj.join('\n') }
  </ItemGroup>
    `;

  //console.error({files, a, sourcesPath, extraFilters, filesToAdd, fsrc1, fsrc2});
  addedCount += a.length;

  fsrc1_arr.push(fsrc1);
  fsrc2_arr.push(fsrc2);
}



function process_path(rawSourcesPath, is_dir) {
	if (DEBUG > 1) console.error("process_path RAW:", {rawSourcesPath, is_dir});
	while (/\/[^.\/][^\/]*\/\.\.\//.test(rawSourcesPath)) {
		rawSourcesPath = rawSourcesPath.replace(/\/[^.\/][^\/]*\/\.\.\//, '/')
	}
	
	let sourcesPath = unixify(path.resolve(rawSourcesPath.trim()));
	if (DEBUG > 1) console.error("process_path NORMALIZED:", {rawSourcesPath, sourcesPath, is_dir});
	if (!fs.existsSync(sourcesPath)) {
	    console.error("Non-existing path specified:", sourcesPath);
	    process.exit(1);
	}

	const globConfig = Object.assign({}, globDefaultOptions, {
	  nodir: !is_dir,
	  cwd: is_dir ? sourcesPath : path.dirname(sourcesPath)
	});

	let pathWithWildCards = is_dir ? '*' : path.basename(sourcesPath);
	if (DEBUG > 2) console.error("process_path GLOB:", {pathWithWildCards, globConfig, cwd: globConfig.cwd, is_dir});

	let files_rec = glob(pathWithWildCards, globConfig);
	if (DEBUG > 2) console.error("process_path GLOB DONE:", {pathWithWildCards, globConfig, cwd: files_rec.cwd, is_dir, found: files_rec.found});
	process_glob_list(files_rec.found, files_rec.cwd, is_dir, rawSourcesPath);
}



if (spec.special_inject != null) {
	fsrc1_arr.push(spec.special_inject);
}



for (let f of spec.sources) {
	process_path(f, false);
}
for (let f of spec.directories) {
	process_path(f, true);
}

	
filterSrc = filterSrc.replace(/<\/Project>[\s\r\n]*$/, fsrc2_arr.join('\n') + `
</Project>
`)
.replace(/<ItemGroup>[\s\r\n]*?<\/ItemGroup>/g, '')
// and trim out empty lines:
.replace(/[\s\r\n]+\n/g, '\n');

src = src.replace(/[\s\r\n]<\/Project>[\s\r\n]*$/, fsrc1_arr.join('\n') + `
</Project>
`)
.replace(/<ItemGroup>[\s\r\n]*?<\/ItemGroup>/g, '')
// fix ProjectDependencies: MSVC2019 is quite critical about whitespace around the UUID:
.replace(/<Project>[\s\r\n]+[{]/g, '<Project>{')
.replace(/[}][\s\r\n]+<\/Project>/g, '}</Project>')
// and trim out empty lines:
.replace(/[\s\r\n]+\n/g, '\n');

fs.writeFileSync(filepath, src, 'utf8');
fs.writeFileSync(filterFilepath, filterSrc, 'utf8');

if (spec.ignores.length > 0) {
console.info("Added", addedCount, "entries. (", ignoreCount, " files IGNORED due to IgnoreSpec.)");
} else {
console.info("Added", addedCount, "entries.");
}




