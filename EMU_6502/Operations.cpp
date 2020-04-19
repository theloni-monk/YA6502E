#include "Operations.h"

//WRITEME all opcodes
operation _ADC = [](CPU *c, op_code_params *o) -> void
{
	
};


operation Ops[] = {
	_ADC,
	_AND, _ASL,
	_BCC, _BCS, _BEQ, _BIT, _BMI, _BNE, _BPL, _BRK, _BVC, _BVS,
	_CLC, _CLD, _CLI, _CLV, _CMP, _CPX, _CPY,
	_DEC, _DEX, _DEY,
	_EOR,
	_INC, _INX, _INY,
	_JPM, _JSR,
	_LDA, _LDX, _LDY, _LDR,
	_NOP,
	_ORA,
	_PHA, _PHP, _PLA, _PLP,
	_ROL, _ROR, _RTI, _RTS,
	_SBC, _SEC, _SED, _SEI, _STA, _STX, _STY,
	_TAX, _TAY, _TSX, _TXA, _TXS, _TYA
};

