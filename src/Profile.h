#ifndef MMZ_PROFILE_H
#define MMZ_PROFILE_H

#include <array>

#include "Key.h"
#include "Layer.h"

template <uint8_t NUM_OF_KEYS, uint8_t NUM_OF_LAYERS, uint8_t NUM_OF_PROFILES>
class Profile {
public:
    using ProfileCallback = std::function<void(LayeredKeymap<NUM_OF_KEYS, NUM_OF_LAYERS>&)>;

    Profile(Layer<NUM_OF_KEYS, NUM_OF_LAYERS>& profile, ProfileCallback onProfileChange=nullptr)
     : profile_(profile), onProfileChange_(onProfileChange), currentProfile_(0), preProfile_(0) {}

    void init(ProfiledLayers<NUM_OF_KEYS, NUM_OF_LAYERS, NUM_OF_PROFILES> profiles) {
        profiles_ = profiles;

        set(0);
    }

    void set(const uint8_t profile) {
        if (profile >= NUM_OF_PROFILES) { return; }
        preProfile_ = currentProfile_;
        currentProfile_ = profile;

        profile_.setProfile(profiles_[currentProfile_]);

        if (onProfileChange_) { onProfileChange_(profiles_[currentProfile_]); }
    }

    void reset() { set(preProfile_); }

    uint8_t get() const { return currentProfile_; }

private:
    ProfiledLayers<NUM_OF_KEYS, NUM_OF_LAYERS, NUM_OF_PROFILES> profiles_;
    Layer<NUM_OF_KEYS, NUM_OF_LAYERS> &profile_;
    ProfileCallback onProfileChange_;
    uint8_t currentProfile_, preProfile_;
};

#endif
