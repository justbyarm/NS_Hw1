#pragma once

class InstanceTracker {
public:
    inline static int active_instances = 0;

    int value = 0;

    InstanceTracker() {
        ++active_instances;
    }

    explicit InstanceTracker(int v) : value(v) {
        ++active_instances;
    }

    InstanceTracker(const InstanceTracker& other)
            : value(other.value) {
        ++active_instances;
    }

    InstanceTracker& operator=(const InstanceTracker& other) {
        if (this != &other) {
            value = other.value;
        }
        return *this;
    }

    ~InstanceTracker() {
        --active_instances;
    }
};