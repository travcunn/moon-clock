# How Your E-Ink Moon Clock Works: A Journey Through Time and Space

Dear Recipient,

Congratulations on receiving your new E-Ink Moon Clock—a marvel of art, engineering, and celestial computation! This document is my personal guide, explaining in detail the science and ingenuity behind the clock. Whether you’re a seasoned physicist, a math enthusiast, or simply a lover of the cosmos, I hope you find this journey as fascinating as the universe itself.

## Hardware Overview

This clock is built around cutting-edge components that I personally selected for both performance and aesthetic appeal:

- **ESP32 Microcontroller:** The powerful brain that orchestrates all operations.
- **7.5" Z08 E-Ink Display:** Offers a paper-like, glare-free visual experience.
- **GPS Module:** Provides precise time and geographic data.
- **E-Paper Driver HAT:** Ensures smooth, accurate rendering on the e-ink display.

I have meticulously chosen each component to ensure your clock not only tells time but does so with extraordinary precision and beauty.

## The Software and Algorithms

At the core of this clock lie a series of algorithms, each contributing to the dynamic display of time, date, and the moon’s phase.

### Time and Date Management

Timekeeping is achieved by synchronizing with the GPS module, which supplies highly accurate time and location data. Advanced algorithms adjust for time zone differences and ensure seamless transitions even as the clock compensates for leap seconds and other astronomical variations.

### Moon Phase Calculation

The moon phase is computed using elegant astronomical equations. One fundamental formula I use is:

$$
L = \frac{(d \mod 29.53)}{29.53}
$$

where:

- \( d \) is the number of days since a known new moon,
- \( L \) represents the fractional position in the lunar cycle.

This calculation provides the basis for determining not only the phase (new, crescent, quarter, gibbous, full) but also the precise amount of illumination visible on the moon.

### Special Events & Deep Sleep Mode

To celebrate astronomical events and historical milestones:

- **Special Dates:** The clock dynamically displays themed images and messages on significant dates, honoring space exploration and other celestial events.
- **Deep Sleep Mode:** An ingenious power-saving feature puts the clock into a low-power state until the next update, ensuring efficient energy use without sacrificing performance.

## Rendering the Moon Image and Shadow

At the heart of the display is a stunning photograph of the moon that I captured using a William Optics Zenithstar 81 telescope, with acquisition settings of `200 × 2.00ms` on October 23rd, 2023. This image serves as the canvas upon which the current lunar phase is artistically rendered.

### Drawing the Moon Shadow

The function `drawMoonPhase3D` implements a realistic 3D simulation of the moon’s illumination using Lambertian shading. First, it calculates the center and radius of the moon's circular bounding box and determines whether the phase is waxing or waning, which sets the direction of illumination (right for waxing, left for waning).

Next, it computes the illuminated fraction \( f \) as:

$$
f = 1 - 2 \cdot \left| \phi - 0.5 \right|
$$

where \( \phi \) is the moon phase (0 for new, 0.5 for full, and returns to 0 for the next new moon). From \( f \), the sun’s angle \( \alpha \) is derived using:

$$
\alpha = \arccos(2f - 1)
$$

This yields a sun direction vector in the x–z plane with:

- \( x \)-component: \( \sin(\alpha) \) (or \( -\sin(\alpha) \) for waning),
- \( z \)-component: \( \cos(\alpha) \).

The algorithm then iterates over each pixel within the moon’s bounding box. For pixels inside the moon’s circle, it computes normalized coordinates to determine the surface normal at that point. A Lambertian dot product between this normal and the sun direction vector is then calculated; if the dot product is non-positive, the pixel is rendered in shadow. This technique produces a smooth, realistic transition of illumination, accurately capturing the subtle interplay of light and shadow on the lunar surface.

## Fun Facts and Trivia

- **Celestial Rhythms:** The lunar cycle lasts approximately 29.53 days, a period finely tuned by the gravitational dance of the Earth, Moon, and Sun.
- **E-Ink Magic:** Unlike traditional displays, the e-ink technology mimics paper, reducing eye strain and remaining visible even under direct sunlight.
- **Mathematical Beauty:** The same equations that govern the path of celestial bodies are at work in this clock, uniting art and science in a symphony of precision and creativity.

## Conclusion

Your E-Ink Moon Clock is more than just a timepiece—it is an intersection of advanced technology, meticulous engineering, and cosmic artistry. Every detail, from the precise synchronization with GPS data to the dynamic rendering of lunar shadows, reflects countless hours of design and development. I hope that, as you gaze upon your clock, you are reminded of the incredible wonders of the universe and the passion I have for unraveling its mysteries.

## Project Structure Overview

For those who appreciate the craftsmanship behind the scenes, here’s a brief overview of the project’s structure:

- **src/** — Contains the source code, including main application logic, moon phase calculations, event handling, and timezone computations.
- **docs/** — Hosts additional documentation and visual assets.
- **images/** — Holds all the images used in the project, including the featured moon photograph.
- **include/** and **lib/** — Provide supporting files and libraries that enable seamless hardware-software integration.
- **platformio.ini:** The configuration file for PlatformIO, which simplifies the building and uploading of firmware.
- **clock.code-workspace:** A Visual Studio Code workspace file configured to streamline the development environment.

This structured approach facilitates both innovation and maintenance, ensuring that every component contributes to the overall masterpiece that is your E-Ink Moon Clock.

## Source Code

For full details, updates and contributions, please visit the source code repository on GitHub: [https://github.com/travcunn/moon-clock](https://github.com/travcunn/moon-clock)

Enjoy your journey through time and space!

Sincerely,  
Travis Cunningham
