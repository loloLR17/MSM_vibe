from pathlib import Path
import json,datetime,subprocess as s
root=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4R-physical-time-b3');p=Path('/home/lolo/dev/msm/reports/TR2-r4r');hw=p/'hardware';ex=root/'Modbus RTU/00_gouvernance/Echanges_Codex';fw=root/'Modbus RTU/05_Firmware';code=s.check_output(['git','rev-parse','0e808f3'],cwd=root,text=True).strip();candidate=s.check_output(['git','rev-parse','83d4fbd'],cwd=root,text=True).strip();now=datetime.datetime.now(datetime.timezone.utc).isoformat();image=json.loads((p/'event-build.json').read_text());ci=json.loads((p/'candidate-build.json').read_text());rest=json.loads((hw/'restoration-summary.json').read_text());assert rest['result']=='PASS'
report=f'''---
mission_id: TR2-20261016-R4R-PHYSICAL-TIME-B3
status: BLOCKED
base_ref: main
base_sha: 650aac856b3f912dfbe63ee4163bfe02489a8853
initial_head: 9dc28dbb4ed4a0d8b914b667886eca2de8cd388d
result_sha: {code}
created_at_utc: {now}
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
service **2715us >2500us**, nouveau **STARTED indéterminé epoch4/tx5/run5**.
Aucune fenêtre complète ni valeur B3 valide physique ; aucune clôture R4/R6.
La Flash M016 initiale est restaurée exacte sur2Mio, la nouvelle FRAM est
conservée sans rollback. Une correction logicielle du profil de banc est préparée,
compilée/testée, **jamais flashée et non qualifiée physiquement**.

## Référence, périmètre et préservation

Distant initial main9dc28db, contrat/base650aac8, R4-Q DONE vérifié ; différences
initiales : mission seule. Worktree isolé `MSM_vibe-R4R-physical-time-b3`, branche
`codex/r4r-physical-time-b3`, autorisation de publication explicite surmain.
Ancienmain82169af propre/divergent préservé, aucun pull/reset/rebase/merge dessus.
57anciensworktrees, deuxstashes historiques et4021fichiers protégés inchangés,
contrôlés par empreintes ; archives R4-Q mission/rapport exactes issues650aac8.
Contrat, règles de gouvernance, gels V1/R0, dettes, ancien corpus/preuves inchangés.

Modifications : `fifo_event_time.[ch]`, binding optionnel R3-B dans
`live_fifo_acquisition.[ch]`, profil explicite R4R et harness progressif,
test provider/oracle Python/guards CMake ; LIVE_R4R, présent rapport,
journal technique et statuts factuels roadmap. Aucun changement des corps du
parseur V1, du driver ST, du moteur R2 ou du B5 différé R4-Q.

## Temps événement : méthode et portée réelle

Lecture officielle ST DS12569 Rev8§9.28 et AN5444 Rev3§5.5/6.2/6.6 ; liens dans
[LIVE_R4R](../../05_Firmware/LIVE_R4R.md). Le compteur32bits/nominal12,5us et les
slots TAG_CNT permettent une corrélation hybride, mais ne donnent pas UTC.
Le registre FREQ_FINE a été lu1, jamais écrit. Une conversion nominale issue de
ce registre ne remplace pas une comparaison des horloges réelles.

**Observations** : TS AVANT XYZ du mêmeTAG_CNT ; delta24ticks chaque8XYZ,
champs réservés0 et BDR0xA. Préfixe2048mots conservé ;228ancres et1820XYZ analysés
indépendamment, sans anomalie de parité/compteurs/intervalle. Le diagnostic global
compte52894XYZ/6612TS,0anomalie d'association/réservés.

**Implémentation** : un groupe8XYZ est conservé jusqu'à l'ancre suivante ; datation
interpolée entre ces deux timestamps réellement observés, indices/identités exacts.
Aucune grille26667 ni temps de défilement SPI/poll attribué aux XYZ. Début sans
extrapolation car premièreancre avantXYZ0 ; groupe terminal non bracketé jamais
publié comme fenêtre complète. Monotone64bits, extensionmodulo32/demi-domaine,
recul/reset/overflow/groupes incomplets refusés ; UTCfalse/timestamp0. Un nouveau
START doit réinitialiser le provider ; aucun raccordement automatique produit.

Pente sur brackets DWT après100ms : **1896543us/151233ticks =12,540540us/tick**,
197lectures, bracketmax33us et résidumax22us. Nominal HCLK160MHz vérifié ; absence
d'horloge externe étalon : comparaison relative capteur/MCU, pas précision SI,
UTC ou retard analogique du filtre qualifié. B3 réduit ensuite à1ms. Le seuil
expérimental de contrôle100us toutes100ms n'est pas une incertitude garantie.
Delta23..25 autorisé autour24observé avec allowance1tick ; il ne génère pas le temps.
La tentative réelle est arrêtée avant le premier contrôle100ms : **dérive longue
non vérifiée**, rollover réel non atteint (test logiciel seulement).

## Essais physiques effectivement exécutés

UID004A003D/38425018/20333157, ST-LINK003500463235510B37333439/V3J15M7,
NUCLEO-U575ZI-Q ; préflight frais complet Flash2Mio/FRAM262144 et readback capteur
arrêté conformes R4-Q. SPI1=2,5MHz/SPI3=4MHz calculés depuis registres et HCLK.
Sauvegardes originales récupérables conservées et comparées aux références antérieures.

| Phase | Résultat observé |
|---|---|
| Tentative01 code8af2451 | Relecture GDB prématurée avant fin2s, HAL SPI interrompu,52097XYZ ; erreurFIFO1 puis STOP235us. Échec opératoire conservé, jamais PASS. |
| Diagnostic complet code0e808f3 |2,000064s,52894XYZ/6612TS, niveau13, poll401us/gap425us, premiermot10875us ;0erreur/stop confirmé. Attente stage11 avant relecture sur breakpoint, aucune interruption active. FRAM complètea13b inchangée. |
| Vrai START R2 epoch4/tx5 | Nouveau run5 authentiquement réservé/STARTED ; matériel effectivement démarré,118XYZ/15TS,112échantillons datés/14groupes,6non bracketés conservés puis abandonnés àl'arrêt. Pas un run synthétique ni un reçu fabriqué. |
| Coexistence réellement exercée | R2 lit/persiste ses premières phases ;3660RTUloopbackB3 CRC/longueur/snapshot contrôlés,0V/F et0HELD. Dernier statut2/flags1, séquence0, fenêtreN0. Publication et métrologie réelles mais aucune mesure complète valide. |
| Limite fatale |workmax922<=1000us, **servicemax2715>2500us**, gap2141<=5000us, niveau64, pollmax2234us, publicationmax405us/PDUmax540us. Maxima de composants non appariés : aucune somme présentée comme une trace ou une preuve de coexistence. |
| Arrêt sûr | Callback matériel235us, autre safety_stop233us, stop_error0 et capteurarrêté confirmés. Il s'agit d'un arrêt de défaut ; **STOP commande tx6 et sa latence durable non exécutés**. |
| Phases non atteintes | START jamaisCOMPLETED, boucle readonly10s jamais entrée, fenêtres RMS/peak4096+ etHELD physique non obtenus, marge continue/dérive longue/rollover/CPU-pile-IRQ non qualifiés. |

Les données zéro du buffer4096/ADUvalide inutilisé ne sont pas des mesures ni un
oracle réussi. Le calcul RMS de référence est prouvé sur hôte ; l'oracle4096XYZ
physique n'a pas été lancé faute de fenêtre complète. Les identités diagnostiques
volatiles UINT64_MAX-1 n'ont aucune autorité R2 et n'ont jamais été persistées.
Le diagnostic complet utilise un FIFO distinct puis restitue exclusivement le
transport àl'owner R2 ; aucun floor réel n'est abaissé.

## Blocage durable, analyse et correction candidate

**Fait** : guard coopératif fait échec sur service2715us ; moteurstatus9,
indeterminate/has_active/blocking_fault vrais. Slot3 contient RESERVEDgen25 puis
STARTEDgen26 epoch4/tx5/START13/run5/config3/gen3 ; INTENTgen15phase1, racine39,
CRC racines/header/payload/journaux/intent vérifiés horsligne. Dernier terminal
antérieur START4/4 resteconservé.104852octets/3278commandes écrits,6plages toutes
LIVE etstatut0 ;113octets différents, historique et tail byte-identiques.

**Lecture du code / hypothèse ciblée** : le même service compose drainage jusqu'à
128mots, calcul/providernonoptimisés et copie/publication RAM répétée àchaque
checkpointFRAM32octets, puis8pollsRTU. Les maxima observés montrent la nécessité
de réduire cette tranche ; ils n'attribuent pas la durée exacte de chaque
sous-opération du callback fautif. Pas de faute électrique démontrée, aucun seuil
relâché ni disparition du verrou pour poursuivre.

**Candidate logicielle {candidate}** : publication du snapshot seulement
avant la prochaine requête loopback (âge toujours calculé àlalecture), -O2 limité
au harness/provider/métrologie du profilR4R. Algorithmes, formats etgardesinchangés,
V1 par défaut inchangé. ARM etASan/UBSan-O2/provider+448oracles métrologie PASS.
Cette optimisation n'a pas été chronométrée sur cible et ne prouve pas le budget.
Le probe conserve le SHAguard anciena13b : il refuse le nouveau14fb, sans écriture.
Ne pas relancer ses scripts historiques sur l'état actuel.

Le run5 resteconsommé. Une nouvelle admission R2 est bloquée par R0 ; ni commit
logiciel, ni autorisation de flash générale n'autorisent une réconciliation de
STARTED, un replay ou un rollback. **Décision structurante àChatGPT :** définir
un contrat de reprise sur le snapshot14fb, qualifier d'abord le service corrigé
sur diagnostic volatil readonly sans R2, puis décider du traitement explicite du
journal indéterminé avant tout nouveau START. Aucun nettoyage automatique proposé.

## Validation logicielle et images

- Première tranche :140tests complets64,39s etARM ; oracle Python LE32/rollover/
 parité/pente passé avant cible. Deuxième tranche :141tests complets62,28s etARM.
- Test provider final ciblé PASS ; ASan/UBSan ciblés PASS puis variantes-O2
 provider et métrologie/448oracles PASS après changement des options de banc.
 Tests nouveaux non affaiblis ; première fixture refusée pour contexte mission
 manquant, corrigée avec les champs réels attendus. Warning indentation ARM corrigé,
 sorties d'échec conservées. Anciennes validations inchangées non répétées sans cause.
- Candidate : ARM dédié PASS ; validationcanonique `bash ./tr2_validate.sh
 --cross-build-only` PASS car dernières modifications limitées au harness/options
 du profil nonproduction. Sources effectivement -O2 consignées ; aucun nouveau
 changement fonctionnel du cœur n'exigeait de refaire141tests déjàréussis.
- BINcanoniqueV1 **85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5**,
 byte-identique R4-Q, non flashé. Profils/refusR4R/CMake vérifiés.
- Image physique0e808f3 :{image['bin_bytes']}octets,
 SHA256 **{image['bin_sha256']}** ; ELF SHA dans event-build.json.
 Text146588/data492/bss651696octets issuslinkARM, pas pic de pile/chargeCPU mesuré.
- Candidate83d4fbd :{ci['bin_bytes']}octets/SHA256 **{ci['bin_sha256']}**,
 nonflashée. Artefact physique exact conservé via readback/compression distincte ;
 builddir réutilisé pourcandidate, ne pas confondre ses symboles/ELF avec l'essai0e.

## État final matériel, preuves et publication

Restauration vérifiée **{rest['utc']}** :18pagesFlash normales/147456octets,
puis relecture **2097152octets exacte** àl'initiale SHA256
`c5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26`.
FRAM262144 relue aprèsrestauration, exacte àl'état obtenu aprèsessai,
SHA256 **14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9**.
Firmware **M016 CommissioningReadOnly WAIT**, capteurarrêté, debugdétaché,
STARTED5 conservé. Aucune OptionBytes/mass erase/écritureFRAM de restauration,
aucun accèsSD/HP/COM/RS485 physique, aucune intervention utilisateur demandée.

Preuves [R4R](preuves/TR2-20261016-R4R/README.md) : logs bruts GDB/flash/build/tests,
rawtags/brackets/XYZpartiels, images dédiées distinctes, snapshots FRAM sans perte,
auditCRC/mesures/préservation etSHA256SUMS. BackupsFlash complets horsGit :
`/home/lolo/dev/msm/reports/TR2-r4r/hardware/flash-before.bin` etflash-restored.bin ;
FRAMavant/aprèslive/aprèsrestauration au même emplacement, aucune suppression.

Journal actualisé : statutBLOCKED, temps caractérisé, échec service/STARTED5,
candidate nonqualifiée, restauration et conditions dereprise. Roadmap R3/R4
actualisée sans modifier les normes ni effacer les acquisR4-P/R4-Q.
Checkpoint **5e4fde66b52902798807c6adcd6522194a591eaa** relu depuisGitHub :113fichiers/
97preuves contrôlés. Le présent bilan etses nouvelles preuves sont àrelire après
push ; BLOCKED désigne le verrou technique, aucune qualification physique implicite.
Le SHA du derniercommit de publication sera donné après vérification distante.
'''
(ex/'DERNIER_RAPPORT.md').write_text(report)
(fw/'LIVE_R4R.md').write_text('''# R4-R — temps événement et essai B3 physique

Statut **BLOCKED**, R4global/R6 ouverts. Rapport détaillé etpreuves :
[DERNIER_RAPPORT](../00_gouvernance/Echanges_Codex/DERNIER_RAPPORT.md),
[preuvesR4R](../00_gouvernance/Echanges_Codex/preuves/TR2-20261016-R4R/README.md).
Code physique0e808f3 ; candidate83d4fbd compilée/testée, jamais flashée.

## Méthode

ST [DS12569 Rev8§9.28](https://www.st.com/resource/en/datasheet/iis3dwb.pdf)
décrit compteur32bits et12,5us nominal, ajustementFREQ_FINE. ST
[AN5444 Rev3§5.5/6.2/6.6](https://www.st.com/resource/en/application_note/an5444-iis3dwb-ultrawide-bandwidth-lownoise-3axis-digital-vibration-sensor-stmicroelectronics.pdf)
décrit gel powerdown, slotsTAG_CNT et corrélationhybride avec timestampdécimé.
L'indication≈30h de cette AN contredit le calcul2^32×12,5us≈14,91h ; ne pas
la coder comme horizon. Extensionmodulo32/demi-domaine et refusrecul utilisés,
aucun passage physique du rollover revendiqué. DriversST/parseurV1 inchangés.

Observation TR2 : TS précèdeXYZ du mêmeTAG_CNT,24ticks/8XYZ, BDR0xA/réservé0.
Provider buffer8XYZ, attendancre suivante, interpole dansla bracket réelle sans
ODR nominal/déqueue/UTC. Pas extrapolationinitiale, pas fenêtreterminaleincomplète.
Le domainetimestamp capteur est relié àDWT via lecturescompteur bracketées,
ratioobservé ;100us de garde expérimentale toutes100ms, pas précision garantie.
Réinitialisation explicite pourun nouveauSTART, indices/run/config vérifiés parR3-B.

## Résultats et limites

- Diagnostic01 interrompu parGDB trop tôt :52097XYZ puisSPIerr1/STOP235us ; conservé.
- Diagnosticcomplet :2,000064s/52894XYZ/6612TS,0erreur, niveau13/poll401us/gap425us,
 premiermot10875us ;197brackets,12,54054us/tick DWT, résidu22us/bracket33us.
 Fine1 lu seulement, pas étalonSI/UTC ou délai analogique qualifié. ArchiveR4-Q exacte.
- VraiR2 START4/5/run5 :118XYZ/15TS,112datés,6nonbracketés ; **service2715>2500us**,
 work922us/gap2141us/niveau64 ;3660RTU RAMcohérents/CRC contrôlés,0B3valide.
 STARTED5 durable, pasCOMPLETED/STOPtx6/10sreadonly/fenêtre4096/HELD physique.
- Correction préparée : -O2 opt-in du chemin de banc etpublication avantrequête,
 garde1000/2500/5000 inchangée. ARM/ASan+UBSan-O2/448oracles PASS ; pas flashée.
 Snapshotguard refuse le nouvelétat14fb. Qualification de ce service puis décision
 humaine de traitementSTARTED requises avantnouveauSTART ; jamaisrejouer/effacer.

141tests hôte62,28s/ARM etsanitizers ciblés ; V1 BIN85b1be60…c0f5 inchangé.
M016 Flashinitiale restaurée exacte2Mio,18pages normales ; FRAMfinale14fb172f…2aa9
conservée sansrollback,113octets modifiés uniquementLIVE/racine39/CRCvalables.
Historique/tail exacts, capteurarrêté/WAIT/GDBdétaché. PasUART/RS485/SD/PC testés.
RMS/peak/Nquatrevaleurs/âgesHELD restent logiciel/àqualifierphysiquement ;
le préfixe112 ne qualifie ni dérivelongue, ni budgets continus, ni perteglobalezéro.
''')
j=root/'Modbus RTU/00_gouvernance/ETAT_COURANT_TR2.md';t=j.read_text();at=t.index('\n\n## R4-R —');t=t[:at]+'''

## R4-R — 16/10/2026 UTC, BLOCKED après dépassement de service

- **OBSERVÉ / PARTIEL** : code0e808f3 ; diagnostic complet2,000064s/52894XYZ/6612TS,
 TS avantXYZ mêmecompteur/delta24,0anomalie/niveau13/poll401us/gap425us.
 Calibration197brackets DWT12,54054us/tick/résidu22us/bracket33us, pasUTC/SI.
 Provider8XYZ ancré surTS réels testé ; premierdiagnosticinterrompuGDB conservé.
- **BLOCKED R2 / B3 NON QUALIFIÉ** : vraiSTART4/5/run5,118XYZ/15TS/112datés,
 service2715>2500us/work922us/gap2141us/niveau64, STOPsûr235us.3660RTU RAM
 contrôlés/0B3valide, pasfenêtre4096/HELD/STOPcommande/10sreadonly/dérivelongue.
 STARTEDgen26/INTENT15/phase1/racine39 CRCvalides ;run5 jamaisréutilisable.
- **CANDIDATE LOGICIELLE** :83d4fbd, publication avantrequête et-O2 profilbanc,
 ARM/sanitizers-O2/provider+448oracles PASS, **nonflashée/nonchronométrée**.
141tests/ARM PASS, canoniqueV1 BIN85b1be60…c0f5 inchangé/nonflashé.
- **RESTAURÉ / PRÉSERVÉ** :20:57:06UTC FlashM016 exacte2Mio c5bc344d…f5b26,
18pages normales ; FRAMcomplète14fb172f2917d0a8e440438051831c84f3f3e746161702a7de9e9a1a251b2aa9
 inchangée aprèsrestauration,113octetsdifférents seulementLIVE/104852écrits.
 Historique/tail exacts ;M016CommissioningReadOnlyWAIT/capteurarrêté/GDBdétaché.
57worktrees/deuxstashes/4021fichiers protégés etR4-Q archivée préservés.
- **DÉCISION CHATGPT / SUITE** : nouveaucontrat sursnapshot14fb, qualifierd'abord
 servicecorrigé sansadmissionR2, puis traitementexplicitementautorisé deSTARTED
 avantnouveauSTART. Aucunreplay/effacement/reçuinventé/rollback. R4global/R6 ouverts.
[Plan etpreuves](../05_Firmware/LIVE_R4R.md). Checkpoint5e4fde6 relu113fichiers/97preuves.
''';j.write_text(t)
road=root/'Modbus RTU/00_gouvernance/ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';t=road.read_text();lines=t.splitlines()
for i,line in enumerate(lines):
 if line.startswith('| **R3 —'):
  lines[i]='| **R3 — FIFO/calcul/B3** | **LOGICIEL VALIDÉ ; R4-R PHYSIQUE PARTIEL / BLOCKED** | Provider DEC8 ancré sur TS réels, R3-B/B3/RTU RAM préparés ; production V1 conservée. |141tests/ARM/sanitizers ; diagnostic temps52894XYZ observé, mais essai R2 arrêté après118XYZ/0fenêtre complète. Aucun B3 physique valide ni dérive longue qualifiée. [R4-R](../05_Firmware/LIVE_R4R.md). |'
 if line.startswith('| **R4 —'):
  lines[i]='| **R4 — Temps réel** | **R4-R BLOCKED : SERVICE2715us ET STARTED5** | Acquis R4-P/R4-Q conservés ; R4-R réel R2/FIFO/RTU en RAM lancé, arrêt sûr235us. |work922us/gap2141us conformes, service2715>2500us ; journal epoch4/tx5/run5 indéterminé conservé. Flash M016 restaurée exacte/FRAM14fb intacte après restauration. Candidate83d4fbd ARM/O2 testée, non flashée ; qualifier service puis décision de réconciliation. R4global/R6 ouverts. [R4-R](../05_Firmware/LIVE_R4R.md). |'
road.write_text('\n'.join(lines)+'\n')
print('Final technical BLOCKED report prepared; restoration required and verified')
