include_directories(${DYNINST_INCLUDE_DIR})

set(CALIPER_INSTLOOKUP_SOURCES
  InstLookup.cpp)

add_library(caliper-instlookup OBJECT ${CALIPER_INSTLOOKUP_SOURCES})

add_service_objlib("caliper-instlookup")
add_caliper_service("instlookup CALIPER_HAVE_DYNINST")
