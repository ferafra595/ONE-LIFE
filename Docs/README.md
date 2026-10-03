# ONE LIFE — struttura pulita

Ho ridotto il progetto a sole 3 cartelle principali:

- Source — tutto il codice C++
- Config — configurazione Unreal
- Docs — istruzioni e roadmap

Il file OneLife.uproject resta nella root perché Unreal lo richiede.

## Cosa contiene già il codice
- personaggio third-person
- camera
- camminata/corsa/salto
- Enhanced Input
- interazioni tramite line trace
- bisogni: fame, sete, energia, igiene, socialità, stress, vescica
- denaro, istruzione, carriera, reputazione
- memoria persistente delle decisioni
- data/ora del mondo
- GameMode
- predisposizione Lumen/Nanite
- plugin Chaos Vehicles / StateTree / Mass

## Asset da creare nell'Editor
Il codice non può incorporare asset grafici AAA. Nell'Editor vanno creati/importati:
- personaggio realistico + AnimBP
- IA_Move, IA_Look, IA_Jump, IA_Sprint, IA_Interact
- IMC_OnFoot
- City_Slice_01
- auto e relative configurazioni Chaos
- edifici, interni, props, materiali PBR, audio

## Primo obiettivo giocabile
Una giornata completa:
sveglia → bagno → vestirsi → colazione → uscire → auto/a piedi → scuola/lavoro → negozio → incontro NPC → attività serale → casa → sonno.

Il multiplayer resta escluso fino al completamento del single-player.
