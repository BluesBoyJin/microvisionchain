// Copyright (c) 2009-2022 The Bitcoin Core developers
// Copyright (c) 2021-2024 The MVC developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#pragma once

#include "task.h"
#include "threadpool.h"

#include <future>
#include <type_traits>

// Helper method to create task with a specified priority.
template<typename ThreadPool, typename Priority, typename Callable, typename... Args>
auto make_task(ThreadPool& pool, Priority priority, Callable&& call, Args&&... args)
    -> std::future<typename std::result_of<Callable(Args...)>::type>
{
    CTask task { priority };
    auto future { task.injectTask(std::forward<Callable>(call), std::forward<Args>(args)...) };
    pool.submit(std::move(task));
    return future;
}

// Helper method to create a default priority task.
template<typename ThreadPool, typename Callable, typename... Args>
auto make_task(ThreadPool& pool, Callable&& call, Args&&... args)
    -> std::future<typename std::result_of<Callable(Args...)>::type>
{
    // Default to medium priority
    CTask::Priority priority { CTask::Priority::Medium };
    return make_task(pool, priority, std::forward<Callable>(call), std::forward<Args>(args)...);
}
