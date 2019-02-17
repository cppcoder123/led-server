//
//
//

#include <iostream>

#include "unix/timer-launch.hpp"

int main ()
{
  asio::io_context context;
  
  unix::timer_launch_t launch
    (true,
     [](){
      std::cout << "Starting...\n";
      return true;
    },
     [](){
       std::cout << "Stopping...\n";
     },
     context, 500);

  if (launch.start () == false)
    std::cout << "launch::start failed\n";

  try {
    context.run ();
  }
  catch (std::exception &e) {
    std::cout << "Error: " << e.what ();
  }
  
  return 0;
}
