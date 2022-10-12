DEF_CMD(HALT, 0, 0)

DEF_CMD(PUSH, 1, 1)

DEF_CMD(POP , 2, 1)

DEF_CMD(ADD , 3, 0)

DEF_CMD(OUT , 4, 0)

DEF_CMD( IN , 5, 0)

DEF_CMD(DIV , 6, 0)

DEF_CMD(MULT, 7, 0)

DEF_CMD_JUMP(JA ,  9,  >)

DEF_CMD_JUMP(JAE, 10, >=)

DEF_CMD_JUMP(JB , 11,  <)

DEF_CMD_JUMP(JBE, 12, <=)

DEF_CMD_JUMP(JL , 13, ==)

DEF_CMD_JUMP(JM , 14, !=)