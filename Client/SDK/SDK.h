#pragma once
#include <windows.h>
#include <cstdint>
#include <cmath>

namespace SDK {

struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator*(float scalar) const {
        return Vector3(x * scalar, y * scalar, z * scalar);
    }

    float Length() const {
        return sqrtf(x * x + y * y + z * z);
    }

    Vector3 Normalize() const {
        float len = Length();
        if (len > 0.0f)
            return Vector3(x / len, y / len, z / len);
        return Vector3(0, 0, 0);
    }

    float Distance(const Vector3& other) const {
        return (*this - other).Length();
    }
};

struct Rotation {
    float yaw;
    float pitch;
    float roll;

    Rotation() : yaw(0), pitch(0), roll(0) {}
    Rotation(float _yaw, float _pitch, float _roll) : yaw(_yaw), pitch(_pitch), roll(_roll) {}
};

struct Matrix4x4 {
    float m[4][4];

    Matrix4x4() {
        memset(m, 0, sizeof(m));
    }
};

class CEntity {
public:
    char pad_0x0000[0x27C];
    Vector3 position;
    char pad_0x288[0xEC];
    Rotation rotation;
    char pad_0x380[0x100];

    Vector3 GetPosition() {
        return position;
    }

    void SetPosition(const Vector3& pos) {
        position = pos;
    }

    Rotation GetRotation() {
        return rotation;
    }

    void SetRotation(const Rotation& rot) {
        rotation = rot;
    }

    float GetYaw() { return rotation.yaw; }
    float GetPitch() { return rotation.pitch; }
    float GetRoll() { return rotation.roll; }

    void SetYaw(float yaw) { rotation.yaw = yaw; }
    void SetPitch(float pitch) { rotation.pitch = pitch; }
    void SetRoll(float roll) { rotation.roll = roll; }
};

class CEntityList {
public:
    char pad_0x0000[0x10];
    CEntity* localPlayer;

    CEntity* GetLocalPlayer() {
        return localPlayer;
    }

    CEntity* GetEntityByIndex(int index) {
        uintptr_t* entityArray = (uintptr_t*)((uintptr_t)this);
        uintptr_t entityPtr = entityArray[index * 8 / sizeof(uintptr_t) + 2];
        if (entityPtr)
            return (CEntity*)entityPtr;
        return nullptr;
    }
};

class CGameContext {
public:
    char pad_0x0000[0x18];
    CEntityList* entityList;

    CEntityList* GetEntityList() {
        return entityList;
    }

    CEntity* GetLocalPlayer() {
        if (entityList)
            return entityList->GetLocalPlayer();
        return nullptr;
    }
};

namespace Offsets {
    constexpr uintptr_t GameTickFunction = 0x0;
    constexpr uintptr_t PlayerBase = 0x27C25D8;
    constexpr uintptr_t ViewMatrixBase = 0x27C25E8;
    constexpr uintptr_t InfiniteStamina = 0x3E540A;

    namespace Entity {
        constexpr uintptr_t Position = 0x27C;
        constexpr uintptr_t Rotation = 0x374;
    }

    namespace Player {
        constexpr uintptr_t PosX = 0x27C;
        constexpr uintptr_t PosY = 0x280;
        constexpr uintptr_t PosZ = 0x284;
        constexpr uintptr_t Yaw = 0x374;
        constexpr uintptr_t Pitch = 0x378;
        constexpr uintptr_t Roll = 0x37C;
    }

    namespace ViewMatrixOffsets {
        constexpr uintptr_t Offset1 = 0x30;
        constexpr uintptr_t Offset2 = 0x398;
        constexpr uintptr_t Offset3 = 0x70;
        constexpr uintptr_t Final = 0x480;
    }

    namespace PlayerChain {
        constexpr uintptr_t Offset1 = 0x5E0;
        constexpr uintptr_t Offset2 = 0x48;
        constexpr uintptr_t Offset3 = 0x10;
    }
}

}
