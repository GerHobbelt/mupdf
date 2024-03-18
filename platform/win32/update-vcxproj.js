//
// update vcxproj for an existing project 
// (clean up the CONFIGURATION SETTINGS to ensure all your projects have the same settings)
// 
// usage:
//
//     node ./update-vcxproj.js project-A.vcxproj
//
// or (using [bash for windows] from your [git for windows] rig:
//
//     for f in *.vcxproj ; do node ./update-vcxproj.js $f ; done
//
// when you want to synchronize all your projects to have the same compiler/librarian/linker
// settings throughout the entire Visual Studio solution.
//
// Aside: use the `patch-vcxproj.js` script to clean up any NEW `vcxproj` project
// file that you cloned/copied from another project. Use `patch-vcxproj.js` to set up
// an empty, FRESH project.
//
// Use `update-vcxproj.js` to UPDATE/UPGRADE your existing project(s) WITHOUT altering
// the source files set listed in these projects.
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
let m = /<ProjectName>([^]*?)<\/ProjectName>/.exec(src);
if (m) {
	projectName = m[1].trim();
}
if (process.argv[3]) {
	projectName = process.argv[3];
}

console.error({projectName});

const removeLinkTimeCodeGeneration = true;

// collect the project's settings re Compiler Warnings being disabled: keep that set at least:
function collect_warning_codes(src) {
	let warnings = [];
	let re = /<DisableSpecificWarnings>([^]*?)<\/DisableSpecificWarnings>/g;
	let m = re.exec(src);
	while (m) {
		let wset = m[1].trim().split(';');
		m = re.exec(src);
		warnings.push(...wset);
	}
	return warnings;
}
let warnings = collect_warning_codes(src);


src = src
//    <ProjectName>libcurl</ProjectName>
//    <RootNamespace>libcurl</RootNamespace>
.replace(/<ProjectName>[^]*?<\/ProjectName>/g, (m) => `<ProjectName>${projectName}</ProjectName>`)
.replace(/<RootNamespace>[^]*?<\/RootNamespace>/g, (m) => `<RootNamespace>${projectName}</RootNamespace>`)
//      <TypeLibraryName>./Release/libcurl.tlb</TypeLibraryName>
.replace(/<TypeLibraryName>[^]*?<\/TypeLibraryName>/g, "<TypeLibraryName>$(OutDir)$(TargetName).tlb</TypeLibraryName>")
//       <PreprocessorDefinitions>BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;_CRTDBG_MAP_ALLOC;WIN32;_DEBUG;_WINDOWS;_USRDLL;BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;USE_SCHANNEL;USE_WINDOWS_SSPI;USE_SCHANNEL;USE_WINDOWS_SSPI;%(PreprocessorDefinitions)</PreprocessorDefinitions>
.replace(/<PreprocessorDefinitions>([^]*?)<\/PreprocessorDefinitions>/g, (m, p1) => {
	// sort the defines so the build-type variable ones end up at the end, 
	// so Visual Studio will show us the common ones when we look at the project properties for "All Environments"
	let a = p1.split(';').map((l) => l.trim());

	// deduplicate the preprocessor defines set:
	let pp_set_hash = {};

	// also clean up some mistakes that we know we've made in some projects:
	a = a.map((l) => {
		if (/[A_Z_-]+_CRT_SECURE_NO_WARNINGS/.test(l))
			return "_CRT_SECURE_NO_WARNINGS";
		if ("MAIN_IS_MONOLITHIC" === l)
			return "BUILD_MONOLITHIC";
		
		// also remove defines which now reside in the project's *.props files already:
		if ("NDEBUG" === l)
			return "";
		if ("_DEBUG" === l)
			return "";
		if ("WIN32" === l)
			return "";
		if ("WIN64" === l)
			return "";
		if ("_CRT_SECURE_NO_WARNINGS" === l)
			return "";
		if ("_CRT_NONSTDC_NO_WARNINGS" === l)
			return "";
		if ("_CRTDBG_MAP_ALLOC" === l)
			return "";
		if (l[0] === '%')
			return "";
		
		return l;
	})
	.map(function (el) {
		if (pp_set_hash[el])
			return "";
		
		pp_set_hash[el] = true;
		return el;
	})
	.filter((l) => l.trim().length > 0);
	
	let pnu = projectName.toUpperCase();

	let defs = `${a.join(';')};%(PreprocessorDefinitions)`
	.replace(/;;/g, ';')
	.replace(/^;/g, '')

	return `<PreprocessorDefinitions>${defs}</PreprocessorDefinitions>`;
})
.replace(/<BrowseInformation>[^]*?<\/BrowseInformation>/g, `<BrowseInformation>false</BrowseInformation>`)
//     <OutDir>$(SolutionDir)bin\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\</OutDir>
.replace(/<OutDir>[^]*?<\/OutDir>/g, `<OutDir>$(SolutionDir)bin\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\</OutDir>`)
//    <IntDir>$(SolutionDir)obj\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\</IntDir>
.replace(/<IntDir>[^]*?<\/IntDir>/g, `<IntDir>$(SolutionDir)obj\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\\</IntDir>`)
//       <OmitFramePointers>true</OmitFramePointers>
.replace(/<OmitFramePointers>[^]*?<\/OmitFramePointers>/g, '')
.replace(/<CopyLocalDeploymentContent>[^]*?<\/CopyLocalDeploymentContent>/g, '')
// consolidate the CopyLocalDeploymentContent setting to the main property group:
.replace(/<\/IntDir>/g, `</IntDir>
    <CopyLocalDeploymentContent>true</CopyLocalDeploymentContent>
	`)
/*
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>StaticLibrary</ConfigurationType>
    <PlatformToolset>v143</PlatformToolset>
    <UseOfMfc>false</UseOfMfc>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries>true</UseDebugLibraries>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>
 */
.replace(/<CharacterSet>[^]*?<\/CharacterSet>/g, '')
.replace(/<WholeProgramOptimization>[^]*?<\/WholeProgramOptimization>/g, '')
.replace(/<PlatformToolset>[^]*?<\/PlatformToolset>/g, `<PlatformToolset>v143</PlatformToolset>
	<CharacterSet>Unicode</CharacterSet>`)
/*
    <Lib>
      <AdditionalDependencies>%(AdditionalDependencies)</AdditionalDependencies>
      <LinkTimeCodeGeneration>true</LinkTimeCodeGeneration>
    </Lib>
 */
.replace(/<Lib>([^]*?)<\/Lib>/g, (m, p1) => {
	if (!/LinkTimeCodeGeneration/.test(p1))
		p1 += `
	  		<LinkTimeCodeGeneration>true</LinkTimeCodeGeneration>
	`;

	p1 = p1
	.replace(/<LinkTimeCodeGeneration>[^]*?<\/LinkTimeCodeGeneration>/g, '<LinkTimeCodeGeneration>true</LinkTimeCodeGeneration>')

	return `<Lib>${p1}</Lib>`;
})
/*
<Link>
      <GenerateDebugInformation>DebugFastLink</GenerateDebugInformation>
      <TargetMachine>MachineX86</TargetMachine>
      <AdditionalDependencies>%(AdditionalDependencies)</AdditionalDependencies>
      <OptimizeReferences>true</OptimizeReferences>
      <EnableCOMDATFolding>false</EnableCOMDATFolding>               // /OPT:NOICF : Because /OPT:ICF can cause the same address to be assigned to different functions or read-only data members (that is, const variables when compiled by using /Gy), it can break a program that depends on unique addresses for functions or read-only data members.
      <LinkTimeCodeGeneration>UseFastLinkTimeCodeGeneration</LinkTimeCodeGeneration>
      <ForceFileOutput>MultiplyDefinedSymbolOnly</ForceFileOutput>
</Link>
 */
.replace(/<Link>([^]*?)<\/Link>/g, (m, p1) => {
	if (!/LinkTimeCodeGeneration/.test(p1))
		p1 += `
	  		<LinkTimeCodeGeneration>XXX</LinkTimeCodeGeneration>
	`;

	if (!/EnableCOMDATFolding/.test(p1))
		p1 += `
	  		<EnableCOMDATFolding>XXX</EnableCOMDATFolding>
	`;

	if (!/OptimizeReferences/.test(p1))
		p1 += `
	  		<OptimizeReferences>XXX</OptimizeReferences>
	`;

	if (!/GenerateDebugInformation/.test(p1))
		p1 += `
	  		<GenerateDebugInformation>XXX</GenerateDebugInformation>
	`;

	p1 = p1
	.replace(/<LinkTimeCodeGeneration>[^]*?<\/LinkTimeCodeGeneration>/g, '<LinkTimeCodeGeneration>UseFastLinkTimeCodeGeneration</LinkTimeCodeGeneration>')
	.replace(/<EnableCOMDATFolding>[^]*?<\/EnableCOMDATFolding>/g, '<EnableCOMDATFolding>false</EnableCOMDATFolding>')
	.replace(/<OptimizeReferences>[^]*?<\/OptimizeReferences>/g, '<OptimizeReferences>true</OptimizeReferences>')
	.replace(/<GenerateDebugInformation>[^]*?<\/GenerateDebugInformation>/g, '<GenerateDebugInformation>DebugFastLink</GenerateDebugInformation>')

	return `<Link>${p1}</Link>`;
})
.replace(/<ItemDefinitionGroup\s*\/>/g, '')
.replace(/<Import[^>]+common-project\.props" \/>/g, '')
.replace(/<ImportGroup\s+Label="PropertySheets"\s*\/>/g, '')
.replace(/<ImportGroup\s+Label="PropertySheets"[^]*?<\/ImportGroup>/g, '')
.replace(/<EnableUnitySupport>[^<]*<\/EnableUnitySupport>/g, '')

/*
    <ResourceCompile>
      <AdditionalIncludeDirectories>.;../../thirdparty/owemdjee/wxWidgets/include</AdditionalIncludeDirectories>
    </ResourceCompile>
 */
if (!src.includes("</ResourceCompile>")) {
	src = src.replace(/<\/Link>/g, `</Link>
    <ResourceCompile>
      <AdditionalIncludeDirectories>.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
    </ResourceCompile>
		`);
}
src = src.replace(/<ResourceCompile>([^]*?)<\/ResourceCompile>/g, (m, p1) => {
	if (!/AdditionalIncludeDirectories/.test(p1))
		p1 += `
	  		<AdditionalIncludeDirectories>.;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
	`;

	return `<ResourceCompile>${p1}</ResourceCompile>`;
});

/*
    <ClCompile>
      <Optimization>Disabled</Optimization>
      <AdditionalIncludeDirectories>.;../../thirdparty/owemdjee/clipp/include;%(AdditionalIncludeDirectories)</AdditionalIncludeDirectories>
      <PreprocessorDefinitions>BUILD_MONOLITHIC;BUILDING_LIBCLIPP;CLIPP_STATICLIB;_CRT_SECURE_NO_WARNINGS;_CRTDBG_MAP_ALLOC;WIN32;_DEBUG;_WINDOWS;%(PreprocessorDefinitions)</PreprocessorDefinitions>
      <MinimalRebuild>false</MinimalRebuild>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <PrecompiledHeaderOutputFile>$(IntDir)$(TargetName).pch</PrecompiledHeaderOutputFile>
      <AssemblerListingLocation>$(IntDir)</AssemblerListingLocation>
      <ObjectFileName>$(IntDir)</ObjectFileName>
      <ProgramDataBaseFileName>$(IntDir)$(ProjectName).pdb</ProgramDataBaseFileName>
      <BrowseInformation>false</BrowseInformation>
      <WarningLevel>Level3</WarningLevel>
      <SuppressStartupBanner>true</SuppressStartupBanner>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <DisableSpecificWarnings>4244;4018;4267;5105;4100;4127;4206;%(DisableSpecificWarnings)</DisableSpecificWarnings>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <LanguageStandard_C>stdc17</LanguageStandard_C>
      <SupportJustMyCode>false</SupportJustMyCode>
      <SDLCheck>false</SDLCheck>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <StringPooling>true</StringPooling>
      <ExceptionHandling>Async</ExceptionHandling>

<EnableEnhancedInstructionSet>NotSet</EnableEnhancedInstructionSet>
<EnableEnhancedInstructionSet>AdvancedVectorExtensions2</EnableEnhancedInstructionSet>

      <FloatingPointModel>Fast</FloatingPointModel>
      <ConformanceMode>true</ConformanceMode>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <RuntimeTypeInfo>true</RuntimeTypeInfo>
    </ClCompile>
*/    

let compiler_settings = `
<ItemDefinitionGroup>
    <ClCompile>
      <Optimization>Custom</Optimization>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <PrecompiledHeaderOutputFile>$(IntDir)$(TargetName).pch</PrecompiledHeaderOutputFile>
      <AssemblerListingLocation>$(IntDir)</AssemblerListingLocation>
      <ObjectFileName>$(IntDir)</ObjectFileName>
      <ProgramDataBaseFileName>$(IntDir)$(ProjectName).pdb</ProgramDataBaseFileName>
      <BrowseInformation>false</BrowseInformation>
      <WarningLevel>Level4</WarningLevel>
      <SuppressStartupBanner>true</SuppressStartupBanner>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <DisableSpecificWarnings>4180;4244;4018;4267;5105;4100;4127;4206;%(DisableSpecificWarnings)</DisableSpecificWarnings>
      <LanguageStandard>stdcpp20</LanguageStandard>
      <LanguageStandard_C>stdc17</LanguageStandard_C>
      <SupportJustMyCode>false</SupportJustMyCode>
      <SDLCheck>false</SDLCheck>
      <MultiProcessorCompilation>true</MultiProcessorCompilation>
      <IntrinsicFunctions>true</IntrinsicFunctions>
      <StringPooling>true</StringPooling>
      <ExceptionHandling>Async</ExceptionHandling>
      <EnableEnhancedInstructionSet>AdvancedVectorExtensions2</EnableEnhancedInstructionSet>
      <FloatingPointModel>Fast</FloatingPointModel>
      <EnableFiberSafeOptimizations>true</EnableFiberSafeOptimizations>
      <FloatingPointExceptions>false</FloatingPointExceptions>
      <ConformanceMode>true</ConformanceMode>
      <OmitFramePointers>true</OmitFramePointers>
      <EnableUnitySupport>true</EnableUnitySupport>
      <FavorSizeOrSpeed>Speed</FavorSizeOrSpeed>
      <RuntimeTypeInfo>true</RuntimeTypeInfo>
      <AdditionalOptions>/bigobj /utf-8 /Zc:__cplusplus %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
</ItemDefinitionGroup>
`;

src = src
.replace(/<ItemDefinitionGroup([^>]*)>([^]*?)<\/ItemDefinitionGroup>/g, (m, p1, p2) => {
	let isDebug = p1.length == 0 || /Debug/.test(p1);

	// make sure these entries are always present:
	if (!/<Optimization>/.test(p2)) {
		p2 = p2.replace(/<ClCompile>/, `<ClCompile>
			<Optimization>XXX</Optimization>`);
	}

	if (!/<BasicRuntimeChecks>/.test(p2)) {
		p2 = p2.replace(/<ClCompile>/, `<ClCompile>
			<BasicRuntimeChecks>XXX</BasicRuntimeChecks>`);
	}

	if (!/<RuntimeLibrary>/.test(p2)) {
		p2 = p2.replace(/<ClCompile>/, `<ClCompile>
			<RuntimeLibrary>XXX</RuntimeLibrary>`);
	}

	// now patch the entries and remove those that all build modes have in common:
	p2 = p2
	.replace(/<Optimization>[^]*?<\/Optimization>/g, (m) => `<Optimization>${ isDebug ? "Custom" : "MaxSpeed" }</Optimization>`)
	.replace(/<MinimalRebuild>[^]*?<\/MinimalRebuild>/g, (m) => ``)
	.replace(/<BasicRuntimeChecks>[^]*?<\/BasicRuntimeChecks>/g, (m) => `<BasicRuntimeChecks>${ isDebug ? "EnableFastChecks" : "Default" }</BasicRuntimeChecks>`)
	.replace(/<RuntimeLibrary>[^]*?<\/RuntimeLibrary>/g, (m) => `<RuntimeLibrary>${ isDebug ? "MultiThreadedDebugDLL" : "MultiThreadedDLL" }</RuntimeLibrary>`)
	.replace(/<AssemblerListingLocation>[^]*?<\/AssemblerListingLocation>/g, (m) => ``)
	.replace(/<ObjectFileName>[^]*?<\/ObjectFileName>/g, (m) => ``)
	.replace(/<ProgramDataBaseFileName>[^]*?<\/ProgramDataBaseFileName>/g, (m) => ``)
	.replace(/<BrowseInformation>[^]*?<\/BrowseInformation>/g, (m) => ``)
	.replace(/<WarningLevel>[^]*?<\/WarningLevel>/g, (m) => ``)
	.replace(/<SuppressStartupBanner>[^]*?<\/SuppressStartupBanner>/g, (m) => ``)
	.replace(/<DebugInformationFormat>[^]*?<\/DebugInformationFormat>/g, (m) => ``)
	.replace(/<FunctionLevelLinking>[^]*?<\/FunctionLevelLinking>/g, (m) => ``)
	.replace(/<LanguageStandard>[^]*?<\/LanguageStandard>/g, (m) => ``)
	.replace(/<LanguageStandard_C>[^]*?<\/LanguageStandard_C>/g, (m) => ``)
	.replace(/<SupportJustMyCode>[^]*?<\/SupportJustMyCode>/g, (m) => ``)
	.replace(/<SDLCheck>[^]*?<\/SDLCheck>/g, (m) => ``)
	.replace(/<DisableSpecificWarnings>[^]*?<\/DisableSpecificWarnings>/g, (m) => ``)
	.replace(/<MultiProcessorCompilation>[^]*?<\/MultiProcessorCompilation>/g, (m) => ``)
	.replace(/<IntrinsicFunctions>[^]*?<\/IntrinsicFunctions>/g, (m) => ``)
	.replace(/<StringPooling>[^]*?<\/StringPooling>/g, (m) => ``)
	.replace(/<ExceptionHandling>[^]*?<\/ExceptionHandling>/g, (m) => ``)
	.replace(/<EnableEnhancedInstructionSet>[^]*?<\/EnableEnhancedInstructionSet>/g, (m) => ``)
	.replace(/<FloatingPointModel>[^]*?<\/FloatingPointModel>/g, (m) => ``)
	.replace(/<EnableFiberSafeOptimizations>[^]*?<\/EnableFiberSafeOptimizations>/g, (m) => ``)
	.replace(/<FloatingPointExceptions>[^]*?<\/FloatingPointExceptions>/g, (m) => ``)
	.replace(/<ConformanceMode>[^]*?<\/ConformanceMode>/g, (m) => ``)
	.replace(/<OmitFramePointers>[^]*?<\/OmitFramePointers>/g, '')
	.replace(/<PrecompiledHeaderOutputFile>[^]*?<\/PrecompiledHeaderOutputFile>/g, '')
	.replace(/<FavorSizeOrSpeed>[^]*?<\/FavorSizeOrSpeed>/g, '')
	.replace(/<RuntimeTypeInfo>[^]*?<\/RuntimeTypeInfo>/g, '')
	.replace(/<AdditionalOptions>\/bigobj \/utf-8 \/Zc:__cplusplus %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
	.replace(/<AdditionalOptions>\/bigobj \/utf-8 %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
	.replace(/<AdditionalOptions>\/bigobj %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
	.replace(/<AdditionalOptions>\/utf-8 %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
	.replace(/<AdditionalOptions>\/Zc:__cplusplus \/utf-8 %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')

	return `<ItemDefinitionGroup${p1}>${p2}</ItemDefinitionGroup>`;
});

if (removeLinkTimeCodeGeneration) {
	src = src
	.replace(/<LinkTimeCodeGeneration>[^]*?<\/LinkTimeCodeGeneration>/g, '')
}

// and remove any dandruff from a previous run:
src = src
.replace(/<ItemDefinitionGroup>[\s\r\n]*<ClCompile>[\s\r\n]*<Optimization>[^<]*<\/Optimization>[\s\r\n]*<BasicRuntimeChecks>[^<]*<\/BasicRuntimeChecks>[\s\r\n]*<RuntimeLibrary>[^<]*<\/RuntimeLibrary>[\s\r\n]*<PrecompiledHeaderOutputFile>[^<]*<\/PrecompiledHeaderOutputFile>[\s\r\n]*<\/ClCompile>[\s\r\n]*<\/ItemDefinitionGroup>/g, '')
.replace(/<ItemDefinitionGroup>[\s\r\n]*<ClCompile>[\s\r\n]*<Optimization>[^<]*<\/Optimization>[\s\r\n]*<BasicRuntimeChecks>[^<]*<\/BasicRuntimeChecks>[\s\r\n]*<RuntimeLibrary>[^<]*<\/RuntimeLibrary>[\s\r\n]*<\/ClCompile>[\s\r\n]*<\/ItemDefinitionGroup>/g, '')

warnings.push(...collect_warning_codes(compiler_settings));
// and make sure the warnings are sorted and unique:
warnings.sort();
warnings = warnings.filter(function flt(el, i, arr) {
	if (!el.length)
		return false;
	if (i > 0 && arr[i-1] === el)
		return false;
	if (el[0] === '%')
		return false;
	return true;
});
compiler_settings = compiler_settings.replace(/<DisableSpecificWarnings>.*?<\/DisableSpecificWarnings>/, `<DisableSpecificWarnings>${ warnings.join(';') };%(DisableSpecificWarnings)</DisableSpecificWarnings>`);

// prepend this common blob before all other blobs:
src = src
.replace(/<ItemDefinitionGroup/, (m) => {
	return `${compiler_settings}${m}`;
})
// append this next bit near the start of the project file:
.replace(/<Import Project="[^>]+Microsoft\.Cpp\.props"\s+\/>/, (m) => {
  return `${m}

  <ImportGroup Label="PropertySheets" >
    <Import Project="$(SolutionDir)\\common-project.props" Label="SolutionWideSettings" />
  </ImportGroup>

  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
    <Import Project="$(SolutionDir)\\common-project-Debug-Win32.props"  Condition="exists('$(SolutionDir)\\common-project-Debug-Win32.props')" Label="SolutionWideDebugWin32Settings" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|Win32'">
    <Import Project="$(SolutionDir)\\common-project-Release-Win32.props"  Condition="exists('$(SolutionDir)\\common-project-Release-Win32.props')" Label="SolutionWideReleaseWin32Settings" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|x64'">
    <Import Project="$(SolutionDir)\\common-project-Debug-Win64.props"  Condition="exists('$(SolutionDir)\\common-project-Debug-Win64.props')" Label="SolutionWideDebugWin64Settings" />
  </ImportGroup>
  <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <Import Project="$(SolutionDir)\\common-project-Release-Win64.props"  Condition="exists('$(SolutionDir)\\common-project-Release-Win64.props')" Label="SolutionWideReleaseWin64Settings" />
  </ImportGroup>

  `;
})
// and append this bit near the very end to ensure that 
// the 'last seen is the winner' rule of Visual Studio property parsing 
// makes this bunch the ultimate winners, just in case we need it:
.replace(/[\s\r\n]<\/Project>[\s\r\n]*$/, (m) => {
  return `
  <ImportGroup Label="PropertySheets" >
    <Import Project="$(SolutionDir)\\common-project-ultimate-override.props" Label="SolutionWideSettingsOverride" />
  </ImportGroup>
${m}
  `;
});

// make sure all include path sets are the same: pick up the set from the largest entry and copy it around:
//
// Note: ResourceCmpiler SHOULD follow, NOT lead. This means we SHOULD NOT look at the include path listed for the 
// (sometimes hidden) ResourceCompiler as this will thwart our editing efforts (done in the ClCompile section, etc.)
// when we actually REDUCE the paths' set! (We ran into this issue while editing the libleptonica paths for example)
let include_paths = '.;%(AdditionalIncludeDirectories)';
const inc_re = /<AdditionalIncludeDirectories>([^]*?)<\/AdditionalIncludeDirectories>/g;
let search_src = src.replace(/<ResourceCompile>[^]*?<\/ResourceCompile>/g, '');
for (;;) {
	let m = inc_re.exec(search_src);
	if (!m)
		break;
	let p1 = m[1].trim();
	if (p1.length > include_paths.length) {
		include_paths = p1;
	}
}

// make sure the searchdir list has these entries and at the priority position where we want them (front = first):
// - clean up
// - append/prepend the required paths
include_paths = `;${include_paths.trim()};`
	.replace(/\\/g, '/')
	.replace(/;..\/..\/include\/system-override;/g, ';')
	.replace(/;%\(AdditionalIncludeDirectories\);/g, ';')
	.replace(/;.;/g, ';')

// patch: when there's a single /boost/ reference in there, get the whole damn bunch:
if (/\/owemdjee\/boost\//.test(include_paths) || /\/scripts\/boost\//.test(include_paths)) {
	include_paths = `${include_paths}
	../../scripts/boost/include
	../../thirdparty/owemdjee/boost
	../../thirdparty/owemdjee/boost/libs/algorithm/include
	../../thirdparty/owemdjee/boost/libs/any/include
	../../thirdparty/owemdjee/boost/libs/array/include
	../../thirdparty/owemdjee/boost/libs/asio/include
	../../thirdparty/owemdjee/boost/libs/assert/include
	../../thirdparty/owemdjee/boost/libs/atomic/include
	../../thirdparty/owemdjee/boost/libs/bind/include
	../../thirdparty/owemdjee/boost/libs/chrono/include
	../../thirdparty/owemdjee/boost/libs/concept_check/include
	../../thirdparty/owemdjee/boost/libs/config/include
	../../thirdparty/owemdjee/boost/libs/container/include
	../../thirdparty/owemdjee/boost/libs/container_hash/include
	../../thirdparty/owemdjee/boost/libs/core/include
	../../thirdparty/owemdjee/boost/libs/crc/include
	../../thirdparty/owemdjee/boost/libs/date_time/include
	../../thirdparty/owemdjee/boost/libs/detail/include
	../../thirdparty/owemdjee/boost/libs/dll/include
	../../thirdparty/owemdjee/boost/libs/fiber/include
	../../thirdparty/owemdjee/boost/libs/filesystem/include
	../../thirdparty/owemdjee/boost/libs/function/include
	../../thirdparty/owemdjee/boost/libs/fusion/include
	../../thirdparty/owemdjee/boost/libs/integer/include
	../../thirdparty/owemdjee/boost/libs/io/include
	../../thirdparty/owemdjee/boost/libs/iterator/include
	../../thirdparty/owemdjee/boost/libs/lexical_cast/include
	../../thirdparty/owemdjee/boost/libs/locale/include
	../../thirdparty/owemdjee/boost/libs/lockfree/include
	../../thirdparty/owemdjee/boost/libs/log/include
	../../thirdparty/owemdjee/boost/libs/logic/include
	../../thirdparty/owemdjee/boost/libs/move/include
	../../thirdparty/owemdjee/boost/libs/mp11/include
	../../thirdparty/owemdjee/boost/libs/mp11/include/
	../../thirdparty/owemdjee/boost/libs/mpl/include
	../../thirdparty/owemdjee/boost/libs/multiprecision/include
	../../thirdparty/owemdjee/boost/libs/numeric/conversion/include
	../../thirdparty/owemdjee/boost/libs/optional/include
	../../thirdparty/owemdjee/boost/libs/parameter/include
	../../thirdparty/owemdjee/boost/libs/phoenix/include
	../../thirdparty/owemdjee/boost/libs/predef/include
	../../thirdparty/owemdjee/boost/libs/preprocessor/include
	../../thirdparty/owemdjee/boost/libs/program_options/include
	../../thirdparty/owemdjee/boost/libs/proto/include
	../../thirdparty/owemdjee/boost/libs/range/include
	../../thirdparty/owemdjee/boost/libs/ratio/include
	../../thirdparty/owemdjee/boost/libs/regex/include
	../../thirdparty/owemdjee/boost/libs/smart_ptr/include
	../../thirdparty/owemdjee/boost/libs/static_assert/include
	../../thirdparty/owemdjee/boost/libs/system/include
	../../thirdparty/owemdjee/boost/libs/test/include
	../../thirdparty/owemdjee/boost/libs/thread/include
	../../thirdparty/owemdjee/boost/libs/throw_exception/include
	../../thirdparty/owemdjee/boost/libs/type_index/include
	../../thirdparty/owemdjee/boost/libs/type_traits/include
	../../thirdparty/owemdjee/boost/libs/typeof/include
	../../thirdparty/owemdjee/boost/libs/utility/include
	../../thirdparty/owemdjee/boost/libs/uuid/include
	../../thirdparty/owemdjee/boost/libs/winapi/include
	`.replace(/\n/g, ';')
	.replace(/\s+/g, '');
}

// patch: when there's a single /openCV/ reference in there, get the whole damn bunch:
if (/\/owemdjee\/opencv\//.test(include_paths) || /\/scripts\/OpenCV\//.test(include_paths)) {
	include_paths = `${include_paths}
	../../scripts/OpenCV
	../../scripts/OpenCV/modules/core
	../../scripts/OpenCV/modules/stitching
	../../thirdparty/owemdjee/opencv/include
	../../thirdparty/owemdjee/opencv/modules/calib3d/include
	../../thirdparty/owemdjee/opencv/modules/core/include
	../../thirdparty/owemdjee/opencv/modules/dnn
	../../thirdparty/owemdjee/opencv/modules/dnn/include
	../../thirdparty/owemdjee/opencv/modules/features2d/include
	../../thirdparty/owemdjee/opencv/modules/flann/include
	../../thirdparty/owemdjee/opencv/modules/gapi/include
	../../thirdparty/owemdjee/opencv/modules/gapi/src
	../../thirdparty/owemdjee/opencv/modules/highgui/include
	../../thirdparty/owemdjee/opencv/modules/imgcodecs/include
	../../thirdparty/owemdjee/opencv/modules/imgproc/include
	../../thirdparty/owemdjee/opencv/modules/ml/include
	../../thirdparty/owemdjee/opencv/modules/objdetect/include
	../../thirdparty/owemdjee/opencv/modules/photo/include
	../../thirdparty/owemdjee/opencv/modules/stitching/include
	../../thirdparty/owemdjee/opencv/modules/ts/include
	../../thirdparty/owemdjee/opencv/modules/ts/include/
	../../thirdparty/owemdjee/opencv/modules/video/include
	../../thirdparty/owemdjee/opencv/modules/videoio/include
	../../thirdparty/owemdjee/opencv/modules/world/include
	../../thirdparty/owemdjee/opencv_contrib/modules/alphamat/include
	../../thirdparty/owemdjee/opencv_contrib/modules/aruco/include
	../../thirdparty/owemdjee/opencv_contrib/modules/bgsegm/include
	../../thirdparty/owemdjee/opencv_contrib/modules/bioinspired/include
	../../thirdparty/owemdjee/opencv_contrib/modules/ccalib/include
	../../thirdparty/owemdjee/opencv_contrib/modules/cnn_3dobj/include
	../../thirdparty/owemdjee/opencv_contrib/modules/cudaarithm/include
	../../thirdparty/owemdjee/opencv_contrib/modules/cudacodec/include
	../../thirdparty/owemdjee/opencv_contrib/modules/cudalegacy/include
	../../thirdparty/owemdjee/opencv_contrib/modules/cudaoptflow/include
	../../thirdparty/owemdjee/opencv_contrib/modules/cvv/include
	../../thirdparty/owemdjee/opencv_contrib/modules/sfm/src/libmv_light
	../../thirdparty/owemdjee/opencv_contrib/modules/viz/include
	../../thirdparty/owemdjee/opencv_contrib/modules/xphoto/include
	../../thirdparty/owemdjee/opencv_contrib/modules/xfeatures2d/include
	../../thirdparty/owemdjee/opencv_contrib/modules/ximgproc/include
	../../thirdparty/owemdjee/opencv_contrib/modules/intensity_transform/include
	../../scripts/OpenCV/modules/imgproc
	../../thirdparty/owemdjee/opencv_contrib/modules/optflow/include
	../../thirdparty/owemdjee/opencv_contrib/modules/freetype/include
	../../thirdparty/owemdjee/opencv_contrib/modules/hdf/include
	../../thirdparty/owemdjee/opencv_contrib/modules/img_hash/include
	../../thirdparty/owemdjee/opencv_contrib/modules/face/include
	../../thirdparty/owemdjee/opencv/3rdparty/openvx/include
	`.replace(/\n/g, ';')
	.replace(/\s+/g, '');
}


include_paths = `../../include/system-override;.;${include_paths};`;
	

// deduplicate the include paths set:
let inc_set_hash = {};
include_paths = include_paths.split(';')
.filter(function flt(el) {
	if (el.trim().length === 0)
		return false;
	if (el === `%(AdditionalIncludeDirectories)`)
		return false;
	return true;
})
.map(function (el) {
	if (inc_set_hash[el])
		return "";
	
	inc_set_hash[el] = true;
	return el;
})
.filter(function flt(el) {
	return (el.trim().length > 0);
})
.join(';');

include_paths = `${include_paths};%(AdditionalIncludeDirectories)`
	.replace(/;;/g, ';')
	.replace(/^;/g, '')
	
src = src
.replace(/<AdditionalIncludeDirectories>[^]*?<\/AdditionalIncludeDirectories>/g, `<AdditionalIncludeDirectories>${include_paths}</AdditionalIncludeDirectories>`);

const dupliBlockbase = `
  <ItemDefinitionGroup>
    <ClCompile>
      <Optimization>Custom</Optimization>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <AdditionalOptions>/bigobj /utf-8 /Zc:__cplusplus %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
  </ItemDefinitionGroup>
`
const dupliBlockbaseReParticle = dupliBlockbase
.trim()
.replace(/([\/()])/g, '\\$1')
.replace(/\\\/bigobj /g, '(?:\\/bigobj )?')
.replace(/\\\/Zc:__cplusplus /g, '(?:\\/Zc:__cplusplus )?')
.replace(/[\s\r\n]+/g, '[\\s\\r\\n]*')

const dupliBlockre = new RegExp(`(?:${ dupliBlockbaseReParticle }[\\s\\r\\n]*){2,}`, 'g');

src = src
// and remove any lingering duplicated compiler settings blocks:
.replace(dupliBlockre, dupliBlockbase)
 
src = src
// fix ProjectDependencies: MSVC2019 is quite critical about whitespace around the UUID:
.replace(/<Project>[\s\r\n]+[{]/g, '<Project>{')
.replace(/[}][\s\r\n]+<\/Project>/g, '}</Project>')
.replace(/\t/g, '    ')
// and trim out empty lines:
.replace(/[\s\r\n]+\n/g, '\n');


fs.writeFileSync(filepath, src, 'utf8');

