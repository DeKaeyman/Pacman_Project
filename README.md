* [x] Maak een cleane projectstructuur met twee targets: **logic** (static/dynamic lib, geen SFML-dependentie) en **app** (SFML representatie). Mappen: `logic/` (world, entities, utils), `app/` (views, states, game), `assets/`, `cmake/`, `tests/`. Zet `CMakeLists.txt` zo dat `app` linkt tegen `logic`. 
* [x] Voeg namespaces toe (`pacman::logic`, `pacman::app`). 
* [x] Definieer in `logic/` een **World** klasse (model/controller) die entiteiten beheert, update-t en botsingen detecteert (rect-intersectie, zonder SFML). Plan hier ook level-reset & level-advance. 
* [x] Maak in `app/` een **Game** bootstrapper die enkel de SFML-window aanmaakt, loop draait en een **StateManager** voorbereidt (geen spelregels hier). Bestand: `app/Game.cpp`. 
* [x] Zet een **StateManager** met stack (MenuState → LevelState → Paused/Victory/GameOver). Elke state beslist eigen transities; StateManager kent concrete states niet. Bestanden: `app/states/*`. 
* [x] Voorzie in `app/` een **AbstractFactory** interface in `logic/` (publieke header) en een **ConcreteFactory** in `app/` die Views koppelt aan Models bij creatie van entiteiten. World krijgt enkel de abstracte factory. 
* [x] Introduceer **MVC**: elk logic-model heeft een view in `app/`. Updates lopen via Observer-events; View tekent, Model bevat geen SFML. Plaats Model in `logic/entities/`, View in `app/views/`. 
* [x] Bouw een **Stopwatch** als Singleton (C++ chrono, géén SFML Clock): levert `deltaTime` voor consistente snelheid/animaties. Header in `logic/utils/Stopwatch.hpp`. 
* [x] Bouw een **Random** als Singleton (Mersenne Twister + distributions). Gebruik één generator-instantie; geen `rand/srand`. Bestand: `logic/utils/Random.hpp`. 
* [x] Definieer in `logic/camera/Camera.hpp` een projectie: wereldcoördinaten in [-1,1] × [-1,1] naar pixels. Gebruik dit in Views om spriteposities en groottes af te leiden. 
* [x] Start **MenuState** (app): tekent titel, top-5 highscores, en “Play” knop. Interactie: muisklik/Enter → push LevelState. Bestand: `app/states/MenuState.cpp`. 
* [x] Leg highscore-bestanden vast (pad in `assets/data/highscores.txt`). Score klasse in `logic/score/Score.hpp` beheert score en persistente top-5. MenuState leest en toont. 
* [x] Maak in `logic/world/TileMap.hpp` een beschrijving van het doolhof (repliceer figuur; walls, coins, fruits). World laadt/maakt grid en entiteiten. Representatie van muren kan in views met SFML-shapes. 
* [x] Voeg **Coin** en **Fruit** modellen toe (collectables). World houdt hun posities bij; Observer-events bij oppakken. Views tekenen simpele sprites uit `assets/sprites`. 
* [x] Maak **PacMan** model (positie, richting, snelheid; continue beweging). Logic beslist beweging & botsing, géén SFML. Bestand: `logic/entities/PacMan.hpp`. 
* [x] Koppel **PacManView** in `app/views/PacManView.cpp`: kiest sprite op basis van richting; animatie mond open/dicht o.b.v. Stopwatch-tijd. Observer-update triggert redraw. 
* [x] Laat **LevelState** (app) een World instantie creëren, ConcreteFactory injecteren en input vertalen naar “actions” (links/rechts/omhoog/omlaag), zonder logic binnen de state. Bestand: `app/states/LevelState.cpp`. 
* [x] Implementeer in World continue beweging: Pac-Man volgt laatste richting tot nieuwe input of botsing met wall; botsing stopt/clipt beweging. Gebruik deltaTime snelheid. 
* [x] Voeg **Ghost** basisklasse (model) toe met subtypes (4 stuks). Opslag: center-spawn. Snelheid als parameter; gedrag per type in strategie-methode. 
* [x] Definieer **GhostView**: sprite per richting; animatie van “lopen”; ogen tonen locked richting. Tijdens fear mode: blauw sprite. Bestand: `app/views/GhostView.cpp`. 
* [x] Integreer **Observer**: Models publiceren events (tick, moved(dir), collected, died, modeChanged). Views en Score zijn observers; attach bij creatie via Factory. 
* [x] Plaats **coins** gelijkmatig volgens layout; World detecteert overlap Pac-Man ↔ coin en verstuurt `CoinCollected`. Score verhoogt met tijdsafhankelijke bonus (snelle ketens = meer). 
* [x] Toon **HUD** (app): huidige score linksboven, resterende levens, level-nummer. Bestand: `app/ui/Hud.cpp`. 
* [x] Implementeer **Score** logica: basis-decrement over tijd, coins vergroten score met factor gebaseerd op tijd sinds laatste coin; fruits & ghosts geven bonus; score gaat mee naar volgende level. 
* [x] Zet **Fruit** posities (linksboven en rechtsonder). World detecteert FruitCollected → zet ghost-modus naar **fear** met timer; laat ghosts trager gaan en hun richting omkeren. 
* [x] Maak **fear-timer** in World; bij fear: ghost kiest acties die de Manhattan-afstand tot Pac-Man maximaliseren (viable moves), ties random. Na timeout terug naar chasing. 
* [x] Werk **Ghost AI** (chasing) uit:

  1. Ghost A: altijd locked richting tot corner/intersection; bij hoek/keuze: met p=0.5 lock naar random viable richting.
  2. Ghost B & C: kies move die min. Manhattan-afstand tot “voor Pac-Man” geeft (kijk naar Pac-Man facing).
  3. Ghost D: min. Manhattan-afstand tot Pac-Man positie. 
* [x] Stel release-timers in: 2 ghosts starten meteen, 2 na 5s en 10s (Stopwatch). World start klokken bij levelstart. 
* [x] Implementeer **collision** Pac-Man ↔ Wall: continuous beweging stopt tegen muur, geen penetratie. Logic: AABB/segment sweep; geen SFML helpers in logic. 
* [x] Implementeer **collision** Pac-Man ↔ Ghost: in chasing → Pac-Man verliest 1 leven; reset posities naar start; verzamelde items blijven verzameld. In fear → ghost “gegeten” → respawn center, direct chasing. 
* [x] Voeg **lives** systeem toe (start met 3). HUD toont resterende levens. Bij 0: LevelState pusht Victory/GameOver state terug naar menu. 
* [ ] Fix all point increases like ghost deaths pacman deaths and so on
* [ ] Fix it so max 20fps
* [ ] Fix ghosts phasing trough walls when coming out of spawn
* [ ] Detecteer **level clear**: alle coins & fruits opgegeten → verhoog level, bonuspunten, respawn items, ghosts naar center, behoud score & levens. Ghosts sneller, fear korter per level. 
* [ ] Breid **State** flow uit: Escape → PausedState; van Paused terug naar Level; bij GameOver → Menu; bij Victory → volgende LevelState. Gebruik stack-push/pop. 
* [ ] Leg **input-mapping** vast in LevelState (app): pijlen zetten gewenste richting in World; geen logic in app; alleen doorgeven van intenties.
* [ ] Plaats **Camera** gebruik in tekenpad: Views vragen Camera om wereld→pixel; pas window resize toe zonder logic aan te raken.
* [x] Maak **AssetManager** in app om textures/fonts/sounds te cachen; paths in `assets/`. (Optioneel geluid later.)
* [x] Richt **animation system** in views: frame-duur op Stopwatch gebaseerd; Pac-Man mond toggelt; ghosts lopen; fear kleur blauw. 
* [ ] Voeg **scoreboard** persistentie toe: Score schrijft top-5 bij game-over; MenuState leest en toont boven “Play”. 
* [x] Introduceer **Entity** basis in logic (id, bbox, update). World beheert collecties en iteraties; vermijd dynamic_cast; gebruik polymorfisme/visitors indien nodig. 
* [x] Definieer **Observer** interfaces: Subject (attach/detach/notify), Observer (onEvent). Views en Score subscriben op Models; events zijn klein (enum + payload). 
* [x] Voeg **AbstractFactory** methods toe: createPacMan, createGhost(type), createCoin, createFruit, createWall. ConcreteFactory maakt model + view en koppelt Observer. 
* [x] Teken **maze walls** in app met SFML shapes (esthetiek hoeft niet exact). Map-definitie blijft in logic. 
* [ ] Implementeer **StartFlow**: App start in MenuState (scoreboard + Play); klik Play → LevelState init met Level 1, spawnt entiteiten via factory. 
* [ ] Voeg **difficulty scaling** parameters toe in World: ghostSpeed += Δ, fearDuration *= factor<1 per level. Stel concrete waardes in en documenteer. 
* [ ] Maak **config** (ini/json/toml) voor snelheden, timings, tilegrootte; laad in app en geef door aan logic bij World-constructie.
* [ ] Bewaak **codekwaliteit**: smart pointers overal; expliciete initialisaties; const/override; geen memory leaks (virtuele destructors); referenties i.p.v. kopieën. Voeg clang-format config toe. 
* [ ] Schrijf **exceptions** voor missende assets/config; vang in app en toon foutmelding in MenuState. 
* [ ] Integreer **frame cap** (optioneel) in app loop (bijv. 60 FPS), maar logic draait met Stopwatch deltaTime (geen busy waiting). 
* [ ] Verfijn **path viability**: bij AI beslissingen alleen richtingen zonder wall-botsing; random keuzes via Random singleton. 
* [ ] Werk **turning** uit: Pac-Man accepteert bufferd richting bij naderen van kruising; als viable, wissel richting soepel.
* [ ] Voeg **tunnels** toe (optioneel): teleport links↔rechts; update Camera/Views voor wrap-around.
* [ ] Implementeer **ghost house** logica (center spawn) en release-poort; eaten ghosts keren als ogen terug naar center en hergroenen naar chasing. 
* [ ] Breid **HUD** uit met timers: fear-resttijd, level, score multiplier (op basis van coin-combo). 
* [x] Maak **score-multipliers** coherent met eisen: hoe korter tussen twee coins, hoe groter de bonus; reset na pauze of bij schade. 
* [ ] Plaats **lives UI** als kleine pacman-icons rechtsboven; update via Observer bij life-change.
* [ ] Zet **GameOver flow**: bij 0 levens → GameOverState met score, naaminput (optioneel) en “Back to Menu”.
* [ ] Zet **Victory flow**: bij level clear → VictoryState met bonuspunten en “Next Level”.
* [ ] Ontkoppel **input** van framerate: verzamel events, vertaal naar intenties; World consumeert per update met deltaTime.
* [ ] Test **logic-lib** los: maak een kleine console test (zonder SFML) die World simuleert en asserts draait op botsingen/AI beslissingen.
* [ ] Schrijf **unit tests** (Catch2/GoogleTest) voor Score, Stopwatch, Random, collisions, AI keuzes (Manhattan).
* [ ] Documenteer **AI beslissingsboom** in comments en README (uitleg p en Manhattan regels). 
* [ ] Optimaliseer **collision broadphase** (grid buckets) om iteraties te beperken bij grotere levels.
* [ ] Voeg **sound hooks** (optioneel) in app toe: chomp, fruit, death, start-jingle. Logic triggert events; app speelt audio. (Bonus idee.) 
* [ ] Implementeer **pause overlay** met donker filter en menu (Resume/Restart/Menu).
* [ ] Zorg voor **resize-safe UI** (HUD en sprites schalen via Camera; geen magic pixels).
* [x] Verbeter **sprite selectie**: Pac-Man/ghost sprite index op richting; ogen tonen lock-richting. 
* [ ] Voeg **tile snapping** toleranties toe zodat richtingwissel bij kruising natuurlijk aanvoelt.
* [x] Maak **level loader** (tekst/bitmap) die walls/coins/fruits in World vult; zo kun je snel layout wijzigen.
* [ ] Automatiseer **build**: CMake targets `logic`, `app`, `tests`; install rules voor assets; compileer met C++20.
* [ ] Configureer **CI** (bijv. CircleCI) om op Ubuntu 24.04.3 met SFML 2.6.1, CMake 3.28.3, g++13/clang18 te bouwen; toon badge in README. 
* [ ] Voeg **clang-format** (geleverd config) en pre-commit hook toe; format alle bestanden. 
* [ ] Controleer **smart pointer ownership** (unique voor exclusief, shared waar nodig, weak om cycles te breken). Geen raw pointers behalve waar patroon vereist (documenteer). 
* [ ] Vermijd **code duplicatie**: factoriseer via polymorfisme/templating (bijv. observers/visitors). 
* [ ] Voeg **documentation** toe: korte ontwerp-overzicht (2 A4), designkeuzes, patronen, extensies, link naar gameplay-video (10 min) en repo. 
* [ ] Maak **report** secties: architectuur, patronen (MVC/Observer/Factory/Singleton/State), quality, AI, scaling, risico’s, future work. 
* [ ] Leg **inputs** vast: pijlen = richting, Enter = start, Esc = pauze; UI tooltips in Menu/HUD.
* [ ] Beperk **dependencies** in logic: alleen STL/chrono/random; geen SFML headers in logic.
* [ ] Test **edge cases**: tunnel wrap, simultane botsing coin+wall, fear timeout precies op hoek.
* [ ] Tune **speeds**: Pac-Man iets sneller dan ghosts in fear; in hogere levels ghostSpeed omhoog, fear korter. 
* [ ] Implementeer **ghost cornering**: bij kruising evalueren alle viable moves; breek ties random; voorkom 180° omkeer buiten fear tenzij nodig.
* [ ] Voeg **spawn invulnerability** korte duur voor Pac-Man na verlies van leven om spawn-kill te vermijden.
* [ ] Optimaliseer **render volgorde**: walls → coins → fruits → ghosts → pacman → HUD; reduceer state changes (textures).
* [ ] Voeg **debug overlay** (F1) toe: FPS, deltaTime, entity count, AI keuze, bounding boxes.
* [ ] Maak **replay/record** (optioneel): input loggen voor debug; logic deterministic met seed uit Random. (Bonus.) 
* [ ] Verwerk **errors** gracieus: ontbrekende sprite → placeholder; highscore bestand niet schrijfbaar → waarschuwing. 
* [ ] Finaliseer **MenuState** visuals: titel, top-5, Play knop centraal; muis/keyboard focus. 
* [ ] Finaliseer **LevelState** loop: input → World actions; World update(dt) → events → Views/HUD tekenen; check transitions.
* [ ] Finaliseer **PausedState**: stack-pop om terug te keren; geen World hercreatie. 
* [ ] Finaliseer **Victory/GameOver** schermen met knoppen (Menu/Next). State maakt nieuwe World voor volgend level. 
* [ ] Check **compilatie** op referentieplatform; assets meegeleverd; run scriptje `./run.sh`. 
* [ ] Speel **volledige playthrough**: start → level1 clear → level2 sneller/korter fear → verlies leven → respawns → game over → menu.
* [ ] Verfijn **aesthetics**: lichte tweening bij direction change, subtiele shadows/bloom (indien gewenst), consistente kleuren. 
* [ ] Kalibreer **score**: basis-decrement tempo, coin base value, chain-multipliers, level-clear bonus, ghost/fruit bonus; leg vast in config. 
* [ ] Bevestig **sprites**: gebruik set uit figuur (genoeg frames), fear = blauw, ogen tonen richting; niet alle figuren nodig. 
* [ ] Valideer **AI p=0.5** gedrag bij locked-ghost op kruising; log keuzes om te zien of verdeling klopt. 
* [ ] Zorg dat **scoreboard** zichtbaar is bij game start; update na game over; persist naar file. 
* [ ] Voeg **input buffering** toe zodat richting alvast kan worden ingegeven en toegepast zodra viable.
* [ ] Test **performance** op lagere hardware; verlaag draw calls; gebruik texture atlases.
* [ ] Schrijf **README** met build-instructies, controls, features, patronen en CI badge + student-id. 
* [ ] Neem **video** op: 3 min gameplay + 7 min design/implementatie; link in rapport. 
* [ ] Voeg **bonus features** indien tijd: geluid/muziek, slimmere AI (BFS/A*), random maps, templates/extra patronen, multithreading (met zorg). 
* [ ] Doe **valgrind/asan** run; check leaks en UB; fix virtuele destructors en initialisatie. 
* [ ] Run **static analysis** (clang-tidy) voor referenties i.p.v. kopieën, const-correctness, no exceptions leaks. 
* [ ] Final QA: window scaling, HUD clipping, input latency, pause resume, state transitions, level carry-over.
* [ ] Tag **release** op GitHub; zorg dat CI groen is op laatste commit; upload naar Blackboard + repo-link. 
* [ ] Buffer **spare time** voor bugfixes en polish; maak lijstje van bekende issues en workarounds in README.
* [ ] Laat iemand **playtest** en noteer feedback over moeilijkheid, snelheid, duidelijkheid van HUD en responsiviteit.
* [ ] Freeze **config** waardes (speeds/timers/scores) en zet ze vast in een versie; commit met changelog.
* [ ] Print dit lijstje en vink alles af bij voortgang; houd commits klein en beschrijvend.