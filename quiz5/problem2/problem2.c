/*
 * @file    - problem2.c
 * @brief   - hazard pointer 實作 lock-free
 */

#include <assert.h>
#include <err.h>
#include <pthread.h>
#include <stdio.h>
#include "hp.h"
#include "domain.h"

#define N_READERS 1
#define N_WRITERS 1
#define N_ITERS 20
#define ARRAY_SIZE(x) sizeof(x) / sizeof(*x)

typedef struct {
    unsigned int v1, v2, v3;
} config_t;

static config_t *shared_config;
static domain_t *config_dom;

config_t *create_config()
{
    config_t *out = calloc(1, sizeof(config_t));
    if (!out)
        err(EXIT_FAILURE, "calloc");
    return out;
}

void delete_config(config_t *conf)
{
    assert(conf);
    free(conf);
}

static void print_config(const char *name, const config_t *conf)
{
    printf("%s : { 0x%08x, 0x%08x, 0x%08x }\n", name, conf->v1, conf->v2,
           conf->v3);
}

void init()
{
    shared_config = create_config();
    config_dom = domain_new(delete_config);
    if (!config_dom)
        err(EXIT_FAILURE, "domain_new");
}

void deinit()
{
    delete_config(shared_config);
    domain_free(config_dom);
}

static void *reader_thread(void *arg)
{
    (void) arg;

    for (int i = 0; i < N_ITERS; ++i) {
        config_t *safe_config =
            (config_t *) load(config_dom, (uintptr_t *) &shared_config);
        if (!safe_config)
            err(EXIT_FAILURE, "load");

        print_config("read config    ", safe_config);
        drop(config_dom, (uintptr_t) safe_config);
    }

    return NULL;
}

static void *writer_thread(void *arg)
{
    (void) arg;

    for (int i = 0; i < N_ITERS / 2; ++i) {
        config_t *new_config = create_config();
        new_config->v1 = rand();
        new_config->v2 = rand();
        new_config->v3 = rand();
        print_config("updating config", new_config);

        swap(config_dom, (uintptr_t *) &shared_config, (uintptr_t) new_config,
             0);
        print_config("updated config ", new_config);
    }

    return NULL;
}

int main()
{
    pthread_t readers[N_READERS], writers[N_WRITERS];

    init();

    /* Start threads */
    for (size_t i = 0; i < ARRAY_SIZE(readers); ++i) {
        if (pthread_create(readers + i, NULL, reader_thread, NULL))
            warn("pthread_create");
    }
    for (size_t i = 0; i < ARRAY_SIZE(writers); ++i) {
        if (pthread_create(writers + i, NULL, writer_thread, NULL))
            warn("pthread_create");
    }

    /* Wait for threads to finish */
    for (size_t i = 0; i < ARRAY_SIZE(readers); ++i) {
        if (pthread_join(readers[i], NULL))
            warn("pthread_join");
    }
    for (size_t i = 0; i < ARRAY_SIZE(writers); ++i) {
        if (pthread_join(writers[i], NULL))
            warn("pthread_join");
    }

    deinit();

    return EXIT_SUCCESS;
}