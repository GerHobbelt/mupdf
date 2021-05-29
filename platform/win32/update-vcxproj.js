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
      <DisableSpecificWarnings>4244;4018;4244;4267;5105</DisableSpecificWarnings>
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
      <Optimization>Disabled</Optimization>
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
    </ClCompile>
</ItemDefinitionGroup>
`;
		
src = src
.replace(/<ItemDefinitionGroup([^>]*)>([\s\S]*?)<\/ItemDefinitionGroup>/g, (m, p1, p2) => {
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
	.replace(/<Optimization>[^<]+<\/Optimization>/g, (m) => `<Optimization>${ isDebug ? "Disabled" : "MaxSpeed" }</Optimization>`)
	.replace(/<MinimalRebuild>[^<]+<\/MinimalRebuild>/g, (m) => ``)
	.replace(/<BasicRuntimeChecks>[^<]+<\/BasicRuntimeChecks>/g, (m) => `<BasicRuntimeChecks>${ isDebug ? "EnableFastChecks" : "Default" }</BasicRuntimeChecks>`)
	.replace(/<RuntimeLibrary>[^<]+<\/RuntimeLibrary>/g, (m) => `<RuntimeLibrary>${ isDebug ? "MultiThreadedDebugDLL" : "MultiThreadedDLL" }</RuntimeLibrary>`)
	.replace(/<AssemblerListingLocation>[^<]+<\/AssemblerListingLocation>/g, (m) => ``)
	.replace(/<ObjectFileName>[^<]+<\/ObjectFileName>/g, (m) => ``)
	.replace(/<ProgramDataBaseFileName>[^<]+<\/ProgramDataBaseFileName>/g, (m) => ``)
	.replace(/<BrowseInformation>[^<]+<\/BrowseInformation>/g, (m) => ``)
	.replace(/<WarningLevel>[^<]+<\/WarningLevel>/g, (m) => ``)
	.replace(/<SuppressStartupBanner>[^<]+<\/SuppressStartupBanner>/g, (m) => ``)
	.replace(/<DebugInformationFormat>[^<]+<\/DebugInformationFormat>/g, (m) => ``)
	.replace(/<FunctionLevelLinking>[^<]+<\/FunctionLevelLinking>/g, (m) => ``)
	.replace(/<LanguageStandard>[^<]+<\/LanguageStandard>/g, (m) => ``)
	.replace(/<LanguageStandard_C>[^<]+<\/LanguageStandard_C>/g, (m) => ``)
	.replace(/<SupportJustMyCode>[^<]+<\/SupportJustMyCode>/g, (m) => ``)
	.replace(/<SDLCheck>[^<]+<\/SDLCheck>/g, (m) => ``)
	.replace(/<MultiProcessorCompilation>[^<]+<\/MultiProcessorCompilation>/g, (m) => ``)
	.replace(/<IntrinsicFunctions>[^<]+<\/IntrinsicFunctions>/g, (m) => ``)
	.replace(/<StringPooling>[^<]+<\/StringPooling>/g, (m) => ``)
	.replace(/<ExceptionHandling>[^<]+<\/ExceptionHandling>/g, (m) => ``)
	.replace(/<EnableEnhancedInstructionSet>[^<]+<\/EnableEnhancedInstructionSet>/g, (m) => ``)
	.replace(/<FloatingPointModel>[^<]+<\/FloatingPointModel>/g, (m) => ``)
	.replace(/<EnableFiberSafeOptimizations>[^<]+<\/EnableFiberSafeOptimizations>/g, (m) => ``)
	.replace(/<FloatingPointExceptions>[^<]+<\/FloatingPointExceptions>/g, (m) => ``)
	.replace(/<ConformanceMode>[^<]+<\/ConformanceMode>/g, (m) => ``)


	return `<ItemDefinitionGroup${p1}>${p2}</ItemDefinitionGroup>`;
});


// prepend this common blob before all other blobs:
src = src
.replace(/<ItemDefinitionGroup/, (m) => {
	return `${compiler_settings}${m}`;
});

fs.writeFileSync(filepath, src, 'utf8');


