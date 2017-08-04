# SEDA_framework

Author : Lei Shi

------------
##Introduction:

This is C++ implement of SEDA framework. EveryOne can use our programming framework to implement a high concurrency system easily.

A http server is a example for the use of SEDA framework. It may give you help. the SEDA can be found at : The Staged Event-Driven Architecture for Highly-Concurrent Server Applications

The advantages of SEDA:
   
   - The SEDA makes the deployment of architecture easier. developors will only focus on the logical function of your own system.
   
   - The SEDA makes full use of system resources with flexible thread control.

The disadvantages:

   - NULL

-----------

##Our architecture and technology implement:

* Stage :


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

*   Each function of module
   -  receiver:
      - The receiver is a subsriber of message-queue.
      - Receive msq from other stage and forwarding to the stage-handler.
      - The receive is implemented by ZeroMQ.

   -  stage-handler:
      - The logic of handling massages and the producer of message-queue, the excute units are in worker-pool.

   -  worker-pool:
      - A worker-pool which can be implement as thread-based or processor-based worker-pool.
      - The thread-based worker-pool is implemented by mutex-based and no-mutex-based.
      - The difference between different types of worker-pool should be done during the following days.

   -  stage-control:
      - Collecting infomation from receiver and worker-pool, then dynamicly adjusting the queue-num and worker-num.
      -  But Using which method to adjust the load is still under thinking.

-----------------------------------------------------------------------------
Following is older version of README.

This is a distibuted big data search system. 
  
&ensp;&ensp;  A http server implemented by SEDA is served as a front. A backend of searching implemented by SEDA. A module interacts with sparksql or presto.
  
