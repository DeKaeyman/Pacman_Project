# Report - Pacman Project (Advanced Programming 2025–2026)
Naam: Adriaan De Kaey  
Studentnummer: 20243910  
Repo: https://github.com/DeKaeyman/Pacman_Project  
Gameplay video (±10 min): <GAMEPLAY_VIDEO_URL>

## 1. Architectuur
### 1.1 Doelstellingen
Het project is opgezet met een strikte scheiding tussen game-logica en representatie, zodat:
- de logica onafhankelijk is van SFML en testbaar blijft,
- de UI/graphics later vervangen kan worden zonder logica te herschrijven.

### 1.2 Modules
- **logic/**: World, Entities, collisionregels, scoreberekening, AI, stopwatch/random
- **app/**: Game loop, SFML window, input mapping, rendering, states, HUD

### 1.3 Data flow (hoog niveau)
- App verwerkt input -> vertaalt naar acties op World (bv. “move left”).
- World update(tick): beweging * deltaTime, collisions, entity-interacties.
- World/Entities sturen events naar observers (views + score).
- App rendert views op basis van model-toestand.

---

## 2. Patronen
Deze patterns zijn verplicht in de opdracht en worden gebruikt om coupling laag te houden. 

### 2.1 MVC
- **Model**: World/Entities/Score
- **View**: SFML views per entity + HUD
- **Controller**: states en input vertaling naar world actions

### 2.2 Observer
Observer wordt gebruikt voor:
1) view-updates per tick (render) en animatie-events,
2) score-updates via gameplay events (coin/fruit/ghost eaten, death, direction).

### 2.3 Abstract Factory
World vraagt entities aan via een abstract interface, zodat logic geen SFML types kent. De concrete factory in de app attach’t meteen de juiste views

### 2.4 Singleton
- Stopwatch: deltaTime voor framerate-onafhankelijke logica (C++ chrono, geen SFML Clock).
- Random: één PRNG instantie (bv. Mersenne Twister), niet rand/srand.

### 2.5 State + State Stack
StateManager beheert een stack van states:
- Start: MenuState
- Play: LevelState
- Pause: PausedState push (pop = hervat)
- Victory / GameOver: aparte states met overgang naar volgende level of menu.

---

## 3. Quality
### 3.1 C++ practices
- const/override waar van toepassing
- expliciete initialisatie van primitieve types
- virtual destructors waar nodig om leaks te vermijden

### 3.2 Ownership & pointers
Smart pointers worden gebruikt voor ownership (unique/shared/weak afhankelijk van verantwoordelijkheid). Raw pointers worden enkel gebruikt waar smart pointers inhiberend zijn (bv. bepaalde observer-links), en dan non-owning + lifecycle veilig.

### 3.3 Exceptions & required files
De opdracht vereist exception handling bij afwezige required files (assets/config).
Keuze: “fail fast” bij missing assets (font/sprites) met duidelijke foutmelding, zodat een deployment-fout onmiddellijk zichtbaar is.

### 3.4 Dynamic casts & duplicatie
De opdracht vraagt expliciet om dynamic_cast te vermijden en duplicatie te reduceren via polymorfisme/templates.
(Indien nog aanwezig: plan om collision-afhandeling te refactoren via Command of Visitor.)

### 3.5 Formatting & comments
- clang-format volgens de meegeleverde configuratie
- API comments op kernklassen (World, Score, Ghost AI, State transitions)

---

## 4. AI
De ghost AI volgt exact de opdrachtdefinitie met:
- release timings (0s,0s,5s,10s),
- 4 types chasing gedrag,
- fear mode gedrag met Manhattan max in plaats van min,
- p-regel (p=0.5) voor de “locked direction ghost”.

### 4.1 Manhattan regels
Voor elke viable actie uit {up,down,left,right} wordt gekeken hoe Manhattan distance zou veranderen na één stap. Ties worden random gebroken.

### 4.2 Fear mode
Fear mode:
- activeert op fruit,
- ghosts trager + reverse,
- op kruispunten Manhattan distance maximaliseren,
- eaten ghost respawn in centrum en chase opnieuw.

---

## 5. Scaling
Level clear wanneer alle coins + fruits op zijn:
- respawn coins/fruits,
- ghosts terug naar centrum,
- score blijft behouden,
- ghosts worden sneller,
- fear duration wordt korter per level (concrete waarden gedocumenteerd in code).

---

## 6. Risico’s
- Platform verschillen (DLLs op Windows, referentieplatform Ubuntu)
- Asset paths / working directory issues
- Timing afhankelijkheden (deltaTime) en edge cases bij lage FPS
- (Indien van toepassing) remaining raw pointers in observer links

---

## 7. Future work
Mogelijke uitbreidingen (bonus):
- sounds/music
- smarter ghosts via BFS/A*
- random/procedural maps
- extra patterns (Command/Visitor) voor collision dispatch
- multithreading voor zware updates (met voorzichtigheid)