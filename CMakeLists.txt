# struct CMake script
# by analyst004
CMAKE_MINIMUM_REQUIRED(VERSION 2.4)
PROJECT(struct)

#SET(CAN_USE_ASSEMBLER TRUE)
#IF(MSVC)

#ENABLE_LANGUAGE(ASM_MASM)
#IF( "${CMAKE_SIZEOF_VOID_P}" STREQUAL "4" )
#SET(ASM_SRC cmpxchg_x86.asm)
#ELSE()
#SET(ASM_SRC cmpxchg_x64.asm)
#ENDIF()

#ENDIF(MSVC)

SET(SRC_LIST
avltree.cpp
hashtable.cpp
rbtree.c
rblist.cpp
strlist.cpp
intlist.cpp
)

#static library
ADD_LIBRARY(${PROJECT_NAME} STATIC ${SRC_LIST})
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES OUTPUT_NAME "${PROJECT_NAME}")
SET_TARGET_PROPERTIES(${PROJECT_NAME} PROPERTIES CLEAN_DIRECT_OUTPUT 1)

SET(EXAMPLE_SRC_LIST 
example.cpp)
add_executable(example ${EXAMPLE_SRC_LIST})
TARGET_LINK_LIBRARIES(example ${PROJECT_NAME})

#install
INSTALL(TARGETS ${PROJECT_NAME} 
	EXPORT struct
	ARCHIVE DESTINATION lib)
INSTALL(FILES avltree.h DESTINATION include/struct)
INSTALL(FILES hashtable.h DESTINATION include/struct)
INSTALL(FILES slist.h DESTINATION include/struct)
INSTALL(FILES hlist.h DESTINATION include/struct)
INSTALL(FILES dlist.h DESTINATION include/struct)
INSTALL(TARGETS example RUNTIME DESTINATION bin)






