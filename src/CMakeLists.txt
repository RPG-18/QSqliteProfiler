set(HEADERS Log.h)
set(SRC Log.cpp)

add_library(QSqliteProfiler STATIC ${SRC} ${HEADERS})
target_link_libraries(QSqliteProfiler Qt5::Core Qt5::Sql sqlite3)

add_executable(Log2db ${HEADERS} Log2db.cpp)
target_link_libraries(Log2db Qt5::Core Qt5::Sql)