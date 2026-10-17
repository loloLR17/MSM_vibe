from pathlib import Path
import json,datetime,hashlib
r=Path('/home/lolo/dev/msm/projects/MSM_vibe-R4U-live-runtime');p=Path('/home/lolo/dev/msm/reports/TR2-r4u');g=r/'Modbus RTU/00_gouvernance';f=r/'Modbus RTU/05_Firmware';d=json.loads((p/'physical-final-summary.json').read_text());restore=json.loads((p/'hardware/restoration-summary.json').read_text());utc=datetime.datetime.now(datetime.timezone.utc).isoformat()
# Correct observable wire contract: internal context is not extra B3 registers.
a=f/'LIVE_R4U.md';s=a.read_text().replace('contrôler status/flags/âge/run/config/UTCabsente','contrôler status/flags/âge/UTCabsente et les autorités B0/B4/B5')
s=s.replace('SQLite/Web/servicePC produit et domaines manquants restent distincts.','''Les identités run/config conservées dans LiveModbusImage sont internes : aucun
registre B3 réservé n'est réaffecté pour les exposer. La corrélation run7/config3
est démontrée ici par GDB/FRAM et oracle ; ne pas attribuer une preuve causale
complète sur fil à B3 seul. R5 devra définir cette corrélation observable dans
son contrat, sans modifier silencieusement les spécifications gelées.
SQLite/Web/servicePC produit et domaines manquants restent distincts.''')
s+='''\n## Résultat final physique — 17/10/2026 UTC\n\nImage04/code806d967, BIN147320octets SHA89951ff52ee997ff742ce0684fdbaff1cb22baef080aca872f9dd4e802056f63,
ELF SHA68e666ba776e654d5a4fb0f4178e91250ac0bd235d4444001ddf20b993aa6383,
text146828/data492/BSS657384. START5/3/run7 COMPLETED4/0, retry3 sans nouvel effet,
collision3/STOP refus27, STOP5/4 COMPLETED4/0.3045698XYZ/380713TS,58fenêtres4096 ;
1632réponses physiques CRC/longueur exactes,731V/F,49HELD, avant fenêtre invalide,
UTCabsente. Oracle première4096 : RMS958/955/75/13mg, crêtes1003/998/107/55mg,
durée154ms, égalité exacte auxXYZbruts.1141contrôles horloge, résidu≤100µs.
Work810/service1669/gap1897µs sous1000/2500/5000 ; FIFOmax56/erreurs0,
UART0drop/0erreur/0failsafe. Coût callbackSTART805/STOP203µs.
**Latence commande durable distincte : START63,100s, STOP63,249s jusqu'au terminal
observé ; ACKSTOP→premierHELD55,757s.** Ce délai comprend les barrières R2 et le
polling/USB ; aucune borne d'arrêt temps réel ou de WCETIRQ isolé prouvée.
Priorité de file STOP testée sur modèle, aucune campagne physique de concurrence
de plusieurs maîtres ni préemption des commits. Ne pas assimiler203µs à la latenceB5.
8commits/419408octets/13112writes bornés LIVE,24plages ; FRAM entière exacte au
modèle métadonnées, journal46/root59/epoch5/run7/config3/gen3/effect27phase3,
SHA95dab7909dbbab0a85abf1a3d9f8292bafb48dc98de73f026ce2786251b9af41.
Historique et tail byte-identiques ; aucune retraite, SD ou ancienneFRAMrestaurée.
M016 restauré10:00:31UTC via18pages, Flash entière2Mio c5bc…f5b26 exacte, FRAM
entière finale relue et inchangée ; contrôles capteur WHO7B/ODRoff/FIFObypass/TSoff
après restauration, GDBdétaché, COM4fermé. Le firmware de qualification n'est pas laissé.

Portée limitée à ce banc/charge/N4096. N8192/16384/32768 physiques non acquis :
collecte brute32768 naïve ajoute172032octets, BSS829416>SRAM786432 avant pile.
Prévoir collecte de référence bornée/streamée dans une mission séparée, pas
augmentation tacite SRAM ni modificationV1. Température/CPU/RTC/B7, UTC, power-cycle,
endurance, défauts HAL/CPU/pile, terminaison/CEM et cause du décalage split restent
non qualifiés. Aucun GO produit LIVE/R4global/R5/R6, contrôle indépendantChatGPT attendu.
''';a.write_text(s)
# Keep historical sections; replace current summary and hardware bullet factually.
j=g/'ETAT_COURANT_TR2.md';s=j.read_text();start=s.index('**Dernière actualisation :**');mid=s.index(' **État du second banc',start)
s=s[:start]+'''**Dernière actualisation :** 17octobre2026UTC, R4-U : composition opt-in FIFO/R2/FRAM/B3/UART-RS485 physique PASS borné N4096, code806d967.3045698XYZ/58fenêtres/1632réponsesCOM4CRCexactes, work810/service1669/gap1897µs. M016 restauré10:00:31UTC, Flash2Mio c5bc…f5b26 exacte ; FRAM finale95dab790…9af41/journal46/root59/epoch5/run7/config3 conservée sans rollback. STOPcallback203µs mais ACK→HELD55,757s viabarrièresR2 : pas d'arrêt industriel temps réel qualifié. Contrôle indépendantChatGPT à réaliser ; R4global/R5/R6 ouverts.'''+s[mid:]
start=s.index('- **Cible principale vérifiée');end=s.index('\n- **Code A04',start)
s=s[:start]+'''- **Cible principale vérifiée le17/10/2026UTC (R4-U) :** TR2-001 / NUCLEO-U575ZI-Q, SN003500463235510B37333439, UID004A003D/38425018/20333157. À10:00:31UTC, M016 CommissioningReadOnly WAIT restauré, Flash2097152octets exacte c5bc344d…f5b26 ; capteur arrêté confirmé après restauration/GDBdétaché/COM4fermé. FRAM262144 finale95dab790…9af41 relue après restauration, historique/tail exacts ; epoch5/run7/config3/gen3, journal46/root59, dernier STOP5/4 COMPLETED4/0, effect27phase3. Ancien STARTED4/5/run5 reste archivé indécidable ; pas de reçu inventé ni nouvelle retraite. Aucun firmware produit LIVE complet laissé sur carte ; états antérieurs ci-dessous historiques.'''+s[end:]
s+='''\n\n## R4-U — résultat final physique du17/10/2026UTC\n\n- **OBSERVÉ BORNÉ / code806d967** : runtime opt-in propriétaire FIFO unique, datation observée/R3-B/R2/R1 et vraiUART-RS485 COM4 ; START5/3/run7 etSTOP5/4terminaux4/0, retry sans effet/collision27.3045698XYZ/380713TS,58fenêtres N4096,1632réponsesCRCexactes dont731V/F et49HELD. Oracle4096 exact, UTCabsente. B0/3/4/5 exposés, B6UNAVAILABLE honnête, B1/2/7non supportés, capabilityLIVE16absente.\n- **MESURÉ** : work810/service1669/gap1897µs, bornes inchangées1000/2500/5000 ; FIFOmax56/0erreur, UART0drop/0erreur. CallbackSTART805/STOP203µs ; STOPB5 terminal63,249s, ACK→premierHELD55,757s. Priorité de file validée sur modèle seulement, pas de STOPindustriel temps réel ni WCETIRQ isolé. Variante split arrêtée sur bitshift/parité, voie duplex existante réussie bornément ; cause électrique/IRQ non démontrée, aucune garde affaiblie.\n- **RESTAURÉ / PRÉSERVÉ** : M016 Flash2Mio c5bc…f5b26 ; FRAMentière95dab790…9af41 exacte au modèle/journal46/root59/epoch5/run7/config3/effect27,8commits419408octets LIVEseulement ; historique/tail exacts. WHO7B, ODRoff/FIFObypass/TSoff après restauration, COM4fermé/GDBdétaché, aucunSD/HP/rollback.6626fichiers protégés/61autresworktrees/2stashes inchangés.\n- **LOGICIEL / LIMITES** :143anciens tests PASS, nouveau144 PASS ciblé après correction oraclereadonly, sanitizers et ARMdédié/canonique PASS ; BINcanonique85b1…c0f5 inchangé. Nsupérieurs physiques nécessitent collecte adaptée, B3run/config internes non exposés dans nouveaux registres réservés. Prochaine étape : contrôle indépendantChatGPT puis contratR5/capabilities/corrélation et décision sur latenceSTOPdurable, sans GO R4global/R6/industriel. Preuves [R4-U](Echanges_Codex/preuves/TR2-20261017-R4U/physical-final-summary.json).\n''';j.write_text(s)
road=g/'ROADMAP_TR2_VERS_SUPERVISION_VIBRATOIRE.md';s=road.read_text();lines=s.splitlines()
for i,l in enumerate(lines):
 if l.startswith('| **R2 —'):
  lines[i]='| **R2 — Moteur live** | **LOGICIEL VALIDÉ ; R4-U PHYSIQUE OBSERVÉ BORNÉ** | START5/3/run7, retry sans effet/collision27 etSTOP5/4 via COM4, terminaux4/0. | Journal46/root59/epoch5/run7/config3, FRAM finale95dab790…9af41 conservée ; STOPdurable63,249s, pas de recovery produit ni arrêt industriel qualifié. [R4-U](../05_Firmware/LIVE_R4U.md). |'
 elif l.startswith('| **R3 —'):
  lines[i]='| **R3 — FIFO/calcul/B3** | **R4-U N4096 PHYSIQUE OBSERVÉ BORNÉ** |3045698XYZ/58fenêtres,1632réponsesCOM4CRCexactes dont731V/F/49HELD ; oracle4096exact, UTCabsente. | Provider observé/run7/config3/gen3 ; Nsupérieurs/corrélation sur fil et produit complet à qualifier. Contrôle indépendantChatGPT attendu. [R4-U](../05_Firmware/LIVE_R4U.md). |'
 elif l.startswith('| **R4 —'):
  lines[i]='| **R4 — Temps réel** | **R4-U COMPOSITION/UART OBSERVÉ BORNÉ ; R4GLOBAL OUVERT** | Work810/service1669/gap1897µs sous1000/2500/5000, FIFOmax56/0erreur ; coexistence FRAM/FIFO/B3/RTU physique. | Duplex opt-in, défaut split conservé/cause non établie ; M016 restauré/FRAM95da préservée. STOPcallback203µs mais ACK→HELD55,757s ; IRQWCET/pile/fautes/endurance/R6 non qualifiés. [R4-U](../05_Firmware/LIVE_R4U.md). |'
road.write_text('\n'.join(lines)+'\n')
e=g/'Echanges_Codex';(e/'DERNIER_RAPPORT.md').write_text(f'''---
mission_id: TR2-20261017-R4U-LIVE-RUNTIME-RTU-PHYSICAL
status: IN_PROGRESS
base_ref: main
base_sha: fa7ba586114f83fe3fee3c87050620e22ee11930
initial_head: 70bffe63048ebc5b169852ef9814389db59ea56f
result_sha: 806d967b912080872a44f51c1f75887448c62ea3
created_at_utc: {utc}
author: Codex
target_branch: main
allow_commit: true
allow_push: true
allow_flash: true
allow_debug: true
---

# R4-U — runtime LIVE et Modbus physique : résultat borné, M016 restauré

**Critères techniques de la tranche sélectionnée satisfaits. IN_PROGRESS uniquement
pour publication/relecture distante finale**, pas pour un obstacle extérieur.
Vraie acquisition FIFO, R3-B/provider observé, R2/FRAM et UART-RS485 concurrentes.
START5/3/run7 et STOP5/4 COMPLETED4/0, retry identique sans effet, collision27.
Ce résultat ne qualifie ni produit LIVE complet, ni R4global/R5/R6 industriel.

## Référence, autorisations et état initial

Mission READY/basefa7ba58 vérifiée dans l'historique ; distant initial70bffe6,
worktree `MSM_vibe-R4U-live-runtime`, branche `codex/r4u-live-runtime`, destination
explicite `main`. Dépôt principal divergent ancien main82169af conservé propre,
aucun merge/rebase/reset/clean. AGENTS/V2-B3/protocole, journal intégral,
roadmap/R0/B5/R1–R4 et rapports R4-Q/R/S/T consultés. Archives R4T déjà publiées
préservées.6626fichiers protégés,61autres worktrees et deux stashes historiques
6251b988/f3b829e6 inchangés ; étude locale86d0f4b isolée, non intégrée.

Préflight frais09:30:14UTC : TR2-001/SN003500463235510B37333439,
UID004A003D/38425018/20333157, WHO7B/capteur arrêté. Flash2097152octets
SHAc5bc344d64984c48c3e34b05fa9432c9c124e8b67428eb0eef22c0d62ddf5b26,
FRAM262144 SHA371a9d16c6285d0150608751ccfdd5217ff303d42d8e5446d66a8130342e6634,
journal38/root51/epoch5/run6 terminal/config3gen3. Backups entiers vérifiés.
Aucune réapplication de la retraiteR4T ni rollbackFRAM.

Les permissions de mission toutestrue ont été exercées : développement, tests,
commits/push normaux ; quatre images probes distinctes flashées/vérifiées et
restaurationFlashM016 standard, debug qualifié avec UID/SN et breakpoints à l'arrêt.
Aucun mass erase/OB/RDP/SD/changement physique/HP/configurationCodex/politiqueWindows.

## Composition et inventaire

[Plan, matrice B0–B7, gardes et entrée R5](../../05_Firmware/LIVE_R4U.md).
Nouveau `live_runtime.[ch]` : service FIFO/horloge/snapshot toutes10ms/pollUART8,
aucun R2 dans le service ; foreground enter/leave puis un owner_step différé.
Serveur : option B6R0-INV UNAVAILABLE/schema2/health4 et sélecteurRAM ; mode
readonly refuse FC10 avant tout staging/capture. Pas de changement moteurR2,
formats persistants, spécificationsV1/V1.1 ou driverUART. B1/B2/B7 exception02.
B0 persisté protocole2/caps12, **bitLIVE16 absent** : sous-ensemble de qualification,
les clients produit doivent le refuser. B4/R1/config3gen3 et B5 existants réutilisés.
Run/config internes ne deviennent pas de nouveaux registresB3 ; corrélation
prouvée par GDB/FRAM ici, à contractualiser pour R5. Aucun développementPC extensif.

T1000 : COM3=STLINKVCP, **COM4=FTDI VID0403/PID6001/BG04KSSW**, confirmé par
l'utilisateur correctement raccordéTR2. LPUART1 PG7TX/PG8RX AF8, PG4DE-/REcommun,
1152008E1/t1.5=750µs/t3.5=1750µs. Maître natifC# .NETFramework exclusifCOM4,
CRC/longueurs/identité vérifiés, aucune répétition sur erreur transport. Sources,
requêtes et réponses hex/UTC/timing conservées. Terminaison électrique non mesurée.

## Validations logicielles réellement exécutées

- Suite complète initiale144PASS82,84s avant verrou readonly. Après ajout du verrou,
  seconde suite :143anciens PASS, seul nouveau144 échoue car son helper attendait
  ACK sur écriture readonly. Correction du scénario pour exiger exception01,
  puis nouveau144PASS0,37s, aucun test supprimé/affaibli. Même cœur final évalué
  par ces143+1résultats ; pas de fausse déclaration d'une dernière suite complète
  sans échec. Modèle FIFO/FRAM/UART asynchrone, garde réentrée, START/STOP/retry/
  collision, B6oracles/selector/no writes/B1/2/7exceptions et qualité vérifiés.
- Sanitizers O2/ASan/UBSan : ancien serveurPASS4,05s, nouveau144corrigéPASS0,62s.
  Les corrections ultérieures limitées au harness/profil ont justifié ARM dédié,
  sans répéter les143tests sur cœur inchangé.
- `STM32CUBE_U5_ROOT=… bash ./tr2_validate.sh --cross-build-only` finalPASS,
  canonique BIN SHA85b1be6044d76349a715ea0627b442bd3301e63daa606399b3d83bd5b9bbc0f5
  strictement inchangé ; image dédiée04 ARM PASS. Appel direct du script refusé
  car mode non exécutable, lancé normalement par bash sans changer de permission.
- Image04/code806d967 : BIN147320 SHA89951ff52ee997ff742ce0684fdbaff1cb22baef080aca872f9dd4e802056f63,
  ELF SHA68e666ba776e654d5a4fb0f4178e91250ac0bd235d4444001ddf20b993aa6383 ;
  text146828/data492/BSS657384. BIN/ELF compressés/profil et provenance conservés.
  Pas de helper de retraite R4T dans cette image, boot WAIT sans auto-START.

## Erreurs rencontrées et corrections, sans effacer les essais

1. Image01 RAMPASS132602XYZ/505ADU, FRAMexacte ; assert GDB post-dump utilisait
   work.fault inexistant (work.error=OK observé). Pas de répétition de ce code.
   Transition UART refusée avant init : second bind métrologie interdit. Image02
   conserve la liaison unique au lieu de contourner la garde. RAMcorrigéePASS.
2. Image02 second diagnostic refusé par monotonie run_id : même UINT64_MAX-1.
   Aucun nouvel échantillon/serviceUART ni mutation. Image03 isole une phase par
   boot ; aucun compteur abaissé. Le déclencheur GDB exige la fin d'initialisation
   prouvée au WAIT : pas de shell sleep Windows non disponible, pas de trigger
   écrit avant effacement BSS. Deux GDB de test en attente ont été terminés par
   PIDexact uniquement, puis observation arrêtée ; pas de flash/reset caché.
3. Image03 readonlyUART :72réponsesCRCexactes puis arrêt sûr à131050XYZ sur
   BENCH_TAG9/parité1. Au mot11 du burst161octets, décalage d'un bit ; réalignement
   hors ligne retrouve12tags et timestamp+24ticks. HALcomplete/OVRabsent,
   budgets338/1452/1386µs conformes, FRAMentière inchangée. La cause électrique/
   ISR/horloge n'est **pas démontrée** ; aucune donnée corrigée injectée.
4. Image04 active seulementR4U la voie duplex déjà existante (commande+payload
   en HAL_SPI_TransmitReceive) : contrôlesRXpreclose/OVR/parité/config maintenus,
   aucun masqueIRQ ajouté. RAM5s puis readonlyUART10,000207s sur boot neuf PASS :
   265441XYZ/33181TS,79réponsesCOM4,0erreur/0write, FRAM371a entière exacte ;
   work294/service1259/gap1263µs. Cette nouvelle méthode justifiait ce repassage,
   pas une répétition des acquis inchangés. V1/probes précédents gardent split.
5. PowerShell -File refusé par politique scripts : aucun bypass/changement.
   Inventaire par commandes natives en lecture seule et maîtreC# natif compilé.
   SelftestCRC sans transmissionPASS. Lecture GDB d'une adresseSPI supposée refusée
   avant dumps : corrigée en lisant hspi3.Instance réellement présent, sans essai
   physique supplémentaire. Erreurs et captures distinctes conservées.

## Qualification physique finale sur image04

[Résumé calculé](preuves/TR2-20261017-R4U/physical-final-summary.json),
[FRAM finale auditée](preuves/TR2-20261017-R4U/final-fram-audit.json).

- STARTepoch5/tx3/run7, retryidentique3, collision3/STOP→27, STOPepoch5/tx4 :
  vrais terminaux4/0 et un seul démarrage observé. **3045698XYZ/380713timestamps,
  58fenêtresN4096**, identité config3/gen3/autoritétrue observée par GDB.
- **1632réponses UART-RS485 physiques** : CRC/unité/FC/longueur/echo exacts,
  731B3V/F,49HELD, invalidité avant premièrefenêtre, STOPstatus1/HELD/F0,
  UTCabsente. Captures maîtreJSONL complètes compressées sans altération.
- Oracle indépendant première4096XYZ : RMSglobal/X/Y/Z958/955/75/13mg,
  crêtes1003/998/107/55mg, durée154ms ; égalité exacte snapshot/raw. Providerobservé,
  1141contrôles horloge/résidu sous100µs ; pas de datationUTC inventée.
- Max **work810/service1669/gapFIFO1897µs**, bornes1000/2500/5000 inchangées :
  marges observées190/831/3103µs. FIFOmax56/0erreur ; UART0drop/0erreur/0failsafe.
  CallbackSTART805µs/STOP203µs ; coûtsIRQ/TX individuels et WCET non isolés,
  inclus dans les services observés et timingsADU maître, aucun faux WCET.
- **Délai durable distinct** : STARTterminal63,100s, retry0,130s,
  STOPterminal63,249s ; ACKSTOP→premierHELD55,757s observés depuis le PC.
  Les barrièresR2/polling/USB sont inclus ;203µs n'est pas la latenceB5.
  Priorité de file STOP démontrée sur modèle ; pas de campagne physique à plusieurs
  maîtres/charge hostile ni de préemption des commits. Une exigence d'arrêt rapide
  nécessite une décision/mission dédiée, pas une modification silencieuseR2.
-8commitsR2/419408octets/13112appels physiques,24plages exclusivement LIVE.
  FRAM262144 **exacte au modèle RAM de métadonnées** (qui ne prouve pas les effets
  capteur à lui seul), historique[0,122880) et tail[245760,262144) byte-identiques.
  Journal46/root59/epoch5/run7/config3gen3/effect27phase3, dernierSTOP5/4terminal.
  SHAentière **95dab7909dbbab0a85abf1a3d9f8292bafb48dc98de73f026ce2786251b9af41**.
  Cette empreinte remplace371a comme autorité ; jamais réappliquer retraiteR4T
  ni utiliser les clés5/1..4 ou une ancienneFRAM pour reprendre.

## Restauration, limites et suite

M016 restauré **{restore['utc']}** :18pages normales/147456octets depuis backup
entier vérifié, Flash2097152octets relue SHAinitialc5bc…f5b26 **exacte**.
FRAM entière relue après restauration, exacte à95da finale sans rollback.
Contrôle capteur après restauration WHO7B/ODRoff/FIFObypass/TSoff ; GDBdétaché,
COM4fermé, aucunSD/HP/OB/RDP/mass erase ni intervention physique demandée.

B1/B2/B7 non composés, bitLIVE16 absent ; pas de produit protocole2complet.
N8192/16384/32768 physiques non qualifiés, collecte brute adaptée à prévoir
(naïf32768 : BSS829416>SRAM786432 avant pile). Endurance/power-cycle/UTC/CEM/
terminaison/CPU/pile/fauteHAL et cause précise du défaut split restent ouverts.
La réussite duplex bornée ne démontre ni absence universelle du défaut ni
qualification industrielle. V1 gelée, M014/M015, FRAMhistorique/SD et dettesA04 préservés.

Journal **actualisé** (synthèse courante, état matériel, résultats/limites/prochaine
étape) et roadmapR2–R4 **factuelle**, sans qualifier J3–J5/R4global/R6.
À transmettre à ChatGPT : contrôle indépendant ; contratR5 de détectionprofil/
capabilities/qualité/corrélationobservable ; décision sur latenceSTOPdurable,
collecteNsupérieurs et domaines manquants. Aucun changement d'architecture adopté.

## Publication et preuves

Checkpoints main43840fe,9046bba,0f1fa5d,806d967,55e572d publiés par pushnormal ;
relecture des contenus depuis origin/main effectuée aux checkpoints indiqués.
Code réellement qualifié806d967 ; commits suivants documentaires uniquement.
Aucun merge/rebase/forcepush. État prépublication : seuls livrablesR4U indexables,
mainhistorique82169af propre et61autresworktrees/stashes préservés.

Preuves : `preuves/TR2-20261017-R4U/`, manifestBUNDLE etSHA256SUMS ; rawlogs,
ADUhex/UTC/timing, scriptsGDB/commandes, préflight, builds/tests/sanitizers,
images BIN01..04/ELF04 compressés, XYZpremièrefenêtre, snapshotsFlash/FRAM entiers,
modèle de métadonnées/audit et restauration. `zlib.decompress` restitue les octets
originaux ; manifest fournit empreintes brutes et stockées. Externes préservés :
`/home/lolo/dev/msm/reports/TR2-r4u/`. Dernier push/archivage et relecture finale
restent à effectuer ; statut provisoire seulement pour cette étape de protocole.
''')
print('Final technical report prepared',utc)
