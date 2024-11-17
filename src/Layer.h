#ifndef MMZ_LAYER_H
#define MMZ_LAYER_H

#include <array>
#include "Key.h"

constexpr uint8_t BASE = 0;

template<uint8_t NUM_OF_KEYS, uint8_t NUM_OF_LAYERS>
class Layer {
public:
    using LayerCallback = std::function<void(uint8_t)>;

    Layer(std::array<Key, NUM_OF_KEYS>& keys, LayerCallback onLayerChange=nullptr)
     : keys_(keys), onLayerChange_(onLayerChange), currentLayer_(0), preLayer_(0) {}

    void init(LayeredKeymap<NUM_OF_KEYS, NUM_OF_LAYERS> layeredKeymap) {
        layers_ = layeredKeymap;
    }

    void set(const uint8_t layer) {
        if (layer >= NUM_OF_LAYERS) { return; }
        preLayer_ = currentLayer_;
        currentLayer_ = layer;
        if (onLayerChange_ != nullptr) { onLayerChange_(currentLayer_); }

        for (uint16_t i = 0; i < NUM_OF_KEYS; i++) {
            keys_[i].registerMacro(layers_[layer][i]);
        }
    }

    void reset() { set(preLayer_); }

    uint8_t get() const { return currentLayer_; }

private:
    LayeredKeymap<NUM_OF_KEYS, NUM_OF_LAYERS> layers_;
    std::array<Key, NUM_OF_KEYS>& keys_;
    LayerCallback onLayerChange_;
    uint8_t currentLayer_, preLayer_;
};

#endif
