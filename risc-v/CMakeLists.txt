include_directories("${PROJECT_SOURCE_DIR}")


#--------
# RISC-V
#--------

install(FILES
    constant.h
    disasm.h
    load.h
    machine.h
    memory.h
    reg_file.h
    DESTINATION include/risc-v
    )

set(srcs
    disasm.cc
    load.cc
    machine.cc
    memory.cc
    reg_file.cc
    )

add_library(risc-v_core STATIC ${srcs})

set_target_properties(risc-v_core
    PROPERTIES OUTPUT_NAME "risc-v")

install(TARGETS risc-v_core DESTINATION lib)

add_executable(risc-v main.cc)

target_link_libraries(risc-v
    risc-v_core)
     

#------
# Test
#------

set(test_srcs
    disasm_test.cc
    load_test.cc
    machine_test.cc
    memory_test.cc
    reg_file_test.cc
    )

add_executable(risc-v_test ${test_srcs})

target_link_libraries(risc-v_test
    gtest_main
    risc-v_core
    )

add_test(NAME risc-v_test COMMAND risc-v_test)
