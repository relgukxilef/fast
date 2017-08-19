#include <doctest.h>

#include "source/utility/observable.h"

TEST_SUITE("observable") {
    TEST_CASE("observable should store value") {
        fast::observable<int> value;

        value = 5;
        CHECK((int)value == 5);

        value = 10;
        CHECK((int)value == 10);
    }

    TEST_CASE("observable should notify observers on changes") {
        int callbacks_called = 0;

        fast::observable<int> value;

        fast::observer<int> observer1(
            [&callbacks_called] (const int& new_value) {
                CHECK(new_value == 5);
                callbacks_called++;
            }
        );
        fast::observer<int> observer2(
            [&callbacks_called] (const int& new_value) {
                CHECK(new_value == 5);
                callbacks_called++;
            }
        );

        observer1 = value;
        observer2 = value;

        value = 5;

        CHECK(callbacks_called == 2);
    }

    TEST_CASE("observable should not notify observers that have been removed") {
        bool callback_called = false;

        fast::observable<int> value1;
        fast::observable<int> value2;

        fast::observer<int> observer([&callback_called] (const int& new_value) {
            CHECK(new_value == 5);
            callback_called = true;
        });

        observer = value1;
        observer = value2;

        value1 = 5;

        CHECK(callback_called == false);
    }

    TEST_CASE("destroying an observer doesn't break the observable") {
        fast::observable<int> value;

        {
            fast::observer<int> observer([] (const int& new_value) {});

            observer = value;
        }

        value = 5;
    }
}

