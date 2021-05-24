#pragma once

#include <mutex>
#include <condition_variable>
#include <iostream>

class Restaurant {

    enum class Status { idle, newOrder, ready };
    Status orderStatus = Status::idle;

    std::mutex order;
    std::condition_variable orderBell;

public:

    void chef() {

        std::unique_lock<std::mutex> ul(order);
        orderBell.wait(ul, [=]() { return orderStatus == Status::newOrder; });
        //приготовление блюд из заказа
        orderStatus = Status::ready;
        orderBell.notify_one();

    }

    void waiter() {
        {
            std::lock_guard<std::mutex> lg(order);
            orderStatus = Status::newOrder;
            orderBell.notify_one();
        } // lg вне области видимости = order.unlock()

        std::unique_lock<std::mutex> ul(order);

        orderBell.wait(ul, [=]() { return orderStatus == Status::ready; });

        orderStatus = Status::idle;

        ul.unlock();
        //приносят заказ
    }

};