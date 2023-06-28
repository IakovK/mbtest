#pragma once

// command codes
// every command is one byte command code followed by 2 bytes argument

#define READ_COIL           1
#define READ_DISCRETE       2
#define READ_INPUT          3
#define READ_HOLDING        4
#define READ_COIL_ALL       5
#define READ_DISCRETE_ALL   6
#define READ_INPUT_ALL      7
#define READ_HOLDING_ALL    8
#define READ_ALL            9
#define START_LOOP          10
#define STOP_LOOP           11
