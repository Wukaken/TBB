#pragma once

#include <tbb/tbb.h>

static void warmupTBB()
{
    tbb::parallel_for(0, tbb::task_scheduler_init::default_num_threads(),
                      [](int){
                          tbb::tick_count t0 = tbb::tick_count::now();
                          while((tbb::tick_count::now() - t0).seconds() < 0.01);
                      });
}