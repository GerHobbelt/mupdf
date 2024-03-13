//
// prep vcxproj for a new project
// 
// usage: run as
//
//     node ./patch-vcxproj.js your_project.vcxproj [create|tweak]
//
// WARNING: this script REMOVES all source files listed in the project, 
// giving you a clean, EMPTY project to start with.
//
// This allows workflows like these to create a new additional project in a large
// devtree:
//
//     cp project-A.vcxproj project-NEW.vcxproj
//     node ./patch-vcxproj.js project-NEW.vcxproj
// 
// use the update-vcxproj.js script instead when you want to synchronize 
// the settings in many projects WITHOUT ERASING any source files in there:
// update-vcxproj.js is the UPDATE script of this family.
//

let fs = require('fs');
let path = require('path');


let filepath = process.argv[2];
if (!fs.existsSync(filepath)) {
	console.error("must specify valid vcxproj file");
	process.exit(1);
}
let src = fs.readFileSync(filepath, 'utf8');

let mode = process.argv[3] || "create";

let projectName = path.basename(filepath, '.vcxproj');

console.error({projectName, mode});

//     <ClCompile Include="../../thirdparty/curl/src/tool_operhlp.c" />
//     <ClInclude Include="../../thirdparty/curl/include/curl/easy.h" />
//     <None Include="../../thirdparty/curl/src/Makefile.am" />
//     <ResourceCompile Include="../../thirdparty/curl/lib/libcurl.rc" />
src = src
.replace(/<[A-Za-z]+ Include="[^"]*" \/>/g, '')

if (mode === "create") {
	src = src
	//    <ProjectName>libcurl</ProjectName>
	//    <RootNamespace>libcurl</RootNamespace>
	.replace(/<ProjectName>[^<]+<\/ProjectName>/g, (m) => `<ProjectName>${projectName}</ProjectName>`)
	.replace(/<RootNamespace>[^<]+<\/RootNamespace>/g, (m) => `<RootNamespace>${projectName}</RootNamespace>`)
	//      <AdditionalDependencies>crypt32.lib;%(AdditionalDependencies)</AdditionalDependencies>
	.replace(/<AdditionalDependencies>[^<]+<\/AdditionalDependencies>/g, "<AdditionalDependencies>%(AdditionalDependencies)</AdditionalDependencies>")
	//      <TypeLibraryName>./Release/libcurl.tlb</TypeLibraryName>
	.replace(/<TypeLibraryName>[^<]+<\/TypeLibraryName>/g, "<TypeLibraryName>$(OutDir)$(TargetName).tlb</TypeLibraryName>")
	//      <AdditionalIncludeDirectories>../../include/system-override;;.;../../thirdparty/curl/include;../../thirdparty/curl/lib;../../thirdparty/curl/src;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
	.replace(/<AdditionalIncludeDirectories>[^<]+<\/AdditionalIncludeDirectories>/g, "<AdditionalIncludeDirectories>../../include/system-override;.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>")
	//       <PreprocessorDefinitions>BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;_CRTDBG_MAP_ALLOC;WIN32;_DEBUG;_WINDOWS;_USRDLL;BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;USE_SCHANNEL;USE_WINDOWS_SSPI;USE_SCHANNEL;USE_WINDOWS_SSPI;%(PreprocessorDefinitions)</PreprocessorDefinitions>
	.replace(/<PreprocessorDefinitions>([^<]+)<\/PreprocessorDefinitions>/g, (m, p1) => {
		p1 = p1
		.replace(/[A-Z]+_DISABLE_LDAP;/g, '')	
		.replace(/_USRDLL;/g, '')	
		.replace(/BUILDING_[A-Z_-]+;/g, '')	
		.replace(/[A-Z_-]+_STATICLIB;/g, '')	
		.replace(/[A-Z_-]*MONOLITHIC;/g, '')	
		.replace(/[A-Z_-]+-NDEBUG;/g, 'NDEBUG;')	
		.replace(/[A-Z_-]+-_DEBUG;/g, '_DEBUG;')	
		.replace(/[A-Z_-]+-_CRT_/g, '_CRT_')	
		.replace(/USE_SCHANNEL;/g, '')	
		.replace(/USE_WINDOWS_SSPI;/g, '')	
		.replace(/\bNDEBUG;/g, '')	
		.replace(/\b_DEBUG;/g, '')	

		let pnu = projectName.toUpperCase().replace(/-/g, '_');
		p1 = `BUILD_MONOLITHIC;BUILDING_${pnu};${ pnu.replace(/LIB/, '')}_STATICLIB;${p1}`;
		return `<PreprocessorDefinitions>${p1}</PreprocessorDefinitions>`;
	})
	//    <ProjectGuid>{87EE9DA4-DE1E-4448-8324-183C98DCA588}</ProjectGuid>
	.replace(/<ProjectGuid>([^<]+)<\/ProjectGuid>/g, (m, p1) => {
		let r;
		// make sure we have a usable random hexcode to use as part of our new UUID:
		do {
		  r = Math.random().toString(16).toUpperCase().replace(/0\./, '');
		} while (r.length < 6);
		// paste it over the end of the existing one:
		p1 = p1.substr(0, p1.length - 7) + r.substr(0, 6) + '}';
		return `<ProjectGuid>${p1}</ProjectGuid>`;
	})
}

src = src
.replace(/<AdditionalIncludeDirectories>([^<]+)<\/AdditionalIncludeDirectories>/g, (m, p1) => {
	p1 = p1
	.trim()
	.replace(/\\/g, '/')
	.replace(/..\/..\/include\/system-override;/g, '')
	.replace(/;%\(AdditionalIncludeDirectories\)/g, '')
	
	if (p1.length == 0)
		p1 = ".";

	return `<AdditionalIncludeDirectories>../../include/system-override;${p1};%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>`;
})

const sections_to_remove = [
	"ClCompile", "Resourcecompile", "ClInclude", "MASM", "Text", "Image", "Font", "None"
];

for (let s of sections_to_remove) {
	src = src
	.replace(new RegExp(`<${s} Include=[^]+?<\\/${s}>`, 'g'), '');
}

src = src
.replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '');

fs.writeFileSync(filepath, src, 'utf8');


// also patch the FILTERS file:

let filterSrc = '';
let filterFilepath = filepath + '.filters';
if (fs.existsSync(filterFilepath)) {
	// NOTE: *ALWAYS* rebuild the filters file!
	// 
	//filterSrc = fs.readFileSync(filterFilepath, 'utf8');
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


for (let s of sections_to_remove) {
	filterSrc = filterSrc
	.replace(new RegExp(`<${s} Include=[^]+?<\\/${s}>`, 'g'), '');
}

filterSrc = filterSrc
.replace(/<ItemGroup>[\s\r\n]*<\/ItemGroup>/g, '');

fs.writeFileSync(filterFilepath, filterSrc, 'utf8');


