---
mission_id: TR2-20261017-R4U-LIVE-RUNTIME-RTU-PHYSICAL
status: IN_PROGRESS
base_ref: main
base_sha: fa7ba586114f83fe3fee3c87050620e22ee11930
initial_head: 70bffe63048ebc5b169852ef9814389db59ea56f
result_sha: 43840fedf6c7e2e40d5a49eeea822d5d91d1ee55
created_at_utc: 2026-10-17T09:43:42.598482+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-U — checkpoint logiciel avant qualification physique

Runtime opt-in, matrice et plan dans [LIVE_R4U](../../05_Firmware/LIVE_R4U.md).
143 tests existants PASS dans full-validation-02 ; nouveau144 corrigé puis PASS
ciblé0,37s. Première suite144 PASS82,84s avant ajout du verrou readonly ; seconde
suite143/144 : nouvel oracle test attendait ACK sur écriture readonly. Correction
du seul scénario pour exiger exception01 ; aucun contrôle produit affaibli.
Sanitizers ancien serveur PASS4,05s et runtime corrigé PASS0,62s ; cross-build
canonique et dédié PASS. BSS654560, text145548/data492 sur image dédiée.
Les logs effectifs et empreintes sont dans `preuves/TR2-20261017-R4U/`.

Préflight neuf09:30:14UTC : UID/SN attendus, capteur arrêté, Flash2Mio c5bc…f5b26
et FRAM371a…6634 exactes. Aucun flash/START/écriture FRAM/transmission R4-U encore.
COM4 confirmé par l'utilisateur, COM3 STLINK non utilisé pour RS485.
L'exécution PowerShell -File a été refusée par la politique scripts Windows ;
aucune politique changée/contournée. Petit maître natifC# .NETFramework compilé,
selftest CRC et inventaire sans transmission réussis. Aucun développementR5.

Travaux suivants : figer SHA code/image, RAM5s, UARTreadonly10s, si gardes acquises
START5/3 run7 etSTOP5/4 physiques ; capturer budgets/ADU/FRAM, restaurer M016,
publier le bilan avec relecture distante. Critères physiques non encore acquis.

Préservation : dépôt d'origine main82169af et deux stashes historiques intacts,
worktree isolé basé sur main70bffe6. Aucun merge/rebase/reset/clean, aucune reprise
de la retraiteR4T ou du commit étudeR4S86d0f4b. Archives R4T déjà présentes intactes.
Journal actualisé pour préparation, pas de qualification produit/R4global/R6.

Checkpoint43840fe publié et30fichiers relus depuis origin/main. Phase RAM image01 acquise :132602XYZ,505ADU,0write/FRAM371a exacte, work335/service1673/gap1641µs. Transition UART refusée par second bind_event_metrology interdit ; UART non initialisée, une lectureCOM4 sans réponse. Aucun B5 durable lancé. Nouvelle image02 corrige seulement le harness, garde single-use conservée, contrôleur ready renforcé, journalisation maître avant chaque envoi. Cross-build dédié PASS ; reprise RAM courte nécessaire au code changé, suite143tests sur cœur inchangé acquise.

Image02 RAM PASS132618XYZ/503ADU, work336/service1613/gap1616µs. Transition02 refusée avant START : run diagnostic déjà consommé par ce propriétaire, UARTIRQ0/service0, FRAM exacte. Scénario corrigé image03 : une seule phase par boot, UARTreadonly puis boot neuf avant durable ; aucun run_id abaissé, aucun vieux B5 réutilisé. L'erreur de scénario ne justifie pas de conclure à une panne RS485.

UARTreadonly image03 :72réponses physiques CRC/longueur exactes, puis arrêt sûr sur tagFIFO décalé d'un bit (BENCH_TAG9/parité1),131050XYZ, budgets338/1452/1386µs conformes, FRAM371a entière inchangée. Causalité non prouvée ; essai comparatif limité de la voie SPIduplex existante, uniquementR4U, sans garde supprimée. B5durable toujours non engagé. Canonique final recompilé par bash (scriptnonexécutable), BIN85b1…c0f5 exact.
