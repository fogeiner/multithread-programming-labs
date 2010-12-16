#pragma once

class SwitchControlled {
public:

    virtual void activate() = 0;

    virtual void deactivate() = 0;

    virtual bool is_active() const = 0;

    virtual ~SwitchControlled() {
    };
};
