# ![TEMPUS](doc/images/project-logo-wide.png)

timer stuff


### tempus::timer_queue

The _timer_queue_ implementation is sourced from http://www.crazygaze.com/blog/2016/03/24/portable-c-timer-queue/

#### Usage

~~~{.cpp}
using clock = std::chrono::high_resolution_clock;

tempus::timer_queue tiqu;

tiqu.add(
  100,                                        // timeout in milliseconds
  [auto start = clock::now()](bool aborted)   // handler function
  {
    std::cout
      << "aborted: " << aborted << std::endl
      << "elapsed: " << elapsed(start, clock::now()) << " ms" << std::endl;
  }
);

tiqu.add(
  1000,                                       // timeout in milliseconds
  [auto start = clock::now()](bool aborted)   // handler function
  {
    std::cout
      << "aborted: " << aborted << std::endl
      << "elapsed: " << elapsed(start, clock::now()) << " ms" << std::endl;
  }
);

std::this_thread::sleep_for(std::chrono::milliseconds(500));

tiqu.cancel_all();
~~~
~~~
aborted: 0
elapsed: 100 ms
aborted: 1
elapsed: 500 ms
~~~
