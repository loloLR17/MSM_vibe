---
mission_id: TR2-20261016-R4R-PHYSICAL-TIME-B3
status: BLOCKED
base_ref: main
base_sha: 650aac856b3f912dfbe63ee4163bfe02489a8853
initial_head: 9dc28dbb4ed4a0d8b914b667886eca2de8cd388d
result_sha: 0e808f3881844279b268facc5db2543a382b806a
created_at_utc: 2026-10-16T21:01:31.633118+00:00
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-R — diagnostic temporel acquis ; B3 physique bloqué sur dépassement de service

**Bilan : BLOCKED.** Le diagnostic capteur/temps est acquis dans sa durée bornée.
L'essai réel R2/FIFO/métrologie/RTU a été lancé, puis arrêté automatiquement :
service **2715 us >2500 us**, nouveau **STARTED indéterminé epoch 4 / tx 5/run 5**.
Aucune fenêtre complète ni valeur B3 valide physique ; aucune clôture R4/R6.
La Flash M016 initiale est restaurée exacte sur 2 Mio, la nouvelle FRAM est
conservée sans rollback. Une correction logicielle du profil de banc est préparée,
compilée/testée, **jamais flashée et non qualifiée physiquement**.

## Référence, périmètre et préservation

Distant initial main `9dc28db`, contrat/base `650aac8`, R4-Q DONE vérifié ; différences
initiales : mission seule. Worktree isolé `MSM_vibe-R4R-physical-time-b3`, branche
`codex/r4r-physical-time-b3`, autorisation de publication explicite sur main.
Ancien main `82169af` propre/divergent préservé, aucun pull/reset/rebase/merge dessus.
57 anciens worktrees, deux stashes historiques et 4021 fichiers protégés inchangés,
contrôlés par empreintes ; archives R4-Q mission/rapport exactes issues de `650aac8`.
Contrat, règles de gouvernance, gels V1/R0, dettes, ancien corpus/preuves inchangés.

Modifications : `fifo_event_time.[ch]`, binding optionnel R3-B dans
`live_fifo_acquisition.[ch]`, profil explicite R4R et harness progressif,
test provider/oracle Python/guards CMake ; LIVE_R4R, présent rapport,
journal technique et statuts factuels roadmap. Aucun changement des corps du
parseur V1, du driver ST, du moteur R2 ou du B5 différé R4-Q.

## Temps événement : méthode et portée réelle

Lecture officielle ST DS12569 Rev8§9.28 et AN5444 Rev3§5.5/6.2/6.6 ; liens dans
[LIVE_R4R](../../05_Firmware/LIVE_R4R.md). Le compteur 32 bits, nominal 12,5 us et les
slots TAG_CNT permettent une corrélation hybride, mais ne donnent pas UTC.
Le registre FREQ_FINE a été lu à 1, jamais écrit. Une conversion nominale issue de
ce registre ne remplace pas une comparaison des horloges réelles.

**Observations** : TS AVANT XYZ du même TAG_CNT ; delta de 24 ticks pour chaque groupe de 8 XYZ,
champs réservés à 0 et BDR 0xA. Préfixe de 2048 mots conservé ; 228 ancres et 1820 XYZ analysés
indépendamment, sans anomalie de parité/compteurs/intervalle. Le diagnostic global
compte 52894 XYZ/6612 TS,0 anomalie d'association/réservés.

**Implémentation** : un groupe de 8 XYZ est conservé jusqu'à l'ancre suivante ; datation
interpolée entre ces deux timestamps réellement observés, indices/identités exacts.
Aucune grille à 26667 Hz ni temps de défilement SPI/poll attribué aux XYZ. Début sans
extrapolation car première ancre avant XYZ 0 ; groupe terminal non bracketé jamais
publié comme fenêtre complète. Monotone 64 bits, extension modulo 32/demi-domaine,
recul/reset/overflow/groupes incomplets refusés ; UTC false / timestamp 0. Un nouveau
START doit réinitialiser le provider ; aucun raccordement automatique produit.

Pente sur brackets DWT après 100 ms : **1896543 us / 151233 ticks =12,540540 us/tick**,
197 lectures, bracket maximal de 33 us et résidu maximal de 22 us. HCLK nominal 160 MHz vérifié ; absence
d'horloge externe étalon : comparaison relative capteur/MCU, pas précision SI,
UTC ou retard analogique du filtre qualifié. B3 réduit ensuite à1 ms. Le seuil
expérimental de contrôle de 100 us toutes les 100 ms n'est pas une incertitude garantie.
Delta 23..25 autorisé autour de 24 observé avec une tolérance de 1 tick ; il ne génère pas le temps.
La tentative réelle est arrêtée avant le premier contrôle à 100 ms : **dérive longue
non vérifiée**, rollover réel non atteint (test logiciel seulement).

## Essais physiques effectivement exécutés

UID 004A003D/38425018/20333157, ST-LINK 003500463235510B37333439/V3J15M7,
NUCLEO-U575ZI-Q ; préflight frais complet Flash 2 Mio / FRAM 262144 octets et readback capteur
arrêté conformes R4-Q. SPI1=2,5 MHz / SPI3=4 MHz calculés depuis registres et HCLK.
Sauvegardes originales récupérables conservées et comparées aux références antérieures.

| Phase | Résultat observé |
|---|---|
| Tentative 01, code 8af2451 | Relecture GDB prématurée avant fin des 2 s, HAL SPI interrompu,52097 XYZ ; erreur FIFO 1 puis STOP 235 us. Échec opératoire conservé, jamais PASS. |
| Diagnostic complet code 0e808f3 |2,000064 s,52894 XYZ/6612 TS, niveau 13, poll 401 us / gap 425 us, premier mot 10875 us ;0 erreur/stop confirmé. Attente stage 11 avant relecture sur breakpoint, aucune interruption active. FRAM complète a13b inchangée. |
| Vrai START R2 epoch 4 / tx 5 | Nouveau run 5 authentiquement réservé/STARTED ; matériel effectivement démarré,118 XYZ/15 TS,112 échantillons datés/14 groupes,6 non bracketés conservés puis abandonnés à l'arrêt. Pas un run synthétique ni un reçu fabriqué. |
| Coexistence réellement exercée | R2 lit/persiste ses premières phases ;3660 RTU loopback B3 CRC/longueur/snapshot contrôlés,0 V/F et 0 HELD. Dernier statut 2 / flags 1, séquence 0, fenêtre N=0. Publication et métrologie réelles mais aucune mesure complète valide. |
| Limite fatale |work max 922<=1000 us, **service max 2715>2500 us**, gap 2141<=5000 us, niveau 64, poll max 2234 us, publication max 405 us / PDU max 540 us. Maxima de composants non appariés : aucune somme présentée comme une trace ou une preuve de coexistence. |
| Arrêt sûr | Callback matériel 235 us, autre safety_stop 233 us, stop_error 0 et capteur arrêté confirmés. Il s'agit d'un arrêt de défaut ; **STOP commande tx6 et sa latence durable non exécutés**. |
| Phases non atteintes | START jamais COMPLETED, boucle readonly 10 s jamais entrée, fenêtres RMS/peak 4096+ et HELD physique non obtenus, marge continue/dérive longue/rollover/CPU-pile-IRQ non qualifiés. |

Les données zéro du buffer 4096 / ADU valide inutilisé ne sont pas des mesures ni un
oracle réussi. Le calcul RMS de référence est prouvé sur hôte ; l'oracle 4096 XYZ
physique n'a pas été lancé faute de fenêtre complète. Les identités diagnostiques
volatiles UINT64_MAX-1 n'ont aucune autorité R2 et n'ont jamais été persistées.
Le diagnostic complet utilise un FIFO distinct puis restitue exclusivement le
transport à l'owner R2 ; aucun floor réel n'est abaissé.

## Blocage durable, analyse et correction candidate

**Fait** : guard coopératif fait échec sur service 2715 us ; moteur status 9,
indeterminate/has_active/blocking_fault vrais. Slot 3 contient RESERVED gen 25 puis
STARTED gen 26 epoch 4 / tx 5 / START 13/run 5/config 3/gen 3 ; INTENT gen 15 phase 1, racine 39,
CRC racines/header/payload/journaux/intent vérifiés hors ligne. Dernier terminal
antérieur START 4/4 reste conservé. 104852 octets / 3278 commandes écrits, 6 plages toutes
LIVE et statut 0 ;113 octets différents, historique et tail byte-identiques.

**Lecture du code / hypothèse ciblée** : le même service compose drainage jusqu'à
128 mots, calcul/provider non optimisés et copie/publication RAM répétée à chaque
checkpoint FRAM de 32 octets, puis 8 polls RTU. Les maxima observés montrent la nécessité
de réduire cette tranche ; ils n'attribuent pas la durée exacte de chaque
sous-opération du callback fautif. Pas de faute électrique démontrée, aucun seuil
relâché ni disparition du verrou pour poursuivre.

**Candidate logicielle 83d4fbd529d0311013889de81145b612701600bd** : publication du snapshot seulement
avant la prochaine requête loopback (âge toujours calculé à la lecture), -O2 limité
au harness/provider/métrologie du profil R4R. Algorithmes, formats et gardes inchangés,
V1 par défaut inchangé. ARM et ASan/UBSan-O2/provider+448 oracles métrologie PASS.
Cette optimisation n'a pas été chronométrée sur cible et ne prouve pas le budget.
Le probe conserve le garde SHA ancien a13b : il refuse le nouveau 14fb, sans écriture.
Ne pas relancer ses scripts historiques sur l'état actuel.

Le run 5 reste consommé. Une nouvelle admission R2 est bloquée par R0 ; ni commit
logiciel, ni autorisation de flash générale n'autorisent une réconciliation de
STARTED, un replay ou un rollback. **Décision structurante à ChatGPT :** définir
un contrat de reprise sur le snapshot 14fb, qualifier d'abord le service corrigé
sur diagnostic volatil readonly sans R2, puis décider du traitement explicite du
journal indéterminé avant tout nouveau START. Aucun nettoyage automatique proposé.

## Validation logicielle et images

- Première tranche :140 tests complets en 64,39 s et ARM ; oracle Python LE32/rollover/
 parité/pente passé avant cible. Deuxième tranche :141 tests complets en 62,28 s et ARM.
- Test provider final ciblé PASS ; ASan/UBSan ciblés PASS puis variantes-O2
 provider et métrologie/448 oracles PASS après changement des options de banc.
 Nouveaux tests non affaiblis ; première fixture refusée pour contexte mission
 manquant, corrigée avec les champs réels attendus. Warning indentation ARM corrigé,
 sorties d'échec conservées. Anciennes validations inchangées non répétées sans cause.
- Candidate : ARM dédié PASS ; validation canonique `bash ./tr2_validate.sh
 --cross-build-only` PASS car dernières modifications limitées au harness/options
 du profil non production. Sources effectivement -O2 consignées ; aucun nouveau
 changement fonctionnel du cœur n'exigeait de refaire 141 tests déjà réussis.
- BIN canonique V1 **85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5**,
 byte-identique R4-Q, non flashé. Profils/refus R4R/CMake vérifiés.
- Image physique 0e808f3 :147448 octets,
 SHA256 **bf225bf171e44b982490046a8f45a318aea70557b3bdf565d0abbe18e8ac560f** ; ELF SHA dans event-build.json.
 Text 146588 / data 492 / bss 651696 octets issus du lien ARM, pas pic de pile/charge CPU mesuré.
- Candidate 83d4fbd :142688 octets/SHA256 **94780136a62fc4a875413677a7162976fbda0d812abc4bca956dceaac610593b**,
 non flashée. Artefact physique exact conservé via readback/compression distincte ;
 builddir réutilisé pour la candidate, ne pas confondre ses symboles/ELF avec l'essai 0e.

## État final matériel, preuves et publication

Restauration vérifiée **2026-10-16T20:57:06.652246+00:00** :18 pages Flash normales/147456 octets,
puis relecture **2097152 octets exacte** à l'initiale SHA256
`c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`.
FRAM 262144 octets relue après restauration, exacte à l'état obtenu après essai,
SHA256 **14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9**.
Firmware **M016 CommissioningReadOnly WAIT**, capteur arrêté, debug détaché,
STARTED 5 conservé. Aucune Option Bytes/mass erase/écriture FRAM de restauration,
aucun accès SD/HP/COM/RS485 physique, aucune intervention utilisateur demandée.

Preuves [R4R](preuves/TR2-20261016-R4R/README.md) : logs bruts GDB/flash/build/tests,
rawtags/brackets/XYZ partiels, images dédiées distinctes, snapshots FRAM sans perte,
audit CRC/mesures/préservation et SHA256SUMS. Backups Flash complets hors Git :
`/home/lolo/dev/msm/reports/TR2-r4r/hardware/flash-before.bin` et `flash-restored.bin` ;
FRAM avant/après live/après restauration au même emplacement, aucune suppression.

Journal actualisé : statut BLOCKED, temps caractérisé, échec service/STARTED 5,
candidate non qualifiée, restauration et conditions de reprise. Roadmap R3/R4
actualisée sans modifier les normes ni effacer les acquis R4-P/R4-Q.
Checkpoint **5e4fde66b52902798807c6adcd6522194a591eaa** relu depuis GitHub :113 fichiers/
97 preuves contrôlés. Le bilan technique **444f01acc07ec4eb1389b508b634ead7710a1923**
a ensuite été récupéré depuis GitHub et relu intégralement : **155 fichiers comparés,
138 preuves SHA256 vérifiées**, le 16/10/2026 UTC. La trace complète est publiée
avec les preuves. Statut **BLOCKED technique** maintenu pour STARTED run 5 et budget
non satisfait ; publication vérifiée distincte de la qualification physique.
Le commit de confirmation de cette relecture aura son SHA fourni dans la réponse
finale, après contrôle de sa propre publication. Aucun nouvel accès matériel depuis
la restauration vérifiée à 20:57:06 UTC.
