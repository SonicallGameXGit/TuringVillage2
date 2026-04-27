#pragma once
#include <utility>

template <typename T, auto Deleter>
class Resource {
private:
    T handle;
public:
    explicit Resource(T handle) : handle(handle) {}
    ~Resource() {
        if (this->handle) {
            Deleter(this->handle);
        }
    }
    Resource(Resource &&other) noexcept : handle(std::exchange(other.handle, {})) {}
    Resource &operator=(Resource &&other) noexcept {
        if (this != &other) {
            if (this->handle) {
                Deleter(this->handle);
            }
            this->handle = std::exchange(other.handle, {});
        }
        return *this;
    }
    Resource(const Resource&) = delete;
    Resource &operator=(const Resource&) = delete;

    T get() const {
        return handle;
    }
};