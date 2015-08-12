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
