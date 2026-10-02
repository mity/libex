
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -DDEBUG")


set(SOURCES
    buffer.c        buffer.h
    cmdline.c       cmdline.h
    crc32.c         crc32.h
    fnv1a_32.c      fnv1a_32.h
    fnv1a_64.c      fnv1a_64.h
    goodalloc.c     goodalloc.h
    rbtree.c        rbtree.h
    memchunk.c      memchunk.h
    murmur3_32.c    murmur3_32.h
)
if(WIN32)
    list(APPEND SOURCES memstream.c memstream.h)
endif()


# The main libex library:
add_library(ex STATIC ${SOURCES})
target_include_directories(ex PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>"
    "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

add_library(libex ALIAS ex)


# Special build of the library for the unit testing
# (This should NEVER be used for any other purposes as it bloats the library
# with additional symbols for unit testing, e.g. to check expected internal
# invariants or consistency of internally held structures):
if(PROJECT_IS_TOP_LEVEL)
    add_library(ex_test STATIC ${SOURCES})
    target_compile_definitions(ex_test PRIVATE EX_TEST)
    target_include_directories(ex_test PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/src>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
    )

    add_library(libex_test ALIAS ex_test)
endif()
