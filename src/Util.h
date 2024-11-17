#ifndef MMZ_UTIL_H
#define MMZ_UTIL_H

#ifdef USE_KEYBOARD_H
#include "Keyboard_h_Util.h"
#endif

#include "Layer.h"
#include "Key.h"

template<uint8_t NUM_OF_KEYS, uint8_t NUM_OF_LAYERS>
class LayerUtil {
public:
    LayerUtil(Layer<NUM_OF_KEYS, NUM_OF_LAYERS>& layer) : layer_(layer) {}

    inline KeyAssign to(uint8_t layer) {
        return [this, layer](Key key) {
            if (key.hasOccurred(Key::Event::RISING_EDGE)) {
                layer_.set(layer);
            }
        };
    }
    inline KeyAssign back(uint8_t layer) {
        return [this, layer](Key key) {
            if (key.hasOccurred(Key::Event::FALLING_EDGE)) {
                layer_.set(layer);
            }
        };
    }
    inline KeyAssign reset() {
        return [this](Key key) {
            if (key.hasOccurred(Key::Event::RISING_EDGE)) {
                layer_.reset();
            }
        };
    }

private:
    Layer<NUM_OF_KEYS, NUM_OF_LAYERS>& layer_;
};

#define PRESS_A pressTo('a')
#define PRESS_B pressTo('b')
#define PRESS_C pressTo('c')
#define PRESS_D pressTo('d')
#define PRESS_E pressTo('e')
#define PRESS_F pressTo('f')
#define PRESS_G pressTo('g')
#define PRESS_H pressTo('h')
#define PRESS_I pressTo('i')
#define PRESS_J pressTo('j')
#define PRESS_K pressTo('k')
#define PRESS_L pressTo('l')
#define PRESS_M pressTo('m')
#define PRESS_N pressTo('n')
#define PRESS_O pressTo('o')
#define PRESS_P pressTo('p')
#define PRESS_Q pressTo('q')
#define PRESS_R pressTo('r')
#define PRESS_S pressTo('s')
#define PRESS_T pressTo('t')
#define PRESS_U pressTo('u')
#define PRESS_V pressTo('v')
#define PRESS_W pressTo('w')
#define PRESS_X pressTo('x')
#define PRESS_Y pressTo('y')
#define PRESS_Z pressTo('z')


#endif
