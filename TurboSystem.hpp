#pragma once

enum class DeviceMode {
    MAIN,
    SLEEP
};

class StateManager {
private:
    DeviceMode currentMode;

public:
    StateManager() : currentMode(DeviceMode::MAIN) {}

    void set_mode(DeviceMode new_mode) {
        currentMode = new_mode;
    }

    DeviceMode getMode() const {
        return currentMode;
    }
};
