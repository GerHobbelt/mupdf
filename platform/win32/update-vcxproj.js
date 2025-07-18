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
//.replace(/<ProjectName>[^]*?<\/ProjectName>/g, (m) => `<ProjectName>${projectName}</ProjectName>`)
//.replace(/<RootNamespace>[^]*?<\/RootNamespace>/g, (m) => `<RootNamespace>${projectName}</RootNamespace>`)
.replace(/<ProjectName>[^]*?<\/ProjectName>/g, (m) => ``)
.replace(/<RootNamespace>[^]*?<\/RootNamespace>/g, (m) => ``)
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
//       <OmitFramePointers>true</OmitFramePointers>
.replace(/<OmitFramePointers>[^]*?<\/OmitFramePointers>/g, '')
//       <LinkIncremental>false</LinkIncremental>    -- will be set further below; it doesn't work when specified in the common-project.props.
.replace(/<LinkIncremental>[^]*?<\/LinkIncremental>/g, '')
.replace(/<WholeProgramOptimization>[^]*?<\/WholeProgramOptimization>/g, '')
.replace(/<CopyLocalDeploymentContent>[^]*?<\/CopyLocalDeploymentContent>/g, '')
// remove empty PropertyGroups, e.g.
// <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" />
.replace(/<PropertyGroup Condition="[^<>]* \/>/g, '')
//   <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
//   </PropertyGroup>
.replace(/<PropertyGroup Condition="[^<>]*>[\s]*?<\/PropertyGroup>/gm, '')
// merge unlabeled PropertyGroup sections, e.g.
//   <PropertyGroup>
//     <_ProjectFileVersion>15.0.28307.799</_ProjectFileVersion>
//   </PropertyGroup>
//   <PropertyGroup>
//     ...
.replace(/(<PropertyGroup>[^]*?)<\/PropertyGroup>[\s\r\n]*<PropertyGroup>/g, '$1\n')
// consolidate the CopyLocalDeploymentContent setting to the main property group:
.replace(/<\/IntDir>/g, `</IntDir>
    <CopyLocalDeploymentContent>true</CopyLocalDeploymentContent>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <LinkIncremental>false</LinkIncremental>
    `)
/*
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'" Label="Configuration">
    <ConfigurationType>StaticLibrary</ConfigurationType>
    <PlatformToolset>v143</PlatformToolset>
    <UseOfMfc>false</UseOfMfc>
    <CharacterSet>Unicode</CharacterSet>
    <UseDebugLibraries>true</UseDebugLibraries>
    <WholeProgramOptimization>true</WholeProgramOptimization>
    <LinkIncremental>false</LinkIncremental>
  </PropertyGroup>
*/
.replace(/<PropertyGroup Condition="[^<>]*>[\s\S]*?<\/PropertyGroup>/gm, function (m) {
	let rv = m;

	// do not patch the sections that come *before* the .props file imports:
	if (/ Label="Configuration"/.test(rv))
		return rv;
	
	if (!/<OutDir>/.test(rv)) {
		rv = rv
		.replace(/<\/PropertyGroup>/, `
    <OutDir>xxx</OutDir>
    <IntDir>xxx</IntDir>
  </PropertyGroup>
		`).trim();
	}
	if (!/<IntDir>/.test(rv)) {
		rv = rv
		.replace(/<\/PropertyGroup>/, `
    <IntDir>xxx</IntDir>
  </PropertyGroup>
		`).trim();
	}
	
	return rv;
})
// make sure every debug/release PropertyGroup carries these OutDir and IntDir settings; this prevents MSVC from producing weird-named intermediate build directories directly inside the .../win32/ base directory.
/*
  <PropertyGroup Condition="'$(Configuration)|$(Platform)'=='Release|x64'">
    <NMakeOutput>collection-1.exe</NMakeOutput>
    <NMakePreprocessorDefinitions>NDEBUG;$(NMakePreprocessorDefinitions)</NMakePreprocessorDefinitions>
    <OutDir>$(SolutionDir)bin\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\</OutDir>
    <IntDir>$(SolutionDir)obj\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\$(ConfigurationType)-$(ProjectName)\</IntDir>
  </PropertyGroup>
 */
.replace(/<NMakeOutput>collection-1.exe<\/NMakeOutput>/g, '<NMakeOutput></NMakeOutput>')
//     <OutDir>$(SolutionDir)bin\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\</OutDir>
.replace(/<OutDir>[^]*?<\/OutDir>/g, `<OutDir>$(SolutionDir)bin\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\</OutDir>`)
//    <IntDir>$(SolutionDir)obj\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\$(RootNamespace)-$(ConfigurationType)-$(ProjectName)\</IntDir>
.replace(/<IntDir>[^]*?<\/IntDir>/g, `<IntDir>$(SolutionDir)obj\\$(Configuration)-$(CharacterSet)-$(PlatformArchitecture)bit-$(PlatformShortname)\\$(ConfigurationType)-$(ProjectName)\\</IntDir>`)
.replace(/<CharacterSet>[^]*?<\/CharacterSet>/g, '')
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
//   <ImportGroup Label="PropertySheets" Condition="'$(Configuration)|$(Platform)'=='Debug|Win32'">
//    <Import Project="$(SolutionDir)\common-project-Debug-Win32.props"  Condition="exists('$(SolutionDir)\common-project-Debug-Win32.props')" Label="SolutionWideDebugWin32Settings" />
//   <Import Project="$(SolutionDir)\common-project-ultimate-override.props" Label="SolutionWideSettingsOverride" />
.replace(/<Import[^>]+common-project\.props"(?:\s+Label="[^"]+")?\s*\/>/g, '')
.replace(/<Import[^>]+common-project-ultimate-override\.props"(?:\s+Label="[^"]+")?\s*\/>/g, '')
.replace(/<ImportGroup\s+Label="PropertySheets"\s*\/>/g, '')
.replace(/<ImportGroup\s+Label="PropertySheets"[^]*?<\/ImportGroup>/g, '')
.replace(/<EnableUnitySupport>[^<]*<\/EnableUnitySupport>/g, '')
.replace(/<UseStructuredOutput>[^<]*<\/UseStructuredOutput>/g, '')

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
      <Optimization>Custom</Optimization>
      <BasicRuntimeChecks>EnableFastChecks</BasicRuntimeChecks>
      <RuntimeLibrary>MultiThreadedDebugDLL</RuntimeLibrary>
      <PrecompiledHeaderOutputFile>$(IntDir)$(TargetName).pch</PrecompiledHeaderOutputFile>
      <AssemblerListingLocation>$(IntDir)</AssemblerListingLocation>
      <ObjectFileName>$(IntDir)</ObjectFileName>
      <ProgramDataBaseFileName>$(IntDir)$(ProjectName).pdb</ProgramDataBaseFileName>
      <BrowseInformation>false</BrowseInformation>
      <WarningLevel>EnableAllWarnings</WarningLevel>
      <SuppressStartupBanner>true</SuppressStartupBanner>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <DisableSpecificWarnings>;%(DisableSpecificWarnings)</DisableSpecificWarnings>
      <LanguageStandard>stdcpplatest</LanguageStandard>
      <LanguageStandard_C>stdclatest</LanguageStandard_C>
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
      <AdditionalOptions>/bigobj /utf-8 /Zc:__cplusplus /Zc:preprocessor %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>

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
	
With /WAll (all warnings enabled) we need to kill a few very obnoxious ones that didn't show at warning Level 4. Examples:

- warning C4820: 'opj_poc': '3' bytes padding added after data member 'progorder'
- warning C5045: Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
- warning C4464: relative include path contains '..'
- warning C4668: '_WIN32_WINNT_WIN10_TH2' is not defined as a preprocessor macro, replacing with '0' for '#if/#elif'
- warning C4061: enumerator 'OPJ_PROG_UNKNOWN' in switch of enum 'PROG_ORDER' is not explicitly handled by a case label
- warning C4710: 'void _force_has_format_string(const char *,...)': function not inlined
- warning C4514: 'icu::UnicodeString::reverse': unreferenced inline function has been removed

   -->
 
      <DisableSpecificWarnings>4514;4464;4061;4668;5045;4820;4180;4244;4018;4267;5105;4100;4127;4206;%(DisableSpecificWarnings)</DisableSpecificWarnings>
 
while we keep these:

- warning C4255: 'opj_procedure_list_create': no function prototype given: converting '()' to '(void)'
- warning C4242: 'function': conversion from 'Byte8_t' to 'unsigned int', possible loss of data
- warning C4296: '>=': expression is always true
- warning C4365: 'argument': conversion from '__int64' to 'unsigned int', signed/unsigned mismatch
- warning C4309: 'argument': truncation of constant value
- warning C4310: cast truncates constant value
- warning C4324: 'my_error_mgr': structure was padded due to alignment specifier
- warning C4388: '!=': signed/unsigned mismatch
- warning C4996: 'htmlSAXParseFile': was declared deprecated
- warning C5219: implicit conversion from 'PRUint32' to 'float', possible loss of data
- warning C5262: implicit fall-through occurs here; are you missing a break statement? Use [[fallthrough]] when a break statement is intentionally omitted between cases
- warning C5250: '__builtin_assume_aligned': intrinsic function not declared
- warning C4200: nonstandard extension used: zero-sized array in struct/union
- warning C5031: #pragma warning(pop): likely mismatch, popping warning state pushed in different file
- warning C5264: 'hwy::`anonymous-namespace'::SortI16DescHighwayDispatchTable': 'const' variable is not used
- warning C5266: 'const' qualifier on return type has no effect
- warning C4619: #pragma warning: there is no warning number '4231'
- warning C4152: nonstandard extension, function/data pointer conversion in expression
- warning C4800: Implicit conversion from 'int' to bool. Possible information loss
- warning C5246: '_Elems': the initialization of a subobject should be wrapped in braces
- warning C4355: 'this': used in base member initializer list
- warning C4200: nonstandard extension used: zero-sized array in struct/union
- warning C4152: nonstandard extension, function/data pointer conversion in expression
- warning C4711: function 'pngcrush_pause' selected for automatic inline expansion
- warning C4774: 'snprintf' : format string expected in argument 3 is not a string literal

- warning C4295: 'singleline_comment_1': array is too small to include a terminating null character
- warning C4295: 'xmltranscodetable_ISO8859_2': array is too small to include a terminating null character
- warning C4295: 'in': array is too small to include a terminating null character
- warning C4295: 'xmltranscodetable_ISO8859_5': array is too small to include a terminating null character

- warning C4133: 'function': incompatible types - from 'opj_jp2_t *' to 'opj_j2k_t *'

- warning C4191: 'type cast': unsafe conversion from 'GLADapiproc' to 'PFNGLEVALMESH2PROC'.   Making a function call using the resulting pointer may cause your program to fail.

- warning C4626: 'fmt::detail::counting_buffer<char>': assignment operator was implicitly defined as deleted
- warning C4623: 'OT::LayerList': default constructor was implicitly defined as deleted
- warning C4371: 'hb_filter_iter_factory_t<const hb_set_t *&,OT::HBUINT16 OT::FeatureTableSubstitutionRecord::* >': layout of class may have changed from a previous version of the compiler due to better packing of member 'hb_filter_iter_factory_t<const hb_set_t *&,OT::HBUINT16 OT::FeatureTableSubstitutionRecord::* >::f'

- warning C5026: 'Eigen::internal::evaluator<Eigen::PlainObjectBase<Eigen::Array<double,-1,1,0,-1,1>>>': move constructor was implicitly defined as deleted
- warning C4626: 'Eigen::internal::evaluator<Eigen::PlainObjectBase<Eigen::Array<double,-1,1,0,-1,1>>>': assignment operator was implicitly defined as deleted
- warning C5027: 'Eigen::internal::evaluator<Eigen::PlainObjectBase<Eigen::Array<double,-1,1,0,-1,1>>>': move assignment operator was implicitly defined as deleted
- warning C4625: 'Eigen::internal::evaluator<Eigen::Array<double,-1,1,0,-1,1>>': copy constructor was implicitly defined as deleted
- warning C5026: 'Eigen::internal::evaluator<Eigen::Array<double,-1,1,0,-1,1>>': move constructor was implicitly defined as deleted
- warning C4626: 'Eigen::internal::evaluator<Eigen::Array<double,-1,1,0,-1,1>>': assignment operator was implicitly defined as deleted
- warning C5027: 'Eigen::internal::evaluator<Eigen::Array<double,-1,1,0,-1,1>>': move assignment operator was implicitly defined as deleted

- warning C4263: 'void TDataCollection::error(int)': member function does not override any base class virtual member function
- warning C4264: 'void TNSCollection::error(ccIndex,ccIndex)': no override available for virtual member function from base 'TNSCollection'; function is hidden
- warning C4265: 'tvision::NegativeScreenCursor': class has virtual functions, but its non-trivial destructor is not virtual; instances of this class may not be destructed correctly
- warning C4435: 'TCollection': Object layout under /vd2 will change due to virtual base 'TNSCollection'
- warning C4582: 'fmt::detail::value<Context>::no_value': constructor is not implicitly called
- warning C4710: 'bool fmt::detail::has_const_formatter_impl<Context>(...)': function not inlined
- warning C5214: applying '++' to an operand with a volatile qualified type is deprecated in C++20
- warning C5267: definition of implicit assignment operator for 'TColorGroup' is deprecated because it has a user-provided destructor
- warning C5267: definition of implicit copy constructor for 'cv::debug_build_guard::_InputArray' is deprecated because it has a user-provided destructor
- warning C5038: data member 'spdlog::details::styling_info::styles' will be initialized after data member 'spdlog::details::styling_info::is_start'
- warning C5204: 'CharDistributionAnalysis': class has virtual functions, but its trivial destructor is not virtual; instances of objects derived from this class may not be destructed correctly
- warning C4946: reinterpret_cast used between related classes: 'cv::Matx<double,4,1>' and 'cv::Scalar_<double>'
- warning C5267: definition of implicit assignment operator for 'cv::dnn::dnn4_v20240521::Net' is deprecated because it has a user-provided destructor
- warning C4266: 'void cv::Feature2D::detect(cv::InputArrayOfArrays,std::vector<std::vector<cv::KeyPoint,std::allocator<cv::KeyPoint>>,std::allocator<std::vector<cv::KeyPoint,std::allocator<cv::KeyPoint>>>> &,cv::InputArrayOfArrays)': no override available for virtual member function from base 'cv::Feature2D'; function is hidden.
- warning C5029: nonstandard extension used: alignment attributes in C++ apply to variables, data members and tag types only
- warning C5220: 'Concurrency::_Worker_proxy<_Index_type>::_M_pWorker_range': a non-static data member with a volatile qualified type no longer implies that compiler generated copy/move constructors and copy/move assignment operators are not trivial.
- warning C4582: 'absl::lts_20240722::CordBuffer::Rep::short_rep': constructor is not implicitly called
- warning C4583: 'absl::lts_20240722::flags_internal::Flag<std::string>::Get::U::value': destructor is not implicitly called
- warning C4437: dynamic_cast from virtual base 'testing::gmock_matchers_test::`anonymous-namespace'::Base' to 'testing::gmock_matchers_test::`anonymous-namespace'::AmbiguousCastTypes::VirtualDerived' could fail in some contexts
- warning C4868: compiler may not enforce left-to-right evaluation order in braced initializer list
- warning C4866: compiler may not enforce left-to-right evaluation order for call to 'clipp::operator,'
- warning C4866: compiler may not enforce left-to-right evaluation order for call to 'absl::lts_20240722::log_internal::LogMessage::operator<<<23>'

lest we want to ignore most of these too:
 
      <DisableSpecificWarnings>4774;4711;4295;4152;4200;4355;5246;5267;4866;4868;5220;4582;4583;4435;4619;5029;5266;4371;5031;4946;5250;5262;5038;5219;4710;4388;4324;4242;4365;4623;4626;5026;5027;4625;4514;4464;4061;4668;5045;4820;4180;4244;4018;4267;5105;4100;4127;4206;4102;4146;4189;4245;4334;4389;4457;4458;4456;4459;4701;4702;4706;%(DisableSpecificWarnings)</DisableSpecificWarnings>
	

------------------------------------------------------------------------------------------------------------------------------------------------

**NOTE/WARNING**: 

these warning numbers should be set in the `common-project.props` file to ensure all our projects receive the same base disable set. 
The DisableSpecificWarnings entry in the project files themselves is only intended to set ADDITIONAL PROJECT-SPECIFIC overrides!

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
      <WarningLevel>EnableAllWarnings</WarningLevel>
      <SuppressStartupBanner>true</SuppressStartupBanner>
      <DebugInformationFormat>ProgramDatabase</DebugInformationFormat>
      <FunctionLevelLinking>true</FunctionLevelLinking>
      <DisableSpecificWarnings>%(DisableSpecificWarnings)</DisableSpecificWarnings>
      <LanguageStandard>stdcpplatest</LanguageStandard>
      <LanguageStandard_C>stdclatest</LanguageStandard_C>
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
      <AdditionalOptions>/bigobj /utf-8 /Zc:__cplusplus /Zc:preprocessor %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
</ItemDefinitionGroup>
`;

let props_src = fs.readFileSync('common-project.props', 'utf8');
let common_warnings = collect_warning_codes(props_src);


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
    .replace(/<AdditionalOptions>\/bigobj \/utf-8 \/Zc:__cplusplus \/Zc:preprocessor %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
    .replace(/<AdditionalOptions>\/bigobj \/utf-8 %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
    .replace(/<AdditionalOptions>\/bigobj %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
    .replace(/<AdditionalOptions>\/utf-8 %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')
    .replace(/<AdditionalOptions>\/Zc:__cplusplus \/utf-8 %\(AdditionalOptions\)<\/AdditionalOptions>/g, '')

	// remove <Link> fields which are defined in common-project.props
    p2 = p2
    .replace(/<GenerateMapFile>[^]*?<\/GenerateMapFile>/g, '')
    .replace(/<MapFileName>[^]*?<\/MapFileName>/g, '')
    .replace(/<MapExports>[^]*?<\/MapExports>/g, '')
    .replace(/<Profile>[^]*?<\/Profile>/g, '')

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
	if (common_warnings.includes(el))
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

  <Import Project="$(SolutionDir)\\common-project.props" Label="SolutionWideSettings" />

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

  <Import Project="$(SolutionDir)\\common-project-ultimate-override.props" Label="SolutionWideSettingsOverride" />

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
	.replace(/;..\/..\/thirdparty\/owemdjee\/libassert\/include;/g, ';')
	.replace(/;..\/..\/thirdparty\/owemdjee\/magic_enum\/include;/g, ';')
	.replace(/;..\/..\/thirdparty\/owemdjee\/cpptrace\/include;/g, ';')
	.replace(/;..\/..\/thirdparty\/owemdjee\/fmt\/include;/g, ';')
    .replace(/;%\(AdditionalIncludeDirectories\);/g, ';')
    .replace(/;.;/g, ';')

// patch: when there's a single /boost/ reference in there, get the whole damn bunch:
if (/\/owemdjee\/boost\b/.test(include_paths) || /\/scripts\/boost\b/.test(include_paths)) {
    include_paths = `${include_paths}
    ../../scripts/boost/include
    ../../thirdparty/owemdjee/boost
    ../../thirdparty/owemdjee/boost/libs/accumulators/include
    ../../thirdparty/owemdjee/boost/libs/algorithm/include
    ../../thirdparty/owemdjee/boost/libs/align/include
    ../../thirdparty/owemdjee/boost/libs/any/include
    ../../thirdparty/owemdjee/boost/libs/array/include
    ../../thirdparty/owemdjee/boost/libs/asio/include
    ../../thirdparty/owemdjee/boost/libs/assert/include
    ../../thirdparty/owemdjee/boost/libs/assign/include
    ../../thirdparty/owemdjee/boost/libs/atomic/include
    ../../thirdparty/owemdjee/boost/libs/beast/include
    ../../thirdparty/owemdjee/boost/libs/bimap/include
    ../../thirdparty/owemdjee/boost/libs/bind/include
    ../../thirdparty/owemdjee/boost/libs/callable_traits/include
    ../../thirdparty/owemdjee/boost/libs/charconv/include
    ../../thirdparty/owemdjee/boost/libs/chrono/include
    ../../thirdparty/owemdjee/boost/libs/circular_buffer/include
    ../../thirdparty/owemdjee/boost/libs/cobalt/include
    ../../thirdparty/owemdjee/boost/libs/compat/include
    ../../thirdparty/owemdjee/boost/libs/compatibility/include
    ../../thirdparty/owemdjee/boost/libs/compute/include
    ../../thirdparty/owemdjee/boost/libs/concept_check/include
    ../../thirdparty/owemdjee/boost/libs/config/include
    ../../thirdparty/owemdjee/boost/libs/container/include
    ../../thirdparty/owemdjee/boost/libs/container_hash/include
    ../../thirdparty/owemdjee/boost/libs/context/include
    ../../thirdparty/owemdjee/boost/libs/contract/include
    ../../thirdparty/owemdjee/boost/libs/conversion/include
    ../../thirdparty/owemdjee/boost/libs/convert/include
    ../../thirdparty/owemdjee/boost/libs/core/include
    ../../thirdparty/owemdjee/boost/libs/coroutine/include
    ../../thirdparty/owemdjee/boost/libs/coroutine2/include
    ../../thirdparty/owemdjee/boost/libs/crc/include
    ../../thirdparty/owemdjee/boost/libs/date_time/include
    ../../thirdparty/owemdjee/boost/libs/describe/include
    ../../thirdparty/owemdjee/boost/libs/detail/include
    ../../thirdparty/owemdjee/boost/libs/dll/include
    ../../thirdparty/owemdjee/boost/libs/dynamic_bitset/include
    ../../thirdparty/owemdjee/boost/libs/endian/include
    ../../thirdparty/owemdjee/boost/libs/exception/include
    ../../thirdparty/owemdjee/boost/libs/fiber/include
    ../../thirdparty/owemdjee/boost/libs/filesystem/include
    ../../thirdparty/owemdjee/boost/libs/flyweight/include
    ../../thirdparty/owemdjee/boost/libs/foreach/include
    ../../thirdparty/owemdjee/boost/libs/format/include
    ../../thirdparty/owemdjee/boost/libs/function/include
    ../../thirdparty/owemdjee/boost/libs/function_types/include
    ../../thirdparty/owemdjee/boost/libs/functional/include
    ../../thirdparty/owemdjee/boost/libs/fusion/include
    ../../thirdparty/owemdjee/boost/libs/geometry/include
    ../../thirdparty/owemdjee/boost/libs/gil/include
    ../../thirdparty/owemdjee/boost/libs/graph/include
    ../../thirdparty/owemdjee/boost/libs/graph_parallel/include
    ../../thirdparty/owemdjee/boost/libs/hana/include
    ../../thirdparty/owemdjee/boost/libs/headers/include
    ../../thirdparty/owemdjee/boost/libs/heap/include
    ../../thirdparty/owemdjee/boost/libs/histogram/include
    ../../thirdparty/owemdjee/boost/libs/hof/include
    ../../thirdparty/owemdjee/boost/libs/icl/include
    ../../thirdparty/owemdjee/boost/libs/integer/include
    ../../thirdparty/owemdjee/boost/libs/interprocess/include
    ../../thirdparty/owemdjee/boost/libs/intrusive/include
    ../../thirdparty/owemdjee/boost/libs/io/include
    ../../thirdparty/owemdjee/boost/libs/iostreams/include
    ../../thirdparty/owemdjee/boost/libs/iterator/include
    ../../thirdparty/owemdjee/boost/libs/json/include
    ../../thirdparty/owemdjee/boost/libs/lambda/include
    ../../thirdparty/owemdjee/boost/libs/lambda2/include
    ../../thirdparty/owemdjee/boost/libs/leaf/include
    ../../thirdparty/owemdjee/boost/libs/lexical_cast/include
    ../../thirdparty/owemdjee/boost/libs/local_function/include
    ../../thirdparty/owemdjee/boost/libs/locale/include
    ../../thirdparty/owemdjee/boost/libs/lockfree/include
    ../../thirdparty/owemdjee/boost/libs/log/include
    ../../thirdparty/owemdjee/boost/libs/logic/include
    ../../thirdparty/owemdjee/boost/libs/math/include
    ../../thirdparty/owemdjee/boost/libs/metaparse/include
    ../../thirdparty/owemdjee/boost/libs/move/include
    ../../thirdparty/owemdjee/boost/libs/mp11/include
    ../../thirdparty/owemdjee/boost/libs/mp11/include/
    ../../thirdparty/owemdjee/boost/libs/mpi/include
    ../../thirdparty/owemdjee/boost/libs/mpl/include
    ../../thirdparty/owemdjee/boost/libs/mpl/preprocessed/include
    ../../thirdparty/owemdjee/boost/libs/msm/include
    ../../thirdparty/owemdjee/boost/libs/multi_array/include
    ../../thirdparty/owemdjee/boost/libs/multi_index/include
    ../../thirdparty/owemdjee/boost/libs/multiprecision/include
    ../../thirdparty/owemdjee/boost/libs/mysql/include
    ../../thirdparty/owemdjee/boost/libs/nowide/include
    ../../thirdparty/owemdjee/boost/libs/numeric/conversion/include
    ../../thirdparty/owemdjee/boost/libs/numeric/interval/include
    ../../thirdparty/owemdjee/boost/libs/numeric/odeint/include
    ../../thirdparty/owemdjee/boost/libs/numeric/ublas/include
    ../../thirdparty/owemdjee/boost/libs/optional/include
    ../../thirdparty/owemdjee/boost/libs/outcome/include
    ../../thirdparty/owemdjee/boost/libs/parameter/include
    ../../thirdparty/owemdjee/boost/libs/parameter_python/include
    ../../thirdparty/owemdjee/boost/libs/parser/include	
    ../../thirdparty/owemdjee/boost/libs/pfr/include
    ../../thirdparty/owemdjee/boost/libs/phoenix/include
    ../../thirdparty/owemdjee/boost/libs/poly_collection/include
    ../../thirdparty/owemdjee/boost/libs/polygon/include
    ../../thirdparty/owemdjee/boost/libs/pool/include
    ../../thirdparty/owemdjee/boost/libs/predef/include
    ../../thirdparty/owemdjee/boost/libs/preprocessor/include
    ../../thirdparty/owemdjee/boost/libs/process/include
    ../../thirdparty/owemdjee/boost/libs/program_options/include
    ../../thirdparty/owemdjee/boost/libs/property_map/include
    ../../thirdparty/owemdjee/boost/libs/property_map_parallel/include
    ../../thirdparty/owemdjee/boost/libs/property_tree/include
    ../../thirdparty/owemdjee/boost/libs/proto/include
    ../../thirdparty/owemdjee/boost/libs/ptr_container/include
    ../../thirdparty/owemdjee/boost/libs/python/include
    ../../thirdparty/owemdjee/boost/libs/qvm/include
    ../../thirdparty/owemdjee/boost/libs/random/include
    ../../thirdparty/owemdjee/boost/libs/range/include
    ../../thirdparty/owemdjee/boost/libs/ratio/include
    ../../thirdparty/owemdjee/boost/libs/rational/include
    ../../thirdparty/owemdjee/boost/libs/redis/include
    ../../thirdparty/owemdjee/boost/libs/regex/include
    ../../thirdparty/owemdjee/boost/libs/safe_numerics/include
    ../../thirdparty/owemdjee/boost/libs/scope/include
    ../../thirdparty/owemdjee/boost/libs/scope_exit/include
    ../../thirdparty/owemdjee/boost/libs/serialization/include
    ../../thirdparty/owemdjee/boost/libs/signals2/include
    ../../thirdparty/owemdjee/boost/libs/smart_ptr/include
    ../../thirdparty/owemdjee/boost/libs/sort/include
    ../../thirdparty/owemdjee/boost/libs/spirit/include
    ../../thirdparty/owemdjee/boost/libs/stacktrace/include
    ../../thirdparty/owemdjee/boost/libs/statechart/include
    ../../thirdparty/owemdjee/boost/libs/static_assert/include
    ../../thirdparty/owemdjee/boost/libs/static_string/include
    ../../thirdparty/owemdjee/boost/libs/stl_interfaces/include
    ../../thirdparty/owemdjee/boost/libs/system/include
    ../../thirdparty/owemdjee/boost/libs/test/include
    ../../thirdparty/owemdjee/boost/libs/thread/include
    ../../thirdparty/owemdjee/boost/libs/throw_exception/include
    ../../thirdparty/owemdjee/boost/libs/timer/include
    ../../thirdparty/owemdjee/boost/libs/tokenizer/include
    ../../thirdparty/owemdjee/boost/libs/tti/include
    ../../thirdparty/owemdjee/boost/libs/tuple/include
    ../../thirdparty/owemdjee/boost/libs/type_erasure/include
    ../../thirdparty/owemdjee/boost/libs/type_index/include
    ../../thirdparty/owemdjee/boost/libs/type_traits/include
    ../../thirdparty/owemdjee/boost/libs/typeof/include
    ../../thirdparty/owemdjee/boost/libs/units/include
    ../../thirdparty/owemdjee/boost/libs/unordered/include
    ../../thirdparty/owemdjee/boost/libs/url/include
    ../../thirdparty/owemdjee/boost/libs/utility/include
    ../../thirdparty/owemdjee/boost/libs/uuid/include
    ../../thirdparty/owemdjee/boost/libs/variant/include
    ../../thirdparty/owemdjee/boost/libs/variant2/include
    ../../thirdparty/owemdjee/boost/libs/vmd/include
    ../../thirdparty/owemdjee/boost/libs/wave/include
    ../../thirdparty/owemdjee/boost/libs/winapi/include
    ../../thirdparty/owemdjee/boost/libs/xpressive/include
    ../../thirdparty/owemdjee/boost/libs/yap/include
    ../../thirdparty/owemdjee/boost/tools/auto_index/include
    ../../thirdparty/owemdjee/boost/tools/cmake/include
    ../../thirdparty/owemdjee/boost/tools/docca/include
    `.replace(/\n/g, ';')
    .replace(/\s+/g, '');
}

// patch: when there's a single /openCV/ reference in there, get the whole damn bunch:
if (/\/owemdjee\/opencv\b/.test(include_paths) || /\/scripts\/OpenCV\b/.test(include_paths)) {
    include_paths = `${include_paths}
    ../../scripts/OpenCV
    ../../scripts/OpenCV/modules/core
    ../../scripts/OpenCV/modules/imgproc
    ../../scripts/OpenCV/modules/stitching
	../../scripts/OpenCV/modules/objdetect
	../../scripts/OpenCV/modules/photo
    ../../thirdparty/owemdjee/libeigen
	../../thirdparty/owemdjee/quirc/include
    ../../thirdparty/owemdjee/opencv/3rdparty/openvx/include
    ../../thirdparty/owemdjee/opencv/include
    ../../thirdparty/owemdjee/opencv/modules/calib3d/include
    ../../thirdparty/owemdjee/opencv/modules/core/include
    ../../thirdparty/owemdjee/opencv/modules/dnn
    ../../thirdparty/owemdjee/opencv/modules/dnn/include
    ../../thirdparty/owemdjee/opencv/modules/dnn/src/ocl4dnn/include
    ../../thirdparty/owemdjee/opencv/modules/dnn/src/vkcom/include
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
    ../../thirdparty/owemdjee/opencv/modules/video/include
    ../../thirdparty/owemdjee/opencv/modules/videoio/include
    ../../thirdparty/owemdjee/opencv/modules/world/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/alphamat/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/aruco/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/bgsegm/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/bioinspired/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cannops/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/ccalib/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cnn_3dobj/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudaarithm/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudabgsegm/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudacodec/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudafeatures2d/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudafilters/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudaimgproc/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudalegacy/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudaobjdetect/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudaoptflow/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudastereo/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudastructuredlight/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudawarping/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cudev/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/cvv/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/datasets/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/dnn_objdetect/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/dnn_superres/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/dpm/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/face/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/freetype/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/fuzzy/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/hdf/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/hfs/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/img_hash/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/intensity_transform/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/julia/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/line_descriptor/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/matlab/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/mcc/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/ml/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/optflow/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/ovis/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/phase_unwrapping/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/plot/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/quality/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/rapid/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/reg/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/rgbd/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/saliency/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/sfm/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/sfm/src/libmv_light
    ../../thirdparty/owemdjee/opencv_contrib/modules/shape/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/signal/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/structured_light/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/superres/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/surface_matching/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/text/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/tracking/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/videostab/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/viz/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/wechat_qrcode/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/xfeatures2d/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/ximgproc/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/xobjdetect/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/xphoto/include
    ../../thirdparty/owemdjee/opencv_contrib/modules/xstereo/include
    `.replace(/\n/g, ';')
    .replace(/\s+/g, '');
}

// patch: when there's a single /DCMTK/ reference in there, get the whole damn bunch:
if (/\/owemdjee\/dcmtk\b/.test(include_paths) || /\/scripts\/dcmtk\b/.test(include_paths)) {
    include_paths = `${include_paths}
	../../scripts/dcmtk/include
	../../thirdparty/owemdjee/dcmtk/dcmdata/include
	../../thirdparty/owemdjee/dcmtk/dcmect/include
	../../thirdparty/owemdjee/dcmtk/dcmfg/include
	../../thirdparty/owemdjee/dcmtk/dcmimage/include
	../../thirdparty/owemdjee/dcmtk/dcmimgle/include
	../../thirdparty/owemdjee/dcmtk/dcmiod/include
	../../thirdparty/owemdjee/dcmtk/dcmjpeg/include
	../../thirdparty/owemdjee/dcmtk/dcmjpeg/libijg12
	../../thirdparty/owemdjee/dcmtk/dcmjpeg/libijg16
	../../thirdparty/owemdjee/dcmtk/dcmjpeg/libijg8
	../../thirdparty/owemdjee/dcmtk/dcmjpls/include
	../../thirdparty/owemdjee/dcmtk/dcmjpls/libcharls
	../../thirdparty/owemdjee/dcmtk/dcmnet/include
	../../thirdparty/owemdjee/dcmtk/dcmpmap/include
	../../thirdparty/owemdjee/dcmtk/dcmpstat/include
	../../thirdparty/owemdjee/dcmtk/dcmqrdb/include
	../../thirdparty/owemdjee/dcmtk/dcmrt/include
	../../thirdparty/owemdjee/dcmtk/dcmseg/include
	../../thirdparty/owemdjee/dcmtk/dcmsign/include
	../../thirdparty/owemdjee/dcmtk/dcmsr/include
	../../thirdparty/owemdjee/dcmtk/dcmtls/include
	../../thirdparty/owemdjee/dcmtk/dcmtract/include
	../../thirdparty/owemdjee/dcmtk/dcmwlm/include
	../../thirdparty/owemdjee/dcmtk/oficonv/include
	../../thirdparty/owemdjee/dcmtk/oficonv/libsrc
	../../thirdparty/owemdjee/dcmtk/oflog/include
	../../thirdparty/owemdjee/dcmtk/ofstd/include
	../../thirdparty/owemdjee/dcmtk/dcmapps/include
    `.replace(/\n/g, ';')
    .replace(/\s+/g, '');
}

// patch: when there's a single /OpenEXR/ reference in there, get the whole damn bunch:
if (/\/owemdjee\/OpenEXR\b/.test(include_paths) || /\/scripts\/OpenEXR\b/.test(include_paths)) {
    include_paths = `${include_paths}
	../../scripts/Imath
	../../scripts/OpenEXR/include
	../../thirdparty/owemdjee/Imath/src/Imath
	../../thirdparty/owemdjee/OpenEXR/src/bin
	../../thirdparty/owemdjee/OpenEXR/src/bin/exrenvmap
	../../thirdparty/owemdjee/OpenEXR/src/lib/Iex
	../../thirdparty/owemdjee/OpenEXR/src/lib/IlmThread
	../../thirdparty/owemdjee/OpenEXR/src/lib/OpenEXR
	../../thirdparty/owemdjee/OpenEXR/src/lib/OpenEXRCore
	../../thirdparty/owemdjee/OpenEXR/src/lib/OpenEXRUtil
    `.replace(/\n/g, ';')
    .replace(/\s+/g, '');
}



//patch: when we have googletest/include --> we also required: ../../thirdparty/owemdjee/abseil-cpp --> requires: ../../thirdparty/owemdjee/re2
if (/\/googletest\//.test(include_paths)) {
    include_paths = `${include_paths}
    ../../thirdparty/owemdjee/abseil-cpp
	../../thirdparty/owemdjee/re2
    `.replace(/\n/g, ';')
    .replace(/\s+/g, '');
}
else if (/\/abseil-cpp\b/.test(include_paths)) {
    include_paths = `${include_paths}
	../../thirdparty/owemdjee/re2
    `.replace(/\n/g, ';')
    .replace(/\s+/g, '');
}

include_paths = `../../include/system-override;.;../../thirdparty/owemdjee/libassert/include;../../thirdparty/owemdjee/magic_enum/include;../../thirdparty/owemdjee/fmt/include;../../thirdparty/owemdjee/cpptrace/include;${include_paths};`;


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
      <AdditionalOptions>/bigobj /utf-8 /Zc:__cplusplus /Zc:preprocessor %(AdditionalOptions)</AdditionalOptions>
    </ClCompile>
  </ItemDefinitionGroup>
`
const dupliBlockbaseReParticle = dupliBlockbase
.trim()
.replace(/([\/()])/g, '\\$1')
.replace(/\\\/bigobj /g, '(?:\\/bigobj )?')
.replace(/\\\/Zc:__cplusplus /g, '(?:\\/Zc:__cplusplus )?')
.replace(/\\\/Zc:preprocessor /g, '(?:\\/Zc:preprocessor )?')
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

