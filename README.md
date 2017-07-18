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

                                    A stage :
                   ---------------------------------------------
                   |                         |---------------| |
                   |                         | stage-control | |
                   |                         |---------------| |
                   |                                 ^         |
                   |                                 |         |
                   |                                 |         |
   |----------|    |  |---------------|      |---------------| |    |-------|
   |    MSG   |-------- Queue(ZeroMQ) ------>| stage-handler ------>|  MSG  |
   |----------|    |  |---------------|      |-------|-------| |    |-------|
                   |                                 ^         |
                   |                                 |         |
                   |                                 |         |
                   |                         |---------------| |
                   |                         | worker-pool   | |
                   |                         |---------------| |
                   ---------------------------------------------

------------
Following is older version of README.
This is a distibuted big data search system. 
  A http server implemented by SEDA is served as a front.
  A backend of searching implemented by SEDA.
  A module interacts with sparksql or presto.
  
  TODO:
