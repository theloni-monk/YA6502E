#include "Operations.hpp"
#include "CPU.hpp"

//TODO: implement reads and writes through memoryinterface

/* FLAG OPERATIONS */
void setFlag(CPU_6502* c, flag_t flag, bool val)
{
	c->regs[STATUS] = (c->regs[STATUS] & ~(1 << flag)) | (val << flag);
}

bool getFlag(CPU_6502* c, flag_t flag)
{
	return c->regs[STATUS];
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
	uint8_t stackVal = c->regs[STACK];
	uint16_t address = 0x0100 | stackVal;
	//0x0100 is hardcoded as stack page
	//lives in 0x0100 to 0x01FF page of mem
	c->addressSpace[address] = operand;
	uint8_t newStackVal = stackVal - 1;
	c->regs[STACK] = newStackVal;
}

// pull value from stack in memory
char PULL(CPU_6502* c) {
	uint8_t stackVal = c->regs[STACK];
	uint8_t newStackVal = stackVal + 1;
	c->regs[STACK] = newStackVal;
	//0x0100 is hardcoded as stack page
	//lives in 0x0100 to 0x01FF page of mem
	uint16_t address = 0x0100 | newStackVal;
	return c->addressSpace[address];
}

/* OPCODE IMPLEMENTATIONS */ //TODO: WRITEME all opcodes
// Add with carry: adds memory value with accumulator 
std::function<void(CPU_6502* c, op_code_params o)> adc = [](CPU_6502 *c, op_code_params o) -> void
{

	int8_t carry = getFlag(c, CARRY);
	int8_t accum = c->regs[ACCUM];
	int8_t operand = o.operand;
	int16_t sum = (0x00FF & carry) + (0x00FF & accum) + (0x00FF & operand);
	int8_t sumByte = sum & 0x00FF;
	
	setZero(c, sumByte); // set zero flag if zero

	// Black Magic from https://github.com/dennis-chen/6502-Emu/blob/master/src/cpu.c
	if (c->regs[STATUS] & DECIMAL_MODE) { //in decimal mode
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
	c->regs[ACCUM] = sum & 0xFF;
};

// And memory with accumulator // NOTE: named and_ instead of and b/c cpp has 'and' reserved for some reason 
std::function<void(CPU_6502* c, op_code_params o)> and_ = [](CPU_6502 * c, op_code_params o) -> void
{
	int8_t res = c->regs[ACCUM] & o.operand;
	setSign(c, res);
	setZero(c, res);
	c->regs[ACCUM] = res;
};

std::function<void(CPU_6502* c, op_code_params o)> asl = [](CPU_6502 * c, op_code_params o) -> void
{
	int8_t operand = o.operand;
	int16_t res = (0x00FF & operand) << 1;
	int8_t resByte = res & 0x00FF;
	setCarry(c, resByte);
	setSign(c, resByte);
	setZero(c, resByte);

	if(o.mode == Accum)
	{
		c->regs[ACCUM] = res;
	}
	else
	{
		c->addressSpace[o.address] = res;
	}

};

std::function<void(CPU_6502* c, op_code_params o)> bcc = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bcs = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> beq = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bit = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bmi = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bne = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bpl = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> brk = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bvc = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> bvs = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> clc = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> cld = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> cli = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> clv = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> cmp = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> cpx = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> cpy = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> dec = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> dex = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> dey = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};

std::function<void(CPU_6502* c, op_code_params o)> eor = [](CPU_6502 * c, op_code_params o) -> void
{
	//TODO
};


std::function<void(CPU_6502* c, op_code_params o)> opcode_to_func[256] = {
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

