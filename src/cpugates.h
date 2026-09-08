#pragma once
#include <cstdint>
#include <cstdio>
#include "utils.h"

class BaseGate {
public:
    template<typename value_t>
    inline static value_t exec(value_t value) {
        value_t final = 0;
        /*for (int i = 0; i < sizeof(value)*8; i++) {
            uint8_t bit = extract_bits(value, 1, i);
            
            // work here.
        }*/
        printf("cpugates: class function not implemented.\n");
        return final;
    }
};

class CPUGates {
public:
    class NOTGate : BaseGate {
    public:
        template<typename value_t>
        inline static value_t exec(value_t value) {
            value_t final = 0;
            for (int i = 0; i < sizeof(value)*8; i++) {
                uint8_t bit = extract_bits(value, 1, i);
                
                final += ((1 - bit) << i);
            }
            return final;
        }
    };
    class ORGate : BaseGate {
    public:
        template<typename value_t>
        inline static value_t exec(value_t value0, value_t value1) {
            value_t final = 0;
            for (int i = 0; i < sizeof(value0)*8; i++) {
                uint8_t bit0 = extract_bits(value0, 1, i);
                uint8_t bit1 = extract_bits(value1, 1, i);
                
                final += (((bit0 == 0 && bit1 == 0) ? 0 : 1) << i);
            }
            return final;
        }
    };
    class XORGate : BaseGate {
    public:
        template<typename value_t>
        inline static value_t exec(value_t value0, value_t value1) {
            value_t final = 0;
            for (int i = 0; i < sizeof(value0)*8; i++) {
                uint8_t bit0 = extract_bits(value0, 1, i);
                uint8_t bit1 = extract_bits(value1, 1, i);
                
                final += (((bit0 == bit1) ? 0 : 1) << i);
            }
            return final;
        }
    };
    class ANDGate : BaseGate {
    public:
        template<typename value_t>
        inline static value_t exec(value_t value0, value_t value1) {
            value_t final = 0;
            for (int i = 0; i < sizeof(value0)*8; i++) {
                uint8_t bit0 = extract_bits(value0, 1, i);
                uint8_t bit1 = extract_bits(value1, 1, i);
                
                final += (((bit0 != bit1 || (bit0 == 0 && bit1 == 0)) ? 0 : 1) << i);
            }
            return final;
        }
    };
    class NANDGate : BaseGate {
    public:
        template<typename value_t>
        inline static value_t exec(value_t value0, value_t value1) {
            value_t final = 0;
            for (int i = 0; i < sizeof(value0)*8; i++) {
                uint8_t bit0 = extract_bits(value0, 1, i);
                uint8_t bit1 = extract_bits(value1, 1, i);
                
                bit0 = extract_bits(ANDGate::exec<uint8_t>(bit0, bit1), 1 ,0);
                
                final += (extract_bits(NOTGate::exec<uint8_t>(bit0), 1, 0) << i);
            }
            return final;
        }
    };
};