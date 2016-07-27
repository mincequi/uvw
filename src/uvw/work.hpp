#pragma once


#include <functional>
#include <utility>
#include <memory>
#include <uv.h>
#include "event.hpp"
#include "request.hpp"
#include "util.hpp"


namespace uvw {


struct WorkEvent: Event<WorkEvent> { };


class Work final: public Request<Work, uv_work_t> {
    static void workCallback(uv_work_t *req) {
        static_cast<Work*>(req->data)->task();
    }

    using Request::Request;

public:
    using Task = std::function<void(void)>;

    template<typename... Args>
    static std::shared_ptr<Work> create(Args&&... args) {
        return std::shared_ptr<Work>{new Work{std::forward<Args>(args)...}};
    }

    void queue(Task t) {
        if(0 == invoke(&uv_queue_work, parent(), get<uv_work_t>(), &workCallback, &defaultCallback<uv_work_t, WorkEvent>)) {
            task = std::move(t);
        }
    }

private:
    Task task{};
};


}
