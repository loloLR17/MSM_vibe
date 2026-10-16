---
mission_id: TR2-20261016-R4P-PHYSICAL-COEXISTENCE
status: IN_PROGRESS
base_ref: main
base_sha: f7366e535b90a4ef79642a3debfa601f35c40b9c
initial_head: 03309c076da7322fb774d41c032112cd663390fd
result_sha: 9f429e513faba26ce89b760361c9cba08057bc24
created_at_utc: 2026-10-16T12:55:58Z
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-P — checkpoint avant unique maintenance physique

Préflight réel : ST-LINK003500463235510B37333439/UID004A003D/38425018/20333157,
Flash2Mio et FRAM262144 doublement relues, récupérables dans reports/TR2-r4p/hardware,
identiques R4-D ; SHA Flashc5bc344d…f5b26, FRAMf2bd7a99…8a0c1. Capteur relu arrêté,
WHO7B/ODRoff/FIFObypass/TSoff. Aucun flash/reset/writeNVM encore exécuté en R4-P.
Mission et rapport R4-D archivés byte-identiques ; stashes/worktrees protégés.

[Plan](../../05_Firmware/LIVE_R4P.md) : une seule isolation logique, état intermédiaire
quarantiné par frontend CLOSED/RETIRING ; vraiRENEW2->3 avant admission, INTENT128
etfloor run2 conservés, ancienneclé refus24, prochainsruns3/4. Aucun reçu pour
l'ancien STARTED. Snapshot intégral initial archivé sans perte ; historique/tail
jamais écrits. Aucun trigger6 de reprise/renettoyage dans ce profil.

Test ciblé139 PASS1,93s,142coupures+4phasesRENEW sur vrais stores/snapshotf2bd,
anti-rejeu/floors/no replay et contrôle négatif du pont. ASan/UBSan ciblé PASS1,12s,
cross-build canonique/probe PASS. Cœur R4-D inchangé,138anciens tests nonrejoués,
BINcanonique V1 inchangé. Oracle finalMODEL-ONLY a13b3401…a2ef9 n'est pas une preuve cible.

Tranche suivante : flash standard de l'image dédiée issue du code369887b9583ca05d0208c3d18bdd485311bcb151, WAIT/UID/readback,
unique trigger4 ; vrai START3/Renew4 sousFIFO/60s/STOP/APPLY/START4 puis faute service
après COMPLETED. Arrêt immédiat si nouveau STARTED indéterminé, dépassement ou défaut.
Si succès, recovery readonly5 après reset, puis Flash initiale restaurée2Mio.
FRAM finale conservée sans rembobinage. Aucun SD/HP/RTUélectrique/R6/B3.


## Checkpoint après arrêt et diagnostic readonly

Le premier essai a publié l'unique isolation LIVE (52426octets/1639commandes),
puis s'est arrêté work1075us>1000us avant RENEW. Aucun nouveau STARTED ni run réservé.
Le statut9 du moteur RAM était l'ancien actif ; snapshot réel du pont à journal vide
713a953384f2688f8448989ba706bf0dcef33ace3f177a1ee90fa5eb63713ba0 préservé intégralement,
historique/tail identiques. Aucune seconde isolation ni écriture après cet arrêt.

Cause démontrée en lecture seule : pile failed->checkpoint->compare_protected,
at245760.3840itérations pour sauter LIVE sans checkpoint, puis première lecturetail.
Correction : saut direct O(1), mêmes139264octets protégés comparés. Ce défaut de harness
est distinct de la correction START/OBSERVE R4-D et n'implique aucun seuil relevé.
L'appel GDB est interrompu au breakpoint pour capturer la pile ; pas une comparaison
réussie ni une nouvelle preuve d'acquisition. Les sorties d'erreur sont conservées.

Code corrigé 9f429e513faba26ce89b760361c9cba08057bc24, test139 enrichi reprise du pont/refus INTENT altéré
PASS1,92s etASan/UBSan1,12s ; ARMcanonique/probe PASS, cœur fonctionnel inchangé.
Plan suivant dans LIVE_R4P : nouvelle image au WAIT, trigger7 readonly/ownershipfalse,
0write et contrôle entier protégé sous1000us. Seulement si PASS avec snapshot inchangé,
reset CLOSED puis trigger8+token, reprise du vrai RENEW2->3 sans réisolation. Le risque
est analysé et le dépassement doit être démontré maîtrisé avant reprise d'écriture.
Aucune de ces opérations de poursuite n'est encore présentée comme exécutée.


## Checkpoint de reprise — contrôle readonly réussi

Trigger7 réel PASS : montage du pont et139264octets protégés comparés, workmax342us,
service4us, zéro write, aucune deadline ; snapshot262144 byte-identique713a9533…13ba0.
Ce contrôle démontre la maîtrise du dépassement avant toute reprise d'écriture.
Reset logiciel vers CLOSED puis trigger8 explicitement lancé le16/10 à13:08:52UTC,
sans seconde isolation : seul le vrai RENEW2->3 ouvre la suite R4-P. Image corrigée
issue du code9f429e5 et empreintes exactes dans build-artifacts.json. Lecture terminale
pas encore effectuée ; aucun succès FRAM/FIFO/START n'est déduit de ce lancement.
