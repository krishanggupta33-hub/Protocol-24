# Protocol-24: Engineering Journal 
**Maker:** Krishang Gupta
**Project:** Smart AC Climate Control Matrix (NodeMCU ESP8266)

## The Vision
Wanted to build something smart-home related that I'd actually use, not just a flex project. Three things were non-negotiable for Protocol-24: it had to be eco-friendly, it had to actually cut electricity use (no more AC blasting at freezing temps all night because someone forgot to bump it back up), and it had to be buildable without dropping money on some expensive proprietary hub.

## Day 1: Breadboard Prototype
First thing was just getting the proof of concept working on a breadboard — NodeMCU ESP8266, IR receiver, IR transmitter, all wired up. The only goal for day one was capturing the hex codes off my actual Voltas remote and confirming the microcontroller could blast them back out and actually trigger the AC. Nothing fancy, just making sure the core idea worked before building anything permanent.

## Going Stealth
Once the logic checked out, I went from prototype to something permanent. Soldered everything together to shrink the footprint, then opened up the AC unit itself and fit the whole soldered assembly inside it. No board sitting on the wall, no wires hanging out — completely invisible from the outside.

## Hooking Up Google Home
With the hardware sorted, it was time for the software side. Used Sinric Pro to bridge the ESP8266 to the cloud so I could link it to Google Home and control it by voice.

## Going Global
Voice control was cool, but I wanted to be able to control it from literally anywhere. So I built a custom HTML/JS dashboard. This is where things got annoying — browser CORS kept blocking my API calls no matter what I tried. Ended up fixing it by deploying the dashboard on Netlify and writing a custom `_redirects` proxy to route the commands properly.

End result: an invisible, eco-friendly AC mod I can control from anywhere on the planet.
