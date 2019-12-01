# ZeldaOS.RISCV
The system level emulator which utilizes dynamic binary translation for RISCV ISA



## Unprivileged ISAs
#### RV32I instructions
- [X] LUI rd, imm
- [X] AUIPC rd, offset
- [X] JAL rd, offset
- [X] JALR rd, rs1, offset
- [X] BEQ rs1, rs2, offset
- [x] BNE rs1, rs2, offset
- [x] BLT rs1, rs2, offset
- [x] BGE rs1, rs2, offset
- [x] BLTU rs1, rs2, offset
- [x] BGEU rs1, rs2, offset
- [x] LB rd, offset(rs1)
- [x] LH rd, offset(rs1)
- [x] LW rd, offset(rs1)
- [x] LBU rd, offset(rs1)
- [x] LHU rd, offset(rs1)
- [x] SB rs2, offset(rs1)
- [x] SH rs2, offset(rs1)
- [x] SW rs2, offset(rs1)
- [x] ADDI rd, rs1, imm
- [x] SLTI rd, rs1, imm
- [x] SLTIU rd, rs1, imm
- [x] XORI rd, rs1, imm
- [x] ORI rd, rs1, imm
- [x] ANDI rd, rs1, imm
- [x] SLLI rd, rs1, imm
- [x] SRLI rd, rs1, imm
- [x] SRAI rd, rs1, imm
- [x] ADD rd, rs1, rs2
- [x] SUB rd, rs1, rs2
- [x] OR rd, rs1, rs2
- [x] AND rd, rs1, rs2
- [x] XOR rd, rs1, rs2
- [x] SLL rd, rs1, rs2
- [x] SRL rd, rs1, rs2
- [x] SRA rd, rs1, rs2
- [x] SLT rd, rs1, rs2
- [x] SLTU rd, rs1, rs2
- [X] FENCE pred, succ
- [x] FENCE.I

#### RV32A instructions

- [ ] LR.W aqrl, rd, (rs1)
- [ ] SC.W aqrl, rd, rs2,(rs1)
- [ ] AMOSWAP.W aqrl, rd, rs2, (rs1) 
- [ ] AMOADD.W aqrl, rd, rs2, (rs1) 
- [ ] AMOXOR.W aqrl, rd, rs2, (rs1) 
- [ ] AMOOR.W aqrl, rd, rs2, (rs1) 
- [ ] AMOAND.W aqrl, rd, rs2, (rs1) 
- [ ] AMOMIN.W aqrl, rd, rs2, (rs1) 
- [ ] AMOMAX.W aqrl, rd, rs2, (rs1) 
- [ ] AMOMINU.W aqrl, rd, rs2, (rs1) 
- [ ] AMOMAXU.W aqrl, rd, rs2, (rs1)

## supervisor-level ISAs
## machine-level ISAs
