# CMake generated Testfile for 
# Source directory: Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest
# Build directory: Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xscblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xscblat1.exe")
  set_tests_properties(xscblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xscblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xscblat1.exe")
  set_tests_properties(xscblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xscblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xscblat1.exe")
  set_tests_properties(xscblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xscblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xscblat1.exe")
  set_tests_properties(xscblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xscblat1 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xscblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xscblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin2")
  set_tests_properties(xscblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xscblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xscblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin2")
  set_tests_properties(xscblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xscblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xscblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin2")
  set_tests_properties(xscblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xscblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xscblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin2")
  set_tests_properties(xscblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xscblat2 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xscblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xscblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin3")
  set_tests_properties(xscblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xscblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xscblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin3")
  set_tests_properties(xscblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xscblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xscblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin3")
  set_tests_properties(xscblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xscblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xscblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/sin3")
  set_tests_properties(xscblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xscblat3 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xdcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xdcblat1.exe")
  set_tests_properties(xdcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xdcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xdcblat1.exe")
  set_tests_properties(xdcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xdcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xdcblat1.exe")
  set_tests_properties(xdcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xdcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xdcblat1.exe")
  set_tests_properties(xdcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xdcblat1 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xdcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xdcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din2")
  set_tests_properties(xdcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xdcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xdcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din2")
  set_tests_properties(xdcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xdcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xdcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din2")
  set_tests_properties(xdcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xdcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xdcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din2")
  set_tests_properties(xdcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xdcblat2 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xdcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xdcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din3")
  set_tests_properties(xdcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xdcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xdcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din3")
  set_tests_properties(xdcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xdcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xdcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din3")
  set_tests_properties(xdcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xdcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xdcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/din3")
  set_tests_properties(xdcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xdcblat3 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xccblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xccblat1.exe")
  set_tests_properties(xccblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xccblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xccblat1.exe")
  set_tests_properties(xccblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xccblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xccblat1.exe")
  set_tests_properties(xccblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xccblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xccblat1.exe")
  set_tests_properties(xccblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xccblat1 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xccblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xccblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin2")
  set_tests_properties(xccblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xccblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xccblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin2")
  set_tests_properties(xccblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xccblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xccblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin2")
  set_tests_properties(xccblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xccblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xccblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin2")
  set_tests_properties(xccblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xccblat2 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xccblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xccblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin3")
  set_tests_properties(xccblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xccblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xccblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin3")
  set_tests_properties(xccblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xccblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xccblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin3")
  set_tests_properties(xccblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xccblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xccblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/cin3")
  set_tests_properties(xccblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xccblat3 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xzcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xzcblat1.exe")
  set_tests_properties(xzcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xzcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xzcblat1.exe")
  set_tests_properties(xzcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xzcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xzcblat1.exe")
  set_tests_properties(xzcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xzcblat1 "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xzcblat1.exe")
  set_tests_properties(xzcblat1 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;46;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xzcblat1 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xzcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xzcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin2")
  set_tests_properties(xzcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xzcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xzcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin2")
  set_tests_properties(xzcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xzcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xzcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin2")
  set_tests_properties(xzcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xzcblat2 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xzcblat2.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin2")
  set_tests_properties(xzcblat2 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;71;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xzcblat2 NOT_AVAILABLE)
endif()
if(CTEST_CONFIGURATION_TYPE MATCHES "^([Dd][Ee][Bb][Uu][Gg])$")
  add_test(xzcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Debug/xzcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin3")
  set_tests_properties(xzcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ee][Aa][Ss][Ee])$")
  add_test(xzcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/Release/xzcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin3")
  set_tests_properties(xzcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Mm][Ii][Nn][Ss][Ii][Zz][Ee][Rr][Ee][Ll])$")
  add_test(xzcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/MinSizeRel/xzcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin3")
  set_tests_properties(xzcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
elseif(CTEST_CONFIGURATION_TYPE MATCHES "^([Rr][Ee][Ll][Ww][Ii][Tt][Hh][Dd][Ee][Bb][Ii][Nn][Ff][Oo])$")
  add_test(xzcblat3 "powershell" "-ExecutionPolicy" "Bypass" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/test_cblas_helper.ps1" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/b/ctest/RelWithDebInfo/xzcblat3.exe" "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/zin3")
  set_tests_properties(xzcblat3 PROPERTIES  _BACKTRACE_TRIPLES "Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;96;add_test;Z:/lib/tooling/qiqqa/MuPDF/thirdparty/owemdjee/OpenBLAS/ctest/CMakeLists.txt;0;")
else()
  add_test(xzcblat3 NOT_AVAILABLE)
endif()