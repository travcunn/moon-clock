# How Your E-Ink Moon Clock Works: A Journey Through Time and Space

Hey there,

Congrats on getting your new E-Ink Moon Clock—a cool blend of art, engineering, and a dash of celestial magic. Let’s break down how this clock ticks (and glows) without any fluff.

## Hardware Overview

This clock runs on carefully chosen parts that balance performance with a sleek look:

- **ESP32 Microcontroller:** The reliable processor that manages all tasks.
- **7.5" Z08 E-Ink Display:** Delivers a paper-like, glare-free display that stays crisp even in bright sunlight.
- **GPS Module:** Supplies ultra-accurate time and location data by tapping into satellite signals.
- **E-Paper Driver HAT:** Makes sure the e-ink display updates smoothly and precisely.

Each component was handpicked to ensure that your clock isn’t just about showing time—it’s about doing it with style and precision.

## The Software and Algorithms

Behind the scenes, a suite of smart algorithms makes sure your clock is always in sync with the cosmos.

### Time and Date Management

The clock synchronizes with the GPS module to pull in precise time and location information. It then automatically adjusts for time zones and even accounts for leap seconds and other astronomical quirks. This keeps your clock ticking accurately no matter where (or when) you are.

### Moon Phase Calculation

The lunar phase is calculated using a simple yet effective formula:

$$
L = \frac{(d \mod 29.53)}{29.53}
$$

Here, \( d \) represents the number of days since a known new moon. The value \( L \) (ranging from 0 to 1) marks your position in the lunar cycle—from one new moon to the next. This fraction is then used to determine the moon's phase (new, crescent, quarter, gibbous, full) and the exact amount of illumination visible.

### Special Events & Deep Sleep Mode

- **Special Dates:** On significant days—think major space milestones or celestial events—the clock spices things up with themed images and messages.
- **Deep Sleep Mode:** To save power, the clock slips into a low-power state between updates, keeping energy use efficient without missing a beat.

## Rendering the Moon Image and Shadow

The clock features a high-resolution photograph of the moon, captured using a William Optics Zenithstar 81 telescope on October 23, 2023. This image forms the backdrop for a dynamic, real-time rendering of the lunar phase.

### Drawing the Moon Shadow

The function `drawMoonPhase3D` creates a realistic 3D look of the moon’s illumination using Lambertian shading:

1. **Defining the Canvas:** It first calculates the moon’s center and radius within its display area.
2. **Determining Illumination Direction:** Based on whether the moon is waxing (increasing) or waning (decreasing), the algorithm sets the light to come from the right (for waxing) or left (for waning).
3. **Calculating Illumination:**
   - The illuminated fraction \( f \) is computed as:
     $$
     f = 1 - 2 \cdot \left| \phi - 0.5 \right|
     $$
     where \( \phi \) is the normalized moon phase (0 at new moon, 0.5 at full moon, then back to 0).
   - The sun’s angle \( \alpha \) is then derived from:
     $$
     \alpha = \arccos(2f - 1)
     $$
     This gives the sun’s direction in the x–z plane. The \( x \)-component is \( \sin(\alpha) \) (or its negative for waning), and the \( z \)-component is \( \cos(\alpha) \).
4. **Rendering:** The algorithm goes through each pixel in the moon’s circle, computes its surface normal, and applies a Lambertian dot product with the sun direction. Pixels where the dot product is non-positive are rendered in shadow, creating a smooth, natural transition between light and dark areas.

## Fun Facts and Trivia

- **Lunar Cycle:** The moon completes its cycle roughly every 29.53 days—a rhythm dictated by the gravitational interplay between the Earth, Moon, and Sun.
- **E-Ink Display:** E-ink mimics the look of real paper, offering low eye strain and readability even in direct sunlight.
- **Celestial Mathematics:** The same mathematical principles that govern planetary motion are at work in this clock, merging art and science seamlessly.

## Conclusion

Your E-Ink Moon Clock isn’t just a tool for telling time—it’s a tribute to the wonder of the universe. From syncing with GPS satellites to dynamically shading a real photograph of the moon, every element is designed with precision and a passion for the cosmos.

## Source Code

For the nitty-gritty details, updates, and to contribute, check out the source code on GitHub: [github.com/travcunn/moon-clock](https://github.com/travcunn/moon-clock)

Enjoy watching the passage of time, and let it remind you of the vast, beautiful universe we live in.

Cheers,  
Travis Cunningham
