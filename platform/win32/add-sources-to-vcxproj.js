//
// add C/C++ source files to a vcxproj file & vcxproj.filter file
// 
// usage: run as
//
//     node ./add-sources-to-vcxproj.js your_project.vcxproj path-to-sources/
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

let rawSourcesPath = process.argv[3];
let sourcesPath = unixify(path.resolve(rawSourcesPath));
const globConfig = Object.assign({}, globDefaultOptions, {
  nodir: false,
  cwd: sourcesPath
});


let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
    console.error("must specify valid vcxproj file");
    process.exit(1);
}
let src = fs.readFileSync(filepath, 'utf8');

let filterSrc = '';
let filterFilepath = filepath + '.filters';
if (fs.existsSync(filterFilepath)) {
    filterSrc = fs.readFileSync(filterFilepath, 'utf8');
}

if (!filterSrc.match(/<\?xml/)) {
    filterSrc = `<?xml version="1.0" encoding="utf-8"?>
    ` + filterSrc; 
}

if (!filterSrc.match(/<Project /)) {
    filterSrc += `
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Source Files">
      <UniqueIdentifier>{d2a97047-4937-4f7a-ab2f-4485e03fd328}</UniqueIdentifier>
    </Filter>
    <Filter Include="Header Files">
      <UniqueIdentifier>{42f0fc98-34f6-4567-b3cf-de13e74a89ab}</UniqueIdentifier>
    </Filter>
    <Filter Include="Misc Files">
      <UniqueIdentifier>{b1798409-2031-413d-9df7-70b0202ddd98}</UniqueIdentifier>
    </Filter>
  </ItemGroup>
</Project>
    `;
}

if (!filterSrc.match(/<Filter Include="Source Files">/)) {
    filterSrc = filterSrc.replace(/<\/Project>/, `
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Source Files">
      <UniqueIdentifier>{d2a97047-4937-4f7a-ab2f-4485e03fd328}</UniqueIdentifier>
    </Filter>
    <Filter Include="Header Files">
      <UniqueIdentifier>{42f0fc98-34f6-4567-b3cf-de13e74a89ab}</UniqueIdentifier>
    </Filter>
    <Filter Include="Misc Files">
      <UniqueIdentifier>{b1798409-2031-413d-9df7-70b0202ddd98}</UniqueIdentifier>
    </Filter>
  </ItemGroup>
</Project>
    `);
}


/*
<?xml version="1.0" encoding="utf-8"?>
<Project ToolsVersion="4.0" xmlns="http://schemas.microsoft.com/developer/msbuild/2003">
  <ItemGroup>
    <Filter Include="Source Files">
      <UniqueIdentifier>{d2a97047-4937-4f7a-ab2f-4485e03fd328}</UniqueIdentifier>
    </Filter>
    <Filter Include="Header Files">
      <UniqueIdentifier>{42f0fc98-34f6-4567-b3cf-de13e74a89ab}</UniqueIdentifier>
    </Filter>
    <Filter Include="Misc Files">
      <UniqueIdentifier>{b1798409-2031-413d-9df7-70b0202ddd98}</UniqueIdentifier>
    </Filter>
  </ItemGroup>
  <ItemGroup>
    <ClCompile Include="..\..\scripts\libclipp\monolithic_main.c">
      <Filter>Source Files</Filter>
    </ClCompile>
  </ItemGroup>
  <ItemGroup>
    <ClInclude Include="..\..\thirdparty\owemdjee\clipp\examples\monolithic_examples.h">
      <Filter>Header Files</Filter>
    </ClInclude>
  </ItemGroup>
</Project>
*/

let pathWithWildCards = '*.*';
glob(pathWithWildCards, globConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

  let filterDirs = new Set();

  let a = files.map((f) => {
    return f.replace(sourcesPath + '/', '');
  }).filter((f) => {
    let base;
    switch (path.extname(f).toLowerCase()) {
    case '.c':
    case '.cc':
    case '.cpp':
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Source Files/' + base;
            filterDirs.add(base);
        }
        return true;

    case '.h':
    case '.hh':
    case '.hpp':
        base = path.dirname(f);
        if (base === '.') {
          base = '';
        }
        if (base.length > 0) {
            base = 'Header Files/' + base;
            filterDirs.add(base);
        }
        return true;

    default:
        return false;
    }
  });

  // construct the filters to add:
  let extraFilters = [];
  let m = filterSrc;            // scratchpad: add new slots as we create them to prevent duplicate new entries
  let fcnt = 0;
  for (let item of filterDirs.keys()) {
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
    case '.hpp':
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

  filterSrc = filterSrc.replace(/<\/Project>/, fsrc2)
  .replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '');
  src = src.replace(/<\/Project>/, fsrc1)
  .replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '');

  fs.writeFileSync(filepath, src, 'utf8');
  fs.writeFileSync(filterFilepath, filterSrc, 'utf8');

  console.log("Added", a.length, "entries.");
});




