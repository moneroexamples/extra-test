cmake_minimum_required(VERSION 2.8)

set(PROJECT_NAME
        extratest)

project(${PROJECT_NAME})

set(CMAKE_CXX_FLAGS
        "${CMAKE_CXX_FLAGS} -std=c++11")

set(CMAKE_INSTALL_PREFIX
        /opt/bitmonero/)

# find boost
find_package(Boost COMPONENTS
        system
        filesystem
        thread
        date_time
        chrono
        regex
        serialization
        program_options
        date_time
        REQUIRED)

# include boost headers
include_directories(${Boost_INCLUDE_DIRS})


# set location of monero static libraries
set(MONERO_LIBS_DIR
        /opt/bitmonero-dev/libs)

# set location of moneroheaders
set(MONERO_HEADERS_DIR
        /opt/bitmonero-dev/headers)

# include monero headers
include_directories(
        ${MONERO_HEADERS_DIR}/src
        ${MONERO_HEADERS_DIR}/external
        ${MONERO_HEADERS_DIR}/contrib/epee/include
        ${MONERO_HEADERS_DIR}/external/db_drivers/liblmdb)



# get individual monero static libraries
# that are needed in this project

add_library(common STATIC IMPORTED)
set_property(TARGET common PROPERTY IMPORTED_LOCATION ${MONERO_LIBS_DIR}/libcommon.a)


add_library(blocks STATIC IMPORTED)
set_property(TARGET blocks PROPERTY IMPORTED_LOCATION ${MONERO_LIBS_DIR}/libblocks.a)


add_library(crypto STATIC IMPORTED)
set_property(TARGET crypto
        PROPERTY IMPORTED_LOCATION ${MONERO_LIBS_DIR}/libcrypto.a)


add_library(cryptonote_core STATIC IMPORTED)
set_property(TARGET cryptonote_core
        PROPERTY IMPORTED_LOCATION ${MONERO_LIBS_DIR}/libcryptonote_core.a)


add_library(blockchain_db STATIC IMPORTED)
set_property(TARGET blockchain_db
        PROPERTY IMPORTED_LOCATION ${MONERO_LIBS_DIR}/libblockchain_db.a)

add_library(lmdb STATIC IMPORTED)
set_property(TARGET lmdb
        PROPERTY IMPORTED_LOCATION ${MONERO_LIBS_DIR}/liblmdb.a)

# add src/ subfolder
add_subdirectory(ext/)

# add src/ subfolder
add_subdirectory(src/)

# speficie source files
set(SOURCE_FILES
        main.cpp)

# make executable called xmreg01
add_executable(${PROJECT_NAME}
        ${SOURCE_FILES})


# link our exexutable xmreg01 against
# libraries
target_link_libraries(${PROJECT_NAME}
        myxrm
        myext
        cryptonote_core
        blockchain_db
        crypto
        blocks
        common
        lmdb
        ${Boost_LIBRARIES}
        pthread
        unbound)

install(TARGETS ${PROJECT_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_PREFIX})