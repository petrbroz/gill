#ifndef GILL_CORE_FILTER_H_
#define GILL_CORE_FILTER_H_

namespace gill { namespace core {

/**
 * An approach for reconstructing an image from computed samples.
 */
class Filter {
public:
    /**
     * Initializes filter with given window dimensions.
     * @param width Half-width of the window in image space affected by the filter.
     * @param height Half-height of the window in image space affected by the filter.
     */
    Filter(float width, float height);
    virtual ~Filter();

    /**
     * Evaluates filter value.
     * @param x X-coordinate relative to the center of the filter window.
     * @param y Y-coordinate relative to the center of the filter window.
     * @returns Unbound (possibly negative) value of the filter at given coordinates.
     */
    virtual float evaluate(float x, float y) const = 0;

    float width() const { return _width; };
    float height() const { return _height; };
    float inv_width() const { return _inv_width; };
    float inv_height() const { return _inv_height; };

protected:
    const float _width, _height;
    const float _inv_width, _inv_height;
};

}}

#endif
