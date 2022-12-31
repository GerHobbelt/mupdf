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
// Use `update-vcxprroj.js` to UPDATE/UPGRADE your existing project(s) WITHOUT altering
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
//      <TypeLibraryName>.\Release/libcurl.tlb</TypeLibraryName>
.replace(/<TypeLibraryName>[^]*?<\/TypeLibraryName>/g, "<TypeLibraryName>$(OutDir)$(TargetName).tlb</TypeLibraryName>")
//       <PreprocessorDefinitions>BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;_CRTDBG_MAP_ALLOC;WIN32;_DEBUG;_WINDOWS;_USRDLL;BUILDING_LIBCURL;CURL_STATICLIB;CURL_DISABLE_LDAP;USE_SCHANNEL;USE_WINDOWS_SSPI;USE_SCHANNEL;USE_WINDOWS_SSPI;%(PreprocessorDefinitions)</PreprocessorDefinitions>
.replace(/<PreprocessorDefinitions>([^]*?)<\/PreprocessorDefinitions>/g, (m, p1) => {
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
.replace(/<BrowseInformation>[^]*?<\/BrowseInformation>/g, (m) => `<BrowseInformation>false</BrowseInformation>`)
//     <OutDir>$(SolutionDir)bin\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\</OutDir>
.replace(/<OutDir>[^]*?<\/OutDir>/g, (m) => `<OutDir>$(SolutionDir)bin\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\</OutDir>`)
//    <IntDir>$(SolutionDir)obj\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\</IntDir>
.replace(/<IntDir>[^]*?<\/IntDir>/g, (m) => `<IntDir>$(SolutionDir)obj\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\\</IntDir>`)
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
      <EnableCOMDATFolding>true</EnableCOMDATFolding>
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
	.replace(/<EnableCOMDATFolding>[^]*?<\/EnableCOMDATFolding>/g, '<EnableCOMDATFolding>true</EnableCOMDATFolding>')
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
      <EnableEnhancedInstructionSet>AdvancedVectorExtensions2</EnableEnhancedInstructionSet>
      <FloatingPointModel>Fast</FloatingPointModel>
      <ConformanceMode>true</ConformanceMode>
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
let include_paths = '.;%(AdditionalIncludeDirectories)';
const inc_re = /<AdditionalIncludeDirectories>([^]*?)<\/AdditionalIncludeDirectories>/g;
for (;;) {
	let m = inc_re.exec(src);
	if (!m)
		break;
	let p1 = m[1].trim();
	if (p1.length > include_paths.length) {
		include_paths = p1;
	}
}

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

