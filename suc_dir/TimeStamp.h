//
// Created by Yunan Chen on 2023/1/10.
//

#ifndef THREADPOOL_TIMESTAMP_H
#define THREADPOOL_TIMESTAMP_H
//#include <concepts>
#include <sys/time.h>
#include <string>
class Timestamp {
public:
    Timestamp() : microSecondsSinceEpoch_(0) {}
    explicit Timestamp(int64_t m) : microSecondsSinceEpoch_(m) {}

    static Timestamp now();

    void swap(Timestamp& t) {
        std::swap(microSecondsSinceEpoch_, t.microSecondsSinceEpoch_);
    }
    // std::string toString() const;
    // std::string toFormattedString();

    bool valid() const { return microSecondsSinceEpoch_ > 0; }
    time_t secondsSinceEpoch() const {
        return static_cast<time_t>(microSecondsSinceEpoch_ / kMicroSecondsPerSecond);
    }

    int64_t microSecondsSinceEpoch() {
        return microSecondsSinceEpoch_;
    }

    static Timestamp invalid() {
        return Timestamp();
    }

    static Timestamp fromUnixTime(time_t t) {
        return fromUnixTime(t, 0);
    }

    static Timestamp fromUnixTime(time_t t, int microseconds) {
        return Timestamp(static_cast<int64_t>(t) * kMicroSecondsPerSecond + microseconds);
    }

    static const int kMicroSecondsPerSecond = 1000 * 1000;

private:
    int64_t microSecondsSinceEpoch_;
};

inline bool operator<(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator<=(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() < rhs.microSecondsSinceEpoch();
}

inline bool operator>(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
}

inline bool operator>=(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() > rhs.microSecondsSinceEpoch();
}

inline bool operator==(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() == rhs.microSecondsSinceEpoch();
}

inline bool operator!=(Timestamp lhs, Timestamp rhs) {
    return lhs.microSecondsSinceEpoch() != rhs.microSecondsSinceEpoch();
}

inline Timestamp addTime(Timestamp timestamp, double seconds) {
    int64_t delta = static_cast<int64_t>(seconds * Timestamp::kMicroSecondsPerSecond);
    return Timestamp(timestamp.microSecondsSinceEpoch() + delta);
}

#endif //THREADPOOL_TIMESTAMP_H
