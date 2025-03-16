# How Your E-Ink Moon Clock Works

Hey there,

Congrats on getting your new E-Ink Moon Clock! I've been working on this for over a year and I'd like to share it with you.

## Hardware Overview

This clock runs on carefully chosen parts:

- **ESP32 Microcontroller:** The reliable processor that manages all tasks.
- **7.5" Z08 E-Ink Display:** Delivers a paper-like, glare-free display that stays crisp even in bright sunlight.
- **GPS Module:** Supplies ultra-accurate time and location data by tapping into satellite signals.
- **E-Paper Driver HAT:** Makes sure the e-ink display updates smoothly and precisely.

### Time and Date Management

The clock synchronizes with the GPS module to pull in precise time and location information. It then automatically adjusts for time zones and even accounts for leap seconds and other astronomical quirks. This keeps your clock ticking accurately no matter where (or when) you are.

### Moon Phase Calculation

The lunar phase is calculated using a simple yet effective formula:

$$
L = \frac{(d \mod 29.53)}{29.53}
$$

Here, 
- $\( d \)$ represents the number of days since a known new moon, and 
- $\( L \)$ (ranging from 0 to 1) marks your position in the lunar cycle—from one new moon to the next.

This fraction is then used to determine the moon's phase (new, crescent, quarter, gibbous, full) and the exact amount of illumination visible.

### Special Events & Deep Sleep Mode

- **Special Dates:** On significant days (major space milestones or celestial events), the displays shows themed images.
- **Deep Sleep Mode:** To save power, the clock slips into a low-power state between updates, keeping energy use efficient without missing a beat.

## Rendering the Moon Image and Shadow

The clock features a high-resolution photograph of the moon, captured using a William Optics Zenithstar 81 telescope on October 23, 2023. This image forms the backdrop for a dynamic, real-time rendering of the lunar phase.

### Drawing the Moon Shadow

The function `drawMoonPhase3D` creates a realistic 3D look of the moon’s illumination using Lambertian shading:

1. **Defining the Canvas:** It first calculates the moon’s center and radius within its display area.
2. **Determining Illumination Direction:** Based on whether the moon is waxing (increasing) or waning (decreasing), the algorithm sets the light to come from the right (for waxing) or left (for waning).
3. **Calculating Illumination:**
The illuminated fraction $\( f \)$ is computed as:
   
$$
f = 1 - 2 \cdot \left| \phi - 0.5 \right|
$$
   
where $\( \phi \)$ is the normalized moon phase (0 at new moon, 0.5 at full moon, then back to 0).
   
The sun’s angle $\( \alpha \)$ is derived from:
   
$$
\alpha = \arccos(2f - 1)
$$
   
This provides the sun’s direction in the x–z plane:
     
The $\( x \)$-component is $\( \sin(\alpha) \)$ (or its negative for waning).
The $\( z \)$-component is $\( \cos(\alpha) \)$.
   
4. **Rendering:** The algorithm iterates over each pixel within the moon’s circle, computes its surface normal, and applies a Lambertian dot product with the sun direction. Pixels where the dot product is non-positive are rendered in shadow, creating a smooth, natural transition between light and dark areas.

## Fun Facts and Trivia

- **Lunar Cycle:** The moon completes its cycle roughly every 29.53 days—a rhythm dictated by the gravitational interplay between the Earth, Moon, and Sun.
- **E-Ink Display:** E-ink mimics the look of real paper, offering low eye strain and readability even in direct sunlight.
- **Celestial Mathematics:** The same mathematical principles that govern planetary motion are at work in this clock, merging art and science seamlessly.

## Source Code

Check out the source code on GitHub: [github.com/travcunn/moon-clock](https://github.com/travcunn/moon-clock)

Enjoy watching the passage of time, and let it remind you of the vast, beautiful universe we live in.

Cheers,  
Travis Cunningham
