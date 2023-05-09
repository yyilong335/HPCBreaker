# HPCBreaker
CS 6501 Hardware Security project to generate evasive attacks to HPC-based detectors

Hardware Performance Counter Breaker.

Modern microarchitectural attack detectors use performance counters to defend covert and side channel attacks. However, the performance counters can be polluted. We doubt if performance counter based detector is a good solution or not.

In this project, we are designing an automatic generator of malicious and benign programs to fool the detectors. We expect to see a low accuracy or high false-positive in recent performance counter based detectors like [EVAX](https://moghimi.org/papers/micro22-evax.pdf).

We probabily going to choose [CloudShield](https://github.com/zechenghe/CloudShield/tree/main) as the target detector. It uses features like:

- Instruction

- - INST_RETIRED.ANY_P
- - EventSel=C0H, UMask=00H,Architectural
- - Number ofinstructions atretirement.

- Stall during issue

- Stall during retirement

- Cycles

- Load

- DTLB read

- Store

- BPU read

- DTLB write

- Branch

- L1D read miss

- L1I read miss

- Context switch