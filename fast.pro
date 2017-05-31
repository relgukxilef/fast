TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES +=

HEADERS += \
    source/atomic/atomic_push_queue.h \
    source/atomic/atomic_unique_ptr.h \
    source/threading/inter_thread_queue.h

test {
    SOURCES += test/main.cpp
    HEADERS += \
        test/atomic/atomic_push_queue_test.h \
        test/atomic/atomic_unique_ptr_test.h \
        test/threading/inter_thread_queue_test.h

    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -lgcov --coverage
} else {
    SOURCES += source/main.cpp
}
