#pragma once

class SwitchControlled {
public:

    virtual void activate() {
    }

    virtual void deactivate() {
    }

    virtual bool is_active() const {
    }

    virtual ~SwitchControlled() {
    };
};
