#pragma once
#include "utils.h"
#include <vector>

struct BaseRegisterIOMode {
protected:
    int16_t stored_type;
    
    inline static const BaseRegisterIOMode* decdef(int16_t type) {
        return new BaseRegisterIOMode(type);
    }
public:
    inline static const BaseRegisterIOMode* LO8 = decdef(0);
    inline static const BaseRegisterIOMode* HI8 = decdef(1);

    inline static const BaseRegisterIOMode* LO16 = decdef(2);
    inline static const BaseRegisterIOMode* HI16 = decdef(3);

    inline static const BaseRegisterIOMode* LO32 = decdef(4);
    inline static const BaseRegisterIOMode* HI32 = decdef(5);

    inline static const BaseRegisterIOMode* LO64 = decdef(6);
    inline static const BaseRegisterIOMode* HI64 = decdef(7);

    inline static const BaseRegisterIOMode* LO128 = decdef(8);
    inline static const BaseRegisterIOMode* HI128 = decdef(9);

    BaseRegisterIOMode(int16_t type): stored_type(type) {}
    virtual ~BaseRegisterIOMode() = default;

    int16_t getStoredType() const { return stored_type; }
};

struct BaseRegister {
protected:
    bool writeable = true;
    bool readable = true;
public:
    BaseRegister() = default;
    BaseRegister(bool isWriteable, bool isReadable): writeable(isWriteable), readable(isReadable) {}
    virtual ~BaseRegister() = default;

    virtual bool isReadable() const { return readable; }
    virtual bool isWriteable() const { return writeable; }

    virtual void switchReadableState(bool value) { readable = value; }
    virtual void switchWriteableState(bool value) { writeable = value; }
};

template<typename int_t>
struct Register : public BaseRegister {
protected:
    int_t value = 0;
public:
    Register(): BaseRegister() {}
    Register(int_t value): value(value) {}
    virtual ~Register() = default;

    virtual void write(int_t value, const BaseRegisterIOMode* mode) {
        if (!writeable)
            return;

        if (mode == BaseRegisterIOMode::LO8) {
            size_t remaining = extract_bits(this->value, sizeof(int_t)*8, 4);
            this->value = (remaining << 4) + value;
        }
        if (mode == BaseRegisterIOMode::LO16) {
            size_t remaining = extract_bits(this->value, sizeof(int_t)*8, 8);
            this->value = (remaining << 8) + value;
        }
        if (mode == BaseRegisterIOMode::LO32) {
            size_t remaining = extract_bits(this->value, sizeof(int_t)*8, 16);
            this->value = (remaining << 32) + value;
        }
        if (mode == BaseRegisterIOMode::LO64) {
            size_t remaining = extract_bits(this->value, sizeof(int_t)*8, 32);
            this->value = (remaining << 32) + value;
        }
        // TODO: Implement 128-bit LO write data.

        if (mode == BaseRegisterIOMode::HI8) {
            size_t remaining0 = extract_bits(this->value, 4, 0);
            if (sizeof(int_t)*8 == 8) {
                this->value = (value << 4) + remaining0;
                return;
            }
            size_t remaining1 = extract_bits(this->value, (sizeof(int_t)*8)-4, 8);
            this->value = (remaining1 << 8) + (value << 4) + remaining0;
        }
        if (mode == BaseRegisterIOMode::HI16) {
            size_t remaining0 = extract_bits(this->value, 8, 0);
            if (sizeof(int_t)*8 == 16) {
                this->value = (value << 8) + remaining0;
                return;
            }
            size_t remaining1 = extract_bits(this->value, (sizeof(int_t)*8)-8, 16);
            this->value = (remaining1 << 16) + (value << 8) + remaining0;
        }
        if (mode == BaseRegisterIOMode::HI32) {
            size_t remaining0 = extract_bits(this->value, 16, 0);
            if (sizeof(int_t)*8 == 32) {
                this->value = (value << 16) + remaining0;
                return;
            }
            size_t remaining1 = extract_bits(this->value, (sizeof(int_t)*8)-16, 32);
            this->value = (remaining1 << 32) + (value << 16) + remaining0;
        }
        if (mode == BaseRegisterIOMode::HI64) {
            size_t remaining0 = extract_bits(this->value, 32, 0);
            if (sizeof(int_t)*8 == 64) {
                this->value = (value << 32) + remaining0;
                return;
            }
            size_t remaining1 = extract_bits(this->value, (sizeof(int_t)*8)-32, 64);
            this->value = (remaining1 << 64) + (value << 32) + remaining0;
        }
        // TODO: Implement 128-bit HI write data.
    }
    virtual int_t read(const BaseRegisterIOMode* mode) {
        int_t value = 0;

        if (!readable)
            return value;

        if (mode == BaseRegisterIOMode::LO8) {
            value = extract_bits(this->value, 4, 0);
        }
        if (mode == BaseRegisterIOMode::LO16) {
            value = extract_bits(this->value, 8, 0);
        }
        if (mode == BaseRegisterIOMode::LO32) {
            value = extract_bits(this->value, 16, 0);
        }
        if (mode == BaseRegisterIOMode::LO64) {
            value = extract_bits(this->value, 32, 0);
        }
        // TODO: Implement 128-bit LO read data.

        if (mode == BaseRegisterIOMode::HI8) {
            value = extract_bits(this->value, 4, 4);
        }
        if (mode == BaseRegisterIOMode::HI16) {
            value = extract_bits(this->value, 8, 8);
        }
        if (mode == BaseRegisterIOMode::HI32) {
            value = extract_bits(this->value, 16, 16);
        }
        if (mode == BaseRegisterIOMode::HI64) {
            value = extract_bits(this->value, 32, 32);
        }
        // TODO: Implement 128-bit HI read data.
        
        return value;
    }
};

struct BaseRegisterID {
protected:
    int16_t stored_type;
    
    inline static const BaseRegisterID* decdef(int16_t type) {
        return new BaseRegisterID(type);
    }
public:
    inline static const BaseRegisterID* GENERIC = decdef(-1);

    BaseRegisterID(int16_t type): stored_type(type) {}
    virtual ~BaseRegisterID() = default;

    int16_t getStoredType() const { return stored_type; }
};

class BaseCPUState {
protected:
    vector<BaseRegister*> cpu_regs;
public:
    BaseCPUState(): cpu_regs(32) {}
    BaseCPUState(vector<BaseRegister*> cpu_regs): cpu_regs(cpu_regs) {}
    virtual ~BaseCPUState() {
        for (auto reg: cpu_regs) {
            delete reg;
            reg = nullptr;
        }

        cpu_regs.clear();
    }

    // TODO: Implement it.
    virtual BaseRegister*& getRegById(const BaseRegisterID* id) = 0;
};