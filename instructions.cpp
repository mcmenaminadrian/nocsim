#include <iostream>
#include <boost/proto/proto.hpp>

using namespace boost;


//limited RISC instruction set
//based on Ridiciulously Simple Computer concept
//instructions:
//	add 	rA, rB, rC	: rA <- rB + rC
//	addi	rA, rB, imm	: rA <- rB + imm
//	and	rA, rB, rC	: rA <- rB & rC
//	sw	rA, rB, rC	: rA -> *(rB + rC)
//	swi	rA, rB, imm	: rA -> *(rB + imm)
//	lw	rA, rB, rC	: rA <- *(rB + rC)
//	lwi	rA, rB, imm	: rA <-	*(rB + imm)
//	beq	rA, rB, imm	: PC <- imm iff rA == rB
//	br	imm		: PC <- imm


template<long ll>
struct reg
{};

proto::terminal<reg<0> >::type _reg0 = {{}}; 
proto::terminal<reg<1> >::type _reg1 = {{}}; 
proto::terminal<reg<2> >::type _reg2 = {{}}; 
proto::terminal<reg<3> >::type _reg3 = {{}}; 
proto::terminal<reg<4> >::type _reg4 = {{}}; 
proto::terminal<reg<5> >::type _reg5 = {{}}; 
proto::terminal<reg<6> >::type _reg6 = {{}}; 
proto::terminal<reg<7> >::type _reg7 = {{}}; 
proto::terminal<reg<8> >::type _reg8 = {{}}; 
proto::terminal<reg<9> >::type _reg9 = {{}}; 
proto::terminal<reg<10> >::type _reg10 = {{}}; 
proto::terminal<reg<11> >::type _reg11 = {{}}; 
proto::terminal<reg<12> >::type _reg12 = {{}}; 
proto::terminal<reg<13> >::type _reg13 = {{}}; 
proto::terminal<reg<14> >::type _reg14 = {{}}; 
proto::terminal<reg<15> >::type _reg15 = {{}}; 
proto::terminal<reg<16> >::type _reg16 = {{}}; 
proto::terminal<reg<17> >::type _reg17 = {{}}; 
proto::terminal<reg<18> >::type _reg18 = {{}}; 
proto::terminal<reg<19> >::type _reg19 = {{}}; 
proto::terminal<reg<20> >::type _reg20 = {{}}; 
proto::terminal<reg<21> >::type _reg21 = {{}}; 
proto::terminal<reg<22> >::type _reg22 = {{}}; 
proto::terminal<reg<23> >::type _reg23 = {{}}; 
proto::terminal<reg<24> >::type _reg24 = {{}}; 
proto::terminal<reg<25> >::type _reg25 = {{}}; 
proto::terminal<reg<26> >::type _reg26 = {{}}; 
proto::terminal<reg<27> >::type _reg27 = {{}}; 
proto::terminal<reg<28> >::type _reg28 = {{}}; 
proto::terminal<reg<29> >::type _reg29 = {{}}; 
proto::terminal<reg<30> >::type _reg30 = {{}}; 
proto::terminal<reg<31> >::type _reg31 = {{}}; 







