
sources:
	../../scripts/googlelog-unittest/pch.h
	../../scripts/googlelog-unittest/main.cpp
	../../scripts/googlelog-unittest/test_glog.cpp
	../../scripts/googlelog-unittest/packages.config

also-ignore:
	VersionResource.rc
	bazel/example/
	
special-inject:
  <ItemGroup>
    <ClCompile Include="../../scripts/googlelog-unittest/pch.cpp">
      <PrecompiledHeader>Create</PrecompiledHeader>
    </ClCompile>
  </ItemGroup>
	