#pragma once

#include "Operations.hpp"
#include "CPU.hpp"

//TODO: possibly implement register manipulation via function calls instead of direct

/* FLAG OPERATIONS */
void setFlag(CPU_6502* c, flag_t flag, bool val)
{
	c->setReg(STATUS,(c->getReg(STATUS) & ~(1 << flag)) | (val << flag));
}

bool getFlag(CPU_6502* c, flag_t flag)
{
	uint8_t status = c->getReg(STATUS);
	bool flagVal = status >> flag & 0x01; //TODO: TESTME this is sketch
	return flagVal;
}

void setFlags(CPU_6502* c, uint8_t status)
{
	for (int i = CARRY; i != NEGATIVE; i++)
	{
		flag_t flag = static_cast<Flag>(i);
		bool flag_val = status >> flag & 0x01; //TODO: TESTME
		setFlag(c, flag, flag_val);
	}
	
}

// set Carry sign if it its over 8 bits
void setCarry(CPU_6502* c, int16_t val)
{
	setFlag(c, CARRY, val > 0xFF);
}

// set Carry sign in Decimal mode
void setCarryBCD(CPU_6502* c, int16_t val)
{
	setFlag(c, CARRY, val > 0x99);
}


// NOTE: these two are basically black magic
// setting the overflow flag for twos complement
void setOverflow(CPU_6502* c, int8_t a, int8_t b, int8_t val) {
	//sets overflow if overflow in twos complement 
	//occurred when adding a and b to get val
	//this bit twiddling from:
	//http://nesdev.com/6502.txt
	int8_t overflow = !((a ^ b) & 0x80) && ((a ^ val) & 0x80);
	//overflow = signs of operands are the same AND
	//          sign of result not equal to sign of operands
	setFlag(c, OVRFLW, overflow);
}

// setting overflow flag for twos complement
void setOverflowSubtract(CPU_6502* c, int8_t a, int8_t b, int8_t val) {
	//sets overflow if overflow in twos complement 
	//occurred when subtracting b from a to get val
	//this bit twiddling from:
	//http://nesdev.com/6502.txt
	int8_t overflow = ((a ^ b) & 0x80) && ((a ^ val) & 0x80);
	//overflow = signs of operands are the same AND
	//          sign of result not equal to sign of operands
	setFlag(c, OVRFLW, overflow);
}

// set Zero flag if val is zero
void setZero(CPU_6502* c, int8_t val)
{
	setFlag(c, ZERO, (val ? 0 : 1));
}

// sets the Negative flag based on sign of int
void setSign(CPU_6502* c, int8_t val)
{
	//sets sign flag equal to sign
	//of bit 7 of val
	int8_t sign = val & 0x80 ? 1 : 0;
	setFlag(c, NEGATIVE, sign); 
}



/* STACK OPERATIONS */
// push an operand onto the stack
void PUSH(CPU_6502* c, int8_t operand) {
	uint8_t stackVal = c->getReg(STACK);
	uint16_t address = 0x0100 | stackVal;
	//0x0100 is hardcoded as stack page
	//lives in 0x0100 to 0x01FF page of mem
	c->write(address, operand);
	uint8_t newStackVal = stackVal - 1;
	c->setReg(STACK, newStackVal);
}

// pull value from stack in memory
char PULL(CPU_6502* c) {
	uint8_t stackVal = c->getReg(STACK);
	uint8_t newStackVal = stackVal + 1;
	c->setReg(STACK, newStackVal);
	//0x0100 is hardcoded as stack page
	//lives in 0x0100 to 0x01FF page of mem
	uint16_t address = 0x0100 | newStackVal;
	return c->read(address);
}

/* OPCODE IMPLEMENTATIONS */ 
// Add with carry: adds memory value with accumulator 
std::function<void(CPU_6502* c, op_code_params* o)> adc = [](CPU_6502 *c, op_code_params* o) -> void
{
	int8_t carry = getFlag(c, CARRY);
	int8_t accum = c->getReg(ACCUM);
	int8_t operand = o->operand;
	int16_t sum = (0x00FF & carry) + (0x00FF & accum) + (0x00FF & operand);
	int8_t sumByte = sum & 0x00FF;
	
	setZero(c, sumByte); // set zero flag if zero

	// Black Magic from https://github.com/dennis-chen/6502-Emu/blob/master/src/cpu.c
	if (c->getReg(STATUS) & DECIMAL_MODE) { //in decimal mode
		//if lower 4 bits of operands plus
		//the carry in are larger than 9,
		//then we need to apply conversions
		//to remain in binary coded decimal format.
		if ((accum & 0xF) + (operand & 0xF)
			+ carry > 9) {
			sum += 6;
		}
		setSign(c, sum & 0xFF); // set the sign 
		setOverflow(c, accum, operand, sum & 0xFF);
		//if the higher bits aren't in
		//BCD format we need to add 96 to convert.
		//Black magic from http://nesdev.com/6502.txt
		sum += sum > 0x99 ? 96 : 0;
		setCarryBCD(c, sum);
	}
	else {
		setSign(c, sumByte);
		setOverflow(c, accum, operand, sumByte);
		setCarry(c, sum);
	}
	c->setReg(ACCUM,sum & 0xFF);
};

// And memory with accumulator // NOTE: named and_ instead of and b/c cpp has 'and' reserved for some reason 
std::function<void(CPU_6502* c, op_code_params* o)> and_ = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t res = c->getReg(ACCUM) & o->operand;
	setSign(c, res);
	setZero(c, res);
	c->setReg(ACCUM, res);
};

// Arithmetic shift left
std::function<void(CPU_6502* c, op_code_params* o)> asl = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t operand = o->operand;
	int16_t res = (0x00FF & operand) << 1;
	int8_t resByte = res & 0x00FF;
	setCarry(c, resByte);
	setSign(c, resByte);
	setZero(c, resByte);

	if(o->mode == Accum_mode)
	{
		c->setReg(ACCUM, resByte);
	}
	else
	{
		c->write(o->address, resByte);
	}

};

// Branch if carry clear
std::function<void(CPU_6502* c, op_code_params* o)> bcc = [](CPU_6502 * c, op_code_params* o) -> void
{
	if(!getFlag(c, CARRY))
	{
		c->setPc(o->address);

		//add cycles
	}
};

// Branch if carry set
std::function<void(CPU_6502* c, op_code_params* o)> bcs = [](CPU_6502 * c, op_code_params* o) -> void
{
	if(getFlag(c, CARRY))
	{
		c->Pc = o->address;
	}
};

// Branch if equals aka branch if zero flag
std::function<void(CPU_6502* c, op_code_params* o)> beq = [](CPU_6502 * c, op_code_params* o) -> void
{
	if(getFlag(c, ZERO))
	{
		c->Pc = o->address;
	}
};

// Test bits in memory with accumulator
std::function<void(CPU_6502* c, op_code_params* o)> bit = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t src = o->operand;
	int8_t accum = c->getReg(ACCUM);
	setFlag(c, OVRFLW, (src & 0x40) ? 1 : 0); // get 6th bit of src
	setFlag(c, NEGATIVE, (src & 0x80) ? 1 : 0); // get 7th bit of src
	setFlag(c, ZERO, (src& accum) ? 0 : 1);
};

// Branch if minus
std::function<void(CPU_6502* c, op_code_params* o)> bmi = [](CPU_6502 * c, op_code_params* o) -> void
{
	if (!getFlag(c, NEGATIVE))
	{
		c->setPc(o->address);
	}
};

// Branch if not equal aka zero
std::function<void(CPU_6502* c, op_code_params* o)> bne = [](CPU_6502 * c, op_code_params* o) -> void
{
	if (!getFlag(c, ZERO))
	{
		c->setPc(o->address);
	}
};

// Branch if result plus
std::function<void(CPU_6502* c, op_code_params* o)> bpl = [](CPU_6502 * c, op_code_params* o) -> void
{
	if (!getFlag(c, NEGATIVE))
	{
		c->setPc(o->address);
	}
};

// Break, force interrupt
std::function<void(CPU_6502* c, op_code_params* o)> brk = [](CPU_6502 * c, op_code_params* o) -> void
{
	c->setPc(c->getPc() + 1); // when we return go to next instruction
	// split program counter into two bytes and push them onto stack
	PUSH(c, (c->getPc() >> 8) & 0xFF);
	PUSH(c, c->getPc() & 0xFF);
	
	setFlag(c, BRK_COMMAND, true);
	PUSH(c, c->getReg[STATUS]); //push flags onto stack

	uint8_t lowerByte = c->read(0xFFFE);
	uint8_t upperByte = c->read(0xFFFF);
	uint16_t returnAddress = (upperByte << 8) | lowerByte; // dedicated memory location for interrupt routine

	c->setPc(returnAddress);
};

// Branch if overflow clear
std::function<void(CPU_6502* c, op_code_params* o)> bvc = [](CPU_6502 * c, op_code_params* o) -> void
{
	if (!getFlag(c, OVRFLW))
	{
		c->setPc(o->address);
	}
};

// Branch if overflow set
std::function<void(CPU_6502* c, op_code_params* o)> bvs = [](CPU_6502 * c, op_code_params* o) -> void
{
	if (getFlag(c, OVRFLW))
	{
		c->setPc(o->address);
	}
};

// Clear carry flag
std::function<void(CPU_6502* c, op_code_params* o)> clc = [](CPU_6502 * c, op_code_params* o) -> void
{
	setFlag(c, CARRY, false);
};

// Clear decimal mode
std::function<void(CPU_6502* c, op_code_params* o)> cld = [](CPU_6502 * c, op_code_params* o) -> void
{
	setFlag(c, DECIMAL_MODE, false);
};

// Clear interrupt disable
std::function<void(CPU_6502* c, op_code_params* o)> cli = [](CPU_6502 * c, op_code_params* o) -> void
{
	setFlag(c, IRQ_DISABLE, false);
};

// Clear overflow flag
std::function<void(CPU_6502* c, op_code_params* o)> clv = [](CPU_6502 * c, op_code_params* o) -> void
{
	setFlag(c, OVRFLW, false);
};

// Compare memory and Accum reg
std::function<void(CPU_6502* c, op_code_params* o)> cmp = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t accum_val = c->getReg[ACCUM];
	int8_t operand = o->operand;
	int8_t diff = accum_val - operand;

	// needed to check if carry needed
	uint16_t longDiff = (0x00FF & accum_val) - (0x00FF & operand);

	setFlag(c, CARRY, longDiff < 0x100);
	setFlag(c, NEGATIVE, (diff & 0x08) ? 1 : 0);
	setFlag(c, ZERO, diff ? 0 : 1);
};

// Compare Memory and index X reg
std::function<void(CPU_6502* c, op_code_params* o)> cpx = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t x_val = c->getReg[IND_X];
	int8_t operand = o->operand;
	int8_t diff = x_val - operand;

	// needed to check if carry needed
	uint16_t longDiff = (0x00FF & x_val) - (0x00FF & operand);

	setFlag(c, CARRY, longDiff < 0x100);
	setFlag(c, NEGATIVE, (diff & 0x08) ? 1 : 0);
	setFlag(c, ZERO, diff ? 0 : 1);
};

// Compare Memory and index Y reg
std::function<void(CPU_6502* c, op_code_params* o)> cpy = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t y_val = c->getReg[IND_Y];
	int8_t operand = o->operand;
	int8_t diff = y_val - operand;

	// needed to check if carry needed
	uint16_t longDiff = (0x00FF & y_val) - (0x00FF & operand);

	setFlag(c, CARRY, longDiff < 0x100);
	setFlag(c, NEGATIVE, (diff & 0x08) ? 1 : 0);
	setFlag(c, ZERO, diff ? 0 : 1);
};

// Decrement byte at specified address by 1
std::function<void(CPU_6502* c, op_code_params* o)> dec = [](CPU_6502 * c, op_code_params* o) -> void
{
	uint8_t operand = o->operand;
	uint8_t res = operand - 1;

	setSign(c, res);
	setZero(c, res);

	c->write(o->address, res);
};

// Decrement index X reg
std::function<void(CPU_6502* c, op_code_params* o)> dex = [](CPU_6502 * c, op_code_params* o) -> void
{
	uint8_t x_val = c->getReg(IND_X);
	uint8_t res = x_val - 1;
	setSign(c, res);
	setZero(c, res);
	c->setReg(IND_X, res);
};

// Decrement index Y reg
std::function<void(CPU_6502* c, op_code_params* o)> dey = [](CPU_6502 * c, op_code_params* o) -> void
{
	uint8_t y_val = c->getReg(IND_Y);
	uint8_t res = y_val - 1;
	setSign(c, res);
	setZero(c, res);
	c->setReg(IND_Y, res);
};

// Xor memory with accumulator
std::function<void(CPU_6502* c, op_code_params* o)> eor = [](CPU_6502 * c, op_code_params* o) -> void
{
	int8_t accum = c->getReg[ACCUM];
	int8_t res = accum ^ o->operand;
	c->setReg(ACCUM, res);
	setSign(c, res);
	setZero(c, res);
};

// Future unimplemented opcode: Asserts 0
std::function<void(CPU_6502* c, op_code_params* o)> fut = [](CPU_6502 * c, op_code_params * o) -> void
{
	printf("Unimplemented opcode called, possible opcode fetch error");
	_ASSERT(0);
};

// Increment value in memory by one
std::function<void(CPU_6502* c, op_code_params* o)> inc = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint8_t operand = o->operand;
	uint8_t res = operand + 1;
	setSign(c, res);
	setZero(c, res);
	c->write(o->address, res);
};

// Increment index X reg by one
std::function<void(CPU_6502* c, op_code_params* o)> inx = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint8_t x_val = c->getReg(IND_X);
	uint8_t res = x_val + 1;
	setSign(c, res);
	setZero(c, res);
	c->setReg(IND_X, res);
};

// Increment index Y reg by one
std::function<void(CPU_6502* c, op_code_params* o)> iny = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint8_t y_val = c->getReg(IND_Y);
	uint8_t res = y_val + 1;
	setSign(c, res);
	setZero(c, res);
	c->setReg(IND_Y, res);
};

// Jump to address;
std::function<void(CPU_6502* c, op_code_params* o)> jmp = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->setPc( 0xFFFF & (o->address));
};

// Jump to subroutine
std::function<void(CPU_6502* c, op_code_params* o)> jsr = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->setPc(c->getPc()-1); // decrement so that when it jumps back it will go up by one to original value
	//STACK only holds 8 bit values so we split the 16 bit addr into two bytes
	uint8_t upperByte = ((c->getPc()) >> 8) & 0xFF;
	uint8_t lowerByte = c->getPc();
	PUSH(c, upperByte);
	PUSH(c, lowerByte);
	c->setPc(o->address);
};

// Load Accumulator
std::function<void(CPU_6502* c, op_code_params* o)> lda = [](CPU_6502 * c, op_code_params * o) -> void
{
	setSign(c, o->operand);
	setZero(c, o->operand);
	c->setReg(ACCUM, o->operand);
};

// Load index X reg
std::function<void(CPU_6502* c, op_code_params* o)> ldx = [](CPU_6502 * c, op_code_params * o) -> void
{
	setSign(c, o->operand);
	setZero(c, o->operand);
	c->setReg(IND_X, o->operand);
};

// Load index Y reg
std::function<void(CPU_6502* c, op_code_params* o)> ldy = [](CPU_6502 * c, op_code_params * o) -> void
{
	setSign(c, o->operand);
	setZero(c, o->operand);
	c->setReg(IND_Y, o->operand);
};

// Logical Shift Right
std::function<void(CPU_6502* c, op_code_params* o)> lsr = [](CPU_6502 * c, op_code_params * o) -> void
{
	setFlag(c, CARRY, o->operand & 0x01); //shift rightmost bit into carry

	int8_t shifted = ((uint8_t)o->operand) >> 1;
	setSign(c, shifted);
	setZero(c, shifted);
	if(o->mode == Accum_mode)
	{
		c->setReg(ACCUM, shifted);
	}
	else
	{
		c->write(o->address, shifted);
	}
};

// No operation
std::function<void(CPU_6502* c, op_code_params* o)> nop = [](CPU_6502 * c, op_code_params * o) -> void{};

// OR memory with Accumulator
std::function<void(CPU_6502* c, op_code_params* o)> ora = [](CPU_6502 * c, op_code_params * o) -> void
{
	int8_t res = c->getReg(ACCUM) | o->operand;
	setZero(c, res);
	setSign(c, res);

	c->setReg(ACCUM, res);
};

// Push accumulator onto the stack
std::function<void(CPU_6502* c, op_code_params* o)> pha = [](CPU_6502 * c, op_code_params * o) -> void
{
	PUSH(c, c->getReg(ACCUM));
};

// Push status onto the stack
std::function<void(CPU_6502* c, op_code_params* o)> php = [](CPU_6502 * c, op_code_params * o) -> void
{
	PUSH(c, c->getReg(STATUS));
};

// Pull from stack onto accumulator
std::function<void(CPU_6502* c, op_code_params* o)> pla = [](CPU_6502 * c, op_code_params * o) -> void
{
	int8_t res = PULL(c);
	c->setReg(ACCUM, res);
	setSign(c, res);
	setZero(c, res);
};

// Pull processor status
std::function<void(CPU_6502* c, op_code_params* o)> plp = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint8_t res = PULL(c);
	setFlags(c, res); //TODO: TESTME
};

// Rotate one bit Left
std::function<void(CPU_6502* c, op_code_params* o)> rol = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint16_t src = o->operand << 1;
	if (getFlag(c, CARRY)) src |= 0x1; //shift carry bit into it
	setFlag(c, CARRY, src > 0xFF); // set carry if larger than byte
	src &= 0xFF; // truncate
	setSign(c, src);
	setZero(c, src);
	if(o->mode == Accum_mode)
	{
		c->setReg(ACCUM, src);
	}
	else
	{
		c->write(o->address, src);
	}
};

// Rotate one bit Right
std::function<void(CPU_6502* c, op_code_params* o)> ror = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint16_t src = 0xFF & o->operand; 
	if (getFlag(c, CARRY)) src |= 0x100; //shift carry bit into it from right
	setFlag(c, CARRY, src & 0x01); // set carry if rightmost bit is 1
	src >>= 0xFF; // shift
	setSign(c, src);
	setZero(c, src);
	if (o->mode == Accum_mode)
	{
		c->setReg(ACCUM, src);
	}
	else
	{
		c->write(o->address, src);
	}
};

// Return from interrupt
std::function<void(CPU_6502* c, op_code_params* o)> rti = [](CPU_6502 * c, op_code_params * o) -> void
{
	// interrupts push flags onto the stack
	uint8_t status = PULL(c);
	c->setReg(STATUS, status);
	
	uint8_t lowByte = PULL(c);
	uint8_t highByte = PULL(c);
	uint16_t address = ((highByte << 8) | lowByte); 
	c->setPc(address);
};

// Return from subroutine
std::function<void(CPU_6502* c, op_code_params* o)> rts = [](CPU_6502 * c, op_code_params * o) -> void
{
	uint8_t lowByte = PULL(c);
	uint8_t highByte = PULL(c);
	uint16_t address = ((highByte << 8) | lowByte) + 1; // add 1 to set it back to original place
	c->setPc(address);
};

// Subtract from accum with borrow from carry; NOTE: blantantly stolen bc it is twos complenent bit black magic
std::function<void(CPU_6502* c, op_code_params* o)> sbc = [](CPU_6502 * c, op_code_params * o) -> void {
	//we want to subtract the opposite of the carry bit
	int8_t carry = getFlag(c, CARRY) ? 0 : 1;
	int8_t accum = c->getReg(ACCUM);
	int8_t operand = o->operand;
	uint16_t diff = (0x00FF & accum) - (0x00FF & operand) - (0x00FF & carry);
	setSign(c, diff & 0xFF);
	setZero(c, diff & 0xFF);
	setOverflowSubtract(c, accum, operand, diff & 0xFF);
	if (getFlag(c, DECIMAL_MODE)) { //in decimal mode
		if (((accum & 0xF) - carry) < (operand & 0xF)) {
			diff -= 6;
		}
		if (diff > 0x99) {
			diff -= 0x60;
		}
	}
	setFlag(c, CARRY, diff < 0x100);
	c->setReg(ACCUM, diff & 0xFF);
};

// Set carry flag to 1
std::function<void(CPU_6502* c, op_code_params* o)> sec = [](CPU_6502 * c, op_code_params * o) -> void
{
	setFlag(c, CARRY, true);
};

// Set decimal mode to true
std::function<void(CPU_6502* c, op_code_params* o)> sed = [](CPU_6502 * c, op_code_params * o) -> void
{
	setFlag(c, DECIMAL_MODE, true);
};

// Set interrupt flag to true
std::function<void(CPU_6502* c, op_code_params* o)> sei = [](CPU_6502 * c, op_code_params * o) -> void
{
	setFlag(c, IRQ_DISABLE, true);
};

// Store accumulator to memory
std::function<void(CPU_6502* c, op_code_params* o)> sta = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->write(o->address, c->getReg(ACCUM));
};

// Store index X reg to memory
std::function<void(CPU_6502* c, op_code_params* o)> stx = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->write(o->address, c->getReg(IND_X));
};

// Store index Y reg to memory
std::function<void(CPU_6502* c, op_code_params* o)> sty = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->write(o->address, c->getReg(IND_Y));
};

// Transfer Accumulator to index X reg
std::function<void(CPU_6502* c, op_code_params* o)> tax = [](CPU_6502 * c, op_code_params * o) -> void
{
	int8_t accum = c->getReg(ACCUM);
	setSign(c, accum);
	setZero(c, accum);
	c->setReg(IND_X, accum);
};

// Transfer Accumulator to index Y reg
std::function<void(CPU_6502* c, op_code_params* o)> tay = [](CPU_6502 * c, op_code_params * o) -> void
{
	int8_t accum = c->getReg(ACCUM);
	setSign(c, accum);
	setZero(c, accum);
	c->setReg(IND_Y, accum);
};

// Transfer stack pointer to index X reg
std::function<void(CPU_6502* c, op_code_params* o)> tsx = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->setReg(ND_X] = c->regs[STACK];
};

// Transfer index X reg to Accumulator
std::function<void(CPU_6502* c, op_code_params* o)> txa = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->regs[ACCUM] = c->regs[IND_X];
};

// Transfer index X reg to stack pointer
std::function<void(CPU_6502* c, op_code_params* o)> txs = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->regs[STACK] = c->regs[IND_X];
};

// Transfer index Y reg to Accumulator
std::function<void(CPU_6502* c, op_code_params* o)> tya = [](CPU_6502 * c, op_code_params * o) -> void
{
	c->regs[ACCUM] = c->regs[IND_Y];
};



std::function<void(CPU_6502* c, op_code_params* o)> opcode_to_func[256] = {
	brk, ora, fut, fut, fut, ora, asl, fut,
	php, ora, asl, fut, fut, ora, asl, fut,
	bpl, ora, fut, fut, fut, ora, asl, fut,
	clc, ora, fut, fut, fut, ora, asl, fut,
	jsr, and_, fut, fut, bit, and_, rol, fut,
	plp, and_, rol, fut, bit, and_, rol, fut,
	bmi, and_, fut, fut, fut, and_, rol, fut,
	sec, and_, fut, fut, fut, and_, rol, fut,
	rti, eor, fut, fut, fut, eor, lsr, fut,
	pha, eor, lsr, fut, jmp, eor, lsr, fut,
	bvc, eor, fut, fut, fut, eor, lsr, fut,
	cli, eor, fut, fut, fut, eor, lsr, fut,
	rts, adc, fut, fut, fut, adc, ror, fut,
	pla, adc, ror, fut, jmp, adc, ror, fut,
	bvs, adc, fut, fut, fut, adc, ror, fut,
	sei, adc, fut, fut, fut, adc, ror, fut,
	fut, sta, fut, fut, sty, sta, stx, fut,
	dey, fut, txa, fut, sty, sta, stx, fut,
	bcc, sta, fut, fut, sty, sta, stx, fut,
	tya, sta, txs, fut, fut, sta, fut, fut,
	ldy, lda, ldx, fut, ldy, lda, ldx, fut,
	tay, lda, tax, fut, ldy, lda, ldx, fut,
	bcs, lda, fut, fut, ldy, lda, ldx, fut,
	clv, lda, tsx, fut, ldy, lda, ldx, fut,
	cpy, cmp, fut, fut, cpy, cmp, dec, fut,
	iny, cmp, dex, fut, cpy, cmp, dec, fut,
	bne, cmp, fut, fut, fut, cmp, dec, fut,
	cld, cmp, fut, fut, fut, cmp, dec, fut,
	cpx, sbc, fut, fut, cpx, sbc, inc, fut,
	inx, sbc, nop, fut, cpx, sbc, inc, fut,
	beq, sbc, fut, fut, fut, sbc, inc, fut,
	sed, sbc, fut, fut, fut, sbc, inc, fut
};

AddressingMode instructionModes[256] = {
	Implied, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum_mode, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	Absolute, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum_mode, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	Implied, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum_mode, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	Implied, IndexedIndirect, Implied, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Accum_mode, IMM, Indirect, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPY, ZPY, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteY, AbsoluteY,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPY, ZPY, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteY, AbsoluteY,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
	IMM, IndexedIndirect, IMM, IndexedIndirect, ZP, ZP, ZP, ZP, Implied, IMM, Implied, IMM, Absolute, Absolute, Absolute, Absolute,
	Relative, IndirectIndexed, Implied, IndirectIndexed, ZPX, ZPX, ZPX, ZPX, Implied, AbsoluteY, Implied, AbsoluteY, AbsoluteX, AbsoluteX, AbsoluteX, AbsoluteX,
};

uint8_t instructionSizes[256] = {
	1, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	3, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	1, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	1, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 0, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 0, 3, 0, 0,
	2, 2, 2, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 2, 1, 0, 3, 3, 3, 0,
	2, 2, 0, 0, 2, 2, 2, 0,
	1, 3, 1, 0, 3, 3, 3, 0
};

uint8_t instructionCycle[256] = {
	7, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 3, 2, 2, 2, 3, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	6, 6, 2, 8, 3, 3, 5, 5, 4, 2, 2, 2, 5, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 6, 2, 6, 4, 4, 4, 4, 2, 5, 2, 5, 5, 5, 5, 5,
	2, 6, 2, 6, 3, 3, 3, 3, 2, 2, 2, 2, 4, 4, 4, 4,
	2, 5, 2, 5, 4, 4, 4, 4, 2, 4, 2, 4, 4, 4, 4, 4,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
	2, 6, 2, 8, 3, 3, 5, 5, 2, 2, 2, 2, 4, 4, 6, 6,
	2, 5, 2, 8, 4, 4, 6, 6, 2, 4, 2, 7, 4, 4, 7, 7,
};

uint8_t instructionPageCycles[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	1, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0,
};

OpCode instructionNames[256] = {
	//FUT represents unimplemented op codes
	BRK, ORA, FUT, FUT, FUT, ORA, ASL, FUT,
	PHP, ORA, ASL, FUT, FUT, ORA, ASL, FUT,
	BPL, ORA, FUT, FUT, FUT, ORA, ASL, FUT,
	CLC, ORA, FUT, FUT, FUT, ORA, ASL, FUT,
	JSR, AND, FUT, FUT, BIT, AND, ROL, FUT,
	PLP, AND, ROL, FUT, BIT, AND, ROL, FUT,
	BMI, AND, FUT, FUT, FUT, AND, ROL, FUT,
	SEC, AND, FUT, FUT, FUT, AND, ROL, FUT,
	RTI, EOR, FUT, FUT, FUT, EOR, LSR, FUT,
	PHA, EOR, LSR, FUT, JMP, EOR, LSR, FUT,
	BVC, EOR, FUT, FUT, FUT, EOR, LSR, FUT,
	CLI, EOR, FUT, FUT, FUT, EOR, LSR, FUT,
	RTS, ADC, FUT, FUT, FUT, ADC, ROR, FUT,
	PLA, ADC, ROR, FUT, JMP, ADC, ROR, FUT,
	BVS, ADC, FUT, FUT, FUT, ADC, ROR, FUT,
	SEI, ADC, FUT, FUT, FUT, ADC, ROR, FUT,
	FUT, STA, FUT, FUT, STY, STA, STX, FUT,
	DEY, FUT, TXA, FUT, STY, STA, STX, FUT,
	BCC, STA, FUT, FUT, STY, STA, STX, FUT,
	TYA, STA, TXS, FUT, FUT, STA, FUT, FUT,
	LDY, LDA, LDX, FUT, LDY, LDA, LDX, FUT,
	TAY, LDA, TAX, FUT, LDY, LDA, LDX, FUT,
	BCS, LDA, FUT, FUT, LDY, LDA, LDX, FUT,
	CLV, LDA, TSX, FUT, LDY, LDA, LDX, FUT,
	CPY, CMP, FUT, FUT, CPY, CMP, DEC, FUT,
	INY, CMP, DEX, FUT, CPY, CMP, DEC, FUT,
	BNE, CMP, FUT, FUT, FUT, CMP, DEC, FUT,
	CLD, CMP, FUT, FUT, FUT, CMP, DEC, FUT,
	CPX, SBC, FUT, FUT, CPX, SBC, INC, FUT,
	INX, SBC, NOP, FUT, CPX, SBC, INC, FUT,
	BEQ, SBC, FUT, FUT, FUT, SBC, INC, FUT,
	SED, SBC, FUT, FUT, FUT, SBC, INC, FUT
};
