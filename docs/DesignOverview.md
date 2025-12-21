# Design Overview (±2 A4) - Pacman Project
Naam: Adriaan De Kaey - Student: 20243910

## 1. Probleem & doel
Het doel van dit project is een Pac-Man geïnspireerde game te bouwen met focus op een sterke software-architectuur, moderne C++ practices, en een duidelijke scheiding tussen game-logica en representatie. De opdracht vraagt expliciet om de logica als aparte library te compileren zonder SFML, en de SFML-laag daar bovenop te linken.

## 2. Architectuurkeuze: logic library + app
Ik heb het project opgesplitst in twee modules:
- **logic**: alle spelregels, entity states, collision detectie, AI, scoreberekening, timing (Stopwatch) en random (Random).
- **app**: SFML window, main loop, input mapping, UI/states, rendering en animaties.

Deze scheiding dwingt een “clean boundary” af: logic bevat geen SFML types of utilities. Dit verhoogt testbaarheid en maakt het mogelijk om later een andere UI te bouwen (bv. console rendering) zonder wijzigingen in de kernlogica.

## 3. Tijd & framerate onafhankelijk gedrag (Stopwatch)
Beweging en animaties moeten onafhankelijk zijn van framerate. Daarom gebruikt logic een Stopwatch die deltaTime levert via C++ timing utilities (geen SFML Clock). Elke update verplaatst entities met: distance = speed * deltaTime. Dit voorkomt “busy waiting” en laat een dynamische framerate toe (eventueel met max cap in de app).

## 4. Core gameplay-onderdelen (samengevat)
- Startscherm: top-5 highscores + Play knop.
- Coins: score daalt over tijd, coin-score hangt af van tijd sinds vorige coin.
- Fruits: fear mode triggert tijdelijk, ghosts trager + reverse, AI kiest Manhattan max i.p.v. min.
- Levels: respawn coins/fruits en reset posities, difficulty scaling (ghost speed omhoog, fear duration omlaag).
- Lives: start met 3, bij ghost touch life-- en reset posities, bij 0 terug naar menu.

## 5. AI ontwerp: beslissingspunten + Manhattan + p-regel
Ghosts heroverwegen richting enkel aan kruispunten/hoeken. Dit beperkt computation en maakt gedrag consistent met grid movement.

### 5.1 Chasing mode: 4 ghosts
1) Locked-direction ghost:
- kiest een direction en blijft die volgen,
- bij kruispunt/hoek heroverwegen,
- met kans p = 0.5 kiest hij een random viable direction.

2) Twee “facing” ghosts:
- doel = positie(s) “in front of” Pac-Man gebaseerd op de direction waarin Pac-Man kijkt,
- kies direction die Manhattan distance tot dat doel minimaliseert (ties random).

3) Direct chase ghost:
- minimaliseert Manhattan distance tot Pac-Man zelf.

### 5.2 Fear mode
Fear mode wordt geactiveerd door fruit:
- snelheid omlaag,
- reverse direction bij start,
- aan kruispunten: maximize Manhattan distance (wegvluchten),
- eaten ghost respawn in centrum en chase hervatten.

## 6. Patterns (waarom deze)
- **MVC**: model (logic) blijft los van view (SFML). Dit is kern van de architectuur-eis.
- **Observer**: views en score reageren op events zonder tight coupling, score en animatie-triggers komen uit dezelfde eventstroom.
- **Abstract Factory**: World kan entities aanmaken zonder SFML-kennis, app bindt views.
- **Singleton**: Stopwatch/Random als unieke instanties, eenvoudig toegankelijk
- **State + stack**: pause werkt via push/pop zodat LevelState kan hervatten, transitions gebeuren door states zelf.

## 7. Build & CI
Project bouwt met CMake (C++20) met targets `logic`, `app` en `tests`. Assets worden mee geïnstalleerd via install rules. CI is opgezet zodat elke commit een compile check heeft (zoals gevraagd).

## 8. Afsluitende motivatie
De gekozen structuur maximaliseert uitbreidbaarheid (nieuwe entities, extra levels, andere UI) en minimaliseert coupling. Het gebruik van patterns is niet “cosmetisch”: elk pattern ondersteunt expliciet een eis uit de opdracht (separation, score/events, factories, timekeeping, state transitions).