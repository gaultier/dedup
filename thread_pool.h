#pragma once

#include <pthread.h>

#include "utils.h"

typedef void (*thread_pool_worker_fn)(void*);
typedef struct {
    void* arg;
    thread_pool_worker_fn fn;
} thread_pool_work_item;

//
// API
//
typedef struct thread_pool thread_pool;
static void thread_pool_init(thread_pool* pool, usize count);
static void thread_pool_start(thread_pool* pool);
static void thread_pool_push(thread_pool* pool, thread_pool_work_item w);
static thread_pool_work_item* thread_pool_pop(thread_pool* pool);
static void thread_pool_finish(thread_pool* pool);
static void* thread_pool_worker(void* args);

//
// Implementation
//

struct thread_pool {
    pthread_t* threads;
    usize threads_count;
    thread_pool_work_item* work_items;
    usize work_items_count;
    usize work_items_capacity;
    isize work_item_i;
    pthread_mutex_t work_items_mtx;
    i32 end;
};

static thread_pool_work_item* thread_pool_pop(thread_pool* pool) {
    pthread_mutex_lock(&pool->work_items_mtx);

    pg_assert_int64(pool->work_item_i, >=, -1);
    pg_assert_int64(
        pool->work_item_i, <,
        (isize)__atomic_load_n(&pool->work_items_count, __ATOMIC_ACQUIRE));
    thread_pool_work_item* w =
        pool->work_item_i < (isize)(__atomic_load_n(&pool->work_items_count,
                                                    __ATOMIC_ACQUIRE) -
                                    1)
            ? &pool->work_items[++pool->work_item_i]
            : NULL;

    pthread_mutex_unlock(&pool->work_items_mtx);

    return w;
}

static void thread_pool_init(thread_pool* pool, usize count) {
    pool->threads = pg_malloc(count * sizeof(pthread_t));
    pool->threads_count = count;
    pool->work_items_capacity = 1000 * 1000;
    pool->work_items_count = 0;
    pool->work_items =
        pg_malloc(pool->work_items_capacity * sizeof(thread_pool_work_item));
    pool->work_item_i = -1;
    pthread_mutex_init(&pool->work_items_mtx, NULL);
    pool->end = false;
}

static void thread_pool_start(thread_pool* pool) {
    for (usize i = 0; i < pool->threads_count; i++) {
        pthread_create(&pool->threads[i], NULL, thread_pool_worker, pool);
    }
}

static void thread_pool_finish(thread_pool* pool) {
    __atomic_fetch_add(&pool->end, 1, __ATOMIC_ACQUIRE);

    for (usize i = 0; i < pool->threads_count; i++)
        pg_assert_int(pthread_join(pool->threads[i], NULL), ==, 0);
}

static void thread_pool_push(thread_pool* pool, thread_pool_work_item w) {
    pg_assert_uint64(__atomic_load_n(&pool->work_items_count, __ATOMIC_ACQUIRE),
                     <=, pool->work_items_capacity);

    memcpy(&pool->work_items[__atomic_load_n(&pool->work_items_count,
                                             __ATOMIC_ACQUIRE)],
           &w, sizeof(w));

    __atomic_fetch_add(&pool->work_items_count, 1, __ATOMIC_ACQUIRE);
}

static void* thread_pool_worker(void* args) {
    thread_pool* pool = args;
    thread_pool_work_item* w = NULL;

    while (true) {
        while ((w = thread_pool_pop(pool)) != NULL) {
            pg_assert_ptr((void*)w->fn, !=, NULL);
            pg_assert_ptr(w->arg, !=, NULL);
            w->fn(w->arg);
        }

        if (__atomic_load_n(&pool->end, __ATOMIC_ACQUIRE)) return NULL;
    }
}
