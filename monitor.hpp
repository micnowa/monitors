//
// Created by Michał Nowaliński on 12/10/19.
//

#ifndef MONITORS_MONITOR_HPP
#define MONITORS_MONITOR_HPP

#include <utility>
#include <mutex>


template<class T>
class Monitor {
public:
    template<typename ...Args>
    Monitor(Args &&... args) : m_cl(std::forward<Args>(args)...) {}

    struct monitor_helper {
        monitor_helper(Monitor *mon) : m_mon(mon), m_ul(mon->m_lock) {}

        T *operator->() { return m_mon->m_cl; }

        Monitor *m_mon;
        std::unique_lock<std::mutex> m_ul;
    };

    monitor_helper operator->() { return monitor_helper(this); }

    monitor_helper ManuallyLock() { return monitor_helper(this); }

    T &GetThreadUnsafeAccess() { return m_cl; }

    void setMCl(T *mCl) {
        m_cl = mCl;
    }

private:
    T *m_cl;
    std::mutex m_lock;
};

#endif //MONITORS_MONITOR_HPP
