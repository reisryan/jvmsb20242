#ifndef FRAMESTACK_H
#define FRAMESTACK_H

#include <iostream>
#include <vector>
#include <stack>
#include <unordered_map>
#include <stdexcept>
#include <cstdlib>
#include <cstdint>

class Frame {
public:
    std::vector<uint32_t> localVariables;
    std::stack<uint32_t> operandStack;

    Frame(int maxLocals);
    void setLocalVariable(int index, uint32_t value);
    uint32_t getLocalVariable(int index);
    void setLongOrDouble(int index, uint64_t value);
    uint64_t getLongOrDouble(int index);
    void pushOperand(uint32_t value);
    uint32_t popOperand();
};

class JVMStack {
private:
    std::stack<Frame*> stack;

public:
    Frame* pushFrame(int maxLocals, int thisObject = -1, bool isStatic = false);
    Frame* popFrame();
    Frame* currentFrame();
    bool isEmpty();
    ~JVMStack();
};

#endif // FRAMESTACK_H
