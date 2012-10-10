# struct CMake script
# by analyst004
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(struct)

SET(SRC_LIST
avltree.cpp
hashtable.cpp
slist.cpp
)

#static library
ADD_LIBRARY(${PROJECT_NAME}_static STATIC ${SRC_LIST})
SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES OUTPUT_NAME "${PROJECT_NAME}" PREFIX "lib")
SET_TARGET_PROPERTIES(${PROJECT_NAME}_static PROPERTIES CLEAN_DIRECT_OUTPUT 1)

#install
INSTALL(TARGETS ${PROJECT_NAME}_static ARCHIVE DESTINATION lib)
INSTALL(FILES avltree.h DESTINATION include/struct)
INSTALL(FILES hashtable.h DESTINATION include/struct)
INSTALL(FILES slist.h DESTINATION include/struct)






