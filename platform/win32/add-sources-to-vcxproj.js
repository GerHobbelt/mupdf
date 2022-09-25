//
// add C/C++ source files to a vcxproj file & vcxproj.filter file
// 
// usage: run as
//
//     node ./add-sources-to-vcxproj.js your_project.vcxproj path-to-sources/ [ignoreSpecFile]
//

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

let ignores = [];
let rawIgnoresPath = process.argv[4] || '';
if (rawIgnoresPath.trim() !== '') {
  let ignoresPath = unixify(path.resolve(rawIgnoresPath));
  ignores = fs.readFileSync(ignoresPath, 'utf8').split('\n')
  .map((line) => line.trim())
  // filter out commented lines in the ignore spec
  .filter((line) => line.length > 0 && !/^[#;]/.test(line));

  console.log('Will ignore any paths which include: ', ignores);
}

let rawSourcesPath = process.argv[3];
if (!rawSourcesPath) {
  console.error("Missing sources directory argument");
  console.error("call:\n  add-sources-to-vcxproj.js xyz.vcxproj directory-of-sourcefiles");
  process.exit(1);
}
let sourcesPath = unixify(path.resolve(rawSourcesPath));
if (!fs.existsSync(sourcesPath)) {
    console.error("must specify valid sources directory argument");
    console.error("call:\n  add-sources-to-vcxproj.js xyz.vcxproj directory-of-sourcefiles");
    process.exit(1);
}
const globConfig = Object.assign({}, globDefaultOptions, {
  nodir: false,
  cwd: sourcesPath
});


let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
    console.error("must specify valid vcxproj file");
    console.error("call:\n  add-sources-to-vcxproj.js xyz.vcxproj directory-of-sourcefiles");
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
  "README", 
  "NEWS", 
  "CHANGES", 
  "ChangeLog", 
  "Contributors"
];
let specialFilenameRes = [];

function isSpecialMiscFile(f) {

  let name = path.basename(f);
  if (specialFilenameRes.length === 0) {
    for (let i = 0, len = specialFilenames.length; i < len; i++) {
      let mre = new RegExp(specialFilenames[i], 'i');
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

let pathWithWildCards = '*.*';
glob(pathWithWildCards, globConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

  let filterDirs = new Set();

  let a = files.map((f) => {
    return f.replace(sourcesPath + '/', '');
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
  .filter((f) => {
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
    case '.gcc':
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
  for (let item of Array.from(filterDirs.keys()).sort().values()) {
    item = item.replace(/\//g, '\\');
    do {
      if (!m.includes(`<Filter Include="${item}">`)) {
        fcnt++;
        let cntstr = '' + fcnt;
        cntstr = cntstr.padStart(4, '0');
        let slot = `
    <Filter Include="${item}">
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <Text Include="${f}">
      <Filter>${base}</Filter>
    </Text>
        `;
        filesToAdd.push(slot);

        slot = `
    <Text Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <ClCompile Include="${f}">
      <Filter>${base}</Filter>
    </ClCompile>
        `;
        filesToAdd.push(slot);

        slot = `
    <ClCompile Include="${f}" />
        `;
        filesToAddToProj.push(slot);
        break;

    case '.h':
    case '.hh':
    case '.h++':
    case '.hxx':
    case '.hpp':
    case '.icc':
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <ClInclude Include="${f}">
      <Filter>${base}</Filter>
    </ClInclude>
        `;
        filesToAdd.push(slot);

        slot = `
    <ClInclude Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <ResourceCompile Include="${f}">
      <Filter>${base}</Filter>
    </ResourceCompile>
        `;
        filesToAdd.push(slot);

        slot = `
    <ResourceCompile Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <Image Include="${f}">
      <Filter>${base}</Filter>
    </Image>
        `;
        filesToAdd.push(slot);

        slot = `
    <Image Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <Font Include="${f}">
      <Filter>${base}</Filter>
    </Font>
        `;
        filesToAdd.push(slot);

        slot = `
    <Font Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <Text Include="${f}">
      <Filter>${base}</Filter>
    </Text>
        `;
        filesToAdd.push(slot);

        slot = `
    <Text Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <MASM Include="${f}">
      <Filter>${base}</Filter>
    </MASM>
        `;
        filesToAdd.push(slot);

        slot = `
    <MASM Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <None Include="${f}">
      <Filter>${base}</Filter>
    </None>
        `;
        filesToAdd.push(slot);

        slot = `
    <None Include="${f}" />
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
        f = unixify(`${rawSourcesPath}/${item}`).replace(/\/\//g, '/');
        slot = `
    <Xml Include="${f}">
      <Filter>${base}</Filter>
    </Xml>
        `;
        filesToAdd.push(slot);

        slot = `
    <Xml Include="${f}" />
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
</Project>
    `;
  
  let fsrc1 = `
  <ItemGroup>
    ${ filesToAddToProj.join('\n') }
  </ItemGroup>
</Project>
    `;
  
  //console.error({files, a, sourcesPath, extraFilters, filesToAdd, fsrc1, fsrc2});

  filterSrc = filterSrc.replace(/<\/Project>[\s\r\n]*$/, fsrc2)
  .replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '')
  // and trim out empty lines:
  .replace(/[\s\r\n]+\n/g, '\n');

  src = src.replace(/[\s\r\n]<\/Project>[\s\r\n]*$/, fsrc1)
  .replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '')
  // fix ProjectDependencies: MSVC2019 is quite critical about whitespace around the UUID:
  .replace(/<Project>[\s\r\n]+[{]/g, '<Project>{')
  .replace(/[}][\s\r\n]+<\/Project>/g, '}</Project>')
  // and trim out empty lines:
  .replace(/[\s\r\n]+\n/g, '\n');

  fs.writeFileSync(filepath, src, 'utf8');
  fs.writeFileSync(filterFilepath, filterSrc, 'utf8');

  if (ignores.length > 0) {
    console.log("Added", a.length, "entries. (", ignoreCount, " files IGNORED due to IgnoreSpec.)");
  } else {
    console.log("Added", a.length, "entries.");
  }
});




