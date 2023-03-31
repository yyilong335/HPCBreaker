# HPCBreaker
CS 6501 Hardware Security project to generate evasive attacks to HPC-based detectors

Hardware Performance Counter Breaker.

Modern microarchitectural attack detectors use performance counters to defend covert and side channel attacks. However, the performance counters can be polluted. We doubt if performance counter based detector is a good solution or not.

In this project, we are designing an automatic generator of malicious and benign programs to fool the detectors. We expect to see a low accuracy or high false-positive in recent performance counter based detectors like EVAX [EVAX][https://moghimi.org/papers/micro22-evax.pdf].