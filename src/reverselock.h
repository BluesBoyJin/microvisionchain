// Copyright (c) 2021-2024 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#ifndef MVC_REVERSELOCK_H
#define MVC_REVERSELOCK_H

/**
 * An RAII-style reverse lock. Unlocks on construction and locks on destruction.
 */
template <typename Lock> class reverse_lock {
public:
    explicit reverse_lock(Lock &_lock) : lock(_lock) {
        _lock.unlock();
        _lock.swap(templock);
    }

    ~reverse_lock() {
        templock.lock();
        templock.swap(lock);
    }

private:
    reverse_lock(reverse_lock const &);
    reverse_lock &operator=(reverse_lock const &);

    Lock &lock;
    Lock templock;
};

#endif // MVC_REVERSELOCK_H
