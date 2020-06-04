# Yet Another 6052 Emulator

There are many like it but this one is mine. Written by Theo Cooper

## Architechture

The ```CPU_6502``` class extends the ```MemoryInterface``` class which is a wrapper for any device/object that accesses the virtual memory bus.

The virtual memory bus is implemented in the ```MemoryMapper``` class which at its most base form is essentially a pointer to a 64k byte array.

The fetch cycle of the CPU entails decoding the opcode and using the information of the addressing mode to construct the parameters that will be passed to the instruction. The instructions are written as lamda functions in Operations.cpp which are then put into an array which is used to map the instructions to the actual function pointers. The program counter is then incremented and the instruction run.

The overall architecture of the system was built with flexibility and modularity in mind. It isn't strictly necessary to develop such a complex system by which the CPU accesses its memory. But by routing everything through a memory map and by constructing a special runtime enviorment class to house of of the necessary components for a larger system, the overall implementation becomes very modular.

It would be a logical next step in this project to leverage its modularity to program an emulator for an NES, which runs on a 6502 cpu.