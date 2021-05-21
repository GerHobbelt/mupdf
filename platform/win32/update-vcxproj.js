//
// update/clean vcxproj for an existing project
// 

let fs = require('fs');
let path = require('path');


let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
	console.error("must specify valid vcxproj file");
	process.exit(1);
}
let src = fs.readFileSync(filepath, 'utf8');

let projectName = path.basename(filepath, '.vcxproj');
let m = /<ProjectName>([^<]+)<\/ProjectName>/.exec(src);
if (m) {
	projectName = m[1].trim();
}

console.error({projectName});

src = src
//    <ProjectName>libcurl</ProjectName>
//    <RootNamespace>libcurl</RootNamespace>
.replace(/<ProjectName>[^<]+<\/ProjectName>/g, (m) => `<ProjectName>${projectName}</ProjectName>`)
.replace(/<RootNamespace>[^<]+<\/RootNamespace>/g, (m) => `<RootNamespace>mupdf</RootNamespace>`)
//      <TypeLibraryName>.\Release/libcurl.tlb</TypeLibraryName>
.replace(/<TypeLibraryName>[^<]+<\/TypeLibraryName>/g, "<TypeLibraryName>$(OutDir)$(TargetName).tlb</TypeLibraryName>")
//       <PreprocessorDefinitions>BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;_CRTDBG_MAP_ALLOC;WIN32;_DEBUG;_WINDOWS;_USRDLL;BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;USE_SCHANNEL;USE_WINDOWS_SSPI;USE_SCHANNEL;USE_WINDOWS_SSPI;%(PreprocessorDefinitions)</PreprocessorDefinitions>
.replace(/<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>/g, (m, p1) => {
	// sort the defines so the build-type variable ones end up at the end, 
	// so Visual Studio will show us the common ones when we look at the project properties for "All Environments"
	let a = p1.split(';').map((l) => l.trim());

	// also clean up some mistakes that we know we've made in some projects:
	a = a.map((l) => {
		if (/[A_Z_-]+_CRT_SECURE_NO_WARNINGS/.test(l))
			return "_CRT_SECURE_NO_WARNINGS";
		if ("MAIN_IS_MONOLITHIC" === l)
			return "BUILD_MONOLITHIC";
		return l;
	})

	let pnu = projectName.toUpperCase();

	return `<PreprocessorDefinitions>${a.join(';')}</PreprocessorDefinitions>`;
})
.replace(/<BrowseInformation>[^<]+<\/BrowseInformation>/g, (m) => `<BrowseInformation>false</BrowseInformation>`)
//     <OutDir>$(SolutionDir)bin\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\</OutDir>
.replace(/<OutDir>[^<]+<\/OutDir>/g, (m) => `<OutDir>$(SolutionDir)bin\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\</OutDir>`)
//    <IntDir>$(SolutionDir)obj\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\</IntDir>
.replace(/<IntDir>[^<]+<\/IntDir>/g, (m) => `<IntDir>$(SolutionDir)obj\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\\</IntDir>`)


fs.writeFileSync(filepath, src, 'utf8');


