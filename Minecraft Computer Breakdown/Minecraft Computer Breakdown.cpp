
#include "pch.h"
#include <iostream>
#include <bitset>

#include "data_types.h"
#include "ALU.h"
#include "registers.h"
#include "RAM.h"
#include "ROM.h"
#include "Instructions.h"

int main()
{
	Registers registers;

	const int instructions_count = 5;
	ISA::Instruction* instructions = new ISA::Instruction[instructions_count]{
		ISA::MOV(ISA::Resigters::EAX, U8(0)),
		ISA::MOV(ISA::Resigters::EDX, U8(1)),
		ISA::ADD(ISA::Resigters::EAX, ISA::Resigters::EDX),
		ISA::XCHG(ISA::Resigters::EAX, ISA::Resigters::EDX),
		ISA::JNO(2)
	};
	
	while (registers.EIP < instructions_count) {
		std::cout << "\nInstruction Pointer: " << registers.EIP << "";
		std::cout << "EAX: " << registers.read(ISA::Resigters::EAX) << " (" << std::bitset<32>(registers.read(ISA::Resigters::EAX)) << ")\n";
		std::cout << "EDX: " << registers.read(ISA::Resigters::EDX) << " (" << std::bitset<32>(registers.read(ISA::Resigters::EDX)) << ")\n";

		const ISA::Instruction* base_inst = instructions + registers.EIP;
		switch (base_inst->opcode)
		{
		case ISA::Opcodes::ADD:
		{
			const ISA::ADD* inst = static_cast<const ISA::ADD*>(base_inst);
			bit carry = 0;
			U32 res = ALU::add(registers.read(inst->destination), registers.read(inst->operand), carry);
			registers.write(inst->destination, res);
			break;
		}
		case ISA::Opcodes::MOV:
		{
			const ISA::MOV* inst = static_cast<const ISA::MOV*>(base_inst);
			registers.write(inst->destination, registers.read(inst->source));
			break;
		}
		case ISA::Opcodes::JO:
		{
			const ISA::JO* inst = static_cast<const ISA::JO*>(base_inst);
			if (registers.flag_read_OF()) {
				registers.EIP = inst->address;
				continue; // skip EIP increment
			}
			break;
		}
		case ISA::Opcodes::JNO:
		{
			const ISA::JNO* inst = static_cast<const ISA::JNO*>(base_inst);
			if (!registers.flag_read_OF()) {
				registers.EIP = inst->address;
				continue; // skip EIP increment
			}
			break;
		}
		case ISA::Opcodes::XCHG:
		{
			const ISA::XCHG* inst = static_cast<const ISA::XCHG*>(base_inst);
			U32 tmp = registers.read(inst->destination);
			registers.write(inst->destination, registers.read(inst->source));
			registers.write(inst->source, tmp);
			break;
		}

		default:
			break;
		}
		registers.EIP++;
	}

	std::cout << "\nFinal state: ";
	std::cout << "EAX: " << registers.read(ISA::Resigters::EAX) << " (" << std::bitset<32>(registers.read(ISA::Resigters::EAX)) << ")\n";
	std::cout << "EDX: " << registers.read(ISA::Resigters::EDX) << " (" << std::bitset<32>(registers.read(ISA::Resigters::EDX)) << ")\n";
}
