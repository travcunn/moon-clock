# How Your E-Ink Moon Clock Works

Hey there,

I created a moon phase clock and I’m excited to share the details with you.

## Hardware Overview

I built this clock with carefully chosen parts that not only perform reliably but also has the capability of drawing detailed images:

- **ESP32 Microcontroller:** The reliable processor that handles all the tasks.
- **7.5" Z08 E-Ink Display:** Provides a paper-like, glare-free experience that remains crisp even in bright sunlight.
- **GPS Module:** Delivers ultra-accurate time and location data via satellite signals.
- **E-Paper Driver HAT:** Ensures that the e-ink display updates smoothly and precisely.

## Time and Date Management

I designed the clock to automatically sync with a GPS module, pulling in the exact time and location information. It smartly adjusts for time zones and even accounts for leap seconds and other astronomical quirks, so your clock stays accurate no matter where—or when—you are.

## Moon Phase Calculation

To capture the beauty of the lunar cycle, the moon phase is computed using a straightforward yet effective formula:

$$
L = \frac{(d \mod 29.53)}{29.53}
$$

Here, 
- \( d \) represents the number of days since a known new moon, and 
- \( L \) (ranging from 0 to 1) indicates your position in the lunar cycle—from one new moon to the next.

This simple fraction helps determine the moon's phase (new, crescent, quarter, gibbous, full) and the precise amount of visible illumination.

## Special Events & Deep Sleep Mode

I wanted the clock to be as energy-efficient as it is beautiful:

- **Special Dates:** On significant days, like major space milestones or celestial events, the clock displays themed images that celebrate these moments.
- **Deep Sleep Mode:** To conserve energy, the clock slips into a low-power state between updates, ensuring efficiency without missing a beat.

## Rendering the Moon Image and Shadow

A high-resolution photograph of the moon—captured with a William Optics Zenithstar 81 telescope on October 23, 2023—serves as the backdrop. I use this image to dynamically render the current lunar phase in real time.

### Drawing the Moon Shadow

The function `drawMoonPhase3D` brings the moon to life with realistic 3D illumination using Lambertian shading:

**1) Defining the Canvas:**  
It starts by calculating the moon’s center and radius within its display area.

**2) Determining Illumination Direction:**  
Depending on whether the moon is waxing (increasing) or waning (decreasing), the algorithm sets the light source to come from the right (for waxing) or left (for waning).

**3) Calculating Illumination:**  
The illuminated fraction $\( f \)$ is computed as:

$$
f = 1 - 2 \cdot \left| \phi - 0.5 \right|
$$

where $\( \phi \)$ is the normalized moon phase (0 at new moon, 0.5 at full moon, then back to 0).

The sun’s angle $\( \alpha \)$ is then derived:

$$
\alpha = \arccos(2f - 1)
$$

This gives the sun’s direction in the x–z plane:
- The $\( x \)$-component is $\( \sin(\alpha) \)$ (or its negative for waning).
- The $\( z \)$-component is $\( \cos(\alpha) \)$.

**4) Rendering:**  
The algorithm iterates over each pixel in the moon’s circle, calculates its surface normal, and applies a Lambertian dot product with the sun direction. Pixels with a non-positive dot product are rendered in shadow, creating a smooth and natural transition between light and dark.

## Fun Facts and Trivia

- **Lunar Cycle:**  
  The moon completes its cycle roughly every 29.53 days—a rhythm defined by the gravitational interplay of the Earth, Moon, and Sun.

- **E-Ink Display:**  
  E-ink technology replicates the look of real paper, reducing eye strain and remaining readable even under direct sunlight.

- **Celestial Mathematics:**  
  It’s amazing to think that the same mathematical principles governing planetary motion are behind the mechanics of this clock, blending art and science seamlessly.

## Source Code

I’ve made the full source code available on GitHub so you can dive into the details, contribute, or even fork it for your own tweaks: [github.com/travcunn/moon-clock](https://github.com/travcunn/moon-clock)

Enjoy watching time unfold, and let your new clock remind you that the Moon is ours... A timeless companion that lights our nights and brings us all together.

Cheers,  
Travis Cunningham
