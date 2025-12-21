# Pacman (C++20 / SFML) - Adriaan De Kaey - Student: 20243910

![CI](https://github.com/DeKaeyman/Pacman_Project/actions/workflows/ci-build.yml/badge.svg)  
Repo: https://github.com/DeKaeyman/Pacman_Project  
Gameplay video (±10 min): <GAMEPLAY_VIDEO_URL>

## Overzicht
Deze repository bevat een Pac-Man geïnspireerde game in C++ met SFML. De code is opgesplitst in:
- **`logic/`**: game-logica als standalone library (geen SFML-afhankelijkheden)
- **`app/`**: SFML representatie (rendering, input, states, UI)

De scheiding is bewust: de logica kan in theorie gebouwd worden zonder SFML en kan later met een andere UI/graphics library gekoppeld worden.

---

## Vereisten
- CMake >= 3.28
- C++ compiler met **C++20**
- SFML 2.6.x

Referentieplatform (labs): Ubuntu 24.04.3, SFML 2.6.1, CMake 3.28.3, G++ 13.2.0, Clang 18.0.0.

---

## Build & Run
### Configure + build
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build
````

### Run

* Run de executable via je IDE target (**app**) of uit de build map.

### Tests

```bash
cmake -S . -B build -DBUILD_TESTING=ON
cmake --build build
ctest --test-dir build
```

### Install (assets mee installeren)

```bash
cmake --install build --prefix install_dir
```

Assets komen dan in:
`install_dir/share/pacman/assets`

---

## Windows (SFML DLLs)

Als je op Windows `0xC0000135` krijgt, ontbreekt meestal een runtime DLL (SFML of MinGW).
Voeg in je run-config de volgende folders toe aan PATH:

* `<SFML_ROOT>\bin`
* `<MSYS2>\mingw64\bin`

---

## Controls

* **Arrow keys / WASD**: bewegen
* **Escape**: pauze
* **Any key**: bevestigen in menu/victory/game over

---

## Gameplay features (verplicht)

* Startscherm met **top 5 highscores** + “Play” om te starten.
* Continu bewegen + wall collision, Pac-Man blijft richting volgen tot nieuwe input of wall-collision.
* 4 ghosts met release timings: 2 meteen, 3e na 5s, 4e na 10s.
* Score: score daalt over tijd, coins geven score die afhangt van tijd sinds vorige coin, bonus voor level clear + fruit + ghosts.
* Fruits in **upper-left** en **bottom-right**, fruit activeert fear mode: ghosts trager + reverse + op kruispunten Manhattan **maximaliseren**, ghosts kunnen gegeten worden en respawnen in het midden.
* Level scaling: nieuwe level respawn coins + fruits + ghosts naar centrum, ghosts sneller + fear duration korter per level, score blijft behouden.
* 3 levens, bij touch ghost: life--, Pac-Man + ghosts reset posities, coins/fruits blijven verzameld, bij 0 levens game over -> terug naar menu.

---

## AI beslissingsboom (p-regel + Manhattan regels)

Ghosts kiezen enkel actief een richting aan **kruispunten/hoeken** (beslissingspunten). In corridors blijven ze doorlopen.

### Algemene regels

1. **Ongeldige moves**: muren zijn niet toegestaan.
2. **No immediate reverse**: doorgaans geen onmiddellijke omkering (behalve bij fear mode start waar reverse expliciet gevraagd wordt).
3. Bij ties: breek gelijkstand random.

### Chasing mode: 4 ghost types (verplicht)

1. **Ghost A (random lock ghost)**

  * Houdt een “locked direction” aan.
  * Aan kruispunt/hoek: heroverweeg lock, met kans **p = 0.5** lock naar een willekeurige geldige richting.

2. **Ghost B + C (Pac-Man facing ghosts)**

  * Doel = “tile(s) in front of Pac-Man” (afhankelijk van direction).
  * Kies de actie die de Manhattan distance naar dat doel minimaliseert.

3. **Ghost D (direct chase ghost)**

  * Minimaliseert Manhattan distance naar Pac-Man zelf.

### Fear mode (fruit)

* Fear mode is tijdelijk.
* Ghosts worden trager + reverse.
* Aan kruispunten: kies actie die Manhattan distance **maximaliseert** (wegvluchten).

---

## Architectuur & patterns

* **MVC**: Model (`logic/*`), View (`app/views/*`), Controller (`app/states/*`)
* **Observer**: Views en Score reageren op events (ticks, coin/fruit/ghost eaten, death, direction changes).
* **Abstract Factory**: World kan entities maken zonder SFML-kennis, app levert ConcreteFactory.
* **Singleton**: Stopwatch + Random (1 instantie).
* **State + State stack**: Menu -> Level -> Paused/Victory/GameOver, Pause is push/pop zodat level hervat.

---

## Documentatie

* `docs/DesignOverview.md` (kort ontwerp-overzicht, ~2 A4)
* `REPORT.md` (architectuur, patterns, quality, AI, scaling, risico’s, future work)