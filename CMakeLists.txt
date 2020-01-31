cmake_minimum_required(VERSION 3.10)
project(Marketplace)

set(CMAKE_C_FLAGS "-pthread")
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#cartella del progetto
set(MARKETPLACE ${CMAKE_SOURCE_DIR})

#cartella delle librerie
link_directories(${MARKETPLACE}/lib)
set(LIBRARY_OUTPUT_PATH ${MARKETPLACE}/lib/static)

#cartella degli header
include_directories(${MARKETPLACE}/include)

#cartella dove creare gli eseguibili
set(CMAKE_BINARY_DIR ${MARKETPLACE}/bin)
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})

#aggiunta di tutti i file in lib/ come librerie
AUX_SOURCE_DIRECTORY(${MARKETPLACE}/lib LIBS_FILES)
add_library(libraries ${LIBS_FILES})

#compilazione dei sorgenti
add_executable(serverM ${MARKETPLACE}/src/serverM/main.c ${LIBS_FILES})

add_executable(serverN ${MARKETPLACE}/src/serverN/main.c ${LIBS_FILES})

add_executable(serverC ${MARKETPLACE}/src/serverC/main.c ${LIBS_FILES})

set(LIB_LOGIN ${MARKETPLACE}/src/clientN/login.c)
add_executable(clientN ${MARKETPLACE}/src/clientN/main.c ${LIB_LOGIN} ${LIBS_FILES})

set(LIB_ACQUISTO ${MARKETPLACE}/src/client/acquisto.c)
add_executable(client ${MARKETPLACE}/src/client/main.c ${LIB_ACQUISTO} ${LIBS_FILES})
