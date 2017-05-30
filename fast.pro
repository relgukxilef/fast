TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES +=

HEADERS += \
    source/atomic/atomic_push_queue.h \
    source/atomic/atomic_unique_ptr.h \

test {
    SOURCES += test/main.cpp
    HEADERS += \
        test/atomic/atomic_push_queue_test.h \
        test/atomic/atomic_unique_ptr_test.h
} else {
    SOURCES += source/main.cpp
}
