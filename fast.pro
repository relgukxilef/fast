TEMPLATE = app
CONFIG += console c++14 Wall
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS += -std=c++14 # config += c++14 doesn't seem to work for clang

SOURCES += \
    source/events/event.cpp \
    source/events/event_loop.cpp \
    source/threading/semaphore.cpp

HEADERS += \
    source/atomic/atomic_push_queue.h \
    source/atomic/atomic_unique_ptr.h \
    source/threading/inter_thread_queue.h \
    source/collections/span.h \
    source/collections/arrays.h \
    source/collections/tuple.h \
    source/events/event.h \
    source/events/event_loop.h \
    source/threading/semaphore.h

test {
    SOURCES += test/main.cpp
    HEADERS += \
        test/atomic/atomic_push_queue_test.h \
        test/atomic/atomic_unique_ptr_test.h \
        test/threading/inter_thread_queue_test.h \
        test/collections/span_test.h \
        test/collections/arrays_test.h \
        test/threading/semaphore_test.h \
        test/events/event_loop_test.h

    QMAKE_CXXFLAGS += -fprofile-arcs -ftest-coverage
    QMAKE_LFLAGS += -lgcov --coverage
} else {
    SOURCES += source/main.cpp
}
