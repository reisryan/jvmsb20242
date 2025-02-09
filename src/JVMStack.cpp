#include "JVMStack.hpp"

Frame* JVMStack::pushFrame(int maxLocals, void* constantPool, uint32_t returnAddress, int thisObject, bool isStatic) {
    Frame* frame = new Frame(maxLocals, maxLocals, constantPool); // Usando maxLocals também para maxStack

    if (maxLocals > 32) {
        std::cerr << "StackOverflowError: stack limit exceeded" << std::endl;
        exit(1);
    }

    frame->setReturnAddress(returnAddress);

    if (!isStatic && thisObject != -1) {
        frame->setLocalVariable(0, static_cast<uint32_t>(thisObject));
    }

    stack.push(frame);
    return frame;
}


Frame* JVMStack::popFrame() {
    if (stack.empty()) {
        std::cerr << "[ERRO] Nenhum frame disponível para pop!" << std::endl;
        exit(1);
    }
    Frame* frame = stack.top();
    stack.pop();
    return frame;
}

Frame* JVMStack::currentFrame() {
    if (stack.empty()) {
        std::cerr << "[ERRO] Tentativa de acessar frame atual em pilha vazia!" << std::endl;
        exit(1);
    }
    return stack.top();
}

bool JVMStack::isEmpty() {
    return stack.empty();
}

JVMStack::~JVMStack() {
    while (!stack.empty()) {
        delete stack.top();
        stack.pop();
    }
}