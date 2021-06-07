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
let m = /<ProjectName>([^]*?)<\/ProjectName>/.exec(src);
if (m) {
	projectName = m[1].trim();
}

console.error({projectName});

src = src
//    <ProjectName>libcurl</ProjectName>
//    <RootNamespace>libcurl</RootNamespace>
.replace(/<ProjectName>[^]*?<\/ProjectName>/g, (m) => `<ProjectName>${projectName}</ProjectName>`)
.replace(/<RootNamespace>[^]*?<\/RootNamespace>/g, (m) => `<RootNamespace>mupdf</RootNamespace>`)
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
/*
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>StaticLibrary</ConfigurationType>
    <PlatformToolset>v142</PlatformToolset>
    <UseOfMfc>false</UseOfMfc>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries>true</UseDebugLibraries>
    <WholeProgramOptimization>true</WholeProgramOptimization>
  </PropertyGroup>
 */
.replace(/<CharacterSet>[^]*?<\/CharacterSet>/g, '')
.replace(/<WholeProgramOptimization>[^]*?<\/WholeProgramOptimization>/g, '')
.replace(/<PlatformToolset>[^]*?<\/PlatformToolset>/g, `<PlatformToolset>v142</PlatformToolset>
	<CharacterSet>Unicode</CharacterSet>
	<WholeProgramOptimization>true</WholeProgramOptimization>`)
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
      <DisableSpecificWarnings>4244;4018;4267;5105;4100</DisableSpecificWarnings>
      <LanguageStandard>stdcpp17</LanguageStandard>
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
      <DisableSpecificWarnings>4244;4018;4267;5105;4100</DisableSpecificWarnings>
      <LanguageStandard>stdcpp17</LanguageStandard>
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

	// now patch the entries and remove those that all build modees have in common:
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

	return `<ItemDefinitionGroup${p1}>${p2}</ItemDefinitionGroup>`;
});

// and remove any dandruff from a previous run:
src = src
.replace(/<ItemDefinitionGroup>[\s\r\n]*<ClCompile>[\s\r\n]*<Optimization>[^<]*<\/Optimization>[\s\r\n]*<BasicRuntimeChecks>[^<]*<\/BasicRuntimeChecks>[\s\r\n]*<RuntimeLibrary>[^<]*<\/RuntimeLibrary>[\s\r\n]*<PrecompiledHeaderOutputFile>[^<]*<\/PrecompiledHeaderOutputFile>[\s\r\n]*<\/ClCompile>[\s\r\n]*<\/ItemDefinitionGroup>/g, '')
.replace(/<ItemDefinitionGroup>[\s\r\n]*<ClCompile>[\s\r\n]*<Optimization>[^<]*<\/Optimization>[\s\r\n]*<BasicRuntimeChecks>[^<]*<\/BasicRuntimeChecks>[\s\r\n]*<RuntimeLibrary>[^<]*<\/RuntimeLibrary>[\s\r\n]*<\/ClCompile>[\s\r\n]*<\/ItemDefinitionGroup>/g, '')


// prepend this common blob before all other blobs:
src = src
.replace(/<ItemDefinitionGroup/, (m) => {
	return `${compiler_settings}${m}`;
});

fs.writeFileSync(filepath, src, 'utf8');


