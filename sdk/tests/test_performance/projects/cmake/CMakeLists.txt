project(test_performance)

include_directories(/Users/quarnster/code/asaot ../../../../angelscript/source)
link_directories(/Users/quarnster/code/asaot/build)

add_executable(
        test_performance
        ../../source/main.cpp
        ../../source/scriptstring.cpp
        ../../source/test_assign.cpp
        ../../source/test_basic.cpp
        ../../source/test_basic2.cpp
        ../../source/test_call.cpp
        ../../source/test_call2.cpp
        ../../source/test_fib.cpp
        ../../source/test_int.cpp
        ../../source/test_intf.cpp
        ../../source/test_mthd.cpp
        ../../source/test_string.cpp
        ../../source/test_string2.cpp
        ../../source/test_string_pooled.cpp
        ../../source/test_thisprop.cpp
        ../../source/test_vector3.cpp
        ../../source/utils.cpp

        ../../bin/TestAssign_generated.cpp
        ../../bin/TestBasic2_generated.cpp
        ../../bin/TestBasic_generated.cpp
        ../../bin/TestCall2_generated.cpp
        ../../bin/TestCall_generated.cpp
        ../../bin/TestFib_generated.cpp
        ../../bin/TestInt_generated.cpp
        ../../bin/TestIntf_generated.cpp
        ../../bin/TestMthd_generated.cpp
        ../../bin/TestString2_generated.cpp
        ../../bin/TestStringPooled_generated.cpp
        ../../bin/TestString_generated.cpp
        ../../bin/TestThisProp_generated.cpp
        ../../bin/TestVector3_generated.cpp

        ../../../../add_on/debugger/debugger.cpp
        ../../../../add_on/scriptany/scriptany.cpp
        ../../../../add_on/scriptarray/scriptarray.cpp
        ../../../../add_on/scriptbuilder/scriptbuilder.cpp
        ../../../../add_on/scriptdictionary/scriptdictionary.cpp
        ../../../../add_on/scriptfile/scriptfile.cpp
        ../../../../add_on/scripthandle/scripthandle.cpp
        ../../../../add_on/scripthelper/scripthelper.cpp
        ../../../../add_on/scriptmath/scriptmath.cpp
        ../../../../add_on/scriptmath/scriptmathcomplex.cpp
        ../../../../add_on/scriptstdstring/scriptstdstring.cpp
        ../../../../add_on/scriptstdstring/scriptstdstring_utils.cpp
        ../../../../add_on/serializer/serializer.cpp
        )
target_link_libraries(test_performance Angelscript asaot)
set_target_properties(test_performance PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/../../bin)
