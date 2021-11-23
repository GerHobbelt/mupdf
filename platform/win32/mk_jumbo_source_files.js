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


function unixify(path) {
  return path.replace(/\\/g, '/');
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

let scanspec = '*.cpp';
console.error({scanspec});
glob(scanspec, globConfig, function processGlobResults(err, files) {
  if (err) {
    throw new Error(`glob scan error: ${err}`);
  }

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
  
  let dstCode = `

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

`;
  a.filter((rec) => rec.source.includes('richtext/'))
  .forEach((rec) => {
    let name = path.basename(rec.source);

    dstCode += `

//------------------------------------------------------------------------
// ${name}
 
#include "${rec.file}"
`;
  });

  fs.writeFileSync('../../scripts/wxWidgets/jumbo-richtext-source.cpp', dstCode, 'utf8');


  // next phase: find out which files match in filename (and thus cause trouble in MSVXC project files
  // as they'll generate the same OBJ file); collect these sets for subsequent use.
  
  let duplimap = new Map();

  let r = a.filter((rec) => {
    let x1 = /^(?:common|generic|msw|motif|gtk|gtk1|x11|qt|dfb|osx|univ|unix)\//.test(rec.source);
    let x2 = /dummy\.cpp|strconv\.cpp/.test(rec.source);
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

    if (srcCount >= 50) {
      let dstname = `../../scripts/wxWidgets/jumbo-source${dstNum}.cpp`;
      fs.writeFileSync(dstname, dstCode, 'utf8');
      srcCount = 0;
      dstNum++;
    }
    if (srcCount == 0) {
      dstCode = `

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

`;
    }
    srcCount++;

    console.error({srcCount, dstNum, name})

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
    console.error('dump tail');
    let dstname = `../../scripts/wxWidgets/jumbo-source${dstNum}.cpp`;
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


rm jumbo-source*.cpp




CNT=0
FILENUM=1

cat > jumbo-richtext-source.cpp <<EOF

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

EOF

for f in $(  find ../../thirdparty/owemdjee/wxWidgets/src/richtext/ -name '*.cpp' ) ; do

	g=$f
	f=$( echo $f | sed -e 's/^.*\///' )

	echo "Adding $f..."

	((CNT++))
	echo "Source #${CNT}"


	cat >> jumbo-richtext-source.cpp <<EOF  

//------------------------------------------------------------------------
// $f
 
EOF

	cat >> jumbo-richtext-source.cpp <<EOF
#include "$g"
EOF

done





CNT=0
FILENUM=1



for f in $(  find ../../thirdparty/owemdjee/wxWidgets/src/ -name '*.cpp' | grep -e '/src/\(common\|generic\|msw\|motif\|gtk\|gtk1\|x11\|qt\|dfb\)/' | grep -v 'dummy\.cpp\|strconv\.cpp' | sed -e 's/^.*\///' | sort | uniq ) ; do

	echo "Adding $f..."

	if [ $CNT -ge 10 ] ; then
		((FILENUM++))
		CNT=0
	fi
	if [ $CNT -eq 0 ] ; then
		cat > jumbo-source${FILENUM}.cpp <<EOF

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

EOF
	fi
	((CNT++))
	echo "Source #${CNT}"


	cat >> jumbo-source${FILENUM}.cpp <<EOF  

//------------------------------------------------------------------------
// $f
 
EOF

	for g in $(  find ../../thirdparty/owemdjee/wxWidgets/src/ -name "$f" ) ; do

		PLATFORM=$( echo $g | sed -e 's/^.*\/src\/\([^\/]\+\)\/.*$/\1/' )
		case "$PLATFORM" in 
		msw)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXMSW__)
#  include "$g"
#endif
EOF
			;;

		motif)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXMOTIF__)
#  include "$g"
#endif
EOF
			;;

		gtk1)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXGTK__)
#  include "$g"
#endif
EOF
			;;

		gtk)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXGTK20__)
#  include "$g"
#endif
EOF
			;;

		dfb)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXDFB__)
#  include "$g"
#endif
EOF
			;;

		x11)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXX11__)
#  include "$g"
#endif
EOF
			;;

		osx)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXMAC__)
#  include "$g"
#endif
EOF
			;;

		qt)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if defined(__WXQT__)
#  include "$g"
#endif
EOF
			;;

		univ)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if !defined(__WXMSW__)
#  include "$g"
#endif
EOF
			;;

		unix)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#if !defined(__WINDOWS__)
#  include "$g"
#endif
EOF
			;;

		*)
			cat >> jumbo-source${FILENUM}.cpp <<EOF
#include "$g"
EOF
			;;
		esac
	done
done







*/



