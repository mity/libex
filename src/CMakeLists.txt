
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")


set(SOURCES
    buffer.c ../include/ex/buffer.h
    cmdline.c ../include/ex/cmdline.h
    crc32.c ../include/ex/crc32.h
    fnv1a_32.c ../include/ex/fnv1a_32.h
    fnv1a_64.c ../include/ex/fnv1a_64.h
    goodalloc.c ../include/ex/goodalloc.h
    rbtree.c ../include/ex/rbtree.h
    memchunk.c ../include/ex/memchunk.h
    murmur3_32.c ../include/ex/murmur3_32.h
)
if(WIN32)
    list(APPEND SOURCES memstream.c ../include/ex/memstream.h)
endif()


# The main libex library:
add_library(ex STATIC ${SOURCES})
target_include_directories(ex PRIVATE ../include/ex PUBLIC ../include)


# Special build of the library for the unit testing
# (This should NEVER be used for any other purposes as it bloats the library
# with additional symbols for unit testing, e.g. to check expected internal
# invariants or consistency of internally held structures):
if(PROJECT_IS_TOP_LEVEL)
    add_library(ex_test STATIC ${SOURCES})
    target_compile_definitions(ex_test PRIVATE EX_TEST)
    target_include_directories(ex_test PUBLIC ../include/ex)
endif()
