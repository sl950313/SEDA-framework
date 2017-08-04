# SEDA_framework

Author : Lei Shi

This is C++ implement of SEDA framework. EveryOne can use our programming framework to implement a high concurrency system easily.

A http server is a example for the use of SEDA framework. It may give you help. the SEDA can be found at : The Staged Event-Driven Architecture for Highly-Concurrent Server Applications

The advantages of SEDA:
   1. The SEDA makes the deployment of architecture easier. 
   2. TODO

The disadvantages:
   1. TODO

Our Architecture and Technology implement:

1.	Stage :


```
                                    
                   ---------------------------------------------
                   |                                           |
   ------------    |  |---------------|      |---------------| |    
   |    MSG   |------->    receiver   ------>| stage-handler | |
   ------------    |  |---------------|      |---------------| |  
                   |          ^                      ^         |
                   |          |                      |         |
                   |          |                      |         |
                   |  |---------------|      |---------------| |    -------
                   |  | stage-control |<---->|  worker-pool  ------>| MSG |
                   |  |---------------|      |---------------| |    -------
                   ---------------------------------------------
                   
```

2. Each function of module
--------------------------
i.    receiver:
      a. The receiver is a subsriber of message-queue.
      b. Receive msq from other stage and forwarding to the stage-handler.
      c. The receive is implemented by ZeroMQ.

ii.   stage-handler:
      a. The logic of handling massages and the producer of message-queue, the excute units are in worker-pool.

iii.  worker-pool:
      a. A worker-pool which can be implement as thread-based or processor-based worker-pool.
      b. The thread-based worker-pool is implemented by mutex-based and no-mutex-based.
      c. The difference between different types of worker-pool should be done during the following days.

iv.   stage-control:
      a. Collecting infomation from receiver and worker-pool, then dynamicly adjusting the queue-num and worker-num.
      b. But Using which method to adjust the load is still under thinking.

-----------------------------------------------------------------------------
Following is older version of README.
This is a distibuted big data search system. 
  A http server implemented by SEDA is served as a front.
  A backend of searching implemented by SEDA.
  A module interacts with sparksql or presto.
  
  TODO:
