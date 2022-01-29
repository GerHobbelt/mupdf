//
// generate 'JUMBO' source files, which #include most of the core wxWidgets sources for the various platforms.
// 
// usage: run as
//
//     node ./mk_jumbo_source_files.js
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


function unixify(p) {
  return p.replace(/\\/g, '/');
}

let rawSourcesPath = '../../thirdparty/owemdjee/wxWidgets/src/';
let sourcesPath = unixify(path.resolve(rawSourcesPath));
if (!fs.existsSync(sourcesPath)) {
    console.error("must have wxWidgets submodule initialized");
    process.exit(1);
}
const globConfig = Object.assign({}, globDefaultOptions, {
  nodir: false,
  cwd: sourcesPath
});


// nuke the old generated files first:
let rawDestPath = '../../scripts/wxWidgets/';
let destPath = unixify(path.resolve(rawDestPath));
const globNukeConfig = Object.assign({}, globDefaultOptions, {
  nodir: true,
  cwd: destPath
});

let nukespec = 'jumbo-*.cpp';
glob(nukespec, globNukeConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

  files.forEach((file) => {
    fs.unlinkSync(file);
  });
});

// now collect all wxW source files and construct the jumbo files from the filtered set.

let scanspec = '*.c*';
console.error({scanspec});
glob(scanspec, globConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

  files = files.filter((f) => /\.c(:?pp)?/.test(path.extname(f)) );

  files.sort();

  let a = files.map((f) => {
    return {
      source: f.replace(sourcesPath + '/', ''),
      file: f.replace(/^.*\/thirdparty\/owemdjee\/wxWidgets/, '../../thirdparty/owemdjee/wxWidgets')
    };
  });

  /*
cat > jumbo-richtext-source.cpp <<EOF

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

EOF

for f in $(  find ../../thirdparty/owemdjee/wxWidgets/src/richtext/ -name '*.cpp' ) ; do

  g=$f
  f=$( echo $f | sed -e 's/^.*\///' )

  echo "Adding $f..."

  cat >> jumbo-richtext-source.cpp <<EOF  

//------------------------------------------------------------------------
// $f
 
EOF

  cat >> jumbo-richtext-source.cpp <<EOF
#include "$g"
EOF

done
   */
  
  let dstCode1 = `

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD

`;
  let dstCode2 = dstCode1;
  let dstCode3 = dstCode1;
  let dstCode4 = dstCode1;

  a.filter((rec) => rec.source.includes('richtext/'))
  .forEach((rec, idx) => {
    let name = path.basename(rec.source);

    let chunk = `

//------------------------------------------------------------------------
// ${name}
 
#include "${rec.file}"
`;

    let x2 = /(?:richtextbuffer|richtextctrl|richtextprint|richtextstyledlg|richtextstyles)\.cpp/.test(rec.source);
    let x1 = /(?:richtexttabspage)\.cpp/.test(rec.source);
    if (x2 && !x1) {
      if (idx < 14) {
        dstCode3 += chunk;
      } else {
        dstCode4 += chunk;
      }
    }
    else if ((idx >= 17) && !x1) {
      dstCode2 += chunk;
    }
    else {
      dstCode1 += chunk;
    }
  });

  fs.writeFileSync('../../scripts/wxWidgets/jumbo-richtext-source1.cpp', dstCode1, 'utf8');
  fs.writeFileSync('../../scripts/wxWidgets/jumbo-richtext-source2.cpp', dstCode2, 'utf8');
  fs.writeFileSync('../../scripts/wxWidgets/jumbo-richtext-source3.cpp', dstCode3, 'utf8');
  fs.writeFileSync('../../scripts/wxWidgets/jumbo-richtext-source4.cpp', dstCode4, 'utf8');

  console.log("richtext bunch jumbo-ed.");


  //--------------------
  // and do the same for the ribbon sources:
  let dstCode = `

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD

`;
  a.filter((rec) => rec.source.includes('ribbon/'))
  .forEach((rec) => {
    let name = path.basename(rec.source);

    dstCode += `

//------------------------------------------------------------------------
// ${name}
 
#include "${rec.file}"
`;
  });

  fs.writeFileSync('../../scripts/wxWidgets/jumbo-ribbon-source.cpp', dstCode, 'utf8');

  console.log("ribbon bunch jumbo-ed.");


  //--------------------
  // next phase: find out which files match in filename (and thus cause trouble in MSVXC project files
  // as they'll generate the same OBJ file); collect these sets for subsequent use.
  
  let duplimap = new Map();

  let r = a.filter((rec) => {
    let x1 = /^(?:aui|html|propgrid|xrc|common|generic|msw|motif|gtk|gtk1|x11|qt|dfb|osx|univ|unix|uwp)\//.test(rec.source);
    let x2 = /(?:dummy|dll_init|init|regiong|strconv|graphicsd2d|notifmsgrt|mediactrl_qt|xh_spin|xh_slidr|choice|(?:webview[a-z0-9_]*))\.cpp/.test(rec.source);
    return x1 && !x2;
  });

  r.forEach((rec) => {
    let name = path.basename(rec.source);

    if (duplimap.has(name)) {
      let v = duplimap.get(name);
      v.push({
        name,
        source: rec.source,
        file: rec.file
      });
      duplimap.set(name, v);
    } else {
      let v = [{
        name,
        source: rec.source,
        file: rec.file
      }];
      duplimap.set(name, v);
    }
  });

  // and now generate the jumbo files:
  let dstNum = 1;
  let srcCount = 0;
  dstCode = '';

  r = Array.from(duplimap.keys());
  r.sort();
  r.forEach((name) => {
    let recs = duplimap.get(name);

    if (srcCount >= 20) {
      let dstname = `../../scripts/wxWidgets/jumbo-source${dstNum}.cpp`;
      fs.writeFileSync(dstname, dstCode, 'utf8');

      console.log("written jumbo file", dstname);

      srcCount = 0;
      dstNum++;
    }
    if (srcCount == 0) {
      dstCode = `

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#define WX_JUMBO_MONOLITHIC_BUILD

`;
    }
    srcCount++;

    //console.error({srcCount, dstNum, name})

    dstCode += `

//------------------------------------------------------------------------
// ${name}
`;

    recs.forEach(rec => {  
      let platform = rec.source.replace(/\/.*$/, '');
      switch (platform) {
      case 'msw':
        dstCode += `
#if defined(__WXMSW__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'motif':
        dstCode += `
#if defined(__WXMOTIF__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'gtk1':
        dstCode += `
#if defined(__WXGTK__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'gtk':
        dstCode += `
#if defined(__WXGTK20__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'dfb':
        dstCode += `
#if defined(__WXDFB__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'x11':
        dstCode += `
#if defined(__WXX11__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'osx':
        dstCode += `
#if defined(__WXMAC__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'qt':
        dstCode += `
#if defined(__WXQT__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'uwp':
        dstCode += `
#if defined(__UWP__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'univ':
        dstCode += `
#if !defined(__WXMSW__)
#  include "${rec.file}"
#endif
`;
        break;

      case 'unix':
        dstCode += `
#if !defined(__WINDOWS__)
#  include "${rec.file}"
#endif
`;
        break;

      default:
        dstCode += `
#include "${rec.file}"
`;
        break;
      }
    });
  });

  // also make we dump the tail:
  if (dstCode.trim().length > 0) {
    let dstname = `../../scripts/wxWidgets/jumbo-source${dstNum}.cpp`;

    console.log("written jumbo file", dstname);

    fs.writeFileSync(dstname, dstCode, 'utf8');
  }

  console.log("Done.");
});



/*

#! /bin/bash
#

# Sample output:
# 
# //------------------------------------------------------------------------
# // bmpbndl.cpp
# 
# #include "../../thirdparty/owemdjee/wxWidgets/src/common/bmpbndl.cpp"
# #include "../../thirdparty/owemdjee/wxWidgets/src/msw/bmpbndl.cpp"
# 
# #if defined(__WXMSW__)
#     #include "wx/msw/colour.h"
# #elif defined(__WXMOTIF__)
#     #include "wx/motif/colour.h"
# #elif defined(__WXGTK20__)
#     #include "wx/gtk/colour.h"
# #elif defined(__WXGTK__)
#     #include "wx/gtk1/colour.h"
# #elif defined(__WXDFB__)
#     #include "wx/generic/colour.h"
# #elif defined(__WXX11__)
#     #include "wx/x11/colour.h"
# #elif defined(__WXMAC__)
#     #include "wx/osx/colour.h"
# #elif defined(__WXQT__)
#     #include "wx/qt/colour.h"
# #endif
# 

*/



