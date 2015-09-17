# Agree-Branch-Predictor
Agree Branch Predictor for gem5 Simulator
A Course Project for 'CS683:Advanced Computer Architecture'

#Problem Statement
To implement Agree Branch Predictor for gem5 simulator.[1]

#Project Description
gem5 Simulator has three types of branch predictors viz; 2BitLocal,Tournament and Bi-
mode. Our aim is to implement the Agree branch predictor in addition to existing branch
predictors and compare performance.


#Details:
The Agree branch predictor(here onwards Agree Predictor) reduces destructive aliasing
interference by reinterpreting the PHT counters as a direction agreement bit [3]
The architecture of the Agree predictor.
![architectureAgree](https://github.com/rameshgkwd05/Agree-Branch-Predictor/blob/master/architecture.jpg)
It works very similar to bi-mode branch predictor.In this some bits of branch address are taken as index into the Biasing bit table.Same bits are XORed with Global History Register bits. The result of XOR is used
as index to lookup PHT. The matching bits of PHT are then used for final prediction. If MSB of outcome of PHT is 1 then final prediction is same as biasing bit else it is invert of biasing bit. The biasing bits are initialized with the outcome of the first instance of the branch [2, Chapter 9].

We have implemented Agree Predictor in the similar way as explained in figure.We
have added 2 files agree.hh and agree.cc in the simulator for ARM ISA.


Performance Comparison:
Below chart compares the performance of Branch predictors in gem5 simulator.
![performanceChart](https://github.com/rameshgkwd05/Agree-Branch-Predictor/blob/master/chart.jpg)
















#References 
[1] Community. Gem5 simulator system. http://www.m5sim.org/Main_Page, 2014.

[2] John Paul Shen, M. H. L. Modern Processor Design: Fundamentals of Superscalar
Processors. Waveland Press, Inc, 2013.

[3] Sprangle, E., Chappell, R. S., Alsup, M., and Patt, Y. N. The agree pre-
dictor: A mechanism for reducing negative branch history interference. SIGARCH
Comput. Archit. News 25, 2 (May 1997), 284–291.
