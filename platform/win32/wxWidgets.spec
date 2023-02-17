
directories:
   ..\..\scripts\wxWidgets\
 
sources:
    mk_jumbo_source_files.js

    
also-ignore:
	/samples/
	/tests/
	/utils/
	/src/
	/demos/
	/distrib/
	/misc/
	/build/


special-inject:
  <ItemGroup>
    <ClCompile Include="../../thirdparty/owemdjee/wxWidgets/src/common/dummy.cpp">
      <PrecompiledHeader>Create</PrecompiledHeader>
    </ClCompile>
  </ItemGroup>
	